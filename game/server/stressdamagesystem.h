//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Applies the queued physics stress damage after the entities think.
//
//=============================================================================//

#ifndef STRESSDAMAGESYSTEM_H
#define STRESSDAMAGESYSTEM_H
#ifdef _WIN32
#pragma once
#endif

#include "igamesystem.h"

#include <entity2/entityidentity.h>
#include <tier0/utlvector.h>

//-----------------------------------------------------------------------------
// Registered as "CStressDamageSystem" (static factory). 48 bytes on both platforms. No source has
// the class; the names are named by what the code does.
//-----------------------------------------------------------------------------
class CStressDamageSystem : public CAutoGameSystem
{
public:
	DECLARE_GAME_SYSTEM();

	virtual ~CStressDamageSystem();

	// Empties m_QueuedEntities
	GS_EVENT( GameShutdown );
	// Applies the stress damage of every queued entity that still has a physics object, then
	// empties m_QueuedEntities
	GS_EVENT( ServerPostEntityThink );

	// Forward to the game system factory
	virtual void SetGameSystemGlobalPtrs( void *pValue ) override;
	virtual bool DoesGameSystemReallocate() override;

public:
	CUtlVector< CEntityHandle > m_QueuedEntities;
};

#endif // STRESSDAMAGESYSTEM_H
