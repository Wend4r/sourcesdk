//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Updates the volumes of the smoke grenades a few at a time.
//
//=============================================================================//

#ifndef CSTRIKE15_SMOKEVOLUMESYSTEM_H
#define CSTRIKE15_SMOKEVOLUMESYSTEM_H
#ifdef _WIN32
#pragma once
#endif

#include "igamesystem.h"

#include <entity2/entityidentity.h>
#include <tier0/utlqueue.h>

//-----------------------------------------------------------------------------
// Registered as "SmokeVolumeSystem" (static factory). 48 bytes on both platforms. No source has
// the class; the names are named by what the code does.
//-----------------------------------------------------------------------------
class CSmokeVolumeSystem : public CAutoGameSystem
{
public:
	DECLARE_GAME_SYSTEM();

	virtual ~CSmokeVolumeSystem();

	// Both mark m_UpdateQueue empty (head and tail -1)
	GS_EVENT( GameInit );
	GS_EVENT( GameShutdown );
	// Empty
	GS_EVENT( ServerPreEntityThink );
	// Refills m_UpdateQueue with every smoke grenade projectile when it is empty, then updates the
	// volumes of the queued grenades until the per-frame budget is used
	GS_EVENT( ServerPostEntityThink );
	// Empty
	GS_EVENT( ServerPostSimulate );

	// Forward to the game system factory
	virtual void SetGameSystemGlobalPtrs( void *pValue ) override;
	virtual bool DoesGameSystemReallocate() override;

public:
	// The smoke grenade projectiles whose volume is still to be updated
	CUtlQueue< CEntityHandle > m_UpdateQueue;
};

#endif // CSTRIKE15_SMOKEVOLUMESYSTEM_H
