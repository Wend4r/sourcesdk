//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
// $NoKeywords: $
//=============================================================================

#ifndef GC_JOB_H
#define GC_JOB_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/memdbgon.h"
#include "tier0/fasttimer.h"
#include "tier0/functors.h"
#include "tier0/utllinkedlist.h"
#include "tier0/utlleanvector.h"
#include "vstdlib/coroutine.h"
#include "msgbase.h"
#include "msgprotobuf.h"
#include "workthreadpool.h"

class GCConVar;

namespace GCSDK
{
class CJobMgr;
class CLock;
class CJob;
class IMsgNetPacket;

//-----------------------------------------------------------------------------
// Purpose: Use these macros to declare blocks where it is unsafe to yield.
//	The job will assert if it pauses within the block
//-----------------------------------------------------------------------------
#define DO_NOT_YIELD_THIS_SCOPE()	GCSDK::CDoNotYieldScope doNotYieldScope_##line( FILE_AND_LINE )
#define BEGIN_DO_NOT_YIELD()		GJobCur().PushDoNotYield( FILE_AND_LINE )
#define END_DO_NOT_YIELD()			GJobCur().PopDoNotYield()

class CDoNotYieldScope
{
public:
	CDoNotYieldScope( const char *pchLocation );
	~CDoNotYieldScope();
private:
	// Disallow these constructors and operators
	CDoNotYieldScope();
	CDoNotYieldScope( const CDoNotYieldScope &that );
	CDoNotYieldScope &operator=( const CDoNotYieldScope &that );
};

//-----------------------------------------------------------------------------

// job creation function
typedef CJob *(*JobCreationFunc_t)( void *pvServerParent, void * pvStartParam );


// routing function of a job type (GC side; null in every job type of the CS2 binaries)
typedef void (*JobRoutingFunc_t)( CUtlVector< uint32 > &vecGCsToSendTo, IMsgNetPacket *pNetPacket );

//-----------------------------------------------------------------------------
// Purpose: the contexts a message can arrive from; JobType_t::m_nValidContexts
//			is a mask of them (all game job types use k_eGCMsgContext_All)
//-----------------------------------------------------------------------------
enum EGCMsgContext
{
	k_eGCMsgContext_None = 0,				// nobody may send this message
	k_eGCMsgContext_Client = ( 1 << 0 ),			// from a player/client
	k_eGCMsgContext_UnsecuredServer = ( 1 << 1 ),	// from a dedicated server that is not trusted
	k_eGCMsgContext_TrustedServer = ( 1 << 2 ),		// from a securely managed dedicated server
	k_eGCMsgContext_System = ( 1 << 3 ),			// from the GCH system
	k_eGCMsgContext_OtherGC = ( 1 << 4 ),			// from another sub GC or the master GC

	k_eGCMsgContext_AllServers = k_eGCMsgContext_UnsecuredServer | k_eGCMsgContext_TrustedServer,
	k_eGCMsgContext_All = k_eGCMsgContext_Client | k_eGCMsgContext_TrustedServer | k_eGCMsgContext_UnsecuredServer | k_eGCMsgContext_System | k_eGCMsgContext_OtherGC
};

//-----------------------------------------------------------------------------
// Purpose: static job information
//                      contains information relevant to one type of CJob
//-----------------------------------------------------------------------------
struct JobType_t
{
	const char *m_pchName;				// name of this type of job
	MsgType_t m_eCreationMsg;			// network message that creates this job
	uint32 m_nValidContexts;			// mask of EGCMsgContext the message may arrive from
	JobCreationFunc_t m_pJobFactory;	// virtual constructor
	// The CS:GO type has m_pJobRouter and m_pControlCV here; the CS2 records are 32 bytes and keep
	// one pointer, null in all of them. Named after the first of the two.
	JobRoutingFunc_t m_pJobRouter;
};


//-----------------------------------------------------------------------------
// Purpose: reason as to why the current job has yielded to the main thread (paused)
//			if this is updated, k_prgchJobPauseReason[] in job.cpp also needs to be updated
//-----------------------------------------------------------------------------
enum EJobPauseReason
{
	k_EJobPauseReasonNone,
	k_EJobPauseReasonNotStarted,
	k_EJobPauseReasonNetworkMsg,
	k_EJobPauseReasonSleepForTime,
	k_EJobPauseReasonWaitingForLock,
	k_EJobPauseReasonYield,
	k_EJobPauseReasonSQL,
	k_EJobPauseReasonWorkItem,
	k_EJobPauseReasonJob,

