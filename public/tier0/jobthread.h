//========== Copyright © 2005, Valve Corporation, All rights reserved. ========
//
// Purpose:	A pool of worker threads (IThreadPool) running queued jobs
//			(CThreadedJob).
//
//=============================================================================

#ifndef JOBTHREAD_H
#define JOBTHREAD_H

#if defined( _WIN32 )
#pragma once
#endif

#include <limits.h>
#include <functional>
#include <iterator>
#include <type_traits>
#include <utility>
#include "tier0/platform.h"
#include "tier0/threadtools.h"
#include "tier0/strtools.h"
#include "tier0/refcount.h"
#include "tier0/smartptr.h"
#include "tier0/utlvector.h"
#include "tier0/functors.h"

#define JOB_INTERFACE	PLATFORM_INTERFACE
#define JOB_OVERLOAD	PLATFORM_OVERLOAD
#define JOB_CLASS		DLL_CLASS_IMPORT

#ifdef AddJob  // windows.h print function collisions
#undef AddJob
#undef GetJob
#endif

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------

class CThreadedJob;
class IThreadPool;
class CThreadMultiWaitEvent;
struct ThreadMultiWaitExtra_t;
struct ThreadMultiWaitObjectSet_t;
#ifdef POSIX
struct pollfd;
#endif

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
enum JobStatus_t
{
	JOB_OK,						// operation is successful
	JOB_STATUS_PENDING,			// properly queued, waiting for service
	JOB_STATUS_INPROGRESS,		// being serviced
	JOB_STATUS_ABORTED,			// aborted by caller
	JOB_STATUS_UNSERVICED,		// not yet queued
};

typedef JobStatus_t JobStatusEnum_t;

enum JobFlags_t
{
	JF_SERIAL		= ( 1 << 0 ),	// Job cannot be executed out of order relative to other "strict" jobs
	JF_QUEUE		= ( 1 << 1 ),	// Queue it
	JF_PER_FRAME	= ( 1 << 2 ),	// Added with AddPerFrameJob, YieldWaitPerFrameJobs waits for it
};

enum JobPriority_t
{
	JP_LOW,
	JP_NORMAL,
	JP_HIGH,
	JP_IMMEDIATE,

	JP_NUM_PRIORITIES
};

struct ThreadPoolStartParams_t
{
	int nCoreTypeRequest; // 0 performance, 1 efficiency, 2 split, 3 undifferentiated, 4 auto_threads, 6 max_threads
	int nThreads; // Negative to count the cores nCoreTypeRequest picks
	int nMaxThreads;
	int iThreadPriority;
	int nStackSize; // Negative for the default
	bool bExecOnThreadPoolThreadsOnly;
};

abstract_class IMultipleWorkerJob
{
public:
	virtual ~IMultipleWorkerJob() {}

	virtual void Execute( int nWorker ) = 0;
};

//-----------------------------------------------------------------------------
//
// IThreadPool
//
//-----------------------------------------------------------------------------

