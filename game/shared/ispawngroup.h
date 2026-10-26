#ifndef ISPAWNGROUP_H
#define ISPAWNGROUP_H

#pragma once

#include "spawngroupdesc.h"
#include "entity2/entityidentity.h"
#include "mathlib/mathlib.h"
#include "resourcefile/resourcetype.h"
#include "tier0/utlstring.h"
#include "tier0/utlvector.h"

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
	// SpawnGroupDesc_t::m_bSuppressAllEntities of the descriptor the spawn group was created with.
	virtual bool ShouldSuppressAllEntities() const = 0;
	virtual SpawnGroupHandle_t GetParentSpawnGroup() const = 0;
	virtual uint32 GetChildSpawnGroupCount() const = 0;
	virtual void GetSpawnGroupDesc(SpawnGroupDesc_t *pDesc) const = 0;

public:
	virtual void FlagManualCreation() = 0;
	virtual bool HasManualCreation() = 0;
	// Appends the resource names of every manifest of the spawn group
	virtual void GetResourceNames(CUtlVector<CUtlString> *pResourceNames) = 0;
	virtual void AddSpawnGroupChild(SpawnGroupHandle_t hSpawnGroup) = 0;
	virtual bool HasSetActivePostLoad() const = 0;
	// Updated by the wait for the child spawn groups: the spawn group and its children reached
	// SPAWN_GROUP_READY_TO_SPAWN_ENTITIES
	virtual bool IsReadyToSpawnEntities() const = 0;
	// SpawnGroupDesc_t::m_bSaveEntities of the descriptor the spawn group was created with.
	virtual bool ShouldSaveEntities() const = 0;
	// Sets what GetDestructionTick returns on client spawn groups; empty on server ones.
	virtual void SetDestructionTick(uint32 nTick) = 0;
	virtual bool HasLevelTransition() const = 0;
	virtual WorldGroupId_t GetWorldGroupId() const = 0;
};

#endif // ISPAWNGROUP_H
