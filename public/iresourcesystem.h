#ifndef RESOURCESYSTEM_H
#define RESOURCESYSTEM_H

#pragma once

#include "appframework/iappsystem.h"
#include "resourcefile/resourcetype.h"

class IResourceUpdater;
class IResourceTypeManager;
class CVDataTypeManager;

class IResourceSystem : public IAppSystem
{
public:
	virtual void InstallTypeManager( uint64 nResourceType, IResourceTypeManager *pResourceTypeMgr, const char *pszResourceType, void *unk5 = nullptr ) = 0;
	virtual IResourceTypeManager *InstallNullTypeManager( uint64 nResourceType, const char *pszResourceType ) = 0;
	virtual void Update( int nTick, ResourceSystemUpdateMode_t nMode ) = 0;
	virtual void UpdateSimple() = 0;
	virtual bool HasPendingWork() = 0;
	virtual void RemoveNullTypeManager( IResourceTypeManager *pResourceTypeMgr ) = 0;
	virtual void RemoveResourceTypeManager( IResourceTypeManager *pResourceTypeMgr ) = 0;
	virtual IResourceTypeManager *GetTypeManager( ResourceType_t nResourceType ) = 0;
	virtual void RegisterFinishedFrameCounter( int *pFrameCounter ) = 0;
	virtual void UnregisterFinishedFrameCounter( int *pFrameCounter ) = 0;
	virtual HGameResourceManifest CreateResourceManifest( const char *pszResourceName, ResourceManifestLoadBehavior_t nManifestLoadBehavior, const char *pszManifestName, ResourceManifestLoadPriority_t nLoadPriority ) = 0;
	virtual HGameResourceManifest CreateResourceGroupManifest( const char *pszResourceGroupName, ResourceManifestLoadBehavior_t nManifestLoadBehavior, const char *pszManifestName, ResourceManifestLoadPriority_t nLoadPriority ) = 0;
	virtual HGameResourceManifest CreateResourceRnmdManifest( const char *pszResourceGroupName, ResourceManifestLoadBehavior_t nManifestLoadBehavior, const char *pszManifestName, ResourceManifestLoadPriority_t nLoadPriority ) = 0;
	virtual HGameResourceManifest CreateResourceManifestInternal( ResourceManifestDesc_t &pResourceManifestDesc ) = 0;
	virtual void SetManifestCompletionCallback( HGameResourceManifest pGameResourceManifest, void *pCallback1, void *pParam ) = 0; // FnResourceLoadCompleteCallback_t
	virtual bool IsManifestLoaded( HGameResourceManifest pGameResourceManifest ) = 0;
	virtual void DestroyResourceManifest( HGameResourceManifest pGameResourceManifest ) = 0;
	virtual const char *GetResourceManifestDebugName( HGameResourceManifest pGameResourceManifest ) = 0;
	virtual void ForceSynchronizationAndBlockUntilManifestLoaded( HGameResourceManifest pGameResourceManifest ) = 0;
	virtual ResourceHandle_t FindOrCreateProceduralResource( CResourceString &pResourceName, const void *pPermanentData, size_t nDataSize ) = 0;
	virtual ResourceHandle_t FindOrRegisterResourceByName( CResourceString &pResourceName, bool bCheckValid ) = 0;
	virtual ResourceHandle_t FindResourceByHandle( ResourceId_t nResourceId, ResourceType_t nType ) = 0;
	virtual uint32 GetNamedResourceCount() = 0;
	virtual void *GetNamedResource( int unk_2, int unk_3, CUtlVector< ResourceHandle_t > &pVector, ResourceSystemGetNamedResourcesFlags_t nFlags ) = 0;
	virtual void *GetAllNamedResourcesByType( ResourceType_t nType, CUtlVector< ResourceHandle_t > &pVector, ResourceSystemGetNamedResourcesFlags_t nFlags ) = 0;
	virtual uint32 GetNamedResourceCount( ResourceSystemGetNamedResourcesFlags_t nFlags ) = 0;
	virtual void *GetAllNamedResourcesByHandle( ResourceHandle_t pResourceHdl, CUtlVector< ResourceHandle_t > &pVector, ResourceSystemGetNamedResourcesFlags_t nFlags ) = 0;
	virtual void unk_38() = 0;
	virtual ResourceId_t ResourceHandleToResourceId( ResourceId_t &result, ResourceHandle_t pResourceHandle ) = 0;
	virtual const char *GetResourceName( ResourceHandle_t pResourceHandle, CResourceString &pResourceFixupedName ) = 0;
	virtual void GetResourceID( ResourceHandle_t pResourceHandle, CResourceString &pResult, bool bAllowHeapAllocation ) = 0;
	virtual ResourceType_t GetResourceTypeByHandle( ResourceHandle_t pResourceHandle ) = 0;
	virtual ResourceHandle_t GetErrorResource( ResourceType_t nResourceType ) = 0;
	virtual const char *GetResourceTypeName( ResourceType_t nResourceType ) = 0;
	virtual void MarkErrorResourcesReloaded() = 0;
	virtual ResourceHandle_t CreateEmptyResource( CResourceString &pResourceID ) = 0;
	virtual ResourceHandle_t BlockingLoadResourceByName( ResourceHandle_t pResourceHdl, const char *pszManifestName ) = 0;
	virtual ResourceHandle_t BlockingLoadResourceByNameIntoJustInTimeManifest( ResourceHandle_t pResourceHdl, const char *pszManifestName ) = 0;
	virtual void FreeJustInTimeManifests() = 0;
	virtual uint32 GetJustInTimeManifestResources() = 0;
	virtual void *GetJustInTimeManifest( CUtlVector< ResourceHandle_t > &pVector ) = 0;
	virtual void InstallResourceUpdater( IResourceUpdater *pUpdater ) = 0;
	virtual void UninstallResourceUpdater() = 0;
	virtual const char *GetActualFileName( ResourceHandle_t pResourceHdl, CResourceString &pResourceID, bool bAllowHeapAllocation ) = 0;
	virtual ResourceStatus_t GetResourceStatus( ResourceHandle_t pResourceHdl ) = 0;
	virtual ResourceStatus_t GetResourceStatus( CResourceString &pResourceID ) = 0;
	virtual void RegisterForcedSynchronizationCallback( void *pCallback1, void *pCallback2 ) = 0;
	virtual void UnregisterForcedSynchronizationCallback( void *pCallback1, void *pCallback2 ) = 0;
	virtual void GetResourcesNamesInManifest( HGameResourceManifest *pGameResourceManifest, CUtlVector< CUtlString > &pVector ) = 0;

