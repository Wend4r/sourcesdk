//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The game system that runs entity jobs on the thread pool
//			Factory "CGameJobSystem".
//
//=============================================================================//

#ifndef GAME_JOBS_H
#define GAME_JOBS_H
#ifdef _WIN32
#pragma once
#endif

#include "igamesystem.h"
#include "tier0/threadtools.h"
#include "tier0/utlvector.h"

// A queued job of an entity, allocated with new and deleted once its job is
// queued. On Linux: 72 bytes, the entity's handle at 0, the job function
// (std::function< void() >) at 8, the job's name (char[ 32 ]) at 40; a request of
// an entity that is gone or being deleted is dropped.
struct GameJobRequest_t;

// The thread pool job (IThreadPool::QueueJobWithFlags lambda job), reference counted
class CThreadedJob;

//-----------------------------------------------------------------------------
// Static instance, 112 bytes on both platforms (constant-initialized on
// Windows). The fields are named by what the code does with them.
//-----------------------------------------------------------------------------
class CGameJobSystem : public CAutoGameSystem
{
public:
	DECLARE_GAME_SYSTEM();

	// Waits for and releases m_ActiveJobs, then lets the engine's job queue run
	virtual void Shutdown() override;
	// Queues the requests, then waits for the jobs
	GS_EVENT( ClientPostUpdate );
	// Queues the requests as thread pool jobs
	GS_EVENT( ServerGameBeginAsyncPostTickWork );
	// Waits for and releases m_ActiveJobs
	GS_EVENT( ServerPreEndAsyncPostTickWork );
	// Run the engine's job queue for the frame time
	GS_EVENT( GameFrameBoundary );
	GS_EVENT( OutOfGameFrameBoundary );

	// Forward to the game system factory
	virtual void SetGameSystemGlobalPtrs( void *pValue ) override;
	virtual bool DoesGameSystemReallocate() override;

public:
	// Taken over (moved out) by the next queueing
	CUtlVector< GameJobRequest_t * > m_PendingRequests;
	// Appended to the pending requests by the queueing when the global flag at
	// CGlobalVars offset 84 is set
	CUtlVector< GameJobRequest_t * > m_DeferredRequests;
	// Constructed with the default 200 spin iterations; the queueing and waiting code does
	// not take it
	CThreadFastMutex m_Mutex;
	// The queued jobs (one reference each), waited for and released together
	CUtlVector< CThreadedJob * > m_ActiveJobs;
};

#endif // GAME_JOBS_H
