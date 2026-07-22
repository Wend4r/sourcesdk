#ifndef WORLDRENDERERMGR_H
#define WORLDRENDERERMGR_H

#ifdef _WIN32
	#pragma once
#endif

#include <mathlib/mathlib.h>
#include <tier0/platform.h>
#include <tier0/threadtools.h>
#include <tier0/utlstring.h>
#include <tier1/convar.h>
#include <tier1/utlhash.h>
#include <tier1/utlhashtable.h>
#include <tier1/utlmap.h>
#include <tier1/utlstringmap.h>
#include <tier1/utlvector.h>
#include <tier3/tier3.h>

#include "entitylump.h"
#include "icomputeworldorigin.h"
#include "iworld.h"
#include "iworldloadunload.h"
#include "iworldreference.h"
#include "iworldrenderermgr.h"
#include "worldschema.h"

#include "resourcefile/introspectedtokv3typemanager.h"
#include "resourcefile/resourcehandle.h"
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
	virtual ~CSingleWorldRep();

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
	HResourceManifest m_hResourceManifest;
	uint8 m_priority;
	WorldGroupId_t m_worldGroupId;
	uint8 m_nFlags;
	uint8 m_nFlags2;
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
	uint64 m_reservedA0;
};

class CWorld : public IWorld
{
public:
	World_t *GetWorldData() const { return m_pWorldData; }
	const matrix3x4a_t &GetWorldTransform() const { return m_mWorldTransform; }
	const CUtlVector< HEntityLump > &GetEntityLumps() const { return m_entityLumps; }

public:
	HWorldNodeStrong *m_pWorldNodeArray;
	CNodeSceneObjectData *m_pWorldNodeSceneObjectData;
	EWorldNodeStatusFlags_t *m_pWorldNodeStatus;
	HWorldStrong m_hWorld;
	World_t *m_pWorldData;
	CUtlVector< int32 > m_nodeLoadList;
	matrix3x4a_t m_mWorldTransform;
	CWorldRendererBakedLightingInfo m_worldBakedLightingInfo;
	CUtlMap< WorldTraversalId_t, WorldTraversal_t * > m_traversalMap;
	ISceneWorld *m_pSceneWorld;
	ResourceManifestLoadPriority_t m_priority;

	Vector m_vVisibleBoundsMin;
	Vector m_vVisibleBoundsMax;
	Vector m_vWorldBoundsMin;
	Vector m_vWorldBoundsMax;

	CUtlString m_worldPath;

	bool m_bVisibleBoundsValid : 1;
	bool m_bVisibilityUpdated : 1;
	bool m_bIsEmptyWorld : 1;
	bool m_bBlockUntilLoaded : 1;
	bool m_bPrecacheOnly : 1;
	bool m_bLoadSun : 1;
	bool m_bHasBakedLighting : 1;
	bool m_bLoadRequestsInFlight : 1;
	bool m_bDisableNodeStreaming : 1;

	CWorldSceneObjectRefHash m_sceneObjectRefs;
	CUtlVector< HResourceManifest > m_pendingManifests;
	CUtlVector< HEntityLump > m_entityLumps;
	CUtlHashtable< uint32, HEntityLump > m_uniqueIdToEntityLump;
	uint32 m_nTraversalCount;
	CInterlockedInt m_nAsyncRequestCount;
	CUtlVector< CUtlString > m_layerNames;
};

struct WorldWindParams_t
{
	float m_flParams[6];
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

struct SingleWorldEntityLump_t
{
	CUtlString m_worldName;
	HEntityLumpStrong m_hEntityLump;
};

COMPILE_TIME_ASSERT( sizeof( SingleWorldEntityLump_t ) == 16 );

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
	CUtlVector< SingleWorldEntityLump_t > m_worldEntityLumps;
	CUtlVector< IWorldLoadUnloadCallback * > m_worldLoadHandlers;
	CUtlVector< IWorldLoadUnloadCallback * > m_worldCreateCallbacks;
	CAtomicMutex m_worldRequestsMutex;
	CUtlVector< void * > m_worldRequestQueue;
	CUtlVector< WindEntry_t > m_windEntries;
	Vector m_vDefaultWindOffset;
	CUtlVector< IWorldVPKOverrideManager * > m_vpkOverrideManagers;
	CUtlOrderedMap< const ISceneWorld *, WorldWindParams_t > m_windEntryMap;
	float m_flOverrideRenderFrameTime;
	float m_flOverrideRenderTime;

	CConCommandMemberAccessor< CWorldRendererMgr > entity_lump_list;
	CConCommandMemberAccessor< CWorldRendererMgr > entity_lump_spew;
	CConCommandMemberAccessor< CWorldRendererMgr > world_layer_list;
	CConCommandMemberAccessor< CWorldRendererMgr > world_layer_set_visible;
	CConCommandMemberAccessor< CWorldRendererMgr > world_dump_loaded_worlds;
};

COMPILE_TIME_ASSERT( sizeof( CWorldRendererMgr ) == 864 );

#endif // WORLDRENDERERMGR_H
