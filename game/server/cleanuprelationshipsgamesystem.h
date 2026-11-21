//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Frees the default relationship table of the NPC classes on shutdown.
//
//=============================================================================//

#ifndef CLEANUPRELATIONSHIPSGAMESYSTEM_H
#define CLEANUPRELATIONSHIPSGAMESYSTEM_H
#ifdef _WIN32
#pragma once
#endif

#include "igamesystem.h"

//-----------------------------------------------------------------------------
// Registered as "CCleanupRelationshipsGameSystem" (static factory). It has no members of its own
// and adds no virtual methods to IGameSystem.
//-----------------------------------------------------------------------------
class CCleanupRelationshipsGameSystem : public CAutoGameSystem
{
public:
	DECLARE_GAME_SYSTEM();

	CCleanupRelationshipsGameSystem();
	virtual ~CCleanupRelationshipsGameSystem();

	// Deletes the 13 rows of the global default relationship table and the table itself
	virtual void Shutdown() override;

	// Forward to the game system factory
	virtual void SetGameSystemGlobalPtrs( void *pValue ) override;
	virtual bool DoesGameSystemReallocate() override;
};

#endif // CLEANUPRELATIONSHIPSGAMESYSTEM_H
