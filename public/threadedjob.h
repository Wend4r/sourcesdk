#ifndef THREADEDJOB_H
#define THREADEDJOB_H

#if COMPILER_MSVC
#pragma once
#endif

#include <tier0/platform.h>
#include <tier0/strtools.h>
#include <tier1/refcount.h>
#include <tier1/utlvector.h>

class IThreadPool;
class CThreadEvent;

// Ported from legacy CJob (tier1/jobthread.h). That header is not included directly because it
// does not currently compile cleanly in this project (pre-existing errors in jobthread.h/functors.h).
typedef int JobStatus_t;
enum JobStatusEnum_t
{
	// Use negative for errors
	JOB_OK,					// operation is successful
	JOB_STATUS_PENDING,		// properly queued, waiting for service
	JOB_STATUS_INPROGRESS,	// being serviced
	JOB_STATUS_ABORTED,		// aborted by caller
	JOB_STATUS_UNSERVICED,	// not yet queued
};
DECLARE_POINTER_HANDLE( ThreadPoolData_t );

class CThreadedJob : public CRefCounted1< IRefCounted, CRefCountServiceBase< CRefMT > >
{
public:
	CThreadedJob() : m_nStatus( JOB_STATUS_UNSERVICED ), m_nFlags( 0x200 ), m_pThreadPool( nullptr ), m_pJobEvent( nullptr ), m_ThreadPoolData( nullptr )
	{
		m_szJobName[0] = '\0';
	}

	virtual ~CThreadedJob() {}

	// Falls back to a literal when the job is unnamed
	virtual const char *GetJobName() { return m_szJobName[0] ? m_szJobName : "Job"; }

	// Copies into the fixed buffer; the engine additionally sets a "named" flag byte inside m_nUnk0010 (+0x12)
	virtual void SetJobName( const char *pszName )
	{
		if ( pszName )
			V_strncpy( m_szJobName, pszName, sizeof( m_szJobName ) );
		else
			m_szJobName[0] = '\0';
	}

	// Pure: implemented by concrete leaf jobs
	virtual void Execute() = 0;

	// Base Abort is a no-op; overridden by CThreadedJobWithDependencies
	virtual void Abort( bool b ) {}

protected:
	JobStatus_t m_nStatus;
	uint64 m_nFlags; // packed flags/state word (cf. legacy CJob m_priority/m_flags/m_iServicingThread block), constructor sets 0x200; byte +0x12 is a "named" flag (SetJobName); bit semantics consumed by tier0 thread pool
	IThreadPool *m_pThreadPool; // nullptr selects the global g_pThreadPool
	CThreadEvent *m_pJobEvent; // pooled manual event, released via ReturnJobManualEventToPool (cf. legacy CJob::m_CompleteEvent)
	ThreadPoolData_t m_ThreadPoolData; // thread-pool bookkeeping handle, atomic, cleared on destruct (cf. legacy CJob::m_ThreadPoolData)
	char m_szJobName[32];
};

class CThreadedDependentJob : public CThreadedJob
{
public:
	virtual ~CThreadedDependentJob() {}
	virtual bool IsJobType( uint32 nJobType ) = 0;
	virtual void BeforeJobRuns() = 0;
};

class CThreadedJobWithDependencies : public CThreadedDependentJob
{
public:
	virtual ~CThreadedJobWithDependencies() {};

	// Pure: completes the job then releases/enqueues dependents through the engine thread pool.
	virtual void Execute() override = 0;

	// Resets the pending dependency state. The engine clears m_nPendingDependencies with an
	// interlocked exchange and zeroes the dependency count; RemoveAll() is the clean equivalent.
	virtual void Abort( bool b ) override
	{
		m_nPendingDependencies = 0;
		m_Dependencies.RemoveAll();
	}

	// Default result; concrete jobs override to match a specific type id.
	virtual bool IsJobType( uint32 nJobType ) override { return false; }

	// No-op by default.
	virtual void BeforeJobRuns() override {}

protected:
	int m_nPendingDependencies;
	char m_Pad0054[4];
	CUtlVector< CThreadedJobWithDependencies * > m_Dependencies;
};
#ifdef LINUX
COMPILE_TIME_ASSERT( sizeof( CThreadedJobWithDependencies ) == 112 );
#endif

template < typename T, typename TCall >
class CAsyncCallJob : public CThreadedJobWithDependencies
{
public:
	T *m_pObject;
	TCall m_Call;
};

#endif // THREADEDJOB_H
