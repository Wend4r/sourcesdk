#ifndef ILOADINGSPAWNGROUP_H
#define ILOADINGSPAWNGROUP_H

#pragma once

#include "spawngrouptypes.h"
#include "entity2/entityidentity.h"
#include "mathlib/mathlib.h"

class CKV3Arena;
class CEntityKeyValues;
class CEntityInstance;
class CUtlBuffer;
struct EntitySpawnInfo_t;
class ISpawnGroup;

abstract_class ILoadingSpawnGroup
{
public:
	virtual int EntityCount() const = 0;
	virtual const EntitySpawnInfo_t *GetEntities() const = 0;
	virtual bool ShouldLoadEntitiesFromSave() const = 0;
	virtual const CUtlBuffer *GetSaveRestoreFileData() const = 0;
	virtual void SetLevelTransitionPreviousMap(const char *pLevelTransitionMap, const char *pLandmarkName) = 0;
	virtual bool IsLevelTransition() const = 0;
	virtual const char *GetLevelTransitionPreviousMap() const = 0;
	virtual const char *GetLevelTransitionLandmarkName() const = 0;
	virtual CKV3Arena *GetEntityKeyValuesAllocator() = 0;
	// nSaveFileEntityIndex becomes EntitySpawnInfo_t::m_nSaveFileEntityIndex: -1 for an entity of the map,
	// the index the save restore system gives an entity it restores from a save file
	virtual CEntityInstance *CreateEntityToSpawn(SpawnGroupHandle_t hSpawnGroup, const matrix3x4a_t &vecSpawnOffset, CreateSpawnGroupType_t createType, const CEntityKeyValues *pEntityKeyValues, int nSaveFileEntityIndex) = 0;
	// Releases the queued key values and runs the queued creation of the entity system
	virtual void SpawnEntities() = 0;
	// "CLoadingSpawnGroup::RestoreEntities": the save restore system restores the entities of the spawn group;
	// pnRestoredCount receives their number. pRestoreContext is passed through to the save restore system
	virtual void RestoreEntities(SpawnGroupHandle_t hSpawnGroup, ISpawnGroup *pSpawnGroup, void *pRestoreContext, const matrix3x4a_t &vecSpawnOffset, int *pnRestoredCount) = 0;
	// Named by what it does: asks the save restore system whether the save file holds entities of the spawn group; ShouldLoadEntitiesFromSave calls it
	virtual bool HasSaveFileEntities(SpawnGroupHandle_t hSpawnGroup, ISpawnGroup *pSpawnGroup) = 0;
	virtual void Release() = 0;
	// The spawn group manager does not spawn the entities of the map file while it is set ("suppressing entities from map file")
	virtual void SetSuppressMapEntities(bool bSuppress) = 0;
	virtual bool ShouldSuppressMapEntities() const = 0;
	// Named by what it does: the same array as GetEntities, not const
	virtual EntitySpawnInfo_t *GetMutableEntities() = 0;
};

#endif // ILOADINGSPAWNGROUP_H
