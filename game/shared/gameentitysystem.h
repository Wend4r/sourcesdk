//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The game systems that bracket the lifetime of the game entity system.
//
//=============================================================================//

#ifndef GAMEENTITYSYSTEM_H
#define GAMEENTITYSYSTEM_H
#ifdef _WIN32
#pragma once
#endif

#include "igamesystem.h"

//-----------------------------------------------------------------------------
// Registered as "Source2EntitySystem" (reallocating factory). 24 bytes on both platforms: no
// fields besides the DECLARE_GAME_SYSTEM member, and it overrides no event.
//-----------------------------------------------------------------------------
class CSource2EntitySystem : public CAutoGameSystem
{
public:
	DECLARE_GAME_SYSTEM();

	virtual ~CSource2EntitySystem();

	// Forward to the game system factory
	virtual void SetGameSystemGlobalPtrs( void *pValue ) override;
	virtual bool DoesGameSystemReallocate() override;
};

//-----------------------------------------------------------------------------
// Registered as "Source2EntitySystemDeallocator" (reallocating factory). 24 bytes on both
// platforms: no fields besides the DECLARE_GAME_SYSTEM member, and it overrides no event.
//-----------------------------------------------------------------------------
class CSource2EntitySystemDeallocator : public CAutoGameSystem
{
public:
	DECLARE_GAME_SYSTEM();

	virtual ~CSource2EntitySystemDeallocator();

	// Forward to the game system factory
	virtual void SetGameSystemGlobalPtrs( void *pValue ) override;
	virtual bool DoesGameSystemReallocate() override;
};

#endif // GAMEENTITYSYSTEM_H
