#include "common/assert.h"
#include "common/macros.h"

#include <tier0/platform.h>
#include <tier0/threadtools.h>
#include <tier0/eventutils.h>
#include <tier0/fibertools.h>
#include <tier0/asyncfunction.h>
#include <tier0/tools_stall_monitor.h>

#include <atomic>
#include <cstring>
#include <thread>

#ifndef PLATFORM_WINDOWS
#include <sys/wait.h>
#include <unistd.h>
#endif

REGISTER_NAMED_TEST( "Thread.InitOnce", Thread_InitOnce )
{
	// The first caller runs the initialization, later callers see it finished.
	ThreadInitOnce_t marker = 0;

	TEST_TRUE( ThreadInitOnceCall_ShouldRun( &marker ) );
	ThreadInitOnceCall_Finish( &marker );
	TEST_FALSE( ThreadInitOnceCall_ShouldRun( &marker ) );
}

REGISTER_NAMED_TEST( "Thread.SecondaryMainThread", Thread_SecondaryMainThread )
{
	DeclareCurrentThreadIsSecondaryMainThread();
	TEST_TRUE( ThreadInSecondaryMainThread() );

	ClearSecondaryMainThread();
	TEST_FALSE( ThreadInSecondaryMainThread() );

	DeclareCurrentThreadIsMainThread();
	TEST_TRUE( ThreadInMainThread() );
	TEST_TRUE( ThreadInPrimaryOrSecondaryMainThread() );
	TEST_EQ( ThreadGetMainThreadId(), static_cast< ThreadId_t >( ThreadGetCurrentId() ) );
}

REGISTER_NAMED_TEST( "Thread.Helpers", Thread_Helpers )
{
	ThreadSpin( 100 );
	ThreadYield();
	CThread::Yield();

	// The sorted processor indices are valid logical processor numbers.
	uint16 nIndices[ 256 ];
	const uint16 nCount = GetPerfSortedLogicalProcessors( nIndices, 256 );

	for ( uint16 i = 0; i < nCount; i++ )
	{
		TEST_TRUE( nIndices[ i ] < 256 );
	}
}

class CThreadHandleTestThread : public CThread
{
public:
	ThreadHandle_t GetConstThreadHandle() const { return GetThreadHandle(); }

protected:
	virtual int Run() { return 42; }
};

REGISTER_NAMED_TEST( "Thread.ConstThreadHandle", Thread_ConstThreadHandle )
{
	CThreadHandleTestThread thread;
	thread.SetName( "ConstThreadHandle" );
	TEST_TRUE( thread.Start() );

	// The const handle of the derived classes is the public one.
	TEST_TRUE( thread.GetThreadHandle() != nullptr );
	TEST_TRUE( thread.GetConstThreadHandle() == thread.GetThreadHandle() );

	TEST_TRUE( thread.Join() );
	TEST_EQ( thread.GetResult(), 42 );
}

REGISTER_NAMED_TEST( "Thread.CurrentProcess", Thread_CurrentProcess )
{
	const uint32 nProcessId = ThreadGetCurrentProcessId();

	TEST_TRUE( nProcessId != 0 );
	TEST_EQ( ThreadGetProcessIdFromHandle( ThreadGetCurrentProcessHandle() ), nProcessId );
	TEST_TRUE( ThreadIsProcessIdActive( nProcessId ) );

	const ThreadProcessPriority_t ePriority = ThreadGetProcessPriority( ThreadGetCurrentProcessHandle() );

	TEST_TRUE( ePriority >= THREAD_PROCESS_PRIORITY_IDLE );
	TEST_TRUE( ePriority <= THREAD_PROCESS_PRIORITY_REALTIME );
}

