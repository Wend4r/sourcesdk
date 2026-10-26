#ifndef ILOADINGSPAWNGROUP_H
#define ILOADINGSPAWNGROUP_H

#pragma once

#include "spawngrouptypes.h"
#include "entity2/entityidentity.h"
#include "mathlib/mathlib.h"
#include "tier0/utlvector.h"

class CKV3Arena;
class CEntityKeyValues;
class CEntityInstance;
class ISaveRestoreFileData;
class IStreamEntitiesFromFileCompletion;
struct EntitySpawnInfo_t;

// Entities of a spawn group that is being loaded, created from the map or from a save file
abstract_class ILoadingSpawnGroup
{
public:
	virtual int EntityCount() const = 0;
	virtual const EntitySpawnInfo_t *GetEntities() const = 0;
	virtual bool ShouldLoadEntitiesFromSave() const = 0;
	virtual const ISaveRestoreFileData &GetSaveRestoreFileData() const = 0;
	virtual void SetLevelTransitionPreviousMap( const char *pLevelTransitionMap, const char *pLandmarkName ) = 0;
	virtual bool IsLevelTransition() const = 0;
	virtual const char *GetLevelTransitionPreviousMap() const = 0;
	virtual const char *GetLevelTransitionLandmarkName() const = 0;
	virtual CKV3Arena *GetEntityKeyValuesAllocator() = 0;

	// nSaveEntityTableIndex is EntitySpawnInfo_t::m_nSaveEntityTableIndex, -1 for map entities
	virtual CEntityInstance *CreateEntityToSpawn( SpawnGroupHandle_t hSpawnGroup, const matrix3x4a_t &vecSpawnOffset, CreateSpawnGroupType_t createType, const CEntityKeyValues *pEntityKeyValues, int nSaveEntityTableIndex ) = 0;
	virtual void SpawnEntities() = 0;

	// Streams the entities of "save/<pszSpawnGroupName>.hl1" from the save restore file data, then destroys the precached entities the save file did not use
	virtual void RestoreEntities( SpawnGroupHandle_t hSpawnGroup, const char *pszSpawnGroupName, IStreamEntitiesFromFileCompletion *pCompletion, const Vector &vecSpawnOffset, CUtlVector< CEntityHandle > *pRestoredEntities ) = 0;

	// Whether the save restore file data holds "save/<pszSpawnGroupName>.hl1"
	virtual bool HasSaveRestoreFile( SpawnGroupHandle_t hSpawnGroup, const char *pszSpawnGroupName ) const = 0;

	// Destroys the entities that were not spawned and deletes the loading spawn group
	virtual void Release() = 0;

	// Set when the spawn group loads its entities from the save instead of the map file ("suppressing entities from map file")
	virtual void SetSuppressMapEntities( bool bSuppress ) = 0;
	virtual bool ShouldSuppressMapEntities() const = 0;

	// Writable GetEntities(), for the entity save restore block handler
	virtual EntitySpawnInfo_t *GetMutableEntities() = 0;
};

#endif // ILOADINGSPAWNGROUP_H
