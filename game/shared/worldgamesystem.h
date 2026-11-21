//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Keeps the world settings (bounds, prop screen widths, start colour) of every spawn group
// and world group.
//
//=============================================================================//

#ifndef WORLDGAMESYSTEM_H
#define WORLDGAMESYSTEM_H
#ifdef _WIN32
#pragma once
#endif

#include "igamesystem.h"
#include "spawngrouptypes.h"

#include <tier0/utlhashtable.h>
#include <mathlib/vector.h>
#include <color.h>

//-----------------------------------------------------------------------------
// The settings of one spawn group's world (52 bytes). Fields without a known name are named
// after their keys in the world's KeyValues.
//-----------------------------------------------------------------------------
struct WorldInfo_t
{
	// Filled from the engine's bounds of the world group
	Vector m_WorldMins;
	Vector m_WorldMaxs;
	int m_nRefCount;
	// "update_nav_obstacle_splits_only_on_final_tick"
	bool m_bUpdateNavObstacleSplitsOnlyOnFinalTick;
	// Named by what it holds: the world group of the spawn group
	WorldGroupId_t m_hWorldGroupId;
	// "maxpropscreenwidth", "minpropscreenwidth"
	float m_flMaxPropScreenSpaceWidth;
	float m_flMinPropScreenSpaceWidth;
	// "newunit", "startdark"
	bool m_bNewUnit;
	bool m_bStartDark;
	// "startcolor"; black with full alpha by default
	Color m_StartColor;
};

//-----------------------------------------------------------------------------
// The merged bounds of the worlds of one world group (32 bytes): the first fields of WorldInfo_t,
// the bounds grown with every world added. Named by what it holds.
//-----------------------------------------------------------------------------
struct WorldGroupInfo_t
{
	Vector m_WorldMins;
	Vector m_WorldMaxs;
	// The worlds of the group; the entry is deleted when it drops to 0
	int m_nRefCount;
	bool m_bUpdateNavObstacleSplitsOnlyOnFinalTick;
};

//-----------------------------------------------------------------------------
// Registered as "WorldGameSystem" (reallocating factory). 144 bytes on both platforms.
//-----------------------------------------------------------------------------
class CWorldGameSystem : public CAutoGameSystem
{
public:
	DECLARE_GAME_SYSTEM();

	virtual ~CWorldGameSystem();

	// Deletes every WorldInfo_t and WorldGroupInfo_t
	GS_EVENT( GameShutdown );
	// Empty
	GS_EVENT( GamePostInit );
	// Creates the WorldInfo_t of the spawn group from its world's settings
	GS_EVENT( PreSpawnGroupLoad );
	// Releases the spawn group's WorldInfo_t and its world group's WorldGroupInfo_t
	GS_EVENT( PostSpawnGroupUnload );
	// Applies the settings of the newly active spawn group (m_defaultWorldInfo when it has none)
	GS_EVENT( ActiveSpawnGroupChanged );

	// Forward to the game system factory
	virtual void SetGameSystemGlobalPtrs( void *pValue ) override;
	virtual bool DoesGameSystemReallocate() override;

public:
	// By spawn group
	CUtlHashtable< SpawnGroupHandle_t, WorldInfo_t * > m_worldInfo;
	// Named by what it holds: by world group
	CUtlHashtable< WorldGroupId_t, WorldGroupInfo_t * > m_worldGroupInfo;
	WorldInfo_t m_defaultWorldInfo;
};

#endif // WORLDGAMESYSTEM_H