REGISTER_NAMED_TEST( "Thread.MultiWait", Thread_MultiWait )
{
	CThreadMultiWaitEvent autoReset( false );
	CThreadMultiWaitEvent manualReset( true );
	CThreadMultiWaitObject *pObjects[] = { &autoReset, &manualReset };

	ThreadMultiWaitObjectSet_t *pSet = ThreadMultiWaitObjectSetCreate( pObjects, 2 );

	TEST_NOT_NULL( pSet );
	TEST_EQ( ThreadMultiWaitObjectSetGetCount( pSet ), 2 );
	TEST_EQ( pSet->m_nItems, 2 );
	TEST_EQ( pSet->m_pExtra[ 0 ].nFlags, 2u );
	TEST_EQ( pSet->m_pExtra[ 1 ].nFlags, 4u );

	uint64 nSignalled = 0;

	// Nothing is signalled, so a zero timeout returns no objects.
	TEST_EQ( ThreadMultiWait( pSet, nullptr, nullptr, 0, false, &nSignalled, 0 ), 0 );
	TEST_EQ( nSignalled, static_cast< uint64 >( 0 ) );

	// pSignalled receives a bit per signalled object.
	manualReset.Set();
	TEST_EQ( ThreadMultiWait( pSet, nullptr, nullptr, 0, false, &nSignalled, 0 ), 1 );
	TEST_EQ( nSignalled, static_cast< uint64 >( 2 ) );

	manualReset.Reset();
	autoReset.Set();
	TEST_EQ( ThreadMultiWait( pSet, nullptr, nullptr, 0, false, &nSignalled, 0 ), 1 );
	TEST_EQ( nSignalled, static_cast< uint64 >( 1 ) );

	// The auto-reset event was consumed by the wait.
	TEST_EQ( ThreadMultiWait( pSet, nullptr, nullptr, 0, false, &nSignalled, 0 ), 0 );

	ThreadMultiWaitObjectSetDestroy( pSet );
}

REGISTER_NAMED_TEST( "Thread.ManualResetEventStaysSetUntilReset", Thread_ManualResetEventStaysSetUntilReset )
{
	CThreadEvent event( true );

	TEST_FALSE( event.Check() );

	// Every wait sees a manual-reset event as set until it is reset.
	event.Set();
	TEST_TRUE( event.Check() );
	TEST_TRUE( event.Wait( 0 ) );
	TEST_TRUE( event.Wait( 0 ) );

	event.Reset();
	TEST_FALSE( event.Wait( 0 ) );
}

REGISTER_NAMED_TEST( "Thread.AutoResetEventWakesOneWait", Thread_AutoResetEventWakesOneWait )
{
	CThreadEvent event( false );

	event.Set();
	TEST_TRUE( event.Wait( 0 ) );

	// The first wait consumed the signal.
	TEST_FALSE( event.Wait( 0 ) );
}

REGISTER_NAMED_TEST( "Thread.AtomicSemaphoreHandsOutReleasedCounts", Thread_AtomicSemaphoreHandsOutReleasedCounts )
{
	CAtomicSemaphore semaphore( 1 );

	// The initial count lets one wait through.
	TEST_TRUE( semaphore.TryWait() );
	TEST_FALSE( semaphore.TryWait() );

	semaphore.Release( 2 );
	TEST_EQ( semaphore.GetCount(), 2u );

	semaphore.Wait();
	TEST_EQ( semaphore.GetCount(), 1u );
}

REGISTER_NAMED_TEST( "Thread.AtomicSemaphoreWakesASleepingWait", Thread_AtomicSemaphoreWakesASleepingWait )
{
	CAtomicSemaphore semaphore;
	std::atomic< int > nStage( 0 );

	std::thread waiter( [ & ]()
	{
		nStage = 1;
		semaphore.Wait();
		nStage = 2;
	} );

	// The waiter spins, then sleeps until a count is released.
	ThreadSleep( 50 );
	TEST_EQ( nStage.load(), 1 );

	semaphore.Release();
	waiter.join();

	TEST_EQ( nStage.load(), 2 );
	TEST_EQ( semaphore.GetCount(), 0u );
}

REGISTER_NAMED_TEST( "Thread.MultiWaitObjectCanBeWaitedOnAlone", Thread_MultiWaitObjectCanBeWaitedOnAlone )
{
	CThreadMultiWaitEvent event( true );
	CThreadMultiWaitSemaphore semaphore;

	ThreadMultiWaitItem_t item;
	ThreadMultiWaitExtra_t extra;
	semaphore.GetWaitItem( &item, &extra );
	TEST_EQ( extra.nFlags, 1u );
	TEST_EQ( event.GetExtraFlags(), 4u );

	TEST_FALSE( event.Check() );
	event.Set();

	// A manual-reset event stays set.
	TEST_TRUE( event.Wait( 0 ) );
	TEST_TRUE( event.Check() );

	// A semaphore lets one wait through per release.
	semaphore.Release();
	TEST_TRUE( semaphore.Check() );
	TEST_FALSE( semaphore.Check() );
}

