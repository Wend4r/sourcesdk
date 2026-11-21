//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The game system of the ragdolls whose pose is controlled by the
//			physics (factory "CRagdollPoseControlSystem"). No Valve path names
//			this header.
//
//=============================================================================//

#ifndef RAGDOLL_POSECONTROLSYSTEM_H
#define RAGDOLL_POSECONTROLSYSTEM_H
#ifdef _WIN32
#pragma once
#endif

#include "igamesystem.h"
#include "ehandle.h"
#include "tier0/utlvector.h"

class CBaseEntity;

//-----------------------------------------------------------------------------
// Static instance, 48 bytes on both platforms.
//-----------------------------------------------------------------------------
class CRagdollPoseControlSystem : public CAutoGameSystem
{
public:
	DECLARE_GAME_SYSTEM();

	// Installs two callbacks with the physics world (pre and post simulate)
	GS_EVENT( GamePostInit );
	// Removes them and purges m_Ragdolls
	GS_EVENT( GamePreShutdown );

	// Forward to the game system factory
	virtual void SetGameSystemGlobalPtrs( void *pValue ) override;
	virtual bool DoesGameSystemReallocate() override;

public:
	// Named by what it holds: the ragdoll entities that registered for pose control; each
	// removes its own handle when it releases its physics
	CUtlVector< CHandle< CBaseEntity > > m_Ragdolls;
};

#endif // RAGDOLL_POSECONTROLSYSTEM_H
