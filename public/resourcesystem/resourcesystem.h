#ifndef RESOURCESYSTEM_RESOURCESYSTEM_H
#define RESOURCESYSTEM_RESOURCESYSTEM_H

#ifdef _WIN32
#pragma once
#endif

#include "resourcesystem/iresourcesystem.h"
#include "tier0/threadtools.h"
#include "tier0/tslist.h"
#include "tier0/utlhashtable.h"
#include "tier0/utlleanvector.h"
#include "tier0/utllinkedlist.h"
#include "tier0/utlstringlist.h"
#include "tier0/utlsymbollarge.h"
#include "tier1/utlvector.h"
#include "tier2/tier2.h"
#include "iresourcehandleutils.h"

class CResourceManifest;
struct ExtRefList_t;

struct ResourceTypeRegistration_t
{
	ResourceType_t m_nResourceType;

	// Null when m_bIsPlaceholderEntry is set
	IResourceTypeManager *m_pTypeManager;

	// Header of the embedded error binding read by HandleUtils_MakeSerialHandle and HandleUtils_Deref:
	// the serial whose low bits tag the handle, the binding marker and the serial handle itself
	uint32 m_nErrorResourceSerial;
	uint32 m_nErrorResourceMarker;
	SerialResourceHandle_t m_hErrorResourceSerial;

	ResourceBindingBase_t m_ErrorResource;
	const char *m_pResourceTypeName;
	HGameResourceManifest m_hErrorManifest;
	uint8 m_pad058[ 0x4 ];

	// Cached from the type manager by InstallTypeManager
	bool m_bUsesData;

	bool m_bIsPlaceholderEntry;

	// Cached from the type manager by InstallTypeManager
	bool m_bRequiresFinalizeResourceCall;
	bool m_bWantsAsyncPreFinalize;
	bool m_bNeedsFrameUpdate;
	bool m_bWantsFlushNotifications;
	uint8 m_pad062[ 0x46 ];
};

// Bindings keyed by their resource id, split into 4 submaps picked by the hash, each an
// open-addressing hash set with 16-wide control byte groups that owns its binding nodes
struct ResourceIdMap_t
{
	struct Submap_t
	{
		CThreadSpinRWLock m_Lock;
		int8 *m_pControl;
		ResourceBindingBase_t **m_ppSlots;
		size_t m_nSize;
		size_t m_nCapacity;

#ifdef _WIN32
		uint8 m_unk[ 16 ];
#else
		uint8 m_unk[ 8 ];
#endif

		size_t m_nGrowthLeft;
	};

	Submap_t m_Submaps[ 4 ];
};

class CResourceSystem : public CTier2AppSystem< IResourceSystem >, public IResourceSystemUtils, public IResourceHandleUtils, public IResourceManifestRegistry
{
public:
	struct ForcedSynchronizationCallbackInfo_t
	{
		void ( *m_pCallback )( void *pContext );
		void *m_pContext;
	};

	// Guards the type registrations
	CAtomicMutex m_ResourceTypeLock;

	int m_nResourceTypeCount;
	ResourceTypeRegistration_t m_ResourceType[ 64 ];
	// Copy of m_nResourceTypeCount for the readers that do not take m_ResourceTypeLock
	int m_nPublishedResourceTypeCount;

	int m_nFrameCount;
	int m_nBlockingLoadCount;
	CUtlVector< int * > m_FrameCounters;
	int m_nPendingGlobalStreamingDataRequests;

	// Queued AsyncPreFinalize jobs that have not finished yet
	CInterlockedInt m_nPendingAsyncPreFinalizeJobs;

	CTSQueue< CResourceManifest * > m_PendingResourceManifests;

	CUtlVector< CResourceManifest * > m_PendingResourceManifestDeletions;
	CAtomicMutex m_PendingResourceManifestDeletionsLock;
	CResourceManifest *m_pActiveBlockingManifest;
	CUtlLinkedList< CResourceManifest *, unsigned short > m_activeResourceManifests;
	CUtlBlockLinkedList< ExtRefList_t *, unsigned int > m_extRefs;

	CTSQueue< ResourceHandle_t > m_DeletionRequests;

	CUtlVector< HGameResourceManifest > m_JustInTimeManifests;
	CAtomicMutex m_Unk2B98;

	// CUtlLeanVector of 32-byte items, guarded by m_Unk2B98
	uint64 m_Unk2BA8[ 2 ];

	CUtlVector< void * > m_AsyncResourceListeners;
	IResourceUpdater *m_pResourceUpdater;
	uint64 m_nLongTypeManagerUpdateRelativeTicks;
	ResourceIdMap_t m_IdMap;
	ResourceIdMap_t m_AnonymousMap;
	uint64 m_nNextAnonymousId;
	bool m_bForceAllResourceLoadsToFail;
	bool m_bErrorResourcesReloaded;
	bool m_bInFrameUpdate;
	bool m_bDisallowNewManifestsStarting;
	bool m_bShuttingDown;
	bool m_bDrainingAllAsyncRequests;
	bool m_bExitOnResourceLoadFailure;
	bool m_bSpewBlockingLoads;
	bool m_bTypeManifestPriorityHasBeenSpecified;

	// Filled by SetResourceTypeManifestPriority
	CUtlHashtable< ResourceType_t, int > m_ResourceTypeManifestPriorities;

	CThreadEvent m_ResourceProgressEvent;
	CUtlSymbolTableLargeBase< false, 2048, CThreadSpinRWLock > m_ResourceNameSymbols;
	CUtlStringList m_whiteListedMissingResourceNames;
	CUtlVectorFixedGrowable< ForcedSynchronizationCallbackInfo_t, 8 > m_ForcedSynchronizationCallbacks;
	// Reference counts of the "ugc:" resources, keyed by the published file id (container not identified)
	uint64 m_UGCReferences[ 10 ];
	CUtlVector< IToolsResourcePreReloadListener * > m_ToolsResourceListeners;

	// AddResourceListeners and RemoveResourceListeners pick one by the listener kind
	CUtlVector< IToolsResourceListener * > m_ImmediateResourceListeners;
	CUtlVector< IToolsResourceListener * > m_DeferredResourceListeners;

	// CUtlVector of the 32-byte status changes queued for m_DeferredResourceListeners
	uint64 m_PendingResourceStatusChanges[ 3 ];

	// The queued status changes are flushed only outside ForceSynchronizationAndBlockUntilManifestLoaded
	int m_nForceSynchronizationDepth;

	// Indexed by the SetDebugFlag and GetDebugFlag flag
	bool m_pDebugModeEnabled[ 3 ];

	void *m_pTestFS;
	IFileSystem *m_pPreviousFullFileSystem;

	// Filled by QueueCallbackForNextUpdate
	CUtlVector< CUtlDelegate< void () > > m_NextUpdateCallbacks;

	// Lock, Unlock and IsLockedByCurrentThread
	CAtomicMutex m_Lock;

	// Binding nodes freed from the id maps, reused by the next insertion
	CTSListBase m_FreeBindingNodes;
};

#endif // RESOURCESYSTEM_RESOURCESYSTEM_H
