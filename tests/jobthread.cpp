#include "common/assert.h"
#include "common/macros.h"

#include <tier1/callqueue.h>
#include <tier1/functors.h>
#include <tier1/jobthread.h>
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
