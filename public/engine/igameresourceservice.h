//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#ifndef IGAMERESOURCESERVICE_H
#define IGAMERESOURCESERVICE_H
#ifdef _WIN32
#pragma once
#endif

#include <IEngineService.h>
#include <entity2/entitysystem.h>

class CGameResourceManifestLock;
class CGameResourceManifest;

class IGameResourceService : public IEngineService  
{
public:
	virtual ~IGameResourceService() = 0;
	virtual void LoadGameResourceManifest(const char *, ResourceManifestLoadBehavior_t, const char *, ResourceManifestLoadPriority_t);
	virtual void LoadGameResourceManifestGroup(const char *, ResourceManifestLoadBehavior_t, const char *, ResourceManifestLoadPriority_t);
	virtual void LoadGameResourceManifest(int, const char *, ResourceManifestLoadBehavior_t, const char *, ResourceManifestLoadPriority_t);
	virtual void LoadGameResourceManifest(EntityResourceManifestCreationCallback_t, void*, ResourceManifestLoadBehavior_t, const char *, ResourceManifestLoadPriority_t);
	virtual void unk1() = 0;
	virtual void SetManifestCompletionCallback(HGameResourceManifest*, void*) = 0;
	virtual bool IsManifestLoaded(HGameResourceManifest*) = 0;
	virtual void BlockUntilManifestLoaded(HGameResourceManifest*) = 0;
	virtual void DestroyResourceManifest(HGameResourceManifest*) = 0;
	virtual const char* GetResourceManifestDebugName() = 0;
	virtual bool DoesManifestHaveFutureDependentResources(HGameResourceManifest*) = 0;
	virtual void SetEntitySystem(CGameEntitySystem*) = 0;
	virtual void PrecacheEntitiesAndConfirmResourcesAreLoaded(SpawnGroupHandle_t hSpawnGroup, int nCount, const EntitySpawnInfo_t * pEntities, const matrix3x4a_t * vWorldOffset) = 0;
	virtual void DescribeContents() = 0;
	virtual void unk2() = 0;
	virtual void AllocGameResourceManifest(ResourceManifestLoadBehavior_t eBehavior, const char * pszAllocatorName, ResourceManifestLoadPriority_t ePriority) = 0;
	virtual void unk3() = 0;
	virtual void AppendToGameResourceManifest(HGameResourceManifest*, unsigned int, int, EntitySpawnInfo_t const*, matrix3x4a_t const&) = 0;
	virtual void BuildCompressedManifest(HGameResourceManifest*, CCompressedResourceManifest*, bool) = 0;
	virtual void LockGameResourceManifest(bool, CGameResourceManifestLock&) = 0;
	virtual void AppendToAndCreateGameResourceManifest(HGameResourceManifest *pResourceManifest, SpawnGroupHandle_t hSpawnGroup, int nCount, const EntitySpawnInfo_t *pEntities, const matrix3x4a_t * vWorldOffset) = 0;
};

class CGameResourceService : public CBaseEngineService<IGameResourceService> 
{
public:
	virtual ~CGameResourceService() = 0;

#ifdef WIN32
	char pad56[8];
#endif

	CUtlVector<CGameResourceManifest *> m_Manifests;
	CGameEntitySystem* m_pEntitySystem;
};


#endif // IGAMERESOURCESERVICE_H
