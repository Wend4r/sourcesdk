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

};

#endif // BASESPAWNGROUP_H
