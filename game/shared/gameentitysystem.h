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
#include "tier0/utlvector.h"
#include "spawngrouptypes.h"

class CEntityKeyValues;
class CKV3Arena;

//-----------------------------------------------------------------------------
// Picks the entities of a spawn group that are spawned. The filters register under a name with a
// SpawnGroupEntityFilterType_t ("mapload", "cs_respawn", "shared_environment" and
// "shared_environment_preview_lighting" in the server); a spawn group names its filter.
//-----------------------------------------------------------------------------
abstract_class ISpawnGroupEntityFilter
{
public:
	virtual ~ISpawnGroupEntityFilter() {}

	// Appends the key values of the entities to spawn from pInKeyValues to outKeyValues.
	// pArena is the key values arena of the loading spawn group; a filter that changes an entity
	// allocates the copy there.
	virtual void FilterSpawnGroupEntities( SpawnGroupHandle_t hSpawnGroup, bool bIsPrecaching, const CUtlVector< const CEntityKeyValues * > *pInKeyValues, CKV3Arena *pArena, CUtlVector< const CEntityKeyValues * > &outKeyValues ) = 0;
};

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
