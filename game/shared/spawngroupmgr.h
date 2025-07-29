#ifndef SPAWNGROUPMGR_H
#define SPAWNGROUPMGR_H

#pragma once

#include "spawngrouptypes.h"

abstract_class IGameSpawnGroupMgr
{
public:
	virtual void AllocateSpawnGroup(SpawnGroupHandle_t h, ISpawnGroup *pSpawnGroup) = 0;
	virtual void ReleaseSpawnGroup(SpawnGroupHandle_t h) = 0;
	virtual void SpawnGroupInit(SpawnGroupHandle_t handle, IEntityResourceManifest *pManifest, IEntityPrecacheConfiguration *pConfig, ISpawnGroupPrerequisiteRegistry *pRegistry) = 0;
	virtual ILoadingSpawnGroup *CreateLoadingSpawnGroup(SpawnGroupHandle_t handle, bool bSynchronouslySpawnEntities, bool bConfirmResourcesLoaded, const CUtlVector<const CEntityKeyValues *> *pKeyValues) = 0;
	virtual ILoadingSpawnGroup *CreateLoadingSpawnGroupForSaveFile(SpawnGroupHandle_t handle, const void *pSaveData, size_t nSaveDataSize) = 0;
	virtual void SpawnGroupSpawnEntities(SpawnGroupHandle_t handle) = 0;
	virtual void SpawnGroupShutdown(SpawnGroupHandle_t handle) = 0;
	virtual void SetActiveSpawnGroup(SpawnGroupHandle_t handle) = 0;
	virtual void UnloadSpawnGroup(SpawnGroupHandle_t hSpawnGroup, bool bSaveEntities) = 0;
	virtual SpawnGroupHandle_t GetParentSpawnGroup(SpawnGroupHandle_t hSpawnGroup) = 0;
	virtual int GetChildSpawnGroupCount(SpawnGroupHandle_t handle) = 0;
	virtual SaveRestoreDataHandle_t SaveGame_Start(const char *pSaveName, const char *pOldLevel, const char *pszLandmarkName) = 0;
	virtual bool SaveGame_Finalize(SaveRestoreDataHandle_t hSaveRestore) = 0;
	virtual bool FrameUpdatePostEntityThink(const EventServerPostEntityThink_t &msg) = 0;
	virtual const char *SaveGame_GetLastSaveFile(SaveRestoreDataHandle_t hSaveRestore) = 0;
	virtual bool IsGameReadyToSave() = 0;
	virtual unsigned long UnkGetGlobal1() = 0;
	virtual void * const UnkGetGameEntitySystemSync1() = 0;
	virtual void UnkRelease1() = 0;
	virtual void UnkRelease2() = 0;

	// AMNOTE: Game System related methods
	virtual void UnkSpawnGroupManagerGameSystemMember1() = 0;
	virtual void GameInit(const EventGameInit_t &msg) = 0;
	virtual void GameShutdown(const EventGameInit_t &msg) = 0;
	virtual void FrameBoundary(const EventGameInit_t &msg) = 0;
	virtual void PreSpawnGroupLoad(const EventPreSpawnGroupLoad_t &msg) = 0;
	virtual ~IGameSpawnGroupMgr() {}
};

// AMNOTE: Short stubs representing game class hierarchy
class CBaseSpawnGroup : public ISpawnGroup, public IComputeWorldOriginCallback, public IGameResourceManifestLoadCompletionCallback
{
};

class CLoadingSpawnGroup : public ILoadingSpawnGroup
{
};

class CSpawnGroupMgrGameSystem : public IGameSpawnGroupMgr, public CBaseGameSystem
{
	DECLARE_GAME_SYSTEM();
public:
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
	void *m_pUnk376;
};
#ifdef _WIN32
COMPILE_TIME_ASSERT(sizeof(CSpawnGroupMgrGameSystem) == 384);
#elif defined(POSIX)
COMPILE_TIME_ASSERT(sizeof(CSpawnGroupMgrGameSystem) == 728);
#endif

#endif // SPAWNGROUPMGR_H
