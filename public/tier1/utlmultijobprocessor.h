//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Runs a batch of jobs on the thread pool and waits for all of them.
//
//=============================================================================//

#ifndef UTLMULTIJOBPROCESSOR_H
#define UTLMULTIJOBPROCESSOR_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "tier0/threadtools.h"

//-----------------------------------------------------------------------------
// Linked statically into every module (utlmultijobprocessor.cpp). 32 bytes on Linux, 40
// on Windows (CThreadMultiWaitEvent is 8 / 16 bytes). No virtual destructor: the only virtual is
// AtFinish (one slot on both platforms).
//-----------------------------------------------------------------------------
class CUtlMultiJobProcessor
{
public:
	CUtlMultiJobProcessor();
	~CUtlMultiJobProcessor();

	// Called once the last job has finished
	virtual void AtFinish() {}

	void RegisterUnfinishedJob();
	void MarkJobFinished();
	void EndSubmittingInitialJobs();
	void WaitForCompletion();
	bool WaitForCompletionRunMainThreadJobs( unsigned int nTimeout );
	void DisableThreading( bool bDisable );
	void SetUseCachedWaitSet( bool bUse );

public:
	// Manual-reset event, set while no job is unfinished
	CThreadMultiWaitEvent m_jobsFinishedEvent;
	// Named by what it does: the wait set of m_jobsFinishedEvent built once when
	// m_bUseCachedWaitSet is on, destroyed by the destructor
	ThreadMultiWaitObjectSet_t *m_pCachedWaitSet;
	CInterlockedInt m_nUnfinishedJobsCount;
	bool m_bDisableThreading;
	bool m_bUseCachedWaitSet;
};

#endif // UTLMULTIJOBPROCESSOR_H
