#ifndef RESOURCESYSTEM_H
#define RESOURCESYSTEM_H

#pragma once

#include "appframework/iappsystem.h"
#include "resourcefile/resourcetype.h"
#include "tier1/utldelegate.h"

#include <memory>

class IResourceUpdater;
class IResourceTypeManager;
class IResourceDataProvider;
class ICodeResourceManifestManager;
class CVDataTypeManager;
class CResourceSystemProfiler;
class CResourceSystemLeakTracker;
class IToolsResourceListener;
class IToolsResourcePreReloadListener;
class IFileSystem;
class IAsyncResourceDataRequest;

typedef void ( *FnResourceLoadCompleteCallback_t )( HGameResourceManifest hManifest, void *pUserData );

enum ResourceManifestType_t : uint8
{
	RESOURCE_MANIFEST_TYPE_DEFAULT = 0,
	RESOURCE_MANIFEST_TYPE_BLOCKING_LOAD,
	RESOURCE_MANIFEST_TYPE_BLOCKING_RELOAD,
};

struct ResourceManifestCreationInfo_t
{
	int m_nCount;
	const char **m_ppResourceFiles;

	// Any type other than the default loads with the immediate priority
	ResourceManifestType_t m_nType;
	ResourceManifestLoadBehavior_t m_nLoadBehavior;

	const char *m_pDebugName;
	ResourceManifestLoadPriority_t m_nPriority;

	FnResourceLoadCompleteCallback_t m_callback;
	void *m_pContext;
};

class IResourceSystem : public IAppSystem
{
public:
	virtual void InstallTypeManager( ResourceType_t nResourceType, IResourceTypeManager *pResourceTypeMgr, const char *pszResourceType, const char *pszPreloadManifest = nullptr ) = 0;
	virtual void InstallNullTypeManager( ResourceType_t nResourceType, const char *pszResourceType ) = 0;
	virtual void Update( int nTimeBudgetNs, ResourceSystemUpdateMode_t nMode ) = 0;
	// Update with a 10 ms budget and RESOURCE_SYSTEM_UPDATE_MODE_DEFAULT.
	virtual void UpdateSimple() = 0;
	virtual bool HasPendingWork() = 0;
	virtual void RemoveNullTypeManager( ResourceType_t nResourceType ) = 0;
	virtual void RemoveResourceTypeManager( IResourceTypeManager *pResourceTypeMgr ) = 0;
	virtual IResourceTypeManager *GetTypeManager( ResourceType_t nResourceType ) = 0;
	virtual HGameResourceManifest CreateResourceManifest( const char *pszResourceName, ResourceManifestLoadBehavior_t nManifestLoadBehavior, const char *pszManifestName, ResourceManifestLoadPriority_t nLoadPriority ) = 0;
	virtual HGameResourceManifest CreateResourceGroupManifest( const char *pszResourceGroupName, ResourceManifestLoadBehavior_t nManifestLoadBehavior, const char *pszManifestName, ResourceManifestLoadPriority_t nLoadPriority ) = 0;
	virtual HGameResourceManifest CreateResourceRnmdManifest( const char *pszResourceGroupName, ResourceManifestLoadBehavior_t nManifestLoadBehavior, const char *pszManifestName, ResourceManifestLoadPriority_t nLoadPriority ) = 0;
	virtual HGameResourceManifest CreateResourceManifestInternal( const ResourceManifestCreationInfo_t &info ) = 0;
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
	virtual void RegisterForcedSynchronizationCallback( void ( *pfnCallback )( void *pParam ), void *pParam ) = 0;
	virtual void UnregisterForcedSynchronizationCallback( void ( *pfnCallback )( void *pParam ), void *pParam ) = 0;
	virtual void GetResourcesNamesInManifest( HGameResourceManifest pGameResourceManifest, CUtlVector< CUtlString > &pVector ) = 0;
	virtual void GetAllResourcesInManifestTree( HGameResourceManifest pGameResourceManifest, CUtlVector< ResourceHandle_t > &pVector ) = 0;
	virtual void AddResourceListeners( IToolsResourceListener *pListener ) = 0;
	virtual void RemoveResourceListeners( IToolsResourceListener *pListener ) = 0;
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
	virtual void RegisterToolsResourceListener( IToolsResourcePreReloadListener *pListener ) = 0;
	virtual void UnregisterToolsResourceListener( IToolsResourcePreReloadListener *pListener ) = 0;
	virtual void InstallTestFilesystem( void *pTestFS ) = 0;
	virtual void UninstallTestFilesystem( void *pTestFS ) = 0;
	virtual void Lock() = 0;
	virtual void Unlock() = 0;
	virtual bool IsLockedByCurrentThread() = 0;
	virtual void QueueCallbackForNextUpdate( const CUtlDelegate< void () > &callback ) = 0;
	virtual ResourceHandle_t FindOrRegisterResourceByName_Internal( CResourceString &pResourceName, bool bCheckValid ) = 0;
};

DECLARE_TIER2_INTERFACE( IResourceSystem, g_pResourceSystem );

