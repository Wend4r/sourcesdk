#include "common/assert.h"
#include "common/macros.h"

#include <tier0/platform.h>
#include <tier0/threadtools.h>
#include <tier0/eventutils.h>

#include <atomic>
#include <cstring>
#include <thread>

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
	TEST_EQ( ThreadGetMainThreadId(), static_cast< ThreadId_t >( ThreadGetCurrentId() ) );
}

REGISTER_NAMED_TEST( "Thread.Helpers", Thread_Helpers )
{
	ThreadSpin( 100 );
	ThreadYield();

	// The sorted processor indices are valid logical processor numbers.
	uint16 nIndices[ 256 ];
	const uint16 nCount = GetPerfSortedLogicalProcessors( nIndices, 256 );

	for ( uint16 i = 0; i < nCount; i++ )
	{
		TEST_TRUE( nIndices[ i ] < 256 );
	}
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