	k_EJobPauseReasonCount
};


//-----------------------------------------------------------------------------
// Purpose: contains information used to route a message to a job, or to
//			create a new job from that message type
//-----------------------------------------------------------------------------
struct JobMsgInfo_t
{
	MsgType_t m_eMsg;
	JobID_t m_JobIDSource;
	JobID_t m_JobIDTarget;

	JobMsgInfo_t()
	{
		m_eMsg = (MsgType_t)0;
		m_JobIDSource = k_GIDNil;
		m_JobIDTarget = k_GIDNil;
	}

	JobMsgInfo_t( MsgType_t eMsg, JobID_t jobIDSource, JobID_t jobIDTarget )
	{
		m_eMsg = eMsg;
		m_JobIDSource = jobIDSource;
		m_JobIDTarget = jobIDTarget;
	}
};

typedef void (CJob::*JobThreadFunc_t)();

#define BYieldingAcquireLock( lock ) _BYieldingAcquireLock( lock, __FILE__, __LINE__ ) 
#define BAcquireLockImmediate( lock ) _BAcquireLockImmediate( lock, __FILE__, __LINE__ )
#define ReleaseLock( lock ) _ReleaseLock( lock, false, __FILE__, __LINE__ )

//-----------------------------------------------------------------------------
// Purpose: A job is any server operation that requires state.  Typically, we use jobs for
//			operations that need to pause waiting for responses from other servers.  The
//			job object persists the state of the operation while it waits, and the reply
//			from the remote server re-activates the job.
//-----------------------------------------------------------------------------
class CJob
{
public:
	// Constructors & destructors, when overriding job name a static string pointer must be used
	explicit CJob( CJobMgr &jobMgr, char const *pchJobName = NULL );
	virtual ~CJob();

	// starts the job, storing off the network msg and calling it's Run() function
	void StartJobFromNetworkMsg( IMsgNetPacket *pNetPacket, const JobID_t &gidJobIDSrc, uint32 nContextMask );

	// accessors
	JobID_t GetJobID() const { return m_JobID; }

	// start job immediately
	// mostly for CMD jobs, which should immediately Yield() once
	// so that they don't do their work until the enclosing JobMbr.Run() 
	// is called
	void StartJob( void * pvStartParam );
	// schedules the job for execution, but does not interrup the currently running job. Effectively starts the job on the yielding list as if it had immediately yielded
	// although is more efficient than actually doing so
	void StartJobDelayed( void * pvStartParam );

	// string name of the job
	const char *GetName() const;
	// return reason why we're paused
	EJobPauseReason GetPauseReason() const		{ return m_ePauseReason; }
	// string description of why we're paused
	const char *GetPauseReasonDescription() const;
	// return time at which this job was last paused or continued
	const CJobTime& GetTimeSwitched() const		{ return m_STimeSwitched; }
	// return microseconds run since we were last continued
	uint64 GetMicrosecondsRun() const			{ return m_FastTimerDelta.GetDurationInProgress().GetUlMicroseconds(); }
	bool BJobNeedsToHeartbeat() const			{ return ( m_STimeNextHeartbeat.CServerMicroSecsPassed() >= 0 ); }

	// --- locking pointers
	bool _BYieldingAcquireLock( CLock *pLock, const char *filename = "unknown", int line = 0 );
	bool _BAcquireLockImmediate( CLock *pLock, const char *filename = "unknown", int line = 0 );
	void _ReleaseLock( CLock *pLock, bool bForce = false, const char *filename = "unknown", int line = 0 );
	bool BHoldsAnyLocks() const { return m_vecLocks.Count() > 0; }
	void ReleaseLocks();

	/// If we hold any locks, spew about them and release them.
	/// This is useful for long running jobs, to make sure they don't leak
	/// locks that never get cleaned up
	void ShouldNotHoldAnyLocks();

