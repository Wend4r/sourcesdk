#ifndef RESOURCESYSTEM_H
#define RESOURCESYSTEM_H

#pragma once

#include "appframework/iappsystem.h"
#include "resourcefile/resourcetype.h"

class IResourceUpdater;
class IResourceTypeManager;
class IResourceDataProvider;
class CVDataTypeManager;
class CResourceSystemProfiler;
class CResourceSystemLeakTracker;
class IToolsResourceListener;

typedef void ( *FnResourceLoadCompleteCallback_t )( HGameResourceManifest hManifest, void *pUserData );

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
	virtual HGameResourceManifest CreateResourceManifest( const char *pszResourceName, ResourceManifestLoadBehavior_t nManifestLoadBehavior, const char *pszManifestName, ResourceManifestLoadPriority_t nLoadPriority ) = 0;
	virtual HGameResourceManifest CreateResourceGroupManifest( const char *pszResourceGroupName, ResourceManifestLoadBehavior_t nManifestLoadBehavior, const char *pszManifestName, ResourceManifestLoadPriority_t nLoadPriority ) = 0;
	virtual HGameResourceManifest CreateResourceRnmdManifest( const char *pszResourceGroupName, ResourceManifestLoadBehavior_t nManifestLoadBehavior, const char *pszManifestName, ResourceManifestLoadPriority_t nLoadPriority ) = 0;
	virtual HGameResourceManifest CreateResourceManifestInternal( ResourceManifestDesc_t &pResourceManifestDesc ) = 0;
	virtual void SetManifestCompletionCallback( HGameResourceManifest pGameResourceManifest, FnResourceLoadCompleteCallback_t pfnCallback, void *pUserData ) = 0;
	virtual bool IsManifestLoaded( HGameResourceManifest pGameResourceManifest ) = 0;
	virtual void DestroyResourceManifest( HGameResourceManifest pGameResourceManifest ) = 0;
	virtual const char *GetResourceManifestDebugName( HGameResourceManifest pGameResourceManifest ) = 0;
	virtual void ForceSynchronizationAndBlockUntilManifestLoaded( HGameResourceManifest pGameResourceManifest ) = 0;
	virtual ResourceHandle_t FindOrCreateProceduralResource( CResourceString &pResourceName, const void *pPermanentData, size_t nDataSize ) = 0;
	virtual ResourceHandle_t FindResourceByHandle( ResourceId_t nResourceId, ResourceType_t nType ) = 0;
	virtual void GetAllNamedResourcesByTypeDefault( ResourceType_t nType, CUtlVector< ResourceHandle_t > &pVector, ResourceSystemGetNamedResourcesFlags_t nFlags ) = 0;
	virtual void GetAllNamedResources( CUtlVector< ResourceHandle_t > &pVector, ResourceSystemGetNamedResourcesFlags_t nFlags ) = 0;
	virtual void GetAllNamedResourcesByType( ResourceType_t nType, CUtlVector< ResourceHandle_t > &pVector, ResourceSystemGetNamedResourcesFlags_t nFlags ) = 0;
	virtual void ResourceHandleToResourceId( ResourceId_t &result, ResourceHandle_t pResourceHandle ) = 0;
	virtual void GetResourceIDFull( ResourceHandle_t pResourceHandle, CResourceString &pResult ) = 0;
	virtual void GetResourceName( ResourceHandle_t pResourceHandle, CBufferString &pResult, bool bAllowHeapAllocation ) = 0;
	virtual IResourceTypeManager *GetTypeManagerForBinding( ResourceHandle_t pResourceHandle ) = 0;
	virtual void *GetErrorResource( ResourceType_t nResourceType ) = 0;
	virtual const char *GetResourceTypeName( ResourceType_t nResourceType ) = 0;
	virtual void MarkErrorResourcesReloaded() = 0;
	virtual ResourceHandle_t BlockingLoadResourceByName( ResourceHandle_t pResourceHdl, const char *pszManifestName ) = 0;
	virtual ResourceHandle_t BlockingLoadResourceByNameIntoJustInTimeManifest( ResourceHandle_t pResourceHdl, const char *pszManifestName ) = 0;
	virtual void FreeJustInTimeManifests() = 0;
	virtual uint32 GetJustInTimeManifestCount() = 0;
	virtual void GetJustInTimeManifest( CUtlVector< ResourceHandle_t > &pVector ) = 0;
	virtual bool IsManualResourceDeletionEnabled() = 0;
	virtual void InstallResourceUpdater( IResourceUpdater *pUpdater ) = 0;
	virtual void UninstallResourceUpdater() = 0;
	virtual const char *GetActualFileName( ResourceHandle_t pResourceHdl, CResourceString &pResourceID, bool bAllowHeapAllocation ) = 0;
	virtual ResourceStatus_t GetResourceStatus( ResourceHandle_t pResourceHdl ) = 0;
	virtual ResourceStatus_t GetResourceStatus( CResourceString &pResourceID ) = 0;
	virtual ResourceStatus_t GetResourceStatusFromExternalHandle( void *pExtHandle ) = 0;
	virtual void RegisterForcedSynchronizationCallback( void *pCallback, void *pParam ) = 0;
	virtual void UnregisterForcedSynchronizationCallback( void *pCallback, void *pParam ) = 0;
	virtual void GetResourcesNamesInManifest( HGameResourceManifest *pGameResourceManifest, CUtlVector< CUtlString > &pVector ) = 0;
	virtual void GetAllResourcesInManifestTree( HGameResourceManifest pGameResourceManifest, CUtlVector< ResourceHandle_t > &pVector ) = 0;
	virtual void AddResourceListeners( void *pListener ) = 0;
	virtual void RemoveResourceListeners( void *pListener ) = 0;
	virtual void RegisterAsyncResourceListener( void *pListener ) = 0;
	virtual void UnregisterAsyncResourceListener( void *pListener ) = 0;
	virtual void *GetCodeResourceManifestManager() = 0;
	virtual CResourceSystemProfiler *GetResourceSystemProfiler() = 0;
	virtual CResourceSystemLeakTracker *GetResourceSystemLeakTracker() = 0;
	virtual void SetResourceTypeManifestPriority( int nResourceType, ResourceManifestLoadPriority_t nResourceManifestLoadPriority ) = 0;
	virtual void BlockingFinishAllCurrentlyLoadingManifests() = 0;
	virtual bool IsBlockingOnManifestLoad() = 0;
	virtual void SetDebugFlag( int nFlag, bool bValue ) = 0;
	virtual int GetSomeDebugCode( uint8 nCode ) = 0;
	virtual CVDataTypeManager *GetVDataTypeManager() = 0;
	virtual bool IsResourceSystemStarted() = 0;
	virtual int GetResourceSystemThreadStatus() = 0;
	virtual bool GetDebugFlag( int nFlag ) = 0;
	virtual void RegisterNamedManifest( ResourceManifestDesc_t *pDesc ) = 0;
	virtual void UnregisterNamedManifest( ResourceManifestDesc_t *pDesc ) = 0;
	virtual void InstallTestFilesystem( void *pTestFS ) = 0;
	virtual void UninstallTestFilesystem( void *pTestFS ) = 0;
	virtual void unk_76() = 0;
	virtual void unk_77() = 0;
	virtual void ClearTestFilesystemList() = 0;
	virtual ResourceHandle_t FindOrRegisterResourceByName_Internal( CResourceString &pResourceName, bool bCheckValid ) = 0;

	virtual ~IResourceSystem() = default;
};

DECLARE_TIER2_INTERFACE( IResourceSystem, g_pResourceSystem );

#endif // RESOURCESYSTEM_H
