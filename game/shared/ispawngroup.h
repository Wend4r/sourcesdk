#ifndef ISPAWNGROUP_H
#define ISPAWNGROUP_H

#pragma once

#include "spawngroupdesc.h"
#include "entity2/entityidentity.h"
#include "mathlib/mathlib.h"
#include "resourcefile/resourcetype.h"
#include "tier0/utlstring.h"

class ILoadingSpawnGroup;
class IWorld;
class IWorldReference;
class IResourceManifestRegistry;
class IEntityPrecacheConfiguration;
class ISpawnGroupPrerequisiteRegistry;
class IGameSpawnGroupMgr;

abstract_class ISpawnGroup
{
public:
	virtual const char *GetWorldName() const = 0;
	virtual const char *GetEntityLumpName() const = 0;
	virtual const char *GetEntityFilterName() const = 0;
	virtual SpawnGroupHandle_t GetHandle() const = 0;
	virtual const matrix3x4a_t &GetWorldOffset() const = 0;
	virtual bool ShouldLoadEntitiesFromSave() const = 0;
	virtual const char *GetParentNameFixup() const = 0;
	virtual const char *GetLocalNameFixup() const = 0;
	virtual SpawnGroupHandle_t GetOwnerSpawnGroup() const = 0;
	virtual ILoadingSpawnGroup *GetLoadingSpawnGroup() const = 0;
	virtual void SetLoadingSpawnGroup(ILoadingSpawnGroup *pLoading) = 0;
	virtual IWorldReference *GetWorldReference() const = 0;
	virtual CUtlString Describe() const = 0;

public:
	virtual uint32 GetCreationTick() const = 0;
	virtual void RequestDeferredUnload(bool bAsync = true) = 0;
	virtual uint32 GetDestructionTick() const = 0;

public:
	virtual bool DontSpawnEntities() const = 0;
	// SpawnGroupDesc_t::m_bReservedFlag1.
	virtual bool GetReservedFlag1() const = 0;
	virtual SpawnGroupHandle_t GetParentSpawnGroup() const = 0;
	virtual uint32 GetChildSpawnGroupCount() const = 0;
	virtual void GetSpawnGroupDesc(SpawnGroupDesc_t *pDesc) const = 0;

public:
	virtual void FlagManualCreation() = 0;
	virtual bool HasManualCreation() = 0;
	virtual void LoadAllGameResourceManifests(void *) = 0;
	virtual void AddSpawnGroupChild(SpawnGroupHandle_t hSpawnGroup) = 0;
	virtual bool HasSetActivePostLoad() const = 0;
	// Named by what it does: set while the spawn group waits for its child spawn groups (the
	// WaitForChildSpawnGroups step of the network spawn groups).
	virtual bool IsWaitingForChildSpawnGroups() const = 0;
	// SpawnGroupDesc_t::m_bIsNotPointPrefab.
	virtual bool IsNotPointPrefab() const = 0;
	// Named by what it does: the client spawn group stores the tick GetDestructionTick returns.
	virtual void SetDestructionTick(uint32 nTick) = 0;
	virtual bool HasLevelTransition() const = 0;
	virtual WorldGroupId_t GetWorldGroupId() const = 0;

	// ComputeWorldOrigin and OnGameResourceManifestLoaded are not slots of this interface: the
	// game's spawn groups implement them in their IComputeWorldOriginCallback and
	// IGameResourceManifestLoadCompletionCallback bases (see CBaseSpawnGroup).
	virtual void Init(IResourceManifestRegistry *pResourceManifest, IEntityPrecacheConfiguration *pConfig, ISpawnGroupPrerequisiteRegistry *pRegistry) = 0;
	virtual void Shutdown() = 0;
	virtual bool GetLoadStatus() = 0;
	virtual void ForceBlockingLoad() = 0;
	virtual bool ShouldBlockUntilLoaded() const = 0;
	virtual void ServiceBlockingLoads() = 0;
	virtual bool GetEntityPrerequisites(HGameResourceManifest hManifest) = 0;
	virtual bool EntityPrerequisitesSatisfied() = 0;

public:
	virtual bool LoadEntities() = 0;
	virtual void SaveRestoreMap(ILoadingSpawnGroup *pLoading, SpawnGroupHandle_t hSpawnGroup, const matrix3x4a_t &vecSpawnOffset) = 0;
	virtual void SaveRestoreMap2(ILoadingSpawnGroup *pLoading, SpawnGroupHandle_t hSpawnGroup, const matrix3x4a_t &vecSpawnOffset) = 0;
	virtual void SetParentSpawnGroupForChild(SpawnGroupHandle_t hSpawnGroup) = 0;

public:
	virtual void TransferOwnershipOfManifestsTo(ISpawnGroup *pTarget) = 0;

public:
	virtual IGameSpawnGroupMgr *GetSpawnGroupMgr() const = 0;
};

#endif // ISPAWNGROUP_H
