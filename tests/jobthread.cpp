#include "common/assert.h"
#include "common/macros.h"

#include <tier0/callqueue.h>
#include <tier0/functors.h>
#include <tier0/jobthread.h>
#include <threadedjob.h>

struct JobItem_t
{
	int m_nValue;
};

class CTestJob : public CThreadedJobWithDependencies
{
public:
	size_t GetUpstreamJobsOffset() const { return GetOffset( &m_UpstreamJobs ); }
	size_t GetPendingUpstreamJobsOffset() const { return GetOffset( &m_nPendingUpstreamJobs ); }
	size_t GetDownstreamJobsOffset() const { return GetOffset( &m_DownstreamJobs ); }

private:
	size_t GetOffset( const void *pMember ) const { return ( const char * )pMember - ( const char * )this; }
};

// Instantiates the templates without running them, as they need a started thread pool.
// ParallelForEach, ParallelProcess and CCallQueue are left out: their pointer atomics and
// lock-free queue use interlocked helpers that tier0 doesn't export.
[[maybe_unused]] static void InstantiateJobTemplates( JobItem_t *pItems, unsigned nItems, CUtlVector< CThreadedJobWithDependencies * > &jobs )
{
	CSmartPtr< CThreadedJob, CRefCountAccessor > pLambdaJob = g_pThreadPool->QueueJobWithFlags( "Lambda", JP_HIGH, 0, [ pItems ]() { pItems[ 0 ].m_nValue = 0; } );
	CSmartPtr< CThreadedJob, CRefCountAccessor > pFunctionJob = g_pThreadPool->QueueJobWithFlags( "Function", JP_HIGH, 0, std::function< void() >( [] {} ) );

	ParallelFor( 0, nItems, "ParallelFor", [ pItems ]( int i ) { pItems[ i ].m_nValue++; }, 0, INT_MAX, JP_NORMAL );

	Start( jobs );
	RunSync( jobs );

	pLambdaJob->Execute();
	pFunctionJob->TryExecute();
}

#ifdef PLATFORM_64BITS
REGISTER_NAMED_TEST( "CThreadedJobWithDependencies.Layout", CThreadedJobWithDependencies_Layout )
{
	// Offsets tier0's Start, RunSync and DoExecuteInternal access.
	CTestJob job;
	TEST_EQ( job.GetUpstreamJobsOffset(), static_cast< size_t >( 0x50 ) );
	TEST_EQ( job.GetPendingUpstreamJobsOffset(), static_cast< size_t >( 0x88 ) );
	TEST_EQ( job.GetDownstreamJobsOffset(), static_cast< size_t >( 0x90 ) );
}
#endif

REGISTER_NAMED_TEST( "CThreadedJob.Defaults", CThreadedJob_Defaults )
{
	// A new job is unserviced, keeps its priority and falls back to a literal name.
	class CNamedJob : public CThreadedJob
	{
	public:
		CNamedJob() : CThreadedJob( JP_LOW ) {}

	private:
		virtual void DoExecute() {}
	};

	CNamedJob job;
	TEST_EQ( job.GetStatus(), JOB_STATUS_UNSERVICED );
	TEST_EQ( job.GetPriority(), JP_LOW );
	TEST_TRUE( job.CanExecute() );
	TEST_FALSE( job.IsFinished() );
	TEST_EQ( V_strcmp( job.GetJobName(), "Job" ), 0 );

	job.SetJobName( "Named" );
	TEST_EQ( V_strcmp( job.GetJobName(), "Named" ), 0 );
}

class CFunctorTarget : public CRefCounted<>
{
public:
	void Set( int nValue ) { m_nValue = nValue; }
	int Get() const { return m_nValue; }

private:
	int m_nValue = 0;
};

static void ResetValue() {}
static void SetValue( int *pValue, int nValue ) { *pValue = nValue; }