REGISTER_NAMED_TEST( "Thread.MultiWaitEventArray", Thread_MultiWaitEventArray )
{
	CThreadMultiWaitEvent manualReset( true );
	CThreadMultiWaitEvent *pEvents[] = { &manualReset };

	ThreadMultiWaitObjectSet_t *pSet = ThreadMultiWaitObjectSetCreate( pEvents, 1 );

	TEST_NOT_NULL( pSet );
	TEST_EQ( pSet->m_pExtra[ 0 ].nFlags, 4u );

	uint64 nSignalled = 0;

	manualReset.Set();
	TEST_EQ( ThreadMultiWait( pSet, nullptr, nullptr, 0, false, &nSignalled, 0 ), 1 );
	TEST_EQ( nSignalled, static_cast< uint64 >( 1 ) );

	ThreadMultiWaitObjectSetDestroy( pSet );
}

REGISTER_NAMED_TEST( "Thread.MultiWaitSemaphoreCountsReleases", Thread_MultiWaitSemaphoreCountsReleases )
{
	CThreadMultiWaitSemaphore semaphore;
	CThreadMultiWaitObject *pObjects[] = { &semaphore };

	ThreadMultiWaitObjectSet_t *pSet = ThreadMultiWaitObjectSetCreate( pObjects, 1 );

	TEST_NOT_NULL( pSet );
	TEST_EQ( pSet->m_pExtra[ 0 ].nFlags, 1u );

	uint64 nSignalled = 0;

	// Nothing was released yet.
	TEST_EQ( ThreadMultiWait( pSet, nullptr, nullptr, 0, false, &nSignalled, 0 ), 0 );

	// Two releases let exactly two waits through.
	semaphore.Release();
	semaphore.Release();

	TEST_EQ( ThreadMultiWait( pSet, nullptr, nullptr, 0, false, &nSignalled, 0 ), 1 );
	TEST_EQ( nSignalled, static_cast< uint64 >( 1 ) );
	TEST_EQ( ThreadMultiWait( pSet, nullptr, nullptr, 0, false, &nSignalled, 0 ), 1 );
	TEST_EQ( ThreadMultiWait( pSet, nullptr, nullptr, 0, false, &nSignalled, 0 ), 0 );

	ThreadMultiWaitObjectSetDestroy( pSet );
}

REGISTER_NAMED_TEST( "Thread.BarrierReleasesThreadsOnlyWhenAllArrive", Thread_BarrierReleasesThreadsOnlyWhenAllArrive )
{
	CThreadBarrier barrier( 3 );
	std::atomic< int > nArrived( 0 );
	std::atomic< int > nPassed( 0 );

	auto arrive = [ & ]()
	{
		nArrived++;
		barrier.ArriveAndWait();
		nPassed++;
	};

	std::thread first( arrive );
	std::thread second( arrive );

	while ( nArrived != 2 )
	{
		ThreadSleep( 1 );
	}

	// Two of the three threads are waiting, so none of them may pass yet.
	ThreadSleep( 20 );
	TEST_EQ( nPassed.load(), 0 );

	// The third arrival releases everyone.
	barrier.ArriveAndWait();

	first.join();
	second.join();
	TEST_EQ( nPassed.load(), 2 );
}

REGISTER_NAMED_TEST( "Thread.WorkGatePauseWaitsForRunningWork", Thread_WorkGatePauseWaitsForRunningWork )
{
	CThreadBackgroundWorkGate gate;

	TEST_FALSE( gate.PauseRequested() );

	// Background work is running when another thread asks for a pause.
	gate.OnBeginBackgroundWork();

	std::atomic< int > nStage( 0 );

	std::thread pauser( [ & ]()
	{
		nStage = 1;
		gate.PauseWork();
		nStage = 2;
	} );

	ThreadSleep( 20 );

	// The pause is requested but waits until the work reaches its end.
	TEST_TRUE( gate.PauseRequested() );
	TEST_EQ( nStage.load(), 1 );

	gate.OnEndBackgroundWork();

	pauser.join();
	TEST_EQ( nStage.load(), 2 );

	// Resuming lifts the request.
	gate.ResumeWork();
	TEST_FALSE( gate.PauseRequested() );
}

