#ifndef SPAWNGROUPTYPES_H
#define SPAWNGROUPTYPES_H

#pragma once

#include "tier0/platform.h"
#include "tier0/utlstringtoken.h"
#include "engine2/isource2engine.h"

#define MAX_SPAWN_GROUP_WORLD_NAME_LENGTH 4096

#define INVALID_SPAWNGROUP_HANDLE ( ( SpawnGroupHandle_t ) -1 )

typedef uint32 SpawnGroupHandle_t;
// AMNOTE: Index into the engine's world group table, with the high bit set for client-side groups; -1 is invalid
class WorldGroupId_t
{
public:
	WorldGroupId_t( uint32 nId = 0 ) : m_nId( nId ) {}

	bool operator==( const WorldGroupId_t &other ) const { return m_nId == other.m_nId; }
	bool operator!=( const WorldGroupId_t &other ) const { return m_nId != other.m_nId; }
	bool operator<( const WorldGroupId_t &other ) const { return m_nId < other.m_nId; }

	uint32 m_nId;
};

enum SpawnGroupState_t
{
	SPAWN_GROUP_ALLOCATED = 0,
	SPAWN_GROUP_WORLD_LOADED,
	SPAWN_GROUP_ENTITIES_ALLOCATED,
	SPAWN_GROUP_GAMESYSTEMS_PRECACHE_START,
	SPAWN_GROUP_GAMESYSTEMS_PRECACHE_END,
	SPAWN_GROUP_ENTITY_PRECACHE_START,
	SPAWN_GROUP_ENTITY_PRECACHE_END,
	SPAWN_GROUP_READY_TO_SPAWN_ENTITIES,
	SPAWN_GROUP_ENTITIES_SPAWNED
};

enum CreateSpawnGroupType_t
{
	CREATE_SPAWN_GROUP_IMMEDIATELY = 0,
	CREATE_SPAWN_GROUP_ASYNCHRONOUSLY,
	CREATE_SPAWN_GROUP_ASYNCHRONOUSLY_CONFIRM_RESOURCES_LOADED,
};

enum ESpawnGroupUnloadOption
{
	kSGUO_None = 0,
	kSGUO_SaveEntities,
	kSGUO_MergedIntoOwner,
};

enum SpawnGroupEntityFilterType_t
{
	SPAWN_GROUP_ENTITY_FILTER_FALLBACK = 0,
	SPAWN_GROUP_ENTITY_FILTER_MOD_SPECIFIC,
};

#endif // SPAWNGROUPTYPES_H