	// --- general methods for waiting for events
	// Simple yield to other jobs until Run() called again
	bool BYield();
	// Yield IF JobMgr thinks we need to based on how long we've run and our priority
	bool BYieldIfNeeded( bool *pbYielded = NULL );
	// waits for a set amount of time
	bool BYieldingWaitTime( uint32 m_cMicrosecondsToSleep );
	bool BYieldingWaitOneFrame();
	// waits for another network msg, returning false if none returns
	bool BYieldingWaitForMsg( IMsgNetPacket **ppNetPacket );
	bool BYieldingWaitForMsg( CGCMsgBase *pMsg, MsgType_t eMsg );
	bool BYieldingWaitForMsg( CProtoBufMsgBase *pMsg, MsgType_t eMsg );

#ifdef GC
	bool BYieldingWaitForMsg( CGCMsgBase *pMsg, MsgType_t eMsg, const CSteamID &expectedID );
	bool BYieldingWaitForMsg( CProtoBufMsgBase *pMsg, MsgType_t eMsg, const CSteamID &expectedID );
	bool BYieldingRunQuery( CGCSQLQueryGroup *pQueryGroup, ESchemaCatalog eSchemaCatalog );
#endif

	bool BYieldingWaitTimeWithLimit( uint32 cMicrosecondsToSleep, CJobTime &stimeStarted, int64 nMicroSecLimit );
	bool BYieldingWaitTimeWithLimitRealTime( uint32 cMicrosecondsToSleep, int nSecLimit );

	void RecordWaitTimeout() { m_flags.m_bits.m_bWaitTimeout = true; }

	// wait for pending work items before deleting job
	void WaitForThreadFuncWorkItemBlocking();

	// waits for a work item completion callback
	// You can pass a string that describes what sort of work item you are waiting on.
	// WARNING: This function saves the pointer to the string, it doesn't copy the string
	bool BYieldingWaitForWorkItem( const char *pszWorkItemName = NULL );

	// adds this work item to threaded work pool and waits for it
	bool BYieldingWaitForThreadFuncWorkItem( CWorkItem * );

	// calls a local function in a thread, and yields until it's done
	bool BYieldingWaitForThreadFunc( CFunctor *jobFunctor );

	// Used by the DO_NOT_YIELD() macros
	int32 GetDoNotYieldDepth() const;
	void PushDoNotYield( const char *pchFileAndLine );
	void PopDoNotYield();

#ifdef DBGFLAG_VALIDATE
	virtual void Validate( CValidator &validator, const char *pchName );		// Validate our internal structures
	static void ValidateStatics( CValidator &validator, const char *pchName );
#endif

	// creates a job
	template <typename JOB_TYPE, typename PARAM_TYPE>
	static JOB_TYPE *AllocateJob( PARAM_TYPE *pParam )
	{
		return new JOB_TYPE( pParam );
	}
	// delete a job (the job knows what allocator to use)
	static void DeleteJob( CJob *pJob );

	void SetStartParam( void * pvStartParam )		{ Assert( NULL == m_pvStartParam ); m_pvStartParam = pvStartParam; }
	void SetFromFromMsg( bool bRunFromMsg )			{ m_bRunFromMsg = true; }

	void AddPacketToList( IMsgNetPacket *pNetPacket, const JobID_t gidJobIDSrc );
	// marks a packet as being finished with, releases the packet and frees the memory
	void ReleaseNetPacket( IMsgNetPacket *pNetPacket );

	void EndPause( EJobPauseReason eExpectedState );

	// Generate an assertion in the coroutine of this job
	// (creating a minidump).  Useful for inspecting stuck jobs
	void GenerateAssert( const char *pchMsg = NULL );

	// called to determine if the requested context is valid. If multiple contexts are provided, this will return true only if ALL the contexts are valid
	bool BHasContext( uint32 nContext ) const			{ return ( m_nContextMask & nContext ) == nContext; }
	uint32 GetContexts() const							{ return m_nContextMask; }

protected:
	// main job implementation, in the coroutine.  Every job must implement at least one of these methods.
	virtual bool BYieldingRunJob( void * pvStartParam )				{ Assert( false ); return true; }	// implement this if your job can be started directly
	virtual bool BYieldingRunJobFromMsg( IMsgNetPacket * pNetPacket )	{ Assert( false ); return true; }	// implement this if your job can be started by a network message

	// Can be overridden to return a different timeout per job class
	virtual uint32 CHeartbeatsBeforeTimeout();

	// Called by CJobMgr to send heartbeat message to our listeners during long operations
	void Heartbeat();


	// accessor to get access to the JobMgr from the server we belong to
	CJobMgr &GetJobMgr();
	uint32 m_bRunFromMsg:1,
			m_bWorkItemCanceled:1,			// true if the work item we were waiting on was canceled
			m_bIsTest:1,
			m_bIsLongRunning:1;

private:
	// starts the coroutine that activates the job
	void InitCoroutine();

