#ifndef CWORLD_H
#define CWORLD_H

#ifdef _WIN32
	#pragma once
#endif

#include "iworld.h"
#include "worldschema.h"
#include "centitylump.h"

#include "resourcefile/resourcetype.h"
#include <mathlib/mathlib.h>
#include <tier0/platform.h>
#include <tier0/utlstring.h>
#include <tier1/utlvector.h>
#include <tier1/utlmap.h>
#include <tier1/utlhash.h>
#include <tier1/utlhashtable.h>

class ISceneWorld;
class CNodeSceneObjectData;
class CWorldSceneObjectRef;
struct WorldTraversal_t;

using WorldTraversalId_t = uint64;
using EWorldNodeStatusFlags_t = uint8;

using SceneObjectRefCompareFn_t = bool ( * )( CWorldSceneObjectRef* const&, CWorldSceneObjectRef* const& );
using SceneObjectRefKeyFn_t = unsigned int ( * )( CWorldSceneObjectRef* const& );
using CWorldSceneObjectRefHash = CUtlHash<CWorldSceneObjectRef*, SceneObjectRefCompareFn_t, SceneObjectRefKeyFn_t>;

struct CWorldRendererBakedLightingInfo
{
	uint8 m_sceneSystemState[0x78];
	CStrongHandleVoid* m_hLightmapResource;
	CUtlHashtable<uint32, void*> m_lightmapTable;
	uint64 m_reservedA0;
};

class CWorld : public IWorld
{
public:
	World_t* GetWorldData() const { return m_pWorldData; }
	const matrix3x4a_t& GetWorldTransform() const { return m_mWorldTransform; }
	const CUtlVector<HEntityLump>& GetEntityLumps() const { return m_entityLumps; }

public:
	HWorldNodeStrong* m_pWorldNodeArray;
	CNodeSceneObjectData* m_pWorldNodeSceneObjectData;
	EWorldNodeStatusFlags_t* m_pWorldNodeStatus;
	HWorldStrong m_hWorld;
	World_t* m_pWorldData;
	CUtlVector<int32> m_nodeLoadList;
	matrix3x4a_t m_mWorldTransform;
	CWorldRendererBakedLightingInfo m_worldBakedLightingInfo;
	CUtlMap<WorldTraversalId_t, WorldTraversal_t*> m_traversalMap;
	ISceneWorld* m_pSceneWorld;
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
	CUtlVector<HResourceManifest> m_pendingManifests;
	CUtlVector<HEntityLump> m_entityLumps;
	CUtlHashtable<uint32, HEntityLump> m_uniqueIdToEntityLump;
	uint32 m_nTraversalCount;
	CInterlockedInt m_nAsyncRequestCount;
	CUtlVector<CUtlString> m_layerNames;
};

#endif // CWORLD_H
