#ifndef COJOB_H
#define COJOB_H

#pragma once

#include "tier0/platform.h"
#include "tier0/coroutine.h"
#include "tier0/fasttimer.h"
#include "tier0/utlvector.h"

//-----------------------------------------------------------------------------
// A job run as a coroutine by the job manager of the module (a singleton); CCoJobLambda< F >
// derives from it and adds the lambda it runs. 0x120 bytes on both platforms, the same layout
// on both.
//
// Slot numbers in the Unk_ names are Windows indexes; on Linux the destructor takes two
// slots, so every slot after it is one further. The methods without a name keep a comment
// saying what the body does; their parameters are not recovered.
//
// The objects the job points to without a class name in the game (the locks, the network
// packets, the work item, the job type) are declared as void pointers; each comment says
// what is known of them.
//-----------------------------------------------------------------------------
abstract_class CCoJob
{
public:
	// True when the argument is 2; the job manager asks it for every context it serves
	virtual bool Unk_0() = 0;
	// "%s: %s" with the pause reason and m_pszPauseResourceName, "WOL: 0x%p (%s)" with the
	// lock while waiting for one; "undefined" and "null" for the missing parts
	virtual const char *GetPauseReasonDescription() const = 0;
	// Return 0
	virtual int Unk_2() = 0;
	virtual int Unk_3() = 0;
	// Returns 0; the job manager divides it by the heartbeat interval (5 s) to get the
	// heartbeats before a timeout for one pause reason
	virtual int Unk_4() = 0;
	// Named by what it does: the value of cojob_max_no_yield_time_us, the time a job may run
	// without yielding before a warning
	virtual uint32 GetMaxNoYieldTimeMicroseconds() = 0;
	// Returns 0
	virtual int Unk_6() = 0;

	virtual ~CCoJob() {}

	// The body of the job, run in its coroutine when it was not started from a network
	// message; returns true by default. Returning false marks the job failed.
	virtual bool BYieldingRunJob() = 0;
	// The same for a job started from a network message (the first held packet); returns true
	// by default
	virtual bool BYieldingRunJobFromMsg( void *pNetPacket ) = 0;
	// Returns 4
	virtual uint32 CHeartbeatsBeforeTimeout() = 0;

public:
	// Given by the job manager.
	uint64 m_JobID;
	uint32 m_bRunFromMsg : 1;
	// True if the work item the job waited on was canceled.
	uint32 m_bWorkItemCanceled : 1;
	uint32 m_bIsLongRunning : 1;
	// The message the job waits for.
	uint32 m_unWaitMsgType;
	// Zeroed by the constructor; nothing in the game reads it.
	uint32 m_nReserved;
	// Failures reported in the job statistics.
	union
	{
		struct
		{
			uint32 m_bJobFailed : 1;
			uint32 m_bLocksFailed : 1;
			uint32 m_bLocksLongHeld : 1;
			uint32 m_bLocksLongWait : 1;
			uint32 m_bWaitTimeout : 1;
			uint32 m_bLongInterYield : 1;
			uint32 m_bTimeoutNetMsg : 1;
			uint32 m_bTimeoutOther : 1;
			uint32 m_uUnused : 24;
		} m_bits;
		uint32 m_uFlags;
	} m_flags;
	int32 m_cLocksAttempted;
	int32 m_cLocksWaitedFor;
	// 0 active, 1 not started, 2 network message, 3 sleep for time, 4 waiting for a lock,
	// 5 yielding, 6 SQL, 7 work item, 8 job, 9 manual.
	int32 m_ePauseReason;
	const char *m_pszPauseResourceName;
	// The static description of the job type; its first member is the name.
	const void *m_pJobType;
	// The name of the job; the type name or the class name when empty.
	const char *m_pchJobName;
	// Reference counted packets (the count at +8, released through slot 1) held until the
	// job ends.
	CUtlVectorFixedGrowable< void *, 1 > m_vecNetPackets;
	HCoroutine m_hCoroutine;
	// Job times in microseconds.
	uint64 m_STimeStarted;
	uint64 m_STimeSwitched;
	uint64 m_STimeNextHeartbeat;
	// The time the job runs without yielding.
	CFastTimer m_FastTimerDelta;
	// Named by what the code does: zeroed by the constructor and every time the job
	// continues; nothing reads it.
	uint32 m_nContinueResetCount;
	CCycleCount m_cyclecountTotal;
	// The job that ran when this one continued.
	CCoJob *m_pJobPrev;
	// The locks held; a lock keeps the job that holds it at +8 and its name at +32.
	CUtlVectorFixedGrowable< void *, 2 > m_vecLocks;
	// The lock the job waits for, and where it was requested.
	void *m_pWaitingOnLock;
	const char *m_pWaitingOnLockFilename;
	int32 m_waitingOnLockLine;
	// Named by what it holds: a third string recorded with the file and the line (empty until
	// set) and copied to the lock with them; nothing reads it.
	const char *m_pWaitingOnLockContext;
	// Another job that wants a lock this one holds.
	CCoJob *m_pJobToNotifyOnLockRelease;
	// The work item the job waits for; the job waits for its byte at +32 to clear.
	void *m_pWaitingOnWorkItem;
	uint64 m_jobIDWaitingOn;
	// The jobs waiting for this one to finish, linked through m_pJobWaitingOnNext.
	CCoJob *m_pFirstJobWaitingOnThis;
	CCoJob *m_pJobWaitingOnNext;
	// Named by what they hold: the warning thresholds in milliseconds for the time between
	// yields and for the exclusive time the job ran ("EXCEEDED TIMEOUT THRESHOLD"); 0
	// disables them.
	uint32 m_nYieldTimeThresholdMs;
	uint32 m_nExclusiveTimeThresholdMs;
};

#endif // COJOB_H