REGISTER_NAMED_TEST( "CCallQueue.QueueMethods", CCallQueue_QueueMethods )
{
	// The generated QueueCall and QueueRefCall overloads resolve next to the functor QueueCall.
	TEST_TRUE( ( std::is_same_v< decltype( std::declval< CCallQueue & >().QueueCall( &ResetValue ) ), void > ) );
	TEST_TRUE( ( std::is_same_v< decltype( std::declval< CCallQueue & >().QueueCall( &SetValue, std::declval< int * >(), 1 ) ), void > ) );
	TEST_TRUE( ( std::is_same_v< decltype( std::declval< CCallQueue & >().QueueCall( std::declval< CFunctorTarget * >(), &CFunctorTarget::Set, 1 ) ), void > ) );
	TEST_TRUE( ( std::is_same_v< decltype( std::declval< CCallQueue & >().QueueCall( std::declval< CFunctorTarget * >(), &CFunctorTarget::Get ) ), void > ) );
	TEST_TRUE( ( std::is_same_v< decltype( std::declval< CCallQueue & >().QueueRefCall( std::declval< CFunctorTarget * >(), &CFunctorTarget::Set, 1 ) ), void > ) );
	TEST_TRUE( ( std::is_same_v< decltype( std::declval< CCallQueue & >().QueueRefCall( std::declval< CFunctorTarget * >(), &CFunctorTarget::Get ) ), void > ) );
	TEST_TRUE( ( std::is_same_v< decltype( std::declval< CCallQueue & >().QueueCall( std::declval< std::function< void() > >() ) ), void > ) );
	TEST_TRUE( ( std::is_same_v< decltype( std::declval< CCallQueue & >().QueueFunctor( nullptr ) ), void > ) );
}

REGISTER_NAMED_TEST( "CFunctor.RefCountingMember", CFunctor_RefCountingMember )
{
	// A ref counting functor holds a reference to its object until it is released.
	CFunctorTarget *pTarget = new CFunctorTarget;
	CFunctor *pSet = CreateRefCountingFunctor( pTarget, &CFunctorTarget::Set, 5 );
	CFunctor *pGet = CreateRefCountingFunctor( pTarget, &CFunctorTarget::Get );

	TEST_EQ( pTarget->AddRef(), 4 );
	TEST_EQ( pTarget->Release(), 3 );

	( *pSet )();
	( *pGet )();
	TEST_EQ( pTarget->Get(), 5 );

	pSet->Release();
	pGet->Release();
	TEST_EQ( pTarget->Release(), 0 );
}

#ifdef PLATFORM_64BITS
REGISTER_NAMED_TEST( "CCallQueue.Layout", CCallQueue_Layout )
{
	// tier0's ParallelCallQueued reads the flags after the queue.
	TEST_EQ( sizeof( CCallQueue ), static_cast< size_t >( 80 ) );
	TEST_TRUE( ( std::is_same_v< decltype( std::declval< CCallQueue & >().IsDisabled() ), bool > ) );
	TEST_TRUE( ( std::is_same_v< decltype( std::declval< CCallQueue & >().IsProcessing() ), bool > ) );
}
#endif

// Runs worker jobs in turn on the calling thread and keeps queued jobs without running them
class CTestThreadPool : public IThreadPool
{
public:
	virtual int AddRef() { return 1; }
	virtual int Release() { return 1; }

	virtual bool Start( const ThreadPoolStartParams_t &startParams, const char *pszNameOverride ) { return true; }
	virtual bool Stop() { return true; }
	virtual void Restart( const ThreadPoolStartParams_t &startParams ) {}
	virtual int GetCoreTypeRequest() { return 0; }
	virtual int NumThreads() { return m_nThreads; }
	virtual int NumIdleThreads() { return m_nIdleThreads; }
	virtual int GetCurrentThreadIndex() { return -1; }
	virtual int GetCurrentThreadSlot( int, int nOtherThreadSlot ) { return nOtherThreadSlot; }
	virtual int SuspendExecution() { return 0; }
	virtual int ResumeExecution() { return 0; }
#ifdef _WIN32
	virtual bool YieldWait( void *const *pHandles, const ThreadMultiWaitExtra_t *pExtra, int nObjects, bool bWaitAll, ThreadMultiWaitObjectSet_t **ppObjectSet ) { return true; }
#else
	virtual bool YieldWait( const pollfd *pFds, const ThreadMultiWaitExtra_t *pExtra, int nObjects, bool bWaitAll, ThreadMultiWaitObjectSet_t **ppObjectSet ) { return true; }
#endif
	virtual bool YieldWait( CThreadedJob **ppJobs, int nJobs, bool bWaitAll, bool bExecuteWaitedJobs ) { m_nWaitedJobs += nJobs; return true; }
	virtual bool ExecuteOneJob() { return false; }
	virtual void AddJob( CThreadedJob *pJob ) { m_nQueuedJobs++; }
	virtual void AddPerFrameJob( CThreadedJob *pJob ) {}
	virtual bool YieldWaitPerFrameJobs() { return true; }

