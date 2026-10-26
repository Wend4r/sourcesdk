#ifndef NETWORKSERVERSPAWNGROUP_H
#define NETWORKSERVERSPAWNGROUP_H

#ifdef _WIN32
#pragma once
#endif

#include "basespawngroup.h"

class CNetworkGameServerBase;

class CNetworkServerSpawnGroup : public CBaseSpawnGroup
{
public:
	virtual uint32 GetCreationTick() const override = 0;
	// Empty
	virtual void RequestDeferredUnload( bool bAsync = true ) override = 0;
	// Returns -1
	virtual uint32 GetDestructionTick() const override = 0;
	virtual void GetSpawnGroupDesc( SpawnGroupDesc_t *pDesc ) const override = 0;
	virtual void FlagManualCreation() override = 0;
	virtual bool HasManualCreation() override = 0;
	virtual void AddSpawnGroupChild( SpawnGroupHandle_t hSpawnGroup ) override = 0;
	// Empty
	virtual void SetDestructionTick( uint32 nTick ) override = 0;
	virtual void OnGameResourceManifestLoaded( HGameResourceManifest hManifest, int nResourceCount, ResourceHandle_t *pResourceHandles ) override = 0;
	virtual void Init( IResourceManifestRegistry *pResourceManifest, IEntityPrecacheConfiguration *pConfig, ISpawnGroupPrerequisiteRegistry *pRegistry ) override = 0;
	virtual void Shutdown() override = 0;
	virtual bool GetLoadStatus() override = 0;
	virtual bool LoadEntities() override = 0;
	virtual void AppendSaveGameResources( HGameResourceManifest hManifest, SpawnGroupHandle_t hSpawnGroup, const matrix3x4a_t &vecWorldOffset ) override = 0;
	virtual void AppendTransitionResources( HGameResourceManifest hManifest, SpawnGroupHandle_t hSpawnGroup, const matrix3x4a_t &vecWorldOffset ) override = 0;
	virtual void SetParentSpawnGroupForChild( SpawnGroupHandle_t hChildSpawnGroup ) override = 0;
	virtual IGameSpawnGroupMgr *GetSpawnGroupMgr() const override = 0;

public:
	CNetworkGameServerBase *m_pServer;
	// Server tick of LoadEntities, -1 until then
	int m_nCreationTick;

	// Set by Init; Shutdown then shuts the spawn group down in the game spawn group manager when it has no owner
	uint8 m_bInitialized : 1;
	uint8 m_bCreateClientEntitiesOnLaterConnectingClients : 1;
	// What HasManualCreation returns: set by FlagManualCreation, or from the start without
	// SpawnGroupDesc_t::m_bManualCompletion
	uint8 m_bManualCreationFlagged : 1;
	// Set once the load message is built; clients are only sent the unload of a sent spawn group
	uint8 m_bSentToClients : 1;
};

#endif // NETWORKSERVERSPAWNGROUP_H