REGISTER_NAMED_TEST( "Thread.LocalScopeKeepsTheThreadName", Thread_LocalScopeKeepsTheThreadName )
{
	// tier0 keeps a scope for the main thread.
	CThreadLocalScope *pScope = CThreadLocalScope::FindScopeForThreadId( ThreadGetCurrentId() );

	TEST_NOT_NULL( pScope );
	TEST_TRUE( pScope->IsLocalThreadScope() );

	pScope->SetThreadName( "SdkTestMain" );
	TEST_EQ( strcmp( pScope->GetThreadName(), "SdkTestMain" ), 0 );

	char szName[ 32 ];
	TEST_TRUE( ThreadGetDebugNameForThreadId( ThreadGetCurrentId(), szName, sizeof( szName ) ) );
	TEST_EQ( strcmp( szName, "SdkTestMain" ), 0 );
}

REGISTER_NAMED_TEST( "Thread.OneTimeEventIsResetWhenReused", Thread_OneTimeEventIsResetWhenReused )
{
	COneTimeEvent *pEvent = COneTimeEvent::Allocate();

	TEST_NOT_NULL( pEvent );
	TEST_FALSE( pEvent->Check() );

	pEvent->Set();
	TEST_TRUE( pEvent->Check() );
	TEST_TRUE( pEvent->Wait( 0 ) );

	// Allocate hands out one reference, the last release returns the event to the pool.
	TEST_EQ( pEvent->AddRef(), 2 );
	TEST_EQ( pEvent->Release(), 1 );
	TEST_EQ( pEvent->Release(), 0 );

	// The pool hands the event out again, unset.
	COneTimeEvent *pReused = COneTimeEvent::Allocate();

	TEST_NOT_NULL( pReused );
	TEST_FALSE( pReused->Check() );
	pReused->Release();
}

REGISTER_NAMED_TEST( "Thread.CrossProcessMutexCanBeTakenAndReleased", Thread_CrossProcessMutexCanBeTakenAndReleased )
{
	CThreadCrossProcessMutex mutex( "sourcesdk_tests_cross_process_mutex", false );

	TEST_TRUE( mutex.Wait( 0 ) );
	TEST_TRUE( mutex.Release() );
}

static void FiberStoreJob( void *pContext )
{
	int *pValue = *static_cast< int ** >( pContext );
	*pValue = 42;
}

REGISTER_NAMED_TEST( "Fiber.RunsAJobOnItsOwnStack", Fiber_RunsAJobOnItsOwnStack )
{
	CFiber *pFiber = CFiber::AllocPooledFiber( CFiber::STACKSIZE_64KB, 0 );

	TEST_NOT_NULL( pFiber );
	TEST_FALSE( pFiber->HasAJob() );

	// The context is copied to the top of the fiber stack.
	int nValue = 0;
	int *pValue = &nValue;
	void *pContext = pFiber->SetFiberJob( &FiberStoreJob, &pValue, sizeof( pValue ) );

	TEST_TRUE( pFiber->HasAJob() );
	TEST_EQ( pFiber->GetFiberJobContext(), pContext );
	TEST_TRUE( pFiber->IsMemoryInFiberRange( pContext ) );
	TEST_FALSE( pFiber->IsMemoryInFiberRange( &nValue ) );

	// ResumeJob returns true once the job has finished.
	TEST_TRUE( pFiber->ResumeJob( nullptr ) );
	TEST_EQ( nValue, 42 );
	TEST_FALSE( pFiber->HasAJob() );

	pFiber->Release();
}

static void *s_pFiberResumeValue;

static void FiberResumeValueJob( void *pContext, void *pResumeVal )
{
	s_pFiberResumeValue = pResumeVal;
}

REGISTER_NAMED_TEST( "Fiber.JobGetsTheResumeValue", Fiber_JobGetsTheResumeValue )
{
	CFiber *pFiber = CFiber::AllocPooledFiber( CFiber::STACKSIZE_64KB, 0 );
	TEST_NOT_NULL( pFiber );

	int nResume = 0;
	s_pFiberResumeValue = nullptr;
	pFiber->SetFiberJob( &FiberResumeValueJob );
	TEST_TRUE( pFiber->ResumeJob( &nResume ) );
	TEST_TRUE( s_pFiberResumeValue == &nResume );

	pFiber->Release();
}