abstract_class IResourceManifestRegistry
{
public:
	virtual void RegisterFrameNumberTracker( int *pnFrameNumber ) = 0;
	virtual void UnregisterFrameNumberTracker( int *pnFrameNumber ) = 0;
	virtual void RegisterManifest( ResourceManifestDesc_t *pDesc ) = 0;
	virtual void UnregisterManifest( ResourceManifestDesc_t *pDesc ) = 0;
};

DECLARE_TIER2_INTERFACE( IResourceManifestRegistry, g_pResourceManifestRegistry );

// Passed to IResourceTypeManager; CResourceSystem implements it
abstract_class IResourceSystemUtils
{
public:
	virtual void MarkLoadingWorkPending( ResourceHandle_t hResource, bool bHasWork ) = 0;
	// nReadPriority 1 queues the file read at the front of the async filesystem I/O queue, other values at the back
	virtual std::shared_ptr< IAsyncResourceDataRequest > CreateAsyncResourceDataRequest( ResourceHandle_t hResource, int64 nFileOffset, size_t nLoadSizeBytes, int nReadPriority, const char *pFileNameOverride ) = 0;
	virtual void SubmitAsyncResourceDataRequest( IAsyncResourceDataRequest *pRequestInterface ) = 0;
	virtual bool IsBlockingOnManifestLoad() = 0;

	// Rebuilds the external reference list of the resource from the references of the given types
	virtual void RebuildExternalReferences( ResourceHandle_t hResource, int nTypeCount, const ResourceType_t *pTypes ) = 0;
};

class IRD_RegisterResourceDataUtils;
class IResourceDeallocatorUtils;
struct ResourceFileHeader_t;

// How a resource got its data, passed to the type manager's LoadStreamingData, AsyncPreFinalize and FinalizeResource
enum ResourceLoadType_t : int
{
	RESOURCE_LOAD_TYPE_DEFAULT = 0,

	// The resource failed to load and is bound to the error resource of its type
	RESOURCE_LOAD_TYPE_ERROR_RESOURCE,

	// Loaded through a RESOURCE_MANIFEST_TYPE_BLOCKING_RELOAD manifest (the error resource of such a load counts too)
	RESOURCE_LOAD_TYPE_RELOAD,
};

enum ResourceStreamingType_t : int
{
	RESOURCE_STREAMING_TYPE_NONE = 0,
};

// Returned by IResourceTypeManager::AsyncProcessData
abstract_class IAsyncProcessedDataResult
{
public:
	virtual ~IAsyncProcessedDataResult() = 0;
};

inline IAsyncProcessedDataResult::~IAsyncProcessedDataResult() {}

class IResourceTypeManager
{
public:
	virtual bool Init( IResourceSystemUtils *pUtils ) { return false; }
	virtual void Shutdown() {}

	// Called by RemoveResourceTypeManager after Shutdown, once the queued deletions are drained
	virtual void OnRemoved() {}

	virtual void *GetErrorResource() { return nullptr; }
	virtual bool NeedsFrameUpdate() const { return false; }
	virtual void FrameUpdate( int nFrame ) {}
	virtual bool RequiresFinalizeResourceCall() const { return false; }
	virtual void FinalizeResource( ResourceHandle_t hResource, ResourceLoadType_t nLoadType ) {}

	// Resources of a type that wants it get an AsyncPreFinalize call from an "AsyncPreFinalize" thread pool job
	virtual bool WantsAsyncPreFinalize() { return false; }
	virtual void AsyncPreFinalize( ResourceHandle_t hResource, ResourceLoadType_t nLoadType ) {}

	// Gates the GetNonStreamingDataLoadSize and LoadStreamingData calls
	virtual bool HasStreamingData() { return false; }

	virtual int GetNonStreamingDataLoadSize( ResourceHandle_t hResource, bool bIsBlockingLoad ) { return -1; }
	virtual void LoadStreamingData( ResourceHandle_t hResource, IResourceSystemUtils *pUtils ) {}
	virtual void AllocateResource( ResourceHandle_t hResource, ResourceId_t nId, const ResourceFileHeader_t *pHeader, IRD_RegisterResourceDataUtils *pUtils ) {}
	virtual void DeallocateResource( void *pData, IResourceDeallocatorUtils *pUtils ) {}
	virtual void NotifyResourceWithWorkPendingLoadCancelled( ResourceHandle_t hResource, void *pData ) {}
	virtual bool UsesData() const { return false; }
	// A resource of a type with streaming data gets a LoadStreamingData call when this returns false
	virtual bool IsStreamingDataLoaded( ResourceHandle_t hResource ) { return true; }
	virtual bool WantsAsyncProcessData() const { return false; }
	virtual std::unique_ptr< IAsyncProcessedDataResult > AsyncProcessData( ResourceHandle_t hResource, const ResourceFileHeader_t *pHeader ) { return nullptr; }

	// FlushNotifications runs with the resource status change flush when the type wants it
	virtual bool WantsFlushNotifications() { return false; }
	virtual void FlushNotifications() {}

	// Called around the drain of the resource deletion queue
	virtual void OnBeginResourceDeletions() {}
	virtual void OnEndResourceDeletions() {}
};

#endif // RESOURCESYSTEM_H
