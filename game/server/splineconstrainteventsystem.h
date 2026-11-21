//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Checks the spline constraints on a job during the async post tick work and fires
// their events on the main thread.
//
//=============================================================================//

#ifndef SPLINECONSTRAINTEVENTSYSTEM_H
#define SPLINECONSTRAINTEVENTSYSTEM_H
#ifdef _WIN32
#pragma once
#endif

#include "igamesystem.h"

#include <tier0/utlvector.h>
#include <tier0/utlleanvector.h>

#include <functional>

class CSplineConstraint;
class CJob;

//-----------------------------------------------------------------------------
// Registered as "CSplineConstraintEventSystem" (static factory). 72 bytes on both platforms. No
// source has the class; the names are named by what the code does.
//-----------------------------------------------------------------------------
class CSplineConstraintEventSystem : public CAutoGameSystem
{
public:
	DECLARE_GAME_SYSTEM();

	virtual ~CSplineConstraintEventSystem();

	// Queues the "Spline Events" job (m_pJob) on the thread pool when there are constraints
	GS_EVENT( ServerGameBeginAsyncPostTickWork );
	// Waits for m_pJob and releases it
	GS_EVENT( ServerPreEndAsyncPostTickWork );
	// Calls and destroys every function of m_DeferredEvents
	GS_EVENT( ServerPostEndAsyncPostTickWork );

	// Forward to the game system factory
	virtual void SetGameSystemGlobalPtrs( void *pValue ) override;
	virtual bool DoesGameSystemReallocate() override;

public:
	// The spline constraints the job checks (their physics joint against the spline entity)
	CUtlVector< CSplineConstraint * > m_Constraints;
	// Reference counted; null outside the async post tick work
	CJob *m_pJob;
	// Queued by the job, run on the main thread
	CUtlLeanVector< std::function< void() >, int > m_DeferredEvents;
};

#endif // SPLINECONSTRAINTEVENTSYSTEM_H
