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
#include <tier0/utlstringtoken.h>
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

class CWorld;
class CWorldSceneObjectRef;
class CVoxelVisibility;
class ISceneWorld;
struct CreateWorldInfo_t;
struct WorldNode_t;

FORWARD_DECLARE_HANDLE( RenderBufferHandle_t );
FORWARD_DECLARE_HANDLE( ConstantBufferHandle_t );

// Layouts checked against the constructors, destructors and the methods named in the comments.

// A world the world renderer manages. The class has no virtual destructor; the manager destroys it
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
	// Created by the constructor ("world renderer - <name>") when m_bOwnsSceneWorld is set
	ISceneWorld *m_pSceneWorld;
	ResourceManifestLoadPriority_t m_priority;
	WorldGroupId_t m_worldGroupId;
	// Set by CWorldRendererMgr::CreateWorld_Internal once the world is created
	uint16 m_bLoaded : 1;
	// Named by what it does: CreateWorld_Internal does not attach the visibility data when set
	uint16 m_bSkipVisibility : 1;
	uint16 m_bMarkedForDeletion : 1;
	uint16 m_bHasWorldGeometry : 1;
	uint16 m_bIsEmptyWorld : 1;
	// Named by what it does: the world VPK was mounted for this world and is unmounted with it
	uint16 m_bMountedWorldVPK : 1;
	uint16 m_bBlockUntilLoaded : 1;
	uint16 m_bPrecacheOnly : 1;
	// Named by what it does: IsWorldLoaded asks the geometry world only when set
	uint16 m_bAsyncLoad : 1;
	uint16 m_bOwnsSceneWorld : 1;
	// Named by what it does: set by MarkWorldsNotShareable, CreateWorld does not reuse the world afterwards
	uint16 m_bNotShareable : 1;
	// Named by what it does: the geometry world hides the "world_layer_base" layer
	uint16 m_bHideBaseLayer : 1;
	int32 m_nRefCount;
	SpawnGroupHandle_t m_hSpawnGroup;
};

using SceneObjectRefCompareFn_t = bool ( * )( CWorldSceneObjectRef *const &, CWorldSceneObjectRef *const & );
using SceneObjectRefKeyFn_t = unsigned int ( * )( CWorldSceneObjectRef *const & );
using CWorldSceneObjectRefHash = CUtlHash< CWorldSceneObjectRef *, SceneObjectRefCompareFn_t, SceneObjectRefKeyFn_t >;

// The baked lighting a scene world renders with (scene system type). Filled by
// CWorldRendererBakedLightingInfo::Init from BakedLightingInfo_t
class CSceneSystemBakedLightingInfo
{
public:
	bool m_bHasLightMaps;
	// Named by what the code does: the world renderer only clears it (constructors), it never sets or reads it
	bool m_bReservedForSceneSystem;
	// BakedLightingInfo_t::m_bBakedShadowsGamma20
	bool m_bBakedShadowsGamma20;
	// irradiance, directional_irradiance, direct_light_shadows, debug_chart_color, directional_irradiance_sh2_dc,
	// directional_irradiance_sh2_r, _g, _b (token names of the lightmap textures)
	// CWeakHandle< InfoForResourceTypeCTextureBase > in the game
	CUtlVectorFixed< ResourceHandle_t, 8 > m_textures;
	CUtlVectorFixed< CUtlStringToken, 8 > m_textureBindings;
	// 52 bytes: lightmap uv scale, lightmap size and the texture indices
	ConstantBufferHandle_t m_hConstantBuffer;
};

class CWorldRendererBakedLightingInfo
{
public:
	CSceneSystemBakedLightingInfo m_bakedLightingInfo;
	// BakedLightingInfo_t::m_bakedShadows: light and map hash (key) to the shadow channel
	CUtlHashtable< uint64, int8 > m_bakedShadowChannels;
};

// The scene objects of a world node (CWorld keeps one)
class CNodeSceneObjectData
{
public:
	CUtlVector< CWorldSceneObjectRef * > m_sceneObjects;
	CUtlVector< RenderBufferHandle_t > m_extraVBHandles;
	// The world node layer visibility flags last applied to the scene objects
	uint64 m_nLastLayerVisibilityFlags;
	// Set by CWorldNode::CreateSceneObjects ("Creating %i scene objects for world node")
	bool m_bCreatedSceneObjects;
	// Named by what it does: the layer visibility flags were applied to the scene objects
	bool m_bLayerVisibilityApplied;
	CWorldRendererBakedLightingInfo m_nodeBakedLightingInfo;
	// Layer index of each scene object
	CUtlVector< int8 > m_sceneObjectLayerIndices;
};