	// continues the current job
	void Continue();

	// break into this coroutine - can only be called from OUTSIDE this coroutine
	void Debug();

	// pauses the current job - can only be called from inside a coroutine
	void Pause( EJobPauseReason eReason, const char *pszResourceName );

	static void BRunProxy( void *pvThis );

	JobID_t m_JobID;					// Our unique identifier.
	HCoroutine m_hCoroutine;
	void * m_pvStartParam;				// Start params for our job, if any
	// all these flags indicate some kind of failure and we will want to report them
	union  {
		struct {
			uint32
				m_bJobFailed:1,					// true if BYieldingRunJob returned false
				m_bLocksFailed:1,
				m_bLocksLongHeld:1,
				m_bLocksLongWait:1,
				m_bWaitTimeout:1,
				m_bLongInterYield:1,
				m_bTimeoutNetMsg:1,
				m_bTimeoutOther:1,
				// Named by what they do: BRunProxy sets the first when the coroutine starts and the
				// second when the job body has returned; the destructor cancels the coroutine
				// unless both are set
				m_bCoroutineStarted:1,
				m_bCoroutineFinished:1,
				m_uUnused:22;
			} m_bits;
		uint32 m_uFlags;
		} m_flags;
	int m_cLocksAttempted;
	int m_cLocksWaitedFor;
	EJobPauseReason m_ePauseReason;
	const char *m_pszPauseResourceName;
	MsgType_t	m_unWaitMsgType;
	// Named by what CJobMgr::PassMsgToJob does with them: the reply a job waiting for a message
	// accepts. With m_bWaitMsgCheckFormat set the packet must have this format and m_unWaitMsgType;
	// with m_bWaitMsgCheckSender set it must come from m_steamIDWaitMsgSender (a nil ID means the
	// GC or the system). BYieldingWaitForMsg clears them once the wait ends.
	EMsgFormatType m_eWaitMsgFormatType;
	bool m_bWaitMsgCheckFormat;
	bool m_bWaitMsgCheckSender;
	CSteamID m_steamIDWaitMsgSender;		// packed, at byte 74
	CJobTime m_STimeStarted;				// time (frame) at which this job started
	CJobTime m_STimeSwitched;				// time (frame) at which we were last paused or continued
	CJobTime m_STimeNextHeartbeat;		// Time at which next heartbeat should be performed
	CFastTimer m_FastTimerDelta;		// How much time we've been running for without yielding
	CCycleCount m_cyclecountTotal;		// Total runtime
	uint32 m_nContextMask;				// The context that this job was created in. Typically only used for message jobs to indicate the initiator of the message
	CJob *m_pJobPrev;					// the job that launched us

	// lock manipulation
	void _SetLock( CLock *pLock, const char *filename, int line );
	void UnsetLock( CLock *pLock );
	void PassLockToJob( CJob *pNewJob, CLock *pLock );
	void OnLockDeleted( CLock *pLock );
	void AddJobToNotifyOnLockRelease( CJob *pJob );
	CUtlVectorFixedGrowable< CLock *, 2 > m_vecLocks;
	CLock *m_pWaitingOnLock;			// lock we're waiting on, if any
	const char *m_pWaitingOnLockFilename;
	int m_waitingOnLockLine;
	// Named by its place: the constructor sets it to "" like m_pWaitingOnLockFilename, nothing else
	// in the CS2 binaries reads or writes it
	const char *m_pWaitingOnLockDescription;
	CJob *m_pJobToNotifyOnLockRelease;	// other job that wants this lock
	CWorkItem *m_pWaitingOnWorkItem;	// set if job is waiting for this work item
	JobID_t m_jobIDWaitingOn;			// The job we're waiting on.  This is used if we're actually waiting on a job, but also if we're waiting on a work item, and we know the job that is doing the work.
	CJob *m_pFirstJobWaitingOnThis;		// Are any jobs waiting on this job to finish?  If so, this is the head of the linked list.  (m_pJobWaitingOnNext is the next link)
	CJob *m_pJobWaitingOnNext;			// Next job in linked list of jobs waiting on the same thing.

	CJobMgr &m_JobMgr;					// our job manager
	CUtlVectorFixedGrowable< IMsgNetPacket *, 1 > m_vecNetPackets;			// list of tcp packets currently held by this job (ie, needing release on job exit)

