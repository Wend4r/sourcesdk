//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The game system that tracks the game_ragdoll_manager entity of each
//			spawn group (factory "RagdollGameSystem"). No Valve path names this header.
//
//=============================================================================//

#ifndef RAGDOLL_MANAGER_H
#define RAGDOLL_MANAGER_H
#ifdef _WIN32
#pragma once
#endif

#include "igamesystem.h"
#include "tier0/utlvector.h"

// The game_ragdoll_manager entity
class CRagdollManager;

//-----------------------------------------------------------------------------
// Reallocating game system, 56 bytes on both platforms. The fields are named by
// what the code does with them.
//-----------------------------------------------------------------------------
class CRagdollGameSystem : public CAutoGameSystem
{
public:
	DECLARE_GAME_SYSTEM();

	// Adds an entry for the spawn group to m_SpawnGroupManagers
	GS_EVENT( PreSpawnGroupLoad );
	// Removes the spawn group's entry
	GS_EVENT( PostSpawnGroupLoad );
	// Sets m_bDisableRagdolls and finds the game_ragdoll_manager of the new active spawn group
	GS_EVENT( ActiveSpawnGroupChanged );

	// Forward to the game system factory
	virtual void SetGameSystemGlobalPtrs( void *pValue ) override;
	virtual bool DoesGameSystemReallocate() override;

public:
	struct SpawnGroupRagdollManager_t
	{
		SpawnGroupHandle_t m_hSpawnGroup;
		// Set when a ragdoll manager of that spawn group spawns, cleared when it is deleted
		CRagdollManager *m_pRagdollManager;
	};

	// Set from a command line switch and cleared when the game rules allow ragdolls; the
	// ragdoll limit is zero while it is set
	bool m_bDisableRagdolls;
	// The ragdoll manager of the active spawn group
	CRagdollManager *m_pActiveRagdollManager;
	// The spawn groups that are loading, with the ragdoll manager each one spawned
	CUtlVector< SpawnGroupRagdollManager_t > m_SpawnGroupManagers;
};

#endif // RAGDOLL_MANAGER_H
