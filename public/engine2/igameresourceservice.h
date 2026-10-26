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

#include <engine/IEngineService.h>
#include <entity2/entitysystem.h>
#include <tier0/utlstring.h>
#include <tier0/utlsymbol.h>
#include <tier0/utlvector.h>

class CGameResourceManifestLock;
class CGameResourceManifest;
class IGameResourceManifestLoadCompletionCallback;

// Resource names of a manifest, built by IGameResourceService::BuildCompressedManifest and sent to clients
class CCompressedResourceManifest
{
public:
	struct Resource_t
	{
		// Directory of the resource name, in m_Paths
		CUtlSymbol m_Path;
		// File name without the extension
		CUtlString m_sBaseName;
		// In m_Extensions
		CUtlSymbol m_Extension;
	};

	// Indexed by the symbol of the resource name in m_ResourceNames
	CUtlVector< Resource_t > m_Resources;
	CUtlSymbolTable m_ResourceNames;
	CUtlSymbolTable m_Paths;
	CUtlSymbolTable m_Extensions;
};

class IGameResourceService : public IEngineService
{
public:
	virtual ~IGameResourceService() = 0;
	virtual HGameResourceManifest LoadGameResourceManifest( const char *pszResourceName, ResourceManifestLoadBehavior_t eBehavior, const char *pszManifestName, ResourceManifestLoadPriority_t ePriority ) = 0;
	virtual HGameResourceManifest LoadGameResourceManifestGroup( const char *pszResourceGroupName, ResourceManifestLoadBehavior_t eBehavior, const char *pszManifestName, ResourceManifestLoadPriority_t ePriority ) = 0;
	virtual HGameResourceManifest CreateGameResourceManifest( int nResourceCount, const char *const *ppszResourceNames, ResourceManifestLoadBehavior_t eBehavior, const char *pszManifestName, ResourceManifestLoadPriority_t ePriority ) = 0;
	virtual HGameResourceManifest CreateGameResourceManifest( EntityResourceManifestCreationCallback_t pfnCreationCallback, void *pContext, ResourceManifestLoadBehavior_t eBehavior, const char *pszManifestName, ResourceManifestLoadPriority_t ePriority ) = 0;
	virtual void SetManifestCompletionCallback( HGameResourceManifest hManifest, IGameResourceManifestLoadCompletionCallback *pCallback, int nResourceCount, const char *const *ppszResourceNames, bool bUnk ) = 0;
	virtual bool IsManifestLoaded( HGameResourceManifest hManifest ) = 0;
	virtual void BlockUntilManifestLoaded( HGameResourceManifest hManifest ) = 0;
	virtual void DestroyResourceManifest( HGameResourceManifest hManifest ) = 0;
	virtual const char *GetResourceManifestDebugName( HGameResourceManifest hManifest ) = 0;
	virtual bool DoesManifestHaveFutureDependentResources( HGameResourceManifest hManifest ) = 0;
	virtual void SetEntityResourceManifestHandler( IEntityResourceManifestBuilder *pManifest ) = 0;
	virtual void PrecacheEntitiesAndConfirmResourcesAreLoaded( SpawnGroupHandle_t hSpawnGroup, int nCount, const EntitySpawnInfo_t *pEntities, const matrix3x4a_t *pWorldOffset ) = 0;
	virtual void DescribeContents( uint32 nLoggingChannel, HGameResourceManifest hManifest ) = 0;
	virtual void GetManifestResourceNames( HGameResourceManifest hManifest, CUtlVector< CUtlString > &vecResourceNames ) = 0;
	virtual HGameResourceManifest AllocGameResourceManifest( ResourceManifestLoadBehavior_t eBehavior, const char *pszAllocatorName, ResourceManifestLoadPriority_t ePriority ) = 0;
	virtual bool CreateGameResourceManifest( HGameResourceManifest hManifest ) = 0;
	virtual HGameResourceManifest LoadCompressedGameResourceManifest( const CCompressedResourceManifest *pCompressedManifest, bool bUnk, ResourceManifestLoadBehavior_t eBehavior, const char *pszManifestName, ResourceManifestLoadPriority_t ePriority ) = 0;
	virtual void AppendToGameResourceManifest( HGameResourceManifest hManifest, const CCompressedResourceManifest *pCompressedManifest, bool bUnk1, ResourceManifestLoadBehavior_t eBehavior, bool bUnk2 ) = 0;
	virtual void BuildCompressedManifest( HGameResourceManifest hManifest, CCompressedResourceManifest *pCompressedManifest, bool bOnlyUnloadedResources ) = 0;
	virtual void LockGameResourceManifest( bool bLock, CGameResourceManifestLock &manifestLock ) = 0;
	virtual bool AppendToAndCreateGameResourceManifest( HGameResourceManifest hManifest, SpawnGroupHandle_t hSpawnGroup, int nCount, const EntitySpawnInfo_t *pEntities, const matrix3x4a_t *pWorldOffset ) = 0;
};

class CGameResourceService : public CBaseEngineService<IGameResourceService>
{
public:
	virtual ~CGameResourceService() = 0;

	CUtlVector<CGameResourceManifest *> m_Manifests;
	CGameEntitySystem* m_pEntitySystem;
};

#ifdef _WIN32
static_assert( sizeof( CGameResourceService ) == 0x60, "CGameResourceService layout mismatch" );
#else
static_assert( sizeof( CGameResourceService ) == 0x58, "CGameResourceService layout mismatch" );
#endif


#endif // IGAMERESOURCESERVICE_H