	// pointer to our own static job info
	struct JobType_t const *m_pJobType;

	// Name of the job for when it's not registered
	const char *m_pchJobName;

	// setting the job info
	friend void Job_SetJobType( CJob &job, const JobType_t *pJobType );
	friend class CJobMgr;
	friend class CLock;
};


// Only one job can be running at a time.  We keep a global accessor to it.
extern CJob *g_pJobCur;
inline CJob &GJobCur() { Assert( g_pJobCur != NULL ); return *g_pJobCur; }

#define AssertRunningJob() { Assert( NULL != g_pJobCur ); }
#define AssertRunningThisJob() { Assert( this == g_pJobCur ); }
#define AssertNotRunningThisJob() { Assert( this != g_pJobCur ); }
#define AssertNotRunningJob() { Assert( NULL == g_pJobCur ); }


//-----------------------------------------------------------------------------
// Purpose: simple locking class
//			add this object to any classes you want jobs to be able to lock
//-----------------------------------------------------------------------------
class CLock
{
public:
	CLock( );
	~CLock();
	
	bool BIsLocked()		{ return m_pJob != NULL; }
	CJob *GetJobLocking()	{ return m_pJob; }
	CJob *GetJobWaitingQueueHead()	{ return m_pJobToNotifyOnLockRelease; }
	CJob *GetJobWaitingQueueTail()	{ return m_pJobWaitingQueueTail; }
	void AddToWaitingQueue( CJob *pJob );
	const char *GetName() const;
	void SetName( const char *pchName );
	void SetName( const char *pchPrefix, uint64 ulID );
	void SetName( const CSteamID &steamID );
	int16 GetLockType() const { return m_nsLockType; }
	void SetLockType( int16 nsLockType ) { m_nsLockType = nsLockType; }
	uint64 GetLockSubType() const { return m_unLockSubType; }
	void SetLockSubType( uint64 unLockSubType ) { m_unLockSubType = unLockSubType; }
	int32 GetWaitingCount() const { return m_nWaitingCount; }
	int64 GetMicroSecondsSinceLock() const { return m_sTimeAcquired.CServerMicroSecsPassed(); }
	void IncrementReference();
	int DecrementReference();
	void ClearReference() { m_nRefCount = 0; }
	int32 GetReferenceCount() const { return m_nRefCount; }

	void Dump( const char *pszPrefix = "\t\t", int nPrintMax = 1, bool bPrintWaiting = true ) const;

private:
	enum ENameType
	{
		k_ENameTypeNone = 0,
		k_ENameTypeSteamID = 1,
		k_ENameTypeConstStr = 2,
		k_ENameTypeConcat = 3,
	};

	CJob *m_pJob;						// the job that's currently locking us
	CJob *m_pJobToNotifyOnLockRelease;	// Pointer to the first job waiting on us
	CJob *m_pJobWaitingQueueTail;		// Pointer to the last job waiting on us
	int16 m_nsLockType;					// Lock priority for safely waiting on multiple locks
	int16 m_nsNameType;					// Enum that controls how this lock is named

	uint64 m_ulID;						// ID part of the lock's name
	const char *m_pchConstStr;			// Prefix part of the lock's name
	mutable CUtlString m_strName;		// Cached name

	int32 m_nRefCount;					// # of times locked
	int32 m_nWaitingCount;				// Count of jobs waiting on the lock
	CJobTime m_sTimeAcquired;			// Time the lock was last locked
	uint64 m_unLockSubType;

	const char *m_pFilename;			// Filename of the source file who aquired this lock
	int m_line;							// Line number of the filename

	friend class CJob;
};

//-----------------------------------------------------------------------------
// Purpose: automatic locking class
//-----------------------------------------------------------------------------

class CAutoCLock
{
public:
	CAutoCLock( CLock &refLock )
		: m_pLock ( &refLock)
	{
		DbgVerify( GJobCur().BYieldingAcquireLock( m_pLock ) );
	}

	// explicitly unlock before destruction
	void Unlock()
	{
		if ( m_pLock != NULL )
			GJobCur().ReleaseLock( m_pLock );
		m_pLock = NULL;
	}

	~CAutoCLock( )
	{
		Unlock();
	}

private:
	CLock *m_pLock;
	CJob *m_pJob;
};

} // namespace GCSDK

#include "tier0/memdbgoff.h"

#endif // GC_JOB_H