class CWorld : public IWorld
{
public:
	World_t *GetWorldData() const { return m_pWorldData; }
	const matrix3x4a_t &GetWorldTransform() const { return m_mWorldTransform; }
	const CUtlVector< HEntityLump > &GetEntityLumps() const { return m_entityLumps; }

public:
	// The world node ("<m_worldNodePrefix>.vwnod" of the first node)
	HWorldNodeStrong m_hWorldNode;
	CNodeSceneObjectData m_nodeSceneObjectData;
	// Named by what the code does: 1 precache requested, 2 world node loaded, 4 wanted by UpdateObjectsForRendering
	uint32 m_nWorldNodeStatus;
	HWorldStrong m_hWorld;
	World_t *m_pWorldData;
	matrix3x4a_t m_mWorldTransform;
	CWorldRendererBakedLightingInfo m_worldBakedLightingInfo;
	// Named by what it does: whether the world was loaded at the last update of a world group and split screen slot
	// (key: WorldGroupId_t in the low half, CSplitScreenSlot in the high half); IsFullyLoadedForPlayer reads it
	CUtlMap< uint64, bool, int > m_fullyLoadedForPlayer;
	ISceneWorld *m_pSceneWorld;
	ResourceManifestLoadPriority_t m_priority;

	Vector m_vWorldBoundsMin;
	Vector m_vWorldBoundsMax;

	CUtlString m_worldPath;

	bool m_bIsEmptyWorld : 1;
	bool m_bBlockUntilLoaded : 1;
	bool m_bPrecacheOnly : 1;
	bool m_bBakedLightingIsInvalid : 1;
	bool m_bBakedLightingIsPreview : 1;
	// Named by what it does: the baked lighting was handed to the scene world
	bool m_bBakedLightingRegistered : 1;
	// Named by what it does: hides the "world_layer_base" layer when the scene objects are created
	bool m_bHideBaseLayer : 1;
	// Named by what it does: UpdateObjectsForRendering deferred a PrecacheAllWorldNodes
	bool m_bPrecachePending : 1;

	CWorldSceneObjectRefHash m_sceneObjectRefs;
	CUtlVector< HResourceManifest > m_pendingManifests;
	CUtlVector< HEntityLump > m_entityLumps;
	// The value IWorld::Init receives
	int32 m_nInitParam;
	// Named by what it does: world requests queued for this world; the manager defers destroying it while non-zero
	int32 m_nPendingRequestCount;
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

// Named after the shader attributes the client sets from it (envRainStrength, ...)
struct WorldWeatherParams_t
{
	float m_flRainStrength;
	float m_flPuddleRippleStrength;
	float m_flPuddleRippleDirection;
	float m_flWetnessCoverage;
	float m_flWetnessDryingAmount;
};

// A world VPK an override manager redirected
struct WorldVPKOverride_t
{
	CUtlString m_worldName;
	CUtlString m_vpkPath;
	// Returned by the override manager together with the path and handed back to it
	void *m_pOverrideData;
};

class CWorldNodeManager : public CDefaultIntrospectedToKV3TypeManager< 0x646F6E7776 /* 'vwnod' */, WorldNode_t >
{
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
	// Asked first to last; InstallWorldVPKOverrideManager inserts at the front
	CUtlVector< IWorldVPKOverrideManager * > m_vpkOverrideManagers;
	CUtlVector< IWorldLoadUnloadCallback * > m_worldLoadHandlers;
	CAtomicMutex m_worldRequestsMutex;
	// Records of 96 bytes the manager allocates and ServiceWorldRequests consumes
	CUtlVector< void * > m_worldRequestQueue;
	CUtlVector< WindEntry_t > m_windEntries;
	Vector m_vDefaultWindOffset;
	// Named by what they do: the sets of IWorldRendererMgr::SetInRegisteredSet1 / 2
	CUtlVector< const void * > m_registeredSet1;
	CUtlVector< const void * > m_registeredSet2;
	CUtlMap< const ISceneWorld *, WorldWeatherParams_t, int > m_weatherParams;
	float m_flOverrideRenderFrameTime;
	float m_flOverrideRenderTime;

	CConCommandMemberAccessor< CWorldRendererMgr > entity_lump_list;
	CConCommandMemberAccessor< CWorldRendererMgr > entity_lump_spew;
	CConCommandMemberAccessor< CWorldRendererMgr > world_layer_list;
	CConCommandMemberAccessor< CWorldRendererMgr > world_layer_set_visible;
	CConCommandMemberAccessor< CWorldRendererMgr > world_dump_loaded_worlds;
};

#endif // WORLDRENDERERMGR_H