	virtual void ExecuteMultipleWorkerJob( IMultipleWorkerJob *pJob, int nMaxWorkers, const char *pszName, JobPriority_t priority )
	{
		for ( int i = 0; i < nMaxWorkers; i++ )
			pJob->Execute( i );
	}

	virtual const char *GetName() { return "Test"; }
	virtual void AddFunctionJob( const std::function< void() > &func, CThreadedJob **ppJob, const char *pszDescription, unsigned int nFlags, JobPriority_t priority ) {}

	using IThreadPool::YieldWait;

	int m_nThreads = 0;
	int m_nIdleThreads = 0;
	int m_nQueuedJobs = 0;
	int m_nWaitedJobs = 0;
};

static void AddValue( int *pValue, int nValue ) { *pValue += nValue; }
static void AddRange( int nFrom, int nTo, int *pSum ) { for ( int i = nFrom; i < nTo; i++ ) *pSum += i; }
static void SumLoop( int *pValues, int nFirst, int nCount ) { for ( int i = nFirst; i < nFirst + nCount; i++ ) pValues[ i ] += i; }

REGISTER_NAMED_TEST( "IThreadPool.AddCall", IThreadPool_AddCall )
{
	// Without an idle thread AddCall runs the call here and returns the finished job.
	CTestThreadPool pool;
	int nValue = 0;

	CThreadedJob *pJob = pool.AddCall( &AddValue, &nValue, 3 );
	TEST_EQ( nValue, 3 );
	TEST_TRUE( pJob->Executed() );
	TEST_EQ( pool.m_nQueuedJobs, 0 );
	TEST_EQ( V_strcmp( pJob->Describe(), "Job" ), 0 );
	pJob->Release();

	// QueueCall always queues.
	pJob = pool.QueueCall( &AddValue, &nValue, 4 );
	TEST_EQ( nValue, 3 );
	TEST_EQ( pool.m_nQueuedJobs, 1 );
	TEST_EQ( pJob->GetStatus(), JOB_STATUS_UNSERVICED );
	pJob->Release();

	// A ref counting call holds the object until the job goes away.
	CFunctorTarget *pTarget = new CFunctorTarget;
	pJob = pool.AddRefCall( pTarget, &CFunctorTarget::Set, 7 );
	TEST_EQ( pTarget->Get(), 7 );
	TEST_EQ( pTarget->AddRef(), 3 );
	pTarget->Release();
	pJob->Release();
	TEST_EQ( pTarget->Release(), 0 );
}

REGISTER_NAMED_TEST( "CJobSet.WaitForFinish", CJobSet_WaitForFinish )
{
	CTestThreadPool pool;
	int nValue = 0;

	CJobSet jobSet;
	jobSet += pool.AddCall( &AddValue, &nValue, 1 );
	jobSet += pool.AddCall( &AddValue, &nValue, 2 );
	jobSet.WaitForFinish( &pool );
	TEST_EQ( nValue, 3 );
	TEST_EQ( pool.m_nWaitedJobs, 2 );
}

REGISTER_NAMED_TEST( "Wait.YieldsOnThePoolOfTheJobs", Wait_YieldsOnThePoolOfTheJobs )
{
	CTestThreadPool pool;
	int nValue = 0;

	// The finished job is left out, the queued one goes to YieldWait of its pool.
	CUtlVector< CThreadedJob * > jobs;
	jobs.AddToTail( pool.AddCall( &AddValue, &nValue, 1 ) );
	jobs.AddToTail( pool.QueueCall( &AddValue, &nValue, 2 ) );
	Wait( jobs, true );
	TEST_EQ( pool.m_nWaitedJobs, 1 );

	for ( CThreadedJob *pJob : jobs )
		pJob->Release();
}

REGISTER_NAMED_TEST( "IterRangeParallel.Direct", IterRangeParallel_Direct )
{
	CTestThreadPool pool;
	IThreadPool *pOldPool = g_pThreadPool;
	g_pThreadPool = &pool;

	int nSum = 0;
	IterRangeParallel( &AddRange, 0, 10, &nSum );
	TEST_EQ( nSum, 45 );

	g_pThreadPool = pOldPool;
}

REGISTER_NAMED_TEST( "ParallelLoopProcess.Direct", ParallelLoopProcess_Direct )
{
	// Without pool threads the loop runs on the calling thread in chunks.
	CTestThreadPool pool;
	int values[ 10 ] = {};

	ParallelLoopProcessChunks( &pool, values, 0, 10, 3, &SumLoop );

	for ( int i = 0; i < 10; i++ )
		TEST_EQ( values[ i ], i );
}