static int s_nFiberYieldStage;

static void FiberYieldJob( void * )
{
	s_nFiberYieldStage = 1;
	CFiber::GetCurrentFiber()->FiberYield();
	s_nFiberYieldStage = 2;
}

REGISTER_NAMED_TEST( "Fiber.YieldReturnsToTheResumingThread", Fiber_YieldReturnsToTheResumingThread )
{
	CFiber *pFiber = CFiber::AllocPooledFiber( CFiber::STACKSIZE_64KB, 0 );

	TEST_NOT_NULL( pFiber );

	s_nFiberYieldStage = 0;
	pFiber->SetFiberJob( &FiberYieldJob );

	// The job yields half way, so the first resume returns before it finishes.
	TEST_FALSE( pFiber->ResumeJob( nullptr ) );
	TEST_EQ( s_nFiberYieldStage, 1 );

	TEST_TRUE( pFiber->ResumeJob( nullptr ) );
	TEST_EQ( s_nFiberYieldStage, 2 );

	pFiber->Release();
}

REGISTER_NAMED_TEST( "Fiber.EventLockHandsTheLockToTheWaiter", Fiber_EventLockHandsTheLockToTheWaiter )
{
	CEventLock lock;

	TEST_TRUE( lock.TryLock() );
	TEST_FALSE( lock.TryLock() );

	std::atomic< bool > bWaiterOwns( false );
	std::thread waiter( [ & ]()
	{
		lock.Lock();
		bWaiterOwns = true;
		lock.Unlock();
	} );

	// The waiter stays queued while the lock is held.
	ThreadSleep( 50 );
	TEST_FALSE( bWaiterOwns.load() );

	lock.Unlock();
	waiter.join();

	TEST_TRUE( bWaiterOwns.load() );

	// The waiter freed the lock again.
	TEST_TRUE( lock.TryLock() );
	lock.Unlock();
}

REGISTER_NAMED_TEST( "Fiber.EventRWLockWriteLockNests", Fiber_EventRWLockWriteLockNests )
{
	CEventRWLock lock;
	CEventListNode node;

	lock.LockForWrite( node );
	lock.LockForWrite( node );
	TEST_EQ( lock.GetWriteLockEvent(), &node );

	lock.UnlockWrite();
	TEST_EQ( lock.GetWriteLockEvent(), &node );

	lock.UnlockWrite();
	TEST_NULL( lock.GetWriteLockEvent() );
}

REGISTER_NAMED_TEST( "Fiber.EventRWLockReaderWaitsForTheWriter", Fiber_EventRWLockReaderWaitsForTheWriter )
{
	CEventRWLock lock;

	lock.LockForWrite();

	std::atomic< bool > bReaderOwns( false );
	std::thread reader( [ & ]()
	{
		lock.LockForRead();
		bReaderOwns = true;
		lock.UnlockRead();
	} );

	ThreadSleep( 50 );
	TEST_FALSE( bReaderOwns.load() );

	lock.UnlockWrite();
	reader.join();

	TEST_TRUE( bReaderOwns.load() );
}

REGISTER_NAMED_TEST( "Fiber.EventRWLockWriterWaitsForTheReaders", Fiber_EventRWLockWriterWaitsForTheReaders )
{
	CEventRWLock lock;

	// Read locks are shared.
	lock.LockForRead();
	lock.LockForRead();

	std::atomic< bool > bWriterOwns( false );
	std::thread writer( [ & ]()
	{
		lock.LockForWrite();
		bWriterOwns = true;
		lock.UnlockWrite();
	} );

	ThreadSleep( 50 );
	TEST_FALSE( bWriterOwns.load() );

	lock.UnlockRead();
	ThreadSleep( 50 );
	TEST_FALSE( bWriterOwns.load() );

	lock.UnlockRead();
	writer.join();

	TEST_TRUE( bWriterOwns.load() );
}

REGISTER_NAMED_TEST( "Fiber.EventRWLockWritersTakeTurns", Fiber_EventRWLockWritersTakeTurns )
{
	CEventRWLock lock;
	int nCounter = 0;

	auto work = [ & ]()
	{
		for ( int i = 0; i < 10000; i++ )
		{
			lock.LockForWrite();
			nCounter++;
			lock.UnlockWrite();
		}
	};

	std::thread first( work );
	std::thread second( work );
	first.join();
	second.join();

	TEST_EQ( nCounter, 20000 );
}