	virtual void unk_60() = 0;

	virtual void AddResourceListeners( void *pListener ) = 0;
	virtual void RemoveResourceListeners( void *pListener ) = 0;

	virtual void unk_63() = 0;
	virtual void unk_64() = 0;

	virtual void *GetCodeResourceManifestManager() = 0;
	virtual void *GetResourceSystemProfiler() = 0;
	virtual void *GetResourceSystemLeakTracker() = 0;

	virtual void unk_68() = 0;

	virtual void BlockingFinishAllCurrentlyLoadingManifests() = 0;
	virtual bool IsBlockingOnManifestLoad() = 0;
	virtual void SetResourceTypeManifestPriority( int nResourceType, ResourceManifestLoadPriority_t nResourceManifestLoadPriority ) = 0;

	virtual void unk_72() = 0;

	virtual CVDataTypeManager *GetVDataTypeManager() = 0;

	virtual void unk_74() = 0;
	virtual void unk_76() = 0;
	virtual void unk_77() = 0;
	virtual void unk_78() = 0;
	virtual void unk_79() = 0;

	virtual ~IResourceSystem() = default;
};

//-----------------------------------------------------------------------------
// Globals Exposed
//-----------------------------------------------------------------------------
DECLARE_TIER2_INTERFACE( IResourceSystem, g_pResourceSystem );

#endif // RESOURCESYSTEM_H
