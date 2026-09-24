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
	virtual CEntityInstance *CreateEntityToSpawn(SpawnGroupHandle_t hSpawnGroup, const matrix3x4a_t &vecSpawnOffset, CreateSpawnGroupType_t createType, const CEntityKeyValues *pEntityKeyValues) = 0;
	virtual void SpawnEntities() = 0;
	virtual void Release() = 0;
};

#endif // ILOADINGSPAWNGROUP_H
