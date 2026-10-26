#ifndef SPAWNGROUPMGR_H
#define SPAWNGROUPMGR_H

#pragma once

#include "spawngrouptypes.h"
#include "basespawngroup.h"
#include "iloadingspawngroup.h"
#include "saverestorefiledata.h"
#include "kv3lib/keyvalues3.h"
#include "ispawngroup.h"
#include "mapspawngroup.h"
#include "igamesystem.h"
#include "entity2/entitysystem.h"
#include "resourcefile/igameresourcemanifestloadcompletion.h"
#include "tier0/threadtools.h"
#include "tier0/utlstring.h"
#include "tier0/convar.h"
#include "tier0/utlmap.h"
#include "tier0/utlvector.h"
#include "worldrenderer/icomputeworldorigin.h"

abstract_class IGameSpawnGroupMgr
{
public:
	virtual void AllocateSpawnGroup(SpawnGroupHandle_t h, ISpawnGroup *pSpawnGroup) = 0;
	virtual void ReleaseSpawnGroup(SpawnGroupHandle_t h) = 0;
	virtual void SpawnGroupInit(SpawnGroupHandle_t handle, IEntityResourceManifest *pManifest, IEntityPrecacheConfiguration *pConfig, ISpawnGroupPrerequisiteRegistry *pRegistry) = 0;
	virtual ILoadingSpawnGroup *CreateLoadingSpawnGroup(SpawnGroupHandle_t handle, bool bSynchronouslySpawnEntities, bool bConfirmResourcesLoaded, const CUtlVector<const CEntityKeyValues *> *pKeyValues) = 0;
	virtual ILoadingSpawnGroup *CreateLoadingSpawnGroupForSaveFile( SpawnGroupHandle_t handle, const char *pszSaveFileName, const void *pSaveData, int nSaveDataSize ) = 0;
	virtual void SpawnGroupSpawnEntities(SpawnGroupHandle_t handle) = 0;
	virtual void SpawnGroupShutdown(SpawnGroupHandle_t handle) = 0;
	virtual void SetActiveSpawnGroup(SpawnGroupHandle_t handle) = 0;
	virtual void UnloadSpawnGroup(SpawnGroupHandle_t hSpawnGroup, bool bSaveEntities) = 0;
	virtual SpawnGroupHandle_t GetParentSpawnGroup(SpawnGroupHandle_t hSpawnGroup) = 0;
	virtual int GetChildSpawnGroupCount(SpawnGroupHandle_t handle) = 0;
	virtual SaveRestoreDataHandle_t SaveGame_Start(const char *pSaveName, const char *pOldLevel, const char *pszLandmarkName) = 0;
	virtual bool SaveGame_Finalize(SaveRestoreDataHandle_t hSaveRestore) = 0;
	virtual bool StreamEntitiesToFile(SaveRestoreDataHandle_t hSaveRestore) = 0;
	virtual const char *SaveGame_GetLastSaveFile() = 0;
	virtual bool IsGameReadyToSave() = 0;
	virtual void DumpSpawnGroups() = 0;
	virtual LoggingChannelID_t GetSaveRestoreLogChannel() = 0;
	virtual CUtlString GetSaveRestoreContextString() const = 0;
	// AMNOTE: Dispatch the IGameSystem WorldGroupCreated and WorldGroupDestroyed events
	virtual void OnWorldGroupCreated(WorldGroupId_t hWorldGroup) = 0;
	virtual void OnWorldGroupDestroyed(WorldGroupId_t hWorldGroup) = 0;
	virtual CUtlString GetName() const = 0;
};

