//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The game system that keeps the entities receiving flashlight light
//			(factory "FlashlightReceiverManager"). No Valve path names this
//			header.
//
//=============================================================================//

#ifndef FLASHLIGHTRECEIVER_MANAGER_H
#define FLASHLIGHTRECEIVER_MANAGER_H
#ifdef _WIN32
#pragma once
#endif

#include "igamesystem.h"
#include "entity2/ientitylistener.h"
#include "tier0/utlvector.h"

// Interface of the entities lit by flashlights (the manager finds it with a
// dynamic_cast from CBaseEntity)
class IFlashlightReceiver;

//-----------------------------------------------------------------------------
// Static instance, 56 bytes on both platforms. libserver.so repeats the two
// IEntityListener overrides at the end of the main table (slots 66 and 67).
//-----------------------------------------------------------------------------
class CFlashlightReceiver_Manager : public CAutoGameSystem, public IEntityListener
{
public:
	DECLARE_GAME_SYSTEM();

	// Add and remove this system as a listener of the entity system
	GS_EVENT( GameInit );
	GS_EVENT( GameShutdown );

	// Forward to the game system factory
	virtual void SetGameSystemGlobalPtrs( void *pValue ) override;
	virtual bool DoesGameSystemReallocate() override;

	// IEntityListener: add a spawned and remove a deleted IFlashlightReceiver
	virtual void OnEntitySpawned( CEntityInstance *pEntity ) override;
	virtual void OnEntityDeleted( CEntityInstance *pEntity ) override;

public:
	// Named by what it holds: the IFlashlightReceiver subobjects of the spawned entities
	CUtlVector< IFlashlightReceiver * > m_Receivers;
};

#endif // FLASHLIGHTRECEIVER_MANAGER_H