REGISTER_NAMED_TEST( "Fiber.ThreadSwapJobRunsInPlaceWithoutATargetThread", Fiber_ThreadSwapJobRunsInPlaceWithoutATargetThread )
{
	CThreadSwapTarget target;
	CThreadSwapJob job( &target );

	struct JobContext_t
	{
		ThreadId_t m_hThread = 0;
		int m_nRuns = 0;
	} context;

	job.RunJob( []( void *pContext )
	{
		JobContext_t *pJobContext = static_cast< JobContext_t * >( pContext );

		pJobContext->m_hThread = ThreadGetCurrentId();
		pJobContext->m_nRuns++;
	}, &context );

	TEST_EQ( context.m_nRuns, 1 );
	TEST_EQ( context.m_hThread, ThreadGetCurrentId() );
}

class CTestFiberWorkerPool : public IFiberThreadWorkerPool
{
public:
	virtual void FiberJobAvailable() { m_nJobsAvailable++; }
	virtual void Quit() { m_nQuits++; }

	int m_nJobsAvailable = 0;
	int m_nQuits = 0;
};

REGISTER_NAMED_TEST( "IFiberJobPool.EmptyPool", IFiberJobPool_EmptyPool )
{
	TEST_NOT_NULL( g_pFiberJobPool );

	bool bSkippedFibers = true;
	TEST_NULL( g_pFiberJobPool->GetWaitingFiberJob( &bSkippedFibers ) );
}

REGISTER_NAMED_TEST( "IFiberJobPool.AddGetRemove", IFiberJobPool_AddGetRemove )
{
	CFiber *pFiber = new CFiber;
	CTestFiberWorkerPool workers;
	g_pFiberJobPool->RegisterWorkerThreadPool( &workers );

	// A fiber that is not waiting cannot be removed.
	TEST_FALSE( g_pFiberJobPool->RemoveFiberJob( pFiber ) );

	// Adding a fiber tells the registered pools.
	g_pFiberJobPool->AddFiberToJobPool( pFiber, false );
	TEST_EQ( workers.m_nJobsAvailable, 1 );

	bool bSkippedFibers = true;
	TEST_TRUE( g_pFiberJobPool->GetWaitingFiberJob( &bSkippedFibers ) == pFiber );
	TEST_NULL( g_pFiberJobPool->GetWaitingFiberJob( &bSkippedFibers ) );

	// A waiting fiber can be removed again.
	g_pFiberJobPool->AddFiberToJobPool( pFiber, false );
	TEST_TRUE( g_pFiberJobPool->RemoveFiberJob( pFiber ) );
	TEST_NULL( g_pFiberJobPool->GetWaitingFiberJob( &bSkippedFibers ) );

	g_pFiberJobPool->UnregisterWorkerThreadPool( &workers );
	TEST_EQ( workers.m_nQuits, 0 );

	// An unregistered pool hears of no new jobs.
	g_pFiberJobPool->AddFiberToJobPool( pFiber, false );
	TEST_EQ( workers.m_nJobsAvailable, 2 );
	TEST_TRUE( g_pFiberJobPool->RemoveFiberJob( pFiber ) );
	delete pFiber;
}

static std::atomic< int > s_nScheduledCalls;

static float TestScheduledFunction()
{
	s_nScheduledCalls++;

	// No next call.
	return 0.0f;
}

REGISTER_NAMED_TEST( "IAsyncScheduledFunctions.RegisterCallUnregister", IAsyncScheduledFunctions_RegisterCallUnregister )
{
	TEST_NOT_NULL( g_pScheduledFunctions );
	g_pScheduledFunctions->Init();

	CUtlDelegate< float() > function;
	function.Bind( &TestScheduledFunction );
	AsyncFunctionHandle_t hFunction = g_pScheduledFunctions->RegisterScheduledFunction( function, Plat_FloatTime() );

	// The thread of the scheduler calls it soon.
	for ( int i = 0; i < 200 && s_nScheduledCalls == 0; i++ )
		ThreadSleep( 10 );

	TEST_EQ( s_nScheduledCalls.load(), 1 );

	// A registered function can be moved, then removed.
	TEST_TRUE( g_pScheduledFunctions->RescheduleFunction( hFunction, Plat_FloatTime() + 1000.0 ) );
	g_pScheduledFunctions->UnregisterScheduledFunction( hFunction );
	g_pScheduledFunctions->Shutdown();
	TEST_EQ( s_nScheduledCalls.load(), 1 );
}