// Created by IGameSpawnGroupMgr::CreateLoadingSpawnGroup and CreateLoadingSpawnGroupForSaveFile
class CLoadingSpawnGroup : public ILoadingSpawnGroup
{
public:
	virtual int EntityCount() const override = 0;
	virtual const EntitySpawnInfo_t *GetEntities() const override = 0;
	virtual bool ShouldLoadEntitiesFromSave() const override = 0;
	virtual const ISaveRestoreFileData &GetSaveRestoreFileData() const override = 0;
	virtual void SetLevelTransitionPreviousMap( const char *pLevelTransitionMap, const char *pLandmarkName ) override = 0;
	virtual bool IsLevelTransition() const override = 0;
	virtual const char *GetLevelTransitionPreviousMap() const override = 0;
	virtual const char *GetLevelTransitionLandmarkName() const override = 0;
	virtual CKV3Arena *GetEntityKeyValuesAllocator() override = 0;
	virtual CEntityInstance *CreateEntityToSpawn( SpawnGroupHandle_t hSpawnGroup, const matrix3x4a_t &vecSpawnOffset, CreateSpawnGroupType_t createType, const CEntityKeyValues *pEntityKeyValues, int nSaveEntityTableIndex ) override = 0;
	virtual void SpawnEntities() override = 0;
	virtual void RestoreEntities( SpawnGroupHandle_t hSpawnGroup, const char *pszSpawnGroupName, IStreamEntitiesFromFileCompletion *pCompletion, const Vector &vecSpawnOffset, CUtlVector< CEntityHandle > *pRestoredEntities ) override = 0;
	virtual bool HasSaveRestoreFile( SpawnGroupHandle_t hSpawnGroup, const char *pszSpawnGroupName ) const override = 0;
	virtual void Release() override = 0;
	virtual void SetSuppressMapEntities( bool bSuppress ) override = 0;
	virtual bool ShouldSuppressMapEntities() const override = 0;
	virtual EntitySpawnInfo_t *GetMutableEntities() override = 0;

public:
	SpawnGroupHandle_t m_hSpawnGroup;
	CUtlVector< EntitySpawnInfo_t > m_spawnInfo;
	CKV3Arena m_variantMemory; // GetEntityKeyValuesAllocator
	CSaveRestoreFileData m_SaveRestoreFileData; // The files of a save, CreateLoadingSpawnGroupForSaveFile adds one
	CUtlString m_sLevelTransitionPreviousMap;
	CUtlString m_sLevelTransitionLandmarkName;
	bool m_bSuppressMapEntities;
};

class CSpawnGroupMgrGameSystem : public IGameSpawnGroupMgr, public CBaseGameSystem
{
	DECLARE_GAME_SYSTEM();

public:
	virtual void SetGameSystemGlobalPtrs(void *pValue) override = 0;
	virtual bool DoesGameSystemReallocate() override = 0;
	virtual void QueueSpawnEntityCalls() = 0;
	virtual void FlushQueuedSpawnEntityCalls() = 0;
	virtual void FrameBoundary() = 0;

	struct UnloadRequestQueueItem_t
	{
		SpawnGroupHandle_t m_hSpawnGroup;
		bool m_bSaveEntities;
	};

public:
	CConCommandMemberAccessor<CSpawnGroupMgrGameSystem> m_OnSpawnGroupActivate; // "spawn_group_activate"
	CConCommandMemberAccessor<CSpawnGroupMgrGameSystem> m_OnSpawnGroupLoad; // "spawn_group_load"
	CConCommandMemberAccessor<CSpawnGroupMgrGameSystem> m_OnSpawnGroupUnload; // "spawn_group_unload"
	CUtlVector<SpawnGroupHandle_t> m_SpawnEntitiesCallQueue; // 224
	bool m_bQueueSpawnEntitiesCalls; // 248
	CUtlOrderedMap<SpawnGroupHandle_t, CMapSpawnGroup *, CDefLess<SpawnGroupHandle_t>, unsigned short> m_SpawnGroups; // 256
	CThreadRWLock_FastRead m_hHandleMutex; // 288
	CUtlVector<UnloadRequestQueueItem_t> m_QueuedUnloadRequests; // W 352 | L 696
	CUtlString m_sName;
};
#ifdef _WIN32
COMPILE_TIME_ASSERT(sizeof(CSpawnGroupMgrGameSystem) == 384);
#elif defined(POSIX)
COMPILE_TIME_ASSERT(sizeof(CSpawnGroupMgrGameSystem) == 728);
#endif

#endif // SPAWNGROUPMGR_H
