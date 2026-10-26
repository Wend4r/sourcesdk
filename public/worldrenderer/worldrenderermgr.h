#ifndef WORLDRENDERERMGR_H
#define WORLDRENDERERMGR_H

#ifdef _WIN32
	#pragma once
#endif

#include <mathlib/mathlib.h>
#include <tier0/platform.h>
#include <tier0/threadtools.h>
#include <tier0/utlstring.h>
#include <tier0/convar.h>
#include <tier0/utlhash.h>
#include <tier0/utlhashtable.h>
#include <tier0/utlmap.h>
#include <tier0/utlstringmap.h>
#include <tier0/utlvector.h>
#include <tier3/tier3.h>

#include "entitylump.h"
#include "icomputeworldorigin.h"
#include "iworld.h"
#include "iworldloadunload.h"
#include "iworldreference.h"
#include "iworldrenderermgr.h"
#include "worldschema.h"

#include "resourcefile/introspectedtokv3typemanager.h"
#include "resourcesystem/resourcehandle.h"
#include "resourcefile/resourcetype.h"

class CNodeSceneObjectData;
class CWorld;
class CWorldSceneObjectRef;
class CVoxelVisibility;
class ISceneWorld;
struct CreateWorldInfo_t;
struct WorldNode_t;
struct WorldTraversal_t;

class CSingleWorldRep : public IWorldReference
{
public:
	CSingleWorldRep( const CreateWorldInfo_t &info );
	~CSingleWorldRep();

	const char *GetWorldName() const { return m_worldName.Get(); }
	const char *GetFolder() const { return m_folder.Get(); }
	CWorld *GetGeometryWorld() const { return m_pGeometryWorld; }

public:
	CUtlString m_worldName;
	CUtlString m_folder;
	HResourceManifest m_hWorldManifest;
	HWorldStrong m_hGeometryWorld;
	HWorldVisStrong m_hWorldVis;
	CWorld *m_pGeometryWorld;
	CVoxelVisibility *m_pWorldVisibility;
	IComputeWorldOriginCallback *m_pComputeOriginCallback;
	ISceneWorld *m_pSceneWorld;
	ResourceManifestLoadPriority_t m_priority;
	WorldGroupId_t m_worldGroupId;

	bool m_bLoaded : 1;
	bool m_bLoadVis : 1;
	bool m_bMarkedForDeletion : 1;
	bool m_bUnk3 : 1; // CreateWorldInfo_t::m_bUnk02B, IWorldReference::unk007.
	bool m_bIsEmptyWorld : 1;
	bool m_bMountVPK : 1;
	bool m_bBlockUntilLoaded : 1;
	bool m_bSkipSceneObjectCreation : 1;
	bool m_bDisableNodeStreaming : 1;
	bool m_bOwnsSceneWorld : 1;
	bool m_bExcludedFromSpawnGroupLookup : 1;
	bool m_bHideBaseLayer : 1;

	int32 m_nRefCount;
	SpawnGroupHandle_t m_hSpawnGroup;
};

using WorldTraversalId_t = uint64;
using EWorldNodeStatusFlags_t = uint8;

using SceneObjectRefCompareFn_t = bool ( * )( CWorldSceneObjectRef *const &, CWorldSceneObjectRef *const & );
using SceneObjectRefKeyFn_t = unsigned int ( * )( CWorldSceneObjectRef *const & );
using CWorldSceneObjectRefHash = CUtlHash< CWorldSceneObjectRef *, SceneObjectRefCompareFn_t, SceneObjectRefKeyFn_t >;

struct CWorldRendererBakedLightingInfo
{
	uint8 m_sceneSystemState[0x78];
	CStrongHandleVoid *m_hLightmapResource;
	CUtlHashtable< uint32, void * > m_lightmapTable;
};

FORWARD_DECLARE_HANDLE( VertexBufferHandle_t );

// A "WorldExtraVertexStream" pooled vertex buffer of the world node and its "PooledLayout" input layout.
struct WorldExtraVertexStream_t
{
	VertexBufferHandle_t m_hVertexBuffer;
	void *m_pInputLayout;
};

// The scene objects that a world creates from its world node.
struct CWorldNodeSceneObjects
{
	CUtlVector< CWorldSceneObjectRef * > m_sceneObjectRefs;
	CUtlVector< WorldExtraVertexStream_t > m_extraVertexStreams;

	// The layer mask of the world node last applied to the scene objects.
	uint64 m_nVisibleLayerMask;

	bool m_bSceneObjectsCreated;

	// Clear hides every scene object; set shows the objects whose layer is in the visible layer mask.
	bool m_bVisible;

	CWorldRendererBakedLightingInfo m_bakedLightingInfo;

	// The world layer index of each scene object, in the order of m_sceneObjectRefs.
	CUtlVector< int8 > m_sceneObjectLayerIndices;
};

class CWorld : public IWorld
{
public:
	World_t *GetWorldData() const { return m_pWorldData; }
	const matrix3x4a_t &GetWorldTransform() const { return m_mWorldTransform; }
	const CUtlVector< HEntityLump > &GetEntityLumps() const { return m_entityLumps; }

public:
	HWorldNodeStrong m_hWorldNode;
	CWorldNodeSceneObjects m_worldNodeSceneObjects;
	// Bit 0 while the world node loads, bit 1 once it is loaded, bit 2 when its load is left to UpdateObjectsForRendering
	int32 m_nWorldNodeLoadState;
	HWorldStrong m_hWorld;
	World_t *m_pWorldData;
	matrix3x4a_t m_mWorldTransform;
	CWorldRendererBakedLightingInfo m_worldBakedLightingInfo;
	// Whether the nodes of each traversal are loaded.
	CUtlMap< WorldTraversalId_t, bool, int > m_traversalMap;
	ISceneWorld *m_pSceneWorld;
	ResourceManifestLoadPriority_t m_priority;

