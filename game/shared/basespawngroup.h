#ifndef BASESPAWNGROUP_H
#define BASESPAWNGROUP_H

#pragma once

#include "spawngrouptypes.h"
#include "ispawngroup.h"
#include "iprerequisite.h"
#include "engine2/igameresourceservice.h"
#include "resourcefile/igameresourcemanifestloadcompletion.h"
#include "worldrenderer/icomputeworldorigin.h"
#include "tier0/utlstring.h"
#include "tier0/utlvector.h"

class ILoadingSpawnGroup;
class ISource2WorldSession;
class IWorld;
class IWorldReference;
class IResourceManifestRegistry;
class IEntityPrecacheConfiguration;
class ISpawnGroupPrerequisiteRegistry;
class IGameSpawnGroupMgr;

// Engine spawn group, shared by the server and the client implementations
class CBaseSpawnGroup : public ISpawnGroup, public IComputeWorldOriginCallback, public IGameResourceManifestLoadCompletionCallback
{
public:
	// Resolves m_pWorldOffsetCallback into m_vecWorldOffset once
	virtual matrix3x4_t ComputeWorldOrigin( const char *pWorldName, SpawnGroupHandle_t hSpawnGroup, IWorld *pWorld ) override = 0;
	virtual void Release() override = 0;
	virtual void OnGameResourceManifestLoaded( HGameResourceManifest hManifest, int nResourceCount, ResourceHandle_t *pResourceHandles ) override = 0;

	virtual void Init( IResourceManifestRegistry *pResourceManifest, IEntityPrecacheConfiguration *pConfig, ISpawnGroupPrerequisiteRegistry *pRegistry ) = 0;
	virtual void Shutdown() = 0;
	virtual bool GetLoadStatus() = 0;
	virtual void ForceBlockingLoad() = 0;
	virtual bool ShouldBlockUntilLoaded() const = 0;
	virtual void ServiceBlockingLoads() = 0;
	virtual bool GetEntityPrerequisites( HGameResourceManifest hManifest ) = 0;
	virtual bool EntityPrerequisitesSatisfied() = 0;
	virtual bool LoadEntities() = 0;

	// Forward to ISource2Server::AppendSaveGameResources and AppendTransitionResources on the server
	virtual void AppendSaveGameResources( HGameResourceManifest hManifest, SpawnGroupHandle_t hSpawnGroup, const matrix3x4a_t &vecWorldOffset ) = 0;
	virtual void AppendTransitionResources( HGameResourceManifest hManifest, SpawnGroupHandle_t hSpawnGroup, const matrix3x4a_t &vecWorldOffset ) = 0;

	// Makes this spawn group the parent of hChildSpawnGroup
	virtual void SetParentSpawnGroupForChild( SpawnGroupHandle_t hChildSpawnGroup ) = 0;
	virtual void TransferOwnershipOfManifestsTo( ISpawnGroup *pTarget ) = 0;
	virtual IGameSpawnGroupMgr *GetSpawnGroupMgr() const = 0;

public:
	SpawnGroupHandle_t m_hSpawnGroupHandle;
	SpawnGroupState_t m_nState;
	SpawnGroupHandle_t m_hOwnerSpawnGroup;
	SpawnGroupHandle_t m_hParentSpawnGroup;
	WorldGroupId_t m_hWorldGroupId;
	ISource2WorldSession *m_pWorldSession;
	IWorldReference *m_pWorldReference;
	matrix3x4a_t m_vecWorldOffset;
	IComputeWorldOriginCallback *m_pWorldOffsetCallback;
	CUtlString m_sWorldName;
	CUtlString m_sWorldMountName;
	CUtlString m_sEntityLumpName;
	CUtlString m_sEntityFilterName;
	CUtlString m_sDescriptiveName;
	CUtlString m_sParentNameFixup;
	CUtlString m_sLocalNameFixup;
	CUtlString m_sSaveFileName;
	CUtlString m_sWorldGroupName;
	ResourceManifestLoadPriority_t m_manifestLoadPriority;
	int m_iPriorityLoader;
	// Passed on to the owner spawn group with the manifests when it is higher
	int m_nCreationSequence;
	ILoadingSpawnGroup *m_pLoadingSpawnGroup;
	CCompressedResourceManifest m_CompressedManifest;
	// Manifests added by GetEntityPrerequisites; EntityPrerequisitesSatisfied waits for them
	CGameResourceManifestPrerequisite m_ManifestPrerequisite;
	CUtlVector< SpawnGroupHandle_t > m_ChildSpawnGroups;
	float m_flTimeoutInterval;
	// Plat_FloatTime() of the last m_nState change
	double m_flStateChangeTime;

	// Uses the server game resource service and describes itself as "SV"
	uint16 m_bServerSide : 1;
	uint16 m_padBit1 : 1;
	// Set while the manifest completion callback is pending
	uint16 m_bWaitingForFutureDependentResources : 1;
	// Set on a child by AddSpawnGroupChild of its parent, cleared once the parent's wait for its children completes
	uint16 m_bWaitForParentSpawnGroup : 1;
	uint16 m_bDontSpawnEntities : 1;
	uint16 m_bBlockUntilLoaded : 1;
	// CreateWorldInfo_t::m_bDisableNodeStreaming of the world load request
	uint16 m_bDisableNodeStreaming : 1;
	uint16 m_bSynchronousSpawn : 1;
	uint16 m_bLoadStreamingData : 1;
	uint16 m_bCreateNewSceneWorld : 1;
	uint16 m_bManualCompletion : 1;
	uint16 m_bSetActivePostLoad : 1;
	// SpawnGroupDesc_t::m_bSaveEntities
	uint16 m_bSaveEntities : 1;
	uint16 m_bLevelTransition : 1;
	uint16 m_bReadyToSpawnEntities : 1;
	// SpawnGroupDesc_t::m_bUnk2
	uint16 m_bUnk2 : 1;
	uint8 m_bHideBaseLayer : 1;
};

#endif // BASESPAWNGROUP_H