REGISTER_NAMED_TEST( "AsyncFunctionHandle_t.RawValue", AsyncFunctionHandle_t_RawValue )
{
	AsyncFunctionHandle_t hFirst( 3 );
	TEST_EQ( hFirst.GetRaw(), 3 );

	AsyncFunctionHandle_t hSecond;
	hSecond.SetRaw( 5 );
	TEST_TRUE( hFirst != hSecond );
	TEST_TRUE( hFirst < hSecond );

	*hSecond.GetRawPtrForWrite() = 3;
	TEST_TRUE( hFirst == hSecond );

	hSecond.GetRawRef() = 7;
	const AsyncFunctionHandle_t &hConst = hSecond;
	TEST_EQ( hConst.GetRawRef(), 7 );

	hSecond = 9;
	TEST_EQ( hSecond.GetRaw(), 9 );
	TEST_EQ( sizeof( AsyncFunctionHandle_t ), sizeof( int32 ) );
}

#ifdef PLATFORM_WINDOWS
REGISTER_NAMED_TEST( "ToolsStallMonitor_Shutdown.WithoutMonitor", ToolsStallMonitor_Shutdown_WithoutMonitor )
{
	// Without a monitor both do nothing. A monitor is not started here: a shutdown before its
	// thread creates the window waits for that thread forever.
	ToolsStallMonitor_IndicateActivity();
	ToolsStallMonitor_Shutdown();
}
#endif

#ifndef PLATFORM_WINDOWS
REGISTER_NAMED_TEST( "ThreadMultiWaitObjectSetCreate.Items", ThreadMultiWaitObjectSetCreate_Items )
{
	// A set of the native items, a descriptor of a pipe here.
	int fds[ 2 ];
	TEST_EQ( pipe( fds ), 0 );

	ThreadMultiWaitItem_t items[ 1 ] = {};
	items[ 0 ].fd = fds[ 0 ];
	items[ 0 ].events = POLLIN;
	ThreadMultiWaitExtra_t extra[ 1 ] = {};

	ThreadMultiWaitObjectSet_t *pSet = ThreadMultiWaitObjectSetCreate( items, extra, 1 );
	TEST_NOT_NULL( pSet );
	TEST_EQ( ThreadMultiWaitObjectSetGetCount( pSet ), 1 );
	ThreadMultiWaitObjectSetDestroy( pSet );

	close( fds[ 0 ] );
	close( fds[ 1 ] );
}
#endif

#ifndef PLATFORM_WINDOWS
static int TestFiberJobPoolShutdown()
{
	CTestFiberWorkerPool workers;
	g_pFiberJobPool->RegisterWorkerThreadPool( &workers );

	CFiber *pFiber = CFiber::AllocPooledFiber( CFiber::STACKSIZE_64KB, 0 );
	pFiber->SetFiberJob( &FiberYieldJob );
	g_pFiberJobPool->AddFiberToJobPool( pFiber, false );

	g_pFiberJobPool->Shutdown();

	if ( workers.m_nQuits != 1 )
		return 1;

	// The waiting fiber is gone and the pool hears nothing more.
	bool bSkippedFibers = false;
	if ( g_pFiberJobPool->GetWaitingFiberJob( &bSkippedFibers ) != nullptr )
		return 2;

	return 0;
}

REGISTER_NAMED_TEST( "IFiberJobPool.Shutdown", IFiberJobPool_Shutdown )
{
	// The pool of the process is shut down for good, so in a child.
	const pid_t nChild = fork();
	TEST_TRUE( nChild >= 0 );

	if ( nChild == 0 )
		_exit( TestFiberJobPoolShutdown() );

	int nStatus = 0;
	TEST_EQ( waitpid( nChild, &nStatus, 0 ), nChild );
	TEST_TRUE( WIFEXITED( nStatus ) );
	TEST_EQ( WEXITSTATUS( nStatus ), 0 );
}
#endif