	Vector m_vWorldBoundsMin;
	Vector m_vWorldBoundsMax;

	CUtlString m_worldPath;

	bool m_bIsEmptyWorld : 1;
	bool m_bForceBlockingLoad : 1;
	bool m_bSkipSceneObjectCreation : 1;
	bool m_bBakedLightingIsInvalid : 1;
	bool m_bBakedLightingIsPreview : 1;
	// Set while m_worldBakedLightingInfo is added to the scene world (from Init until Shutdown)
	bool m_bBakedLightingAddedToSceneWorld : 1;
	bool m_bHideBaseLayer : 1;
	bool m_bWorldNodeLoadQueued : 1;

	CWorldSceneObjectRefHash m_sceneObjectRefs;
	CUtlVector< HResourceManifest > m_pendingManifests;
	CUtlVector< HEntityLump > m_entityLumps;
	SpawnGroupHandle_t m_hSpawnGroup;

	// World requests queued for the world; the deletion of the world waits while any is pending
	int32 m_nPendingWorldRequests;
};

// Set by IWorldRendererMgr::SetWeatherForWorld
struct WorldWeatherParams_t
{
	float m_flEnvRainStrength;
	float m_flEnvPuddleRippleStrength;
	float m_flEnvPuddleRippleDirection;
	float m_flEnvWetnessCoverage;
	float m_flEnvWetnessDryingAmount;
	float m_unk014;
};

struct WindEntry_t
{
	const ISceneWorld *m_pSceneWorld;
	Vector m_vTargetWindDir;
	float m_flTargetWindSpeed;
	float m_flLastWindSpeed;
	float m_flLastSetTime;
	Vector m_vWindDir;
	float m_flWindSpeed;
	Vector m_vWindOffset;
	bool m_bOverrideWind;
};
COMPILE_TIME_ASSERT( sizeof( WindEntry_t ) == 0x40 );

class CWorldNodeManager : public CTypesafeIntrospectedToKV3TypeManager< 0x646F6E7776 /* 'vwnod' */, WorldNode_t >
{
public:
	// Clears the global node manager pointer that the constructor sets
	virtual ~CWorldNodeManager();
};

class CWorldManager : public CDefaultIntrospectedToKV3TypeManager< 0x646C727776 /* 'vwrld' */, World_t >
{
};

class CVoxelVisibilityTypeManager : public IResourceTypeManager
{
};

class CEntityMgr : public CTypesafeIntrospectedToKV3TypeManager< 0x73746E6576 /* 'vents' */, PermEntityLumpData_t >
{
};

// Added when an IWorldVPKOverrideManager provides an override for a world VPK being mounted
struct WorldVPKOverride_t
{
	CUtlString m_vpkName;
	CUtlString m_overrideVPKName;

	// Returned by the override manager with the override name and passed back to every manager
	uint64 m_nUnk010;
};

class CWorldRendererMgr : public CTier3AppSystem< IWorldRendererMgr >
{
public:
	uint32 GetWorldCountDirect() const { return m_worldList.Count(); }

public:
	CWorldNodeManager m_worldNodeManager;
	CWorldManager m_worldManager;
	CVoxelVisibilityTypeManager m_worldVisManager;
	CEntityMgr m_entityLumpManager;
	CUtlVector< CSingleWorldRep * > m_worldList;
	CUtlVector< CSingleWorldRep * > m_worldPendingList;
	CUtlVector< CSingleWorldRep * > m_worldDeletionList;
	CThreadRWLock_FastRead m_worldLock;
	CUtlStringMap< int > m_worldVPKRefcounts;
	CUtlVector< WorldVPKOverride_t > m_worldVPKOverrides;
	CUtlVector< IWorldVPKOverrideManager * > m_vpkOverrideManagers;
	CUtlVector< IWorldLoadUnloadCallback * > m_worldLoadHandlers;
	CAtomicMutex m_worldRequestsMutex;
	CUtlVector< void * > m_worldRequestQueue;
	CUtlVector< WindEntry_t > m_windEntries;
	Vector m_vDefaultWindOffset;
	CUtlVector< const ISceneWorld * > m_rainTraceToSkyWorlds;
	CUtlOrderedMap< const ISceneWorld *, WorldWeatherParams_t > m_worldWeatherParams;
	float m_flOverrideRenderFrameTime;
	float m_flOverrideRenderTime;

	CConCommandMemberAccessor< CWorldRendererMgr > entity_lump_list;
	CConCommandMemberAccessor< CWorldRendererMgr > entity_lump_spew;
	CConCommandMemberAccessor< CWorldRendererMgr > world_layer_list;
	CConCommandMemberAccessor< CWorldRendererMgr > world_layer_set_visible;
	CConCommandMemberAccessor< CWorldRendererMgr > world_dump_loaded_worlds;
};

#ifdef _WIN32
COMPILE_TIME_ASSERT( sizeof( CWorldRendererMgr ) == 864 );
#else
COMPILE_TIME_ASSERT( sizeof( CWorldRendererMgr ) == 1288 );
#endif

#endif // WORLDRENDERERMGR_H
