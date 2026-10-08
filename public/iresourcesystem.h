#ifndef RESOURCESYSTEM_H
#define RESOURCESYSTEM_H

#pragma once

#include "appframework/iappsystem.h"
#include "resourcefile/resourcetype.h"
#include "tier1/utldelegate.h"

class IResourceUpdater;
class IResourceTypeManager;
class IResourceDataProvider;
class ICodeResourceManifestManager;
class CVDataTypeManager;
class CResourceSystemProfiler;
class CResourceSystemLeakTracker;
class IToolsResourceListener;

typedef void ( *FnResourceLoadCompleteCallback_t )( HGameResourceManifest hManifest, void *pUserData );

class IResourceSystem : public IAppSystem
{
public:
	virtual void InstallTypeManager( ResourceType_t nResourceType, IResourceTypeManager *pResourceTypeMgr, const char *pszResourceType, const char *pszPreloadManifest = nullptr ) = 0;
	virtual void InstallNullTypeManager( ResourceType_t nResourceType, const char *pszResourceType ) = 0;
	virtual void Update( int nTimeBudgetNs, ResourceSystemUpdateMode_t nMode ) = 0;
	// Update with a 10 ms budget and mode 0.
	virtual void UpdateSimple() = 0;
	virtual bool HasPendingWork() = 0;
	virtual void RemoveNullTypeManager( ResourceType_t nResourceType ) = 0;
	virtual void RemoveResourceTypeManager( IResourceTypeManager *pResourceTypeMgr ) = 0;
	virtual IResourceTypeManager *GetTypeManager( ResourceType_t nResourceType ) = 0;
	virtual HGameResourceManifest CreateResourceManifest( const char *pszResourceName, ResourceManifestLoadBehavior_t nManifestLoadBehavior, const char *pszManifestName, ResourceManifestLoadPriority_t nLoadPriority ) = 0;
	virtual HGameResourceManifest CreateResourceGroupManifest( const char *pszResourceGroupName, ResourceManifestLoadBehavior_t nManifestLoadBehavior, const char *pszManifestName, ResourceManifestLoadPriority_t nLoadPriority ) = 0;
	virtual HGameResourceManifest CreateResourceRnmdManifest( const char *pszResourceGroupName, ResourceManifestLoadBehavior_t nManifestLoadBehavior, const char *pszManifestName, ResourceManifestLoadPriority_t nLoadPriority ) = 0;
	virtual HGameResourceManifest CreateResourceManifestInternal( const void *pCreateInfo ) = 0;
	virtual void SetManifestCompletionCallback( HGameResourceManifest pGameResourceManifest, FnResourceLoadCompleteCallback_t pfnCallback, void *pUserData ) = 0;
	virtual bool IsManifestLoaded( HGameResourceManifest pGameResourceManifest ) = 0;
	virtual void DestroyResourceManifest( HGameResourceManifest pGameResourceManifest ) = 0;
	virtual const char *GetResourceManifestDebugName( HGameResourceManifest pGameResourceManifest ) = 0;
	virtual void ForceSynchronizationAndBlockUntilManifestLoaded( HGameResourceManifest pGameResourceManifest ) = 0;
	virtual ResourceHandle_t FindOrCreateProceduralResource( CResourceString &pResourceName, const void *pPermanentData, size_t nDataSize ) = 0;
	virtual ResourceHandle_t FindResourceById( ResourceId_t nResourceId, ResourceType_t nType ) = 0;
	virtual void GetAllNamedResourcesByTypeDefault( ResourceType_t nType, CUtlVector< ResourceHandle_t > &pVector, ResourceSystemGetNamedResourcesFlags_t nFlags ) = 0;
	virtual void GetAllNamedResources( CUtlVector< ResourceHandle_t > &pVector, ResourceSystemGetNamedResourcesFlags_t nFlags ) = 0;
	virtual void GetAllNamedResourcesByType( ResourceType_t nType, CUtlVector< ResourceHandle_t > &pVector, ResourceSystemGetNamedResourcesFlags_t nFlags ) = 0;
	// Returns 0 for anonymous and error bindings.
	virtual ResourceId_t ResourceHandleToResourceId( ResourceHandle_t pResourceHandle ) = 0;
	virtual void GetResourceName( ResourceHandle_t pResourceHandle, CBufferString &pResult, bool bAllowHeapAllocation ) = 0;
	virtual void GetResourceName( ResourceHandle_t pResourceHandle, CResourceString &pResult ) = 0;
	virtual IResourceTypeManager *GetTypeManagerForBinding( ResourceHandle_t pResourceHandle ) = 0;
	virtual ResourceHandle_t GetErrorResource( ResourceType_t nResourceType ) = 0;
	virtual const char *GetResourceTypeName( ResourceType_t nResourceType ) = 0;
	virtual void MarkErrorResourcesReloaded() = 0;
	virtual ResourceHandle_t BlockingLoadResourceByName( CResourceString &pResourceName, const char *pszManifestName ) = 0;
	virtual ResourceHandle_t BlockingLoadResourceByNameIntoJustInTimeManifest( CResourceString &pResourceName, const char *pszManifestName ) = 0;
	virtual void FreeJustInTimeManifests() = 0;
	virtual uint32 GetJustInTimeManifestCount() = 0;
	virtual void GetJustInTimeManifest( CUtlVector< ResourceHandle_t > &pVector ) = 0;
	virtual bool IsInFrameUpdate() = 0;
	virtual void InstallResourceUpdater( IResourceUpdater *pUpdater ) = 0;
	virtual void UninstallResourceUpdater() = 0;
	virtual const char *GetActualFileName( ResourceHandle_t pResourceHdl, CBufferString &pResult, bool bAllowHeapAllocation ) = 0;
	virtual ResourceStatus_t GetResourceStatus( CResourceString &pResourceName ) = 0;
	virtual ResourceStatus_t GetResourceStatus( ResourceId_t nResourceId ) = 0;
	virtual ResourceStatus_t GetResourceStatus( ResourceHandle_t pResourceHdl ) = 0;
	virtual void RegisterForcedSynchronizationCallback( void *pCallback, void *pParam ) = 0;
	virtual void UnregisterForcedSynchronizationCallback( void *pCallback, void *pParam ) = 0;
	virtual void GetResourcesNamesInManifest( HGameResourceManifest pGameResourceManifest, CUtlVector< CUtlString > &pVector ) = 0;
	virtual void GetAllResourcesInManifestTree( HGameResourceManifest pGameResourceManifest, CUtlVector< ResourceHandle_t > &pVector ) = 0;
	virtual void AddResourceListeners( void *pListener ) = 0;
	virtual void RemoveResourceListeners( void *pListener ) = 0;
	virtual void RegisterAsyncResourceListener( void *pListener ) = 0;
	virtual void UnregisterAsyncResourceListener( void *pListener ) = 0;
	virtual ICodeResourceManifestManager *GetCodeResourceManifestManager() = 0;
	virtual CResourceSystemProfiler *GetResourceSystemProfiler() = 0;
	virtual CResourceSystemLeakTracker *GetResourceSystemLeakTracker() = 0;
	virtual void SetResourceTypeManifestPriority( ResourceType_t nResourceType, ResourceManifestLoadPriority_t nResourceManifestLoadPriority ) = 0;
	virtual void BlockingFinishAllCurrentlyLoadingManifests() = 0;
	virtual bool IsBlockingOnManifestLoad() = 0;
	virtual void SetDebugFlag( int nFlag, bool bValue ) = 0;
	virtual ResourceManifestLoadPriority_t ResolveManifestLoadPriority( ResourceManifestLoadPriority_t nPriority ) = 0;
	virtual CVDataTypeManager *GetVDataTypeManager() = 0;
	virtual bool IsShuttingDown() = 0;
	virtual bool GetDebugFlag( int nFlag ) = 0;
	virtual void Unk_RegisterLoadListener( void *p ) = 0;
	virtual void Unk_UnregisterLoadListener( void *p ) = 0;
	virtual void InstallTestFilesystem( void *pTestFS ) = 0;
	virtual void UninstallTestFilesystem( void *pTestFS ) = 0;
	virtual void Lock() = 0;
	virtual void Unlock() = 0;
	virtual bool IsLockedByCurrentThread() = 0;
	virtual void QueueCallbackForNextUpdate( const CUtlDelegate< void () > &callback ) = 0;
	virtual ResourceHandle_t FindOrRegisterResourceByName_Internal( CResourceString &pResourceName, bool bCheckValid ) = 0;
};

DECLARE_TIER2_INTERFACE( IResourceSystem, g_pResourceSystem );

#endif // RESOURCESYSTEM_H