abstract_class IThreadPool : public IRefCounted
{
public:
	virtual ~IThreadPool() {};

	//-----------------------------------------------------
	// Thread functions
	//-----------------------------------------------------
	virtual bool Start( const ThreadPoolStartParams_t &startParams, const char *pszNameOverride = NULL ) = 0;
	virtual bool Stop() = 0;
	// Stops the threads and starts them again with new parameters under the current name; the
	// name is ours
	virtual void Restart( const ThreadPoolStartParams_t &startParams ) = 0;

	//-----------------------------------------------------
	// Functions for any thread
	//-----------------------------------------------------
	virtual int GetCoreTypeRequest() = 0;
	virtual int NumThreads() = 0;
	virtual int NumIdleThreads() = 0;
	virtual int GetCurrentThreadIndex() = 0;
	// 0 on the main thread, the thread index + 1 on a thread of this pool, otherwise
	// nOtherThreadSlot; the first argument is not read, the name is ours
	virtual int GetCurrentThreadSlot( int nUnused, int nOtherThreadSlot ) = 0;

	//-----------------------------------------------------
	// Pause/resume processing jobs
	//-----------------------------------------------------
	virtual int SuspendExecution() = 0;
	virtual int ResumeExecution() = 0;

	//-----------------------------------------------------
	// Offer the current thread to the pool
	//-----------------------------------------------------
#ifdef _WIN32
	virtual bool YieldWait( void *const *pHandles, const ThreadMultiWaitExtra_t *pExtra, int nObjects, bool bWaitAll, ThreadMultiWaitObjectSet_t **ppObjectSet ) = 0;
#else
	virtual bool YieldWait( const pollfd *pFds, const ThreadMultiWaitExtra_t *pExtra, int nObjects, bool bWaitAll, ThreadMultiWaitObjectSet_t **ppObjectSet ) = 0;
#endif
	virtual bool YieldWait( CThreadedJob **ppJobs, int nJobs, bool bWaitAll, bool bExecuteWaitedJobs ) = 0;
	virtual bool ExecuteOneJob() = 0;

	//-----------------------------------------------------
	// Add a native job to the queue (master thread)
	// Call YieldWaitPerFrameJobs() to wait only until all per-frame jobs are done
	//-----------------------------------------------------
	virtual void AddJob( CThreadedJob *pJob ) = 0;
	virtual void AddPerFrameJob( CThreadedJob *pJob ) = 0;
	virtual bool YieldWaitPerFrameJobs() = 0;

	virtual void ExecuteMultipleWorkerJob( IMultipleWorkerJob *pJob, int nMaxWorkers, const char *pszName, JobPriority_t priority ) = 0;
	virtual const char *GetName() = 0;
	virtual void AddFunctionJob( const std::function< void() > &func, CThreadedJob **ppJob, const char *pszDescription, unsigned int nFlags, JobPriority_t priority ) = 0;

	template < typename T >
	CSmartPtr< CThreadedJob, CRefCountAccessor > QueueJobWithFlags( const char *pszDescription, JobPriority_t priority, unsigned int nFlags, T &&func );

	bool YieldWait( CThreadedJob *pJob ) { return YieldWait( &pJob, 1, true, true ); }

	//-----------------------------------------------------
	// Add a function object to the queue (master thread)
	//-----------------------------------------------------
	void AddFunctor( CFunctor *pFunctor, CThreadedJob **ppJob = NULL, const char *pszDescription = NULL, unsigned nFlags = 0 ) { AddFunctorInternal( RetAddRef( pFunctor ), ppJob, pszDescription, nFlags ); }

	//-----------------------------------------------------
	// Add an arbitrary call to the queue (master thread)
	//
	// Avert thy eyes! Imagine rather:
	//
	// CThreadedJob *AddCall( <function>, [args1, [arg2,]...]
	// CThreadedJob *AddCall( <object>, <function>, [args1, [arg2,]...]
	// CThreadedJob *AddRefCall( <object>, <function>, [args1, [arg2,]...]
	// CThreadedJob *QueueCall( <function>, [args1, [arg2,]...]
	// CThreadedJob *QueueCall( <object>, <function>, [args1, [arg2,]...]
	//
	// The returned job holds a reference for the caller.
	// AddCall runs the call on this thread when no thread is idle.
	//-----------------------------------------------------

	#define DEFINE_NONMEMBER_ADD_CALL(N) \
		template <typename FUNCTION_RETTYPE FUNC_TEMPLATE_FUNC_PARAMS_##N FUNC_TEMPLATE_ARG_PARAMS_##N> \
		CThreadedJob *AddCall(FUNCTION_RETTYPE (*pfnProxied)( FUNC_BASE_TEMPLATE_FUNC_PARAMS_##N ) FUNC_ARG_FORMAL_PARAMS_##N ) \
		{ \
			CThreadedJob *pJob; \
			AddFunctorInternal( CreateFunctor( pfnProxied FUNC_FUNCTOR_CALL_ARGS_##N ), &pJob, NULL, 0, !NumIdleThreads() ); \
			return pJob; \
		}

	//-------------------------------------

	#define DEFINE_MEMBER_ADD_CALL(N) \
		template <typename OBJECT_TYPE, typename FUNCTION_CLASS, typename FUNCTION_RETTYPE FUNC_TEMPLATE_FUNC_PARAMS_##N FUNC_TEMPLATE_ARG_PARAMS_##N> \
		CThreadedJob *AddCall(OBJECT_TYPE *pObject, FUNCTION_RETTYPE ( FUNCTION_CLASS::*pfnProxied )( FUNC_BASE_TEMPLATE_FUNC_PARAMS_##N ) FUNC_ARG_FORMAL_PARAMS_##N ) \
		{ \
			CThreadedJob *pJob; \
			AddFunctorInternal( CreateFunctor( pObject, pfnProxied FUNC_FUNCTOR_CALL_ARGS_##N ), &pJob, NULL, 0, !NumIdleThreads() ); \
			return pJob; \
		}

	//-------------------------------------

	#define DEFINE_CONST_MEMBER_ADD_CALL(N) \
		template <typename OBJECT_TYPE, typename FUNCTION_CLASS, typename FUNCTION_RETTYPE FUNC_TEMPLATE_FUNC_PARAMS_##N FUNC_TEMPLATE_ARG_PARAMS_##N> \
		CThreadedJob *AddCall(OBJECT_TYPE *pObject, FUNCTION_RETTYPE ( FUNCTION_CLASS::*pfnProxied )( FUNC_BASE_TEMPLATE_FUNC_PARAMS_##N ) const FUNC_ARG_FORMAL_PARAMS_##N ) \
		{ \
			CThreadedJob *pJob; \
			AddFunctorInternal( CreateFunctor( pObject, pfnProxied FUNC_FUNCTOR_CALL_ARGS_##N ), &pJob, NULL, 0, !NumIdleThreads() ); \
			return pJob; \
		}

	//-------------------------------------

	#define DEFINE_REF_COUNTING_MEMBER_ADD_CALL(N) \
		template <typename OBJECT_TYPE, typename FUNCTION_CLASS, typename FUNCTION_RETTYPE FUNC_TEMPLATE_FUNC_PARAMS_##N FUNC_TEMPLATE_ARG_PARAMS_##N> \
		CThreadedJob *AddRefCall(OBJECT_TYPE *pObject, FUNCTION_RETTYPE ( FUNCTION_CLASS::*pfnProxied )( FUNC_BASE_TEMPLATE_FUNC_PARAMS_##N ) FUNC_ARG_FORMAL_PARAMS_##N ) \
		{ \
			CThreadedJob *pJob; \
			AddFunctorInternal( CreateRefCountingFunctor( pObject, pfnProxied FUNC_FUNCTOR_CALL_ARGS_##N ), &pJob, NULL, 0, !NumIdleThreads() ); \
			return pJob; \
		}

	//-------------------------------------

	#define DEFINE_REF_COUNTING_CONST_MEMBER_ADD_CALL(N) \
		template <typename OBJECT_TYPE, typename FUNCTION_CLASS, typename FUNCTION_RETTYPE FUNC_TEMPLATE_FUNC_PARAMS_##N FUNC_TEMPLATE_ARG_PARAMS_##N> \
		CThreadedJob *AddRefCall(OBJECT_TYPE *pObject, FUNCTION_RETTYPE ( FUNCTION_CLASS::*pfnProxied )( FUNC_BASE_TEMPLATE_FUNC_PARAMS_##N ) const FUNC_ARG_FORMAL_PARAMS_##N ) \
		{ \
			CThreadedJob *pJob; \
			AddFunctorInternal( CreateRefCountingFunctor( pObject, pfnProxied FUNC_FUNCTOR_CALL_ARGS_##N ), &pJob, NULL, 0, !NumIdleThreads() ); \
			return pJob; \
		}

	//-----------------------------------------------------------------------------

	#define DEFINE_NONMEMBER_QUEUE_CALL(N) \
		template <typename FUNCTION_RETTYPE FUNC_TEMPLATE_FUNC_PARAMS_##N FUNC_TEMPLATE_ARG_PARAMS_##N> \
		CThreadedJob *QueueCall(FUNCTION_RETTYPE (*pfnProxied)( FUNC_BASE_TEMPLATE_FUNC_PARAMS_##N ) FUNC_ARG_FORMAL_PARAMS_##N ) \
		{ \
			CThreadedJob *pJob; \
			AddFunctorInternal( CreateFunctor( pfnProxied FUNC_FUNCTOR_CALL_ARGS_##N ), &pJob ); \
			return pJob; \
		}

	//-------------------------------------

	#define DEFINE_MEMBER_QUEUE_CALL(N) \
		template <typename OBJECT_TYPE, typename FUNCTION_CLASS, typename FUNCTION_RETTYPE FUNC_TEMPLATE_FUNC_PARAMS_##N FUNC_TEMPLATE_ARG_PARAMS_##N> \
		CThreadedJob *QueueCall(OBJECT_TYPE *pObject, FUNCTION_RETTYPE ( FUNCTION_CLASS::*pfnProxied )( FUNC_BASE_TEMPLATE_FUNC_PARAMS_##N ) FUNC_ARG_FORMAL_PARAMS_##N ) \
		{ \
			CThreadedJob *pJob; \
			AddFunctorInternal( CreateFunctor( pObject, pfnProxied FUNC_FUNCTOR_CALL_ARGS_##N ), &pJob ); \
			return pJob; \
		}

	//-------------------------------------

	#define DEFINE_CONST_MEMBER_QUEUE_CALL(N) \
		template <typename OBJECT_TYPE, typename FUNCTION_CLASS, typename FUNCTION_RETTYPE FUNC_TEMPLATE_FUNC_PARAMS_##N FUNC_TEMPLATE_ARG_PARAMS_##N> \
		CThreadedJob *QueueCall(OBJECT_TYPE *pObject, FUNCTION_RETTYPE ( FUNCTION_CLASS::*pfnProxied )( FUNC_BASE_TEMPLATE_FUNC_PARAMS_##N ) const FUNC_ARG_FORMAL_PARAMS_##N ) \
		{ \
			CThreadedJob *pJob; \
			AddFunctorInternal( CreateFunctor( pObject, pfnProxied FUNC_FUNCTOR_CALL_ARGS_##N ), &pJob ); \
			return pJob; \
		}

	//-------------------------------------

	#define DEFINE_REF_COUNTING_MEMBER_QUEUE_CALL(N) \
		template <typename OBJECT_TYPE, typename FUNCTION_CLASS, typename FUNCTION_RETTYPE FUNC_TEMPLATE_FUNC_PARAMS_##N FUNC_TEMPLATE_ARG_PARAMS_##N> \
		CThreadedJob *QueueRefCall(OBJECT_TYPE *pObject, FUNCTION_RETTYPE ( FUNCTION_CLASS::*pfnProxied )( FUNC_BASE_TEMPLATE_FUNC_PARAMS_##N ) FUNC_ARG_FORMAL_PARAMS_##N ) \
		{ \
			CThreadedJob *pJob; \
			AddFunctorInternal( CreateRefCountingFunctor( pObject, pfnProxied FUNC_FUNCTOR_CALL_ARGS_##N ), &pJob ); \
			return pJob; \
		}

	//-------------------------------------

	#define DEFINE_REF_COUNTING_CONST_MEMBER_QUEUE_CALL(N) \
		template <typename OBJECT_TYPE, typename FUNCTION_CLASS, typename FUNCTION_RETTYPE FUNC_TEMPLATE_FUNC_PARAMS_##N FUNC_TEMPLATE_ARG_PARAMS_##N> \
		CThreadedJob *QueueRefCall(OBJECT_TYPE *pObject, FUNCTION_RETTYPE ( FUNCTION_CLASS::*pfnProxied )( FUNC_BASE_TEMPLATE_FUNC_PARAMS_##N ) const FUNC_ARG_FORMAL_PARAMS_##N ) \
		{ \
			CThreadedJob *pJob; \
			AddFunctorInternal( CreateRefCountingFunctor( pObject, pfnProxied FUNC_FUNCTOR_CALL_ARGS_##N ), &pJob ); \
			return pJob; \
		}

	FUNC_GENERATE_ALL( DEFINE_NONMEMBER_ADD_CALL );
	FUNC_GENERATE_ALL( DEFINE_MEMBER_ADD_CALL );
	FUNC_GENERATE_ALL( DEFINE_CONST_MEMBER_ADD_CALL );
	FUNC_GENERATE_ALL( DEFINE_REF_COUNTING_MEMBER_ADD_CALL );
	FUNC_GENERATE_ALL( DEFINE_REF_COUNTING_CONST_MEMBER_ADD_CALL );
	FUNC_GENERATE_ALL( DEFINE_NONMEMBER_QUEUE_CALL );
	FUNC_GENERATE_ALL( DEFINE_MEMBER_QUEUE_CALL );
	FUNC_GENERATE_ALL( DEFINE_CONST_MEMBER_QUEUE_CALL );
	FUNC_GENERATE_ALL( DEFINE_REF_COUNTING_MEMBER_QUEUE_CALL );
	FUNC_GENERATE_ALL( DEFINE_REF_COUNTING_CONST_MEMBER_QUEUE_CALL );

	#undef DEFINE_NONMEMBER_ADD_CALL
	#undef DEFINE_MEMBER_ADD_CALL
	#undef DEFINE_CONST_MEMBER_ADD_CALL
	#undef DEFINE_REF_COUNTING_MEMBER_ADD_CALL
	#undef DEFINE_REF_COUNTING_CONST_MEMBER_ADD_CALL
	#undef DEFINE_NONMEMBER_QUEUE_CALL
	#undef DEFINE_MEMBER_QUEUE_CALL
	#undef DEFINE_CONST_MEMBER_QUEUE_CALL
	#undef DEFINE_REF_COUNTING_MEMBER_QUEUE_CALL
	#undef DEFINE_REF_COUNTING_CONST_MEMBER_QUEUE_CALL

private:
	// Takes over the functor reference, runs the job on this thread when bExecuteNow is set
	void AddFunctorInternal( CFunctor *pFunctor, CThreadedJob **ppJob = NULL, const char *pszDescription = NULL, unsigned nFlags = 0, bool bExecuteNow = false );
};

//-----------------------------------------------------------------------------

PLATFORM_INTERFACE IThreadPool *CreateNewThreadPool();
PLATFORM_INTERFACE void DestroyThreadPool( IThreadPool *pPool );

PLATFORM_INTERFACE CThreadMultiWaitEvent *GetJobManualEventFromPool();
PLATFORM_INTERFACE void ReturnJobManualEventToPool( CThreadMultiWaitEvent *pEvent );

PLATFORM_INTERFACE void ParallelForSharedCode( int nBegin, int nEnd, const char *pszName, const std::function< void( int, int ) > &func, int nChunkSize, int nMaxThreads, JobPriority_t priority );

//-------------------------------------

PLATFORM_INTERFACE void RunThreadPoolTests( int nJobs );
PLATFORM_INTERFACE void RunThreadPoolPerFrameTests();

//-----------------------------------------------------------------------------

PLATFORM_INTERFACE IThreadPool *g_pThreadPool;
PLATFORM_INTERFACE double g_flThreadedJobWaitTimeAccumulator;

// Waits for the unfinished jobs, with one YieldWait of each thread pool for its jobs (g_pThreadPool
// for the jobs without a pool); bExecuteWaitedJobs runs the queued ones in this thread
PLATFORM_INTERFACE void Wait( const CUtlVector< CThreadedJob * > &jobs, bool bExecuteWaitedJobs );

//-----------------------------------------------------------------------------
// Class to combine the metadata for an operation and the ability to perform
// the operation. Meant for inheritance.
//-----------------------------------------------------------------------------

class CThreadedJob : public CRefCounted1< IRefCounted, CRefCountServiceBase< CRefMT > >
{
public:
	CThreadedJob( JobPriority_t priority = JP_HIGH ) : m_nStatus( JOB_STATUS_UNSERVICED ), m_nFlags( 0 ), m_nPriority( ( uint8 )priority ), m_bHasJobName( false ), m_pThreadPool( NULL ), m_pJobEvent( NULL ), m_pWaitCounter( NULL )
	{
		m_szJobName[ 0 ] = '\0';
	}

	virtual ~CThreadedJob()
	{
		if ( m_pJobEvent )
		{
			ReturnJobManualEventToPool( m_pJobEvent );
			m_pJobEvent = NULL;
		}

		m_pWaitCounter = NULL;
	}

	//-----------------------------------------------------
	// Priority (not thread safe)
	//-----------------------------------------------------
	void SetPriority( JobPriority_t priority ) { m_nPriority = ( uint8 )priority; }
	JobPriority_t GetPriority() const { return ( JobPriority_t )m_nPriority; }

	//-----------------------------------------------------

	void SetFlags( unsigned nFlags ) { m_nFlags = ( uint8 )nFlags; }
	unsigned GetFlags() const { return m_nFlags; }

	//-----------------------------------------------------
	// Fast queries
	//-----------------------------------------------------
	bool Executed() const { return ( m_nStatus == JOB_OK ); }
	bool CanExecute() const { return ( m_nStatus == JOB_STATUS_PENDING || m_nStatus == JOB_STATUS_UNSERVICED ); }
	bool IsFinished() const { return ( m_nStatus != JOB_STATUS_PENDING && m_nStatus != JOB_STATUS_INPROGRESS && m_nStatus != JOB_STATUS_UNSERVICED ); }
	JobStatus_t GetStatus() const { return m_nStatus; }

	DLL_CLASS_IMPORT void WaitOrImmediatelyExecute();

	//-----------------------------------------------------
	// Offer the current thread to the pool until the job finishes
	//-----------------------------------------------------
	bool WaitForFinish() { return IsFinished() || ( m_pThreadPool ? m_pThreadPool : g_pThreadPool )->YieldWait( this ); }
	bool WaitForFinishAndRelease() { bool bResult = WaitForFinish(); Release(); return bResult; }

	//-----------------------------------------------------
	// Execute the job on this thread, or wait for the thread executing it
	//-----------------------------------------------------
	JobStatus_t Execute()
	{
		if ( IsFinished() )
		{
			return m_nStatus;
		}

		AddRef();

		WaitOrImmediatelyExecute();
		JobStatus_t result = m_nStatus;

		Release();

		return result;
	}

	JobStatus_t ExecuteAndRelease()
	{
		JobStatus_t status = Execute();
		Release();
		return status;
	}

	//-----------------------------------------------------
	// Execute the job on this thread if it's queued and no thread started it
	//-----------------------------------------------------
	JobStatus_t TryExecute()
	{
		JobStatus_t status = ( JobStatus_t )ThreadInterlockedCompareExchange( ( int32 volatile * )&m_nStatus, JOB_STATUS_INPROGRESS, JOB_STATUS_PENDING );

		if ( status != JOB_STATUS_PENDING )
			return status;

		DoExecute();
		DoCleanup();
		ThreadInterlockedExchange( ( int32 volatile * )&m_nStatus, JOB_OK );

		if ( m_pWaitCounter )
		{
			ThreadInterlockedIncrement( ( int32 volatile * )m_pWaitCounter );
			ThreadAtomicNotifyAll( ( const uint32 * )m_pWaitCounter );
		}

		if ( m_pJobEvent )
			m_pJobEvent->Set();

		return JOB_OK;
	}

	JobStatus_t TryExecuteAndRelease()
	{
		JobStatus_t status = TryExecute();
		Release();
		return status;
	}

	//-----------------------------------------------------
	// Terminate the job, discard if partially or wholly fulfilled
	//-----------------------------------------------------
	DLL_CLASS_IMPORT JobStatus_t Abort( bool bDiscard = true );

	const char *Describe() { return GetJobName(); }

	// Falls back to a literal when the job is unnamed
	virtual const char *GetJobName() { return m_szJobName[ 0 ] ? m_szJobName : "Job"; }

	virtual void SetJobName( const char *pszName )
	{
		if ( pszName )
		{
			V_strncpy( m_szJobName, pszName, sizeof( m_szJobName ) );
			m_bHasJobName = true;
		}
		else
		{
			m_szJobName[ 0 ] = '\0';
		}
	}

private:
	friend class IThreadPool;

	//-----------------------------------------------------
	CThreadedJob( const CThreadedJob &fromRequest );
	void operator=( const CThreadedJob &fromRequest );

	virtual void DoExecute() = 0;
	virtual void DoCleanup() {}

	JobStatus_t m_nStatus;
	uint8 m_nFlags; // 4 for a per-frame job
	uint8 m_nPriority; // JobPriority_t
	bool m_bHasJobName;
	IThreadPool *m_pThreadPool; // nullptr selects the global g_pThreadPool
	CThreadMultiWaitEvent *m_pJobEvent; // Pooled manual event, released via ReturnJobManualEventToPool
	uint32 volatile *m_pWaitCounter; // Waiters sleep on it, the job increments it and wakes them when it finishes
	char m_szJobName[ 32 ];
};

//-----------------------------------------------------------------------------

template < typename T >
inline CSmartPtr< CThreadedJob, CRefCountAccessor > IThreadPool::QueueJobWithFlags( const char *pszDescription, JobPriority_t priority, unsigned int nFlags, T &&func )
{
	class CLambdaJob : public CThreadedJob
	{
	public:
		CLambdaJob( T &&func, const char *pszDescription, JobPriority_t priority, unsigned int nFlags ) : CThreadedJob( priority ), m_func( std::forward< T >( func ) )
		{
			SetFlags( nFlags );
			SetJobName( pszDescription );
		}

	private:
		virtual void DoExecute() { m_func(); }

		T m_func;
	};

	CThreadedJob *pJob = new CLambdaJob( std::forward< T >( func ), pszDescription, priority, nFlags );
	pJob->m_pThreadPool = this;
	AddJob( pJob );

	CSmartPtr< CThreadedJob, CRefCountAccessor > pResult( pJob );
	pJob->Release();
	return pResult;
}

//-----------------------------------------------------------------------------

class CFunctorJob : public CThreadedJob
{
public:
	// Takes over the functor reference
	CFunctorJob( CFunctor *pFunctor, const char *pszDescription = NULL, JobPriority_t priority = JP_HIGH ) : CThreadedJob( priority ), m_pFunctor( pFunctor )
	{
		SetJobName( pszDescription );
	}

private:
	virtual void DoExecute() { ( *m_pFunctor )(); }

	CRefPtr< CFunctor > m_pFunctor;
};

inline void IThreadPool::AddFunctorInternal( CFunctor *pFunctor, CThreadedJob **ppJob, const char *pszDescription, unsigned nFlags, bool bExecuteNow )
{
	CThreadedJob *pJob = new CFunctorJob( pFunctor, pszDescription );
	pJob->SetFlags( nFlags );
	pJob->m_pThreadPool = this;

	if ( bExecuteNow )
	{
		pJob->m_nStatus = JOB_STATUS_PENDING;
		pJob->TryExecute();
	}
	else
	{
		AddJob( pJob );
	}

	if ( ppJob )
		*ppJob = pJob;
	else
		pJob->Release();
}

//-----------------------------------------------------------------------------
// Utility for managing multiple jobs
//-----------------------------------------------------------------------------

class CJobSet
{
public:
	CJobSet( CThreadedJob *pJob = NULL )
	{
		if ( pJob )
		{
			m_jobs.AddToTail( pJob );
		}
	}

	CJobSet( CThreadedJob **ppJobs, int nJobs )
	{
		if ( ppJobs )
		{
			m_jobs.AddMultipleToTail( nJobs, ppJobs );
		}
	}

	~CJobSet()
	{
		for ( int i = 0; i < m_jobs.Count(); i++ )
		{
			m_jobs[ i ]->Release();
		}
	}

	void operator+=( CThreadedJob *pJob )
	{
		m_jobs.AddToTail( pJob );
	}

	void operator-=( CThreadedJob *pJob )
	{
		m_jobs.FindAndRemove( pJob );
	}

	void Execute( bool bRelease = true )
	{
		for ( int i = 0; i < m_jobs.Count(); i++ )
		{
			m_jobs[ i ]->Execute();
			if ( bRelease )
			{
				m_jobs[ i ]->Release();
			}
		}

		if ( bRelease )
		{
			m_jobs.RemoveAll();
		}
	}

	void Abort( bool bRelease = true )
	{
		for ( int i = 0; i < m_jobs.Count(); i++ )
		{
			m_jobs[ i ]->Abort();
			if ( bRelease )
			{
				m_jobs[ i ]->Release();
			}
		}

		if ( bRelease )
		{
			m_jobs.RemoveAll();
		}
	}

	void WaitForFinish( bool bRelease = true )
	{
		for ( int i = 0; i < m_jobs.Count(); i++ )
		{
			m_jobs[ i ]->WaitForFinish();
			if ( bRelease )
			{
				m_jobs[ i ]->Release();
			}
		}

		if ( bRelease )
		{
			m_jobs.RemoveAll();
		}
	}

	void WaitForFinish( IThreadPool *pPool, bool bRelease = true )
	{
		pPool->YieldWait( m_jobs.Base(), m_jobs.Count(), true, true );

		if ( bRelease )
		{
			for ( int i = 0; i < m_jobs.Count(); i++ )
			{
				m_jobs[ i ]->Release();
			}

			m_jobs.RemoveAll();
		}
	}

private:
	CUtlVectorFixed< CThreadedJob *, 16 > m_jobs;
};

//-----------------------------------------------------------------------------
// Job helpers
//-----------------------------------------------------------------------------

#define ThreadExecute g_pThreadPool->QueueCall
#define ThreadExecuteRef g_pThreadPool->QueueRefCall

#define BeginExecuteParallel()	do { CJobSet jobSet
#define EndExecuteParallel()	jobSet.WaitForFinish( g_pThreadPool ); } while (0)

#define ExecuteParallel			jobSet += g_pThreadPool->QueueCall
#define ExecuteRefParallel		jobSet += g_pThreadPool->QueueRefCall

//-----------------------------------------------------------------------------
// Work splitting: array split, best when cost per item is roughly equal
//-----------------------------------------------------------------------------

#define DEFINE_NON_MEMBER_ITER_RANGE_PARALLEL(N) \
	template <typename FUNCTION_RETTYPE FUNC_TEMPLATE_FUNC_PARAMS_##N FUNC_TEMPLATE_ARG_PARAMS_##N, typename ITERTYPE1, typename ITERTYPE2> \
	void IterRangeParallel(FUNCTION_RETTYPE ( *pfnProxied )( ITERTYPE1, ITERTYPE2 FUNC_SEPARATOR_##N FUNC_BASE_TEMPLATE_FUNC_PARAMS_##N ), ITERTYPE1 from, ITERTYPE2 to FUNC_ARG_FORMAL_PARAMS_##N ) \
	{ \
		const int MAX_THREADS = 16; \
		int nIdle = g_pThreadPool->NumIdleThreads(); \
		ITERTYPE1 range = to - from; \
		int nThreads = MIN( nIdle + 1, ( int )range ); \
		if ( nThreads > MAX_THREADS ) \
		{ \
			nThreads = MAX_THREADS; \
		} \
		if ( nThreads < 2 ) \
		{ \
			FunctorDirectCall( pfnProxied, from, to FUNC_FUNCTOR_CALL_ARGS_##N ); \
		} \
		else \
		{ \
			ITERTYPE1 nIncrement = range / nThreads; \
			\
			CJobSet jobSet; \
			while ( --nThreads ) \
			{ \
				ITERTYPE2 thisTo = from + nIncrement; \
				jobSet += g_pThreadPool->AddCall( pfnProxied, from, thisTo FUNC_FUNCTOR_CALL_ARGS_##N ); \
				from = thisTo; \
			} \
			FunctorDirectCall( pfnProxied, from, to FUNC_FUNCTOR_CALL_ARGS_##N ); \
			jobSet.WaitForFinish( g_pThreadPool ); \
		} \
	}

FUNC_GENERATE_ALL( DEFINE_NON_MEMBER_ITER_RANGE_PARALLEL );

#define DEFINE_MEMBER_ITER_RANGE_PARALLEL(N) \
	template <typename OBJECT_TYPE, typename FUNCTION_CLASS, typename FUNCTION_RETTYPE FUNC_TEMPLATE_FUNC_PARAMS_##N FUNC_TEMPLATE_ARG_PARAMS_##N, typename ITERTYPE1, typename ITERTYPE2> \
	void IterRangeParallel(OBJECT_TYPE *pObject, FUNCTION_RETTYPE ( FUNCTION_CLASS::*pfnProxied )( ITERTYPE1, ITERTYPE2 FUNC_SEPARATOR_##N FUNC_BASE_TEMPLATE_FUNC_PARAMS_##N ), ITERTYPE1 from, ITERTYPE2 to FUNC_ARG_FORMAL_PARAMS_##N ) \
	{ \
		const int MAX_THREADS = 16; \
		int nIdle = g_pThreadPool->NumIdleThreads(); \
		ITERTYPE1 range = to - from; \
		int nThreads = MIN( nIdle + 1, ( int )range ); \
		if ( nThreads > MAX_THREADS ) \
		{ \
			nThreads = MAX_THREADS; \
		} \
		if ( nThreads < 2 ) \
		{ \
			FunctorDirectCall( pObject, pfnProxied, from, to FUNC_FUNCTOR_CALL_ARGS_##N ); \
		} \
		else \
		{ \
			ITERTYPE1 nIncrement = range / nThreads; \
			\
			CJobSet jobSet; \
			while ( --nThreads ) \
			{ \
				ITERTYPE2 thisTo = from + nIncrement; \
				jobSet += g_pThreadPool->AddCall( pObject, pfnProxied, from, thisTo FUNC_FUNCTOR_CALL_ARGS_##N ); \
				from = thisTo; \
			} \
			FunctorDirectCall( pObject, pfnProxied, from, to FUNC_FUNCTOR_CALL_ARGS_##N ); \
			jobSet.WaitForFinish( g_pThreadPool ); \
		} \
	}

FUNC_GENERATE_ALL( DEFINE_MEMBER_ITER_RANGE_PARALLEL );

//-----------------------------------------------------------------------------
// A job that is queued once all of its upstream jobs have finished
//-----------------------------------------------------------------------------

class CThreadedJobWithDependencies;

class CThreadedDependentJob : public CThreadedJob
{
public:
	DLL_CLASS_IMPORT void ReleaseUpstreamDependency();
	DLL_CLASS_IMPORT void Wait();

	// Default result; concrete jobs override to match a specific type id
	virtual bool IsJobType( uint32 nJobType ) { return false; }

protected:
	CUtlVectorFixedGrowable< CThreadedJobWithDependencies *, 4 > m_UpstreamJobs;
	CInterlockedInt m_nPendingUpstreamJobs;
};

class CThreadedJobWithDependencies : public CThreadedDependentJob
{
public:
	// Called on each job before Start
	virtual void BeforeJobRuns() {}

protected:
	virtual void DoExecute() { DoExecuteInternal(); }
	virtual void DoCleanup()
	{
		m_UpstreamJobs.RemoveAll();
		m_nPendingUpstreamJobs = 0;
		m_DownstreamJobs.RemoveAll();
	}

	CUtlVectorFixedGrowable< CThreadedDependentJob *, 4 > m_DownstreamJobs;

private:
	DLL_CLASS_IMPORT void DoExecuteInternal();
};

// Queues the jobs without upstream jobs, the rest are queued as their upstream jobs finish
PLATFORM_INTERFACE void Start( CUtlVector< CThreadedJobWithDependencies * > &jobs );

// Runs the jobs on this thread once their upstream jobs finish, removing them from jobs
PLATFORM_INTERFACE void RunSync( CUtlVector< CThreadedJobWithDependencies * > &jobs );

//-----------------------------------------------------------------------------
// Runs a function for every index or element, split between the calling
// thread and the pool's threads
//-----------------------------------------------------------------------------

class CParallelProcessLauncher
{
public:
	template < typename F >
	class CParallelLambdaJob : public IMultipleWorkerJob
	{
	public:
		CParallelLambdaJob( F &func ) : m_pFunc( &func ) {}

		virtual void Execute( int nWorker ) { ( *m_pFunc )( nWorker ); }

	private:
		F *m_pFunc;
	};
};

// The engine's ParallelFor also has a bool template parameter, only ever true
template < typename F >
inline void ParallelFor( int nBegin, int nEnd, const char *pszName, F &&func, int nChunkSize, int nMaxThreads, JobPriority_t priority )
{
	int nCount = nEnd - nBegin;

	if ( nCount <= 0 )
		return;

	int nThreads = MIN( nMaxThreads, g_pThreadPool->NumThreads() + 1 );

	if ( nChunkSize <= 0 )
		nChunkSize = MAX( 1, nCount / ( 2 * nThreads ) );

	int nWorkers = MIN( nThreads, ( nCount + nChunkSize - 1 ) / nChunkSize );

	if ( nWorkers <= 1 )
	{
		for ( int i = nBegin; i < nEnd; i++ )
			func( i );

		return;
	}

	CInterlockedInt nNext = nBegin;
	auto worker = [ & ]( int nWorker )
	{
		for ( ;; )
		{
			int nStart = nNext.AtomicAdd( nChunkSize );

			if ( nStart >= nEnd )
				return;

			int nStop = MIN( nStart + nChunkSize, nEnd );

			for ( int i = nStart; i < nStop; i++ )
				func( i );
		}
	};

	CParallelProcessLauncher::CParallelLambdaJob< decltype( worker ) > job( worker );
	g_pThreadPool->ExecuteMultipleWorkerJob( &job, nWorkers, pszName, priority );
}

// How the engine handles a chunk size of 0 or less is unknown
template < typename C, typename F >
inline void ParallelForEach( C &&container, const char *pszName, F &&func, int nChunkSize, int nMaxThreads, JobPriority_t priority )
{
	using std::begin;
	using std::end;

	int nCount = end( container ) - begin( container );

	if ( nCount <= 0 )
		return;

	auto *pBegin = &*begin( container );
	auto *pEnd = pBegin + nCount;

	nChunkSize = MAX( nChunkSize, 1 );

	int nWorkers = MIN( MIN( ( nCount + nChunkSize - 1 ) / nChunkSize, g_pThreadPool->NumThreads() + 1 ), nMaxThreads );

	CInterlockedPtr< typename std::remove_reference< decltype( *pBegin ) >::type > pNext;
	pNext = pBegin;
	auto worker = [ & ]( int nWorker )
	{
		for ( ;; )
		{
			auto *pStart = pNext.AtomicAdd( nChunkSize );

			if ( pStart >= pEnd )
				return;

			auto *pStop = MIN( pStart + nChunkSize, pEnd );

			for ( auto *pItem = pStart; pItem < pStop; pItem++ )
				func( *pItem );
		}
	};

	CParallelProcessLauncher::CParallelLambdaJob< decltype( worker ) > job( worker );
	g_pThreadPool->ExecuteMultipleWorkerJob( &job, nWorkers, pszName, priority );
}

//-----------------------------------------------------------------------------
// Runs a controller on the pool as one IMultipleWorkerJob
// The controller's OnBegin, OnExecute and OnEnd names are made up, the engine inlines them
//-----------------------------------------------------------------------------

class CParallelProcessorAbstract_Base
{
protected:
	CParallelProcessorAbstract_Base( IThreadPool *pThreadPool )
	{
		m_pThreadPool = pThreadPool ? pThreadPool : g_pThreadPool;
		m_nThreads = 0;

		if ( m_pThreadPool )
		{
			m_nThreads = m_pThreadPool->NumThreads();

			// A pool thread running this is busy with it
			if ( m_pThreadPool->GetCurrentThreadIndex() >= 0 )
				m_nThreads--;

			if ( m_nThreads < 0 )
				m_nThreads = 0;
		}
	}

	IThreadPool *m_pThreadPool;
	int m_nThreads;
};

template < class CONTROLLER_TYPE >
class CParallelProcessorAbstract : public CParallelProcessorAbstract_Base, private IMultipleWorkerJob
{
public:
	CParallelProcessorAbstract( CONTROLLER_TYPE *pController, IThreadPool *pThreadPool = NULL ) : CParallelProcessorAbstract_Base( pThreadPool ), m_pController( pController )
	{
	}

	void Run( const char *pszName )
	{
		int nMaxWorkers = m_nThreads + 1 + ( ThreadInMainThread() ? 0 : 1 );
		int nWorkers = m_pController->OnBegin( nMaxWorkers );
		m_pThreadPool->ExecuteMultipleWorkerJob( this, nWorkers, pszName, JP_HIGH );
		m_pController->OnEnd();
	}

private:
	virtual void Execute( int nWorker )
	{
		m_pController->OnExecute( nWorker );
	}

	CONTROLLER_TYPE *m_pController;
};

//-----------------------------------------------------------------------------
// Work splitting: competitive, best when cost per item varies a lot
//-----------------------------------------------------------------------------

template < typename T >
class CJobItemProcessor
{
public:
	typedef T ItemType_t;
	void Begin() {}
	// void Process( ItemType_t & ) {}
	void End() {}
};

template < typename T >
class CFuncJobItemProcessor : public CJobItemProcessor< T >
{
public:
	void Init( void ( *pfnProcess )( T & ), void ( *pfnBegin )() = NULL, void ( *pfnEnd )() = NULL )
	{
		m_pfnProcess = pfnProcess;
		m_pfnBegin = pfnBegin;
		m_pfnEnd = pfnEnd;
	}

	void Begin() { if ( m_pfnBegin ) ( *m_pfnBegin )(); }
	void Process( T &item ) { ( *m_pfnProcess )( item ); }
	void End() { if ( m_pfnEnd ) ( *m_pfnEnd )(); }

protected:
	void ( *m_pfnProcess )( T & );
	void ( *m_pfnBegin )();
	void ( *m_pfnEnd )();
};

template < typename T, class OBJECT_TYPE, class FUNCTION_CLASS = OBJECT_TYPE >
class CMemberFuncJobItemProcessor : public CJobItemProcessor< T >
{
public:
	void Init( OBJECT_TYPE *pObject, void ( FUNCTION_CLASS::*pfnProcess )( T & ), void ( FUNCTION_CLASS::*pfnBegin )() = NULL, void ( FUNCTION_CLASS::*pfnEnd )() = NULL )
	{
		m_pObject = pObject;
		m_pfnProcess = pfnProcess;
		m_pfnBegin = pfnBegin;
		m_pfnEnd = pfnEnd;
	}

	void Begin() { if ( m_pfnBegin ) ( ( *m_pObject ).*m_pfnBegin )(); }
	void Process( T &item ) { ( ( *m_pObject ).*m_pfnProcess )( item ); }
	void End() { if ( m_pfnEnd ) ( ( *m_pObject ).*m_pfnEnd )(); }

protected:
	OBJECT_TYPE *m_pObject;

	void ( FUNCTION_CLASS::*m_pfnProcess )( T & );
	void ( FUNCTION_CLASS::*m_pfnBegin )();
	void ( FUNCTION_CLASS::*m_pfnEnd )();
};

template < typename T >
class CLoopFuncJobItemProcessor : public CJobItemProcessor< T >
{
public:
	void Init( void ( *pfnProcess )( T *, int, int ), void ( *pfnBegin )() = NULL, void ( *pfnEnd )() = NULL )
	{
		m_pfnProcess = pfnProcess;
		m_pfnBegin = pfnBegin;
		m_pfnEnd = pfnEnd;
	}

	void Begin() { if ( m_pfnBegin ) ( *m_pfnBegin )(); }
	void Process( T *pContext, int nFirst, int nCount ) { ( *m_pfnProcess )( pContext, nFirst, nCount ); }
	void End() { if ( m_pfnEnd ) ( *m_pfnEnd )(); }

protected:
	void ( *m_pfnProcess )( T *, int, int );
	void ( *m_pfnBegin )();
	void ( *m_pfnEnd )();
};

template < typename T, class OBJECT_TYPE, class FUNCTION_CLASS = OBJECT_TYPE >
class CLoopMemberFuncJobItemProcessor : public CJobItemProcessor< T >
{
public:
	void Init( OBJECT_TYPE *pObject, void ( FUNCTION_CLASS::*pfnProcess )( T *, int, int ), void ( FUNCTION_CLASS::*pfnBegin )() = NULL, void ( FUNCTION_CLASS::*pfnEnd )() = NULL )
	{
		m_pObject = pObject;
		m_pfnProcess = pfnProcess;
		m_pfnBegin = pfnBegin;
		m_pfnEnd = pfnEnd;
	}

	void Begin() { if ( m_pfnBegin ) ( ( *m_pObject ).*m_pfnBegin )(); }
	void Process( T *pContext, int nFirst, int nCount ) { ( ( *m_pObject ).*m_pfnProcess )( pContext, nFirst, nCount ); }
	void End() { if ( m_pfnEnd ) ( ( *m_pObject ).*m_pfnEnd )(); }

protected:
	OBJECT_TYPE *m_pObject;

	void ( FUNCTION_CLASS::*m_pfnProcess )( T *, int, int );
	void ( FUNCTION_CLASS::*m_pfnBegin )();
	void ( FUNCTION_CLASS::*m_pfnEnd )();
};

// Begin and End run once on the calling thread, around all the Process calls
template < typename ITEM_TYPE, class ITEM_PROCESSOR_TYPE, int ID_TO_PREVENT_COMDATS_IN_PROFILES = 1 >
class CParallelProcessor
{
public:
	CParallelProcessor( const char *pszName ) : m_pszName( pszName ), m_nMaxParallel( INT_MAX ), m_bShrinkChunks( false )
	{
	}

	// A chunk size of 0 or less picks one from the item and worker counts
	void Run( ITEM_TYPE *pItems, unsigned nItems, int nChunkSize, int nMaxParallel = INT_MAX, IThreadPool *pThreadPool = NULL )
	{
		if ( nItems == 0 )
			return;

		m_pItems = pItems;
		m_nChunkSize = nChunkSize;
		m_pLimit = pItems + nItems;
		m_nMaxParallel = ( nMaxParallel > 0 ) ? nMaxParallel : INT_MAX;

		CParallelProcessorAbstract< CParallelProcessor > processor( this, pThreadPool );
		processor.Run( m_pszName );
	}

	ITEM_PROCESSOR_TYPE m_ItemProcessor;

private:
	friend class CParallelProcessorAbstract< CParallelProcessor >;

	int OnBegin( int nMaxWorkers )
	{
		int nItems = m_pLimit - m_pItems;
		int nChunkSize = MAX( m_nChunkSize, 1 );
		int nJobs = MIN( ( nItems + nChunkSize - 1 ) / nChunkSize, m_nMaxParallel );
		nJobs = MIN( MAX( nJobs, 0 ), nMaxWorkers );

		if ( nMaxWorkers <= 1 )
		{
			m_nChunkSize = nItems;
		}
		else if ( m_nChunkSize <= 0 )
		{
			// Aim for 16 chunks per worker
			m_nChunkSize = ( nItems + 16 * nMaxWorkers - 1 ) / ( 16 * nMaxWorkers );

			if ( m_nChunkSize <= 1 )
				m_nChunkSize = 1;
			else
				m_bShrinkChunks = true;
		}

		m_ItemProcessor.Begin();
		return nJobs;
	}

	void OnExecute( int nWorker )
	{
		if ( m_pItems >= m_pLimit )
			return;

		++m_nActive;

		ITEM_TYPE *pLimit = m_pLimit;
		int nChunkSize = m_nChunkSize;

		for ( ;; )
		{
			ITEM_TYPE *pCurrent;

			if ( m_bShrinkChunks && nChunkSize > 1 )
			{
				ITEM_TYPE *pNext = m_pItems;

				if ( pNext >= pLimit )
					break;

				// Split the last chunks between the workers still running
				int nActive = m_nActive;

				if ( pNext >= m_pLimit - nChunkSize * nActive )
					nChunkSize = ( nActive + ( pLimit - pNext ) - 1 ) / nActive;

				pCurrent = m_pItems.AtomicAdd( nChunkSize );
			}
			else
			{
				pCurrent = m_pItems.AtomicAdd( nChunkSize );

				if ( pCurrent >= pLimit )
					break;
			}

			ITEM_TYPE *pLast = MIN( pLimit, pCurrent + nChunkSize );

			while ( pCurrent < pLast )
			{
				m_ItemProcessor.Process( *pCurrent );
				pCurrent++;
			}
		}

		--m_nActive;
	}

	void OnEnd()
	{
		m_ItemProcessor.End();
	}

	ITEM_TYPE *m_pLimit;
	int m_nChunkSize;
	const char *m_pszName;
	int m_nMaxParallel;
	bool m_bShrinkChunks;
	alignas( 64 ) CInterlockedInt m_nActive;
	alignas( 64 ) CInterlockedPtr< ITEM_TYPE > m_pItems;
};

template < typename ITEM_TYPE >
inline void ParallelProcess( const char *pszName, ITEM_TYPE *pItems, unsigned nItems, void ( *pfnProcess )( ITEM_TYPE & ), void ( *pfnBegin )() = NULL, void ( *pfnEnd )() = NULL, int nMaxParallel = INT_MAX )
{
	CParallelProcessor< ITEM_TYPE, CFuncJobItemProcessor< ITEM_TYPE > > processor( pszName );
	processor.m_ItemProcessor.Init( pfnProcess, pfnBegin, pfnEnd );
	processor.Run( pItems, nItems, -1, nMaxParallel );
}

template < typename ITEM_TYPE, typename OBJECT_TYPE, typename FUNCTION_CLASS >
inline void ParallelProcess( const char *pszName, IThreadPool *pPool, ITEM_TYPE *pItems, unsigned nItems, OBJECT_TYPE *pObject, void ( FUNCTION_CLASS::*pfnProcess )( ITEM_TYPE & ), void ( FUNCTION_CLASS::*pfnBegin )() = NULL, void ( FUNCTION_CLASS::*pfnEnd )() = NULL, int nMaxParallel = INT_MAX )
{
	CParallelProcessor< ITEM_TYPE, CMemberFuncJobItemProcessor< ITEM_TYPE, OBJECT_TYPE, FUNCTION_CLASS > > processor( pszName );
	processor.m_ItemProcessor.Init( pObject, pfnProcess, pfnBegin, pfnEnd );
	processor.Run( pItems, nItems, -1, nMaxParallel, pPool );
}

//-----------------------------------------------------------------------------
// The calls without a name run under a literal one, one item per chunk
//-----------------------------------------------------------------------------

template < typename ITEM_TYPE >
inline void ParallelProcess( ITEM_TYPE *pItems, unsigned nItems, void ( *pfnProcess )( ITEM_TYPE & ), void ( *pfnBegin )() = NULL, void ( *pfnEnd )() = NULL, int nMaxParallel = INT_MAX )
{
	CParallelProcessor< ITEM_TYPE, CFuncJobItemProcessor< ITEM_TYPE > > processor( "ParallelProcess" );
	processor.m_ItemProcessor.Init( pfnProcess, pfnBegin, pfnEnd );
	processor.Run( pItems, nItems, 1, nMaxParallel );
}

template < typename ITEM_TYPE, typename OBJECT_TYPE, typename FUNCTION_CLASS >
inline void ParallelProcess( ITEM_TYPE *pItems, unsigned nItems, OBJECT_TYPE *pObject, void ( FUNCTION_CLASS::*pfnProcess )( ITEM_TYPE & ), void ( FUNCTION_CLASS::*pfnBegin )() = NULL, void ( FUNCTION_CLASS::*pfnEnd )() = NULL, int nMaxParallel = INT_MAX )
{
	CParallelProcessor< ITEM_TYPE, CMemberFuncJobItemProcessor< ITEM_TYPE, OBJECT_TYPE, FUNCTION_CLASS > > processor( "ParallelProcess" );
	processor.m_ItemProcessor.Init( pObject, pfnProcess, pfnBegin, pfnEnd );
	processor.Run( pItems, nItems, 1, nMaxParallel );
}

// Parallel Process that lets you specify threadpool
template < typename ITEM_TYPE >
inline void ParallelProcess( IThreadPool *pPool, ITEM_TYPE *pItems, unsigned nItems, void ( *pfnProcess )( ITEM_TYPE & ), void ( *pfnBegin )() = NULL, void ( *pfnEnd )() = NULL, int nMaxParallel = INT_MAX )
{
	CParallelProcessor< ITEM_TYPE, CFuncJobItemProcessor< ITEM_TYPE > > processor( "ParallelProcess" );
	processor.m_ItemProcessor.Init( pfnProcess, pfnBegin, pfnEnd );
	processor.Run( pItems, nItems, 1, nMaxParallel, pPool );
}

template < typename ITEM_TYPE, typename OBJECT_TYPE, typename FUNCTION_CLASS >
inline void ParallelProcess( IThreadPool *pPool, ITEM_TYPE *pItems, unsigned nItems, OBJECT_TYPE *pObject, void ( FUNCTION_CLASS::*pfnProcess )( ITEM_TYPE & ), void ( FUNCTION_CLASS::*pfnBegin )() = NULL, void ( FUNCTION_CLASS::*pfnEnd )() = NULL, int nMaxParallel = INT_MAX )
{
	CParallelProcessor< ITEM_TYPE, CMemberFuncJobItemProcessor< ITEM_TYPE, OBJECT_TYPE, FUNCTION_CLASS > > processor( "ParallelProcess" );
	processor.m_ItemProcessor.Init( pObject, pfnProcess, pfnBegin, pfnEnd );
	processor.Run( pItems, nItems, 1, nMaxParallel, pPool );
}

// ParallelProcessChunks lets you specify a minimum # of items to process per job. Use this when
// you may have a large set of work items which only take a small amount of time per item, and so
// need to reduce dispatch overhead.
template < typename ITEM_TYPE >
inline void ParallelProcessChunks( ITEM_TYPE *pItems, unsigned nItems, void ( *pfnProcess )( ITEM_TYPE & ), int nChunkSize, int nMaxParallel = INT_MAX )
{
	CParallelProcessor< ITEM_TYPE, CFuncJobItemProcessor< ITEM_TYPE > > processor( "ParallelProcessChunks" );
	processor.m_ItemProcessor.Init( pfnProcess, NULL, NULL );
	processor.Run( pItems, nItems, nChunkSize, nMaxParallel );
}

template < typename ITEM_TYPE, typename OBJECT_TYPE, typename FUNCTION_CLASS >
inline void ParallelProcessChunks( ITEM_TYPE *pItems, unsigned nItems, OBJECT_TYPE *pObject, void ( FUNCTION_CLASS::*pfnProcess )( ITEM_TYPE & ), int nChunkSize, int nMaxParallel = INT_MAX )
{
	CParallelProcessor< ITEM_TYPE, CMemberFuncJobItemProcessor< ITEM_TYPE, OBJECT_TYPE, FUNCTION_CLASS > > processor( "ParallelProcessChunks" );
	processor.m_ItemProcessor.Init( pObject, pfnProcess, NULL, NULL );
	processor.Run( pItems, nItems, nChunkSize, nMaxParallel );
}

template < typename ITEM_TYPE, typename OBJECT_TYPE, typename FUNCTION_CLASS >
inline void ParallelProcessChunks( IThreadPool *pPool, ITEM_TYPE *pItems, unsigned nItems, OBJECT_TYPE *pObject, void ( FUNCTION_CLASS::*pfnProcess )( ITEM_TYPE & ), int nChunkSize, int nMaxParallel = INT_MAX )
{
	CParallelProcessor< ITEM_TYPE, CMemberFuncJobItemProcessor< ITEM_TYPE, OBJECT_TYPE, FUNCTION_CLASS > > processor( "ParallelProcessChunks" );
	processor.m_ItemProcessor.Init( pObject, pfnProcess, NULL, NULL );
	processor.Run( pItems, nItems, nChunkSize, nMaxParallel, pPool );
}

//-----------------------------------------------------------------------------
// Work splitting: index ranges of a context, Begin and End run on every worker
//-----------------------------------------------------------------------------

template < class CONTEXT_TYPE, class ITEM_PROCESSOR_TYPE >
class CParallelLoopProcessor
{
public:
	CParallelLoopProcessor()
	{
		m_nIndex = m_nLimit = 0;
		m_nChunkCount = 0;
		m_nActive = 0;
	}

	void Run( CONTEXT_TYPE *pContext, int nBegin, int nItems, int nChunkCount, int nMaxParallel = INT_MAX, IThreadPool *pThreadPool = NULL )
	{
		if ( !nItems )
			return;

		if ( !pThreadPool )
		{
			pThreadPool = g_pThreadPool;
		}

		m_pContext = pContext;
		m_nIndex = nBegin;
		m_nLimit = nBegin + nItems;
		nChunkCount = MAX( MIN( nItems, nChunkCount ), 1 );
		m_nChunkCount = ( nItems + nChunkCount - 1 ) / nChunkCount;
		int nJobs = ( nItems + m_nChunkCount - 1 ) / m_nChunkCount;
		if ( nJobs > nMaxParallel )
		{
			nJobs = nMaxParallel;
		}

		if ( !pThreadPool )
		{
			DoExecute();
			return;
		}

		int nThreads = pThreadPool->NumThreads();
		if ( nJobs > nThreads )
		{
			nJobs = nThreads;
		}

		if ( nJobs > 0 )
		{
			CThreadedJob **jobs = ( CThreadedJob ** )stackalloc( nJobs * sizeof( CThreadedJob * ) );
			int i = nJobs;

			while ( i-- )
			{
				jobs[ i ] = pThreadPool->QueueCall( this, &CParallelLoopProcessor< CONTEXT_TYPE, ITEM_PROCESSOR_TYPE >::DoExecute );
			}

			DoExecute();

			for ( i = 0; i < nJobs; i++ )
			{
				jobs[ i ]->Abort(); // aborts the ones that never got a thread, waits for the running ones
				jobs[ i ]->Release();
			}
		}
		else
		{
			DoExecute();
		}
	}

	ITEM_PROCESSOR_TYPE m_ItemProcessor;

private:
	void DoExecute()
	{
		m_ItemProcessor.Begin();

		for ( ;; )
		{
			int nIndex = m_nIndex.AtomicAdd( m_nChunkCount );

			if ( nIndex < m_nLimit )
			{
				int nCount = MIN( m_nChunkCount, m_nLimit - nIndex );
				m_ItemProcessor.Process( m_pContext, nIndex, nCount );
			}
			else
			{
				break;
			}
		}

		m_ItemProcessor.End();

		--m_nActive;
	}

	CONTEXT_TYPE *m_pContext;
	CInterlockedInt m_nIndex;
	int m_nLimit;
	int m_nChunkCount;
	CInterlockedInt m_nActive;
};

template < typename CONTEXT_TYPE >
inline void ParallelLoopProcess( IThreadPool *pPool, CONTEXT_TYPE *pContext, int nStart, int nCount, void ( *pfnProcess )( CONTEXT_TYPE *, int, int ), void ( *pfnBegin )() = NULL, void ( *pfnEnd )() = NULL, int nMaxParallel = INT_MAX )
{
	CParallelLoopProcessor< CONTEXT_TYPE, CLoopFuncJobItemProcessor< CONTEXT_TYPE > > processor;
	processor.m_ItemProcessor.Init( pfnProcess, pfnBegin, pfnEnd );
	processor.Run( pContext, nStart, nCount, 1, nMaxParallel, pPool );
}

template < typename CONTEXT_TYPE, typename OBJECT_TYPE, typename FUNCTION_CLASS >
inline void ParallelLoopProcess( IThreadPool *pPool, CONTEXT_TYPE *pContext, int nStart, int nCount, OBJECT_TYPE *pObject, void ( FUNCTION_CLASS::*pfnProcess )( CONTEXT_TYPE *, int, int ), void ( FUNCTION_CLASS::*pfnBegin )() = NULL, void ( FUNCTION_CLASS::*pfnEnd )() = NULL, int nMaxParallel = INT_MAX )
{
	CParallelLoopProcessor< CONTEXT_TYPE, CLoopMemberFuncJobItemProcessor< CONTEXT_TYPE, OBJECT_TYPE, FUNCTION_CLASS > > processor;
	processor.m_ItemProcessor.Init( pObject, pfnProcess, pfnBegin, pfnEnd );
	processor.Run( pContext, nStart, nCount, 1, nMaxParallel, pPool );
}

template < typename CONTEXT_TYPE >
inline void ParallelLoopProcessChunks( IThreadPool *pPool, CONTEXT_TYPE *pContext, int nStart, int nCount, int nChunkSize, void ( *pfnProcess )( CONTEXT_TYPE *, int, int ), void ( *pfnBegin )() = NULL, void ( *pfnEnd )() = NULL, int nMaxParallel = INT_MAX )
{
	CParallelLoopProcessor< CONTEXT_TYPE, CLoopFuncJobItemProcessor< CONTEXT_TYPE > > processor;
	processor.m_ItemProcessor.Init( pfnProcess, pfnBegin, pfnEnd );
	processor.Run( pContext, nStart, nCount, nChunkSize, nMaxParallel, pPool );
}

template < typename CONTEXT_TYPE, typename OBJECT_TYPE, typename FUNCTION_CLASS >
inline void ParallelLoopProcessChunks( IThreadPool *pPool, CONTEXT_TYPE *pContext, int nStart, int nCount, int nChunkSize, OBJECT_TYPE *pObject, void ( FUNCTION_CLASS::*pfnProcess )( CONTEXT_TYPE *, int, int ), void ( FUNCTION_CLASS::*pfnBegin )() = NULL, void ( FUNCTION_CLASS::*pfnEnd )() = NULL, int nMaxParallel = INT_MAX )
{
	CParallelLoopProcessor< CONTEXT_TYPE, CLoopMemberFuncJobItemProcessor< CONTEXT_TYPE, OBJECT_TYPE, FUNCTION_CLASS > > processor;
	processor.m_ItemProcessor.Init( pObject, pfnProcess, pfnBegin, pfnEnd );
	processor.Run( pContext, nStart, nCount, nChunkSize, nMaxParallel, pPool );
}

//-----------------------------------------------------------------------------
// The derived class runs OnBegin, OnProcess until it returns false, and OnEnd
// on every worker
//-----------------------------------------------------------------------------

template < class Derived >
class CParallelProcessorBase
{
protected:
	typedef CParallelProcessorBase< Derived > ThisParallelProcessorBase_t;
	typedef Derived ThisParallelProcessorDerived_t;

public:
	CParallelProcessorBase()
	{
		m_nActive = 0;
	}

protected:
	void Run( int nMaxParallel = INT_MAX, int threadOverride = -1 )
	{
		int i = g_pThreadPool->NumIdleThreads();

		if ( nMaxParallel < i )
		{
			i = nMaxParallel;
		}

		while ( i-- > 0 )
		{
			if ( threadOverride == -1 || i == threadOverride - 1 )
			{
				++m_nActive;
				ThreadExecute( this, &ThisParallelProcessorBase_t::DoExecute )->Release();
			}
		}

		if ( threadOverride == -1 || threadOverride == 0 )
		{
			++m_nActive;
			DoExecute();
		}

		while ( m_nActive )
		{
			ThreadPause();
		}
	}

protected:
	void OnBegin() {}
	bool OnProcess() { return false; }
	void OnEnd() {}

private:
	void DoExecute()
	{
		static_cast< Derived * >( this )->OnBegin();

		while ( static_cast< Derived * >( this )->OnProcess() )
			continue;

		static_cast< Derived * >( this )->OnEnd();

		--m_nActive;
	}

	CInterlockedInt m_nActive;
};

//-----------------------------------------------------------------------------
// Raw thread launching
//-----------------------------------------------------------------------------

inline uintp FunctorExecuteThread( void *pParam )
{
	CFunctor *pFunctor = ( CFunctor * )pParam;
	( *pFunctor )();
	pFunctor->Release();
	return 0;
}

inline ThreadHandle_t ThreadExecuteSoloImpl( CFunctor *pFunctor, const char *pszName = NULL )
{
	ThreadHandle_t hThread;
	hThread = CreateSimpleThread( FunctorExecuteThread, pFunctor );

	if ( pszName )
	{
		ThreadSetDebugNameS2( hThread, pszName );
	}

	return hThread;
}

inline ThreadHandle_t ThreadExecuteSolo( CThreadedJob *pJob ) { return ThreadExecuteSoloImpl( CreateFunctor( pJob, &CThreadedJob::Execute ), pJob->GetJobName() ); }

template < typename T1 >
inline ThreadHandle_t ThreadExecuteSolo( const char *pszName, T1 a1 ) { return ThreadExecuteSoloImpl( CreateFunctor( a1 ), pszName ); }

template < typename T1, typename T2 >
inline ThreadHandle_t ThreadExecuteSolo( const char *pszName, T1 a1, T2 a2 ) { return ThreadExecuteSoloImpl( CreateFunctor( a1, a2 ), pszName ); }

template < typename T1, typename T2, typename T3 >
inline ThreadHandle_t ThreadExecuteSolo( const char *pszName, T1 a1, T2 a2, T3 a3 ) { return ThreadExecuteSoloImpl( CreateFunctor( a1, a2, a3 ), pszName ); }

template < typename T1, typename T2, typename T3, typename T4 >
inline ThreadHandle_t ThreadExecuteSolo( const char *pszName, T1 a1, T2 a2, T3 a3, T4 a4 ) { return ThreadExecuteSoloImpl( CreateFunctor( a1, a2, a3, a4 ), pszName ); }

template < typename T1, typename T2, typename T3, typename T4, typename T5 >
inline ThreadHandle_t ThreadExecuteSolo( const char *pszName, T1 a1, T2 a2, T3 a3, T4 a4, T5 a5 ) { return ThreadExecuteSoloImpl( CreateFunctor( a1, a2, a3, a4, a5 ), pszName ); }

template < typename T1, typename T2, typename T3, typename T4, typename T5, typename T6 >
inline ThreadHandle_t ThreadExecuteSolo( const char *pszName, T1 a1, T2 a2, T3 a3, T4 a4, T5 a5, T6 a6 ) { return ThreadExecuteSoloImpl( CreateFunctor( a1, a2, a3, a4, a5, a6 ), pszName ); }

template < typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7 >
inline ThreadHandle_t ThreadExecuteSolo( const char *pszName, T1 a1, T2 a2, T3 a3, T4 a4, T5 a5, T6 a6, T7 a7 ) { return ThreadExecuteSoloImpl( CreateFunctor( a1, a2, a3, a4, a5, a6, a7 ), pszName ); }

template < typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8 >
inline ThreadHandle_t ThreadExecuteSolo( const char *pszName, T1 a1, T2 a2, T3 a3, T4 a4, T5 a5, T6 a6, T7 a7, T8 a8 ) { return ThreadExecuteSoloImpl( CreateFunctor( a1, a2, a3, a4, a5, a6, a7, a8 ), pszName ); }

template < typename T1, typename T2 >
inline ThreadHandle_t ThreadExecuteSoloRef( const char *pszName, T1 a1, T2 a2 ) { return ThreadExecuteSoloImpl( CreateRefCountingFunctor( a1, a2 ), pszName ); }

template < typename T1, typename T2, typename T3 >
inline ThreadHandle_t ThreadExecuteSoloRef( const char *pszName, T1 a1, T2 a2, T3 a3 ) { return ThreadExecuteSoloImpl( CreateRefCountingFunctor( a1, a2, a3 ), pszName ); }

template < typename T1, typename T2, typename T3, typename T4 >
inline ThreadHandle_t ThreadExecuteSoloRef( const char *pszName, T1 a1, T2 a2, T3 a3, T4 a4 ) { return ThreadExecuteSoloImpl( CreateRefCountingFunctor( a1, a2, a3, a4 ), pszName ); }

template < typename T1, typename T2, typename T3, typename T4, typename T5 >
inline ThreadHandle_t ThreadExecuteSoloRef( const char *pszName, T1 a1, T2 a2, T3 a3, T4 a4, T5 a5 ) { return ThreadExecuteSoloImpl( CreateRefCountingFunctor( a1, a2, a3, a4, a5 ), pszName ); }

template < typename T1, typename T2, typename T3, typename T4, typename T5, typename T6 >
inline ThreadHandle_t ThreadExecuteSoloRef( const char *pszName, T1 a1, T2 a2, T3 a3, T4 a4, T5 a5, T6 a6 ) { return ThreadExecuteSoloImpl( CreateRefCountingFunctor( a1, a2, a3, a4, a5, a6 ), pszName ); }

template < typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7 >
inline ThreadHandle_t ThreadExecuteSoloRef( const char *pszName, T1 a1, T2 a2, T3 a3, T4 a4, T5 a5, T6 a6, T7 a7 ) { return ThreadExecuteSoloImpl( CreateRefCountingFunctor( a1, a2, a3, a4, a5, a6, a7 ), pszName ); }

template < typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8 >
inline ThreadHandle_t ThreadExecuteSoloRef( const char *pszName, T1 a1, T2 a2, T3 a3, T4 a4, T5 a5, T6 a6, T7 a7, T8 a8 ) { return ThreadExecuteSoloImpl( CreateRefCountingFunctor( a1, a2, a3, a4, a5, a6, a7, a8 ), pszName ); }

//-----------------------------------------------------------------------------

#endif // JOBTHREAD_H
