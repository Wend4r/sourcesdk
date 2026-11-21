#ifndef SPAWNGROUPMGR_H
#define SPAWNGROUPMGR_H

#pragma once

#include "spawngrouptypes.h"
#include "iloadingspawngroup.h"
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
	virtual ILoadingSpawnGroup *CreateLoadingSpawnGroupForSaveFile( SpawnGroupHandle_t handle, const char *pSaveFile, const void *pSaveData, int nSaveDataSize ) = 0;
	virtual void SpawnGroupSpawnEntities(SpawnGroupHandle_t handle) = 0;
	virtual void SpawnGroupShutdown(SpawnGroupHandle_t handle) = 0;
	virtual void SetActiveSpawnGroup(SpawnGroupHandle_t handle) = 0;
	virtual void UnloadSpawnGroup( SpawnGroupHandle_t hSpawnGroup ) = 0;
	virtual SpawnGroupHandle_t GetParentSpawnGroup(SpawnGroupHandle_t hSpawnGroup) = 0;
	virtual int GetChildSpawnGroupCount(SpawnGroupHandle_t handle) = 0;
	virtual SaveRestoreDataHandle_t SaveGame_Start( const void *pSaveParams ) = 0;
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

// AMNOTE: Short stubs representing game class hierarchy
class CBaseSpawnGroup : public ISpawnGroup, public IComputeWorldOriginCallback, public IGameResourceManifestLoadCompletionCallback
{
};

class CLoadingSpawnGroup : public ILoadingSpawnGroup
{
};

//-----------------------------------------------------------------------------
// Registered as "SpawnGroupManagerGameSystem" (reallocating factory). 384 bytes on Windows, 728 on
// Linux (CThreadRWLock_FastRead is larger there). IGameSpawnGroupMgr is the primary base.
// The declaration order of the IGameSystem overrides (and of the implicit destructor, last) is the
// order of the entries libserver.so adds to the IGameSpawnGroupMgr table.
//-----------------------------------------------------------------------------
class CSpawnGroupMgrGameSystem : public IGameSpawnGroupMgr, public CAutoGameSystem
{
	DECLARE_GAME_SYSTEM();

public:
	// Forward to the game system factory
	virtual bool DoesGameSystemReallocate() override;
	virtual void SetGameSystemGlobalPtrs( void *pValue ) override;

	// Starts queueing the SpawnGroupSpawnEntities calls
	GS_EVENT( GameInit );
	// Drops the queued SpawnGroupSpawnEntities calls
	GS_EVENT( GameShutdown );
	// Performs the queued unload requests
	GS_EVENT( GameFrameBoundary );
	GS_EVENT( PreSpawnGroupLoad );

public:
	CConCommandMemberAccessor< CSpawnGroupMgrGameSystem > m_OnSpawnGroupActivate; // "spawn_group_activate"
	CConCommandMemberAccessor< CSpawnGroupMgrGameSystem > m_OnSpawnGroupLoad; // "spawn_group_load"
	CConCommandMemberAccessor< CSpawnGroupMgrGameSystem > m_OnSpawnGroupUnload; // "spawn_group_unload"
	CUtlVector< SpawnGroupHandle_t > m_SpawnEntitiesCallQueue;
	bool m_bQueueSpawnEntitiesCalls;
	CUtlOrderedMap< SpawnGroupHandle_t, CMapSpawnGroup *, CDefLess< SpawnGroupHandle_t >, unsigned short > m_SpawnGroups;
	CThreadRWLock_FastRead m_hHandleMutex;
	CUtlVector< SpawnGroupHandle_t > m_QueuedUnloadRequests;
	CUtlString m_sName;
};

#endif // SPAWNGROUPMGR_H
