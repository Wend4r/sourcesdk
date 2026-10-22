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

	uint32 m_nUnk010;
	uint32 m_nUnk014;

	// Points at m_ErrorResource
	SerialResourceHandle_t m_Unk018;

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
	bool m_bUnk061; // IResourceTypeManager::unk020
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
	CAtomicMutex m_Unk040;

	int m_nUnk04C;
	ResourceTypeRegistration_t m_ResourceType[ 64 ];
	int m_nUnk2A50;

	int m_nFrameCount;
	int m_nBlockingLoadCount;
	CUtlVector< int * > m_FrameCounters;
	int m_nPendingGlobalStreamingDataRequests;

	// Queued AsyncPreFinalize jobs that have not finished yet
	CInterlockedInt m_nPendingAsyncPreFinalizeJobs;

	CTSQueue< CResourceManifest * > m_PendingResourceManifests;

	CUtlVector< CResourceManifest * > m_PendingResourceManifestDeletions;
	CAtomicMutex m_Unk2AD8;
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
	bool m_bUnk2DE8;
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
	CUtlSymbolTableLarge m_ResourceNameSymbols;
	CUtlStringList m_whiteListedMissingResourceNames;
	CUtlVectorFixedGrowable< ForcedSynchronizationCallbackInfo_t, 8 > m_ForcedSynchronizationCallbacks;
	// Reference counts of the "ugc:" resources, keyed by the published file id (container not identified)
	uint64 m_UGCReferences[ 10 ];
	CUtlVector< IToolsResourcePreReloadListener * > m_ToolsResourceListeners;

	// AddResourceListeners and RemoveResourceListeners pick one by the listener kind
	CUtlVector< IToolsResourceListener * > m_Unk2FA8;
	CUtlVector< IToolsResourceListener * > m_Unk2FC0;

	// CUtlVector of 32-byte items
	uint64 m_Unk2FD8[ 3 ];

	int m_nUnk2FF0;

	// Indexed by the SetDebugFlag and GetDebugFlag flag
	bool m_pDebugModeEnabled[ 3 ];

	void *m_pTestFS;
	IFileSystem *m_pPreviousFullFileSystem;

	// Filled by QueueCallbackForNextUpdate
	CUtlVector< CUtlDelegate< void () > > m_NextUpdateCallbacks;

	// Lock, Unlock and IsLockedByCurrentThread
	CAtomicMutex m_Lock;

	CTSListBase m_Unk3030;
};

#endif // RESOURCESYSTEM_RESOURCESYSTEM_H
