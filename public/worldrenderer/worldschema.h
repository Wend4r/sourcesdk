#ifndef WORLD_SCHEMA_H
#define WORLD_SCHEMA_H

#ifdef _WIN32
	#pragma once
#endif

#include <resourcesystem/resourcehandle.h>

#include <tier0/platform.h>
#include <tier0/utlstring.h>
#include <tier0/utlvector.h>
#include <mathlib/vector.h>
#include <mathlib/vector2d.h>

struct World_t;
class CWorldNode;
class CVoxelVisibility;

class InfoForResourceTypeWorld_t
{
public:
	using RuntimeClass_t = World_t;
};

class InfoForResourceTypeCWorldNode
{
public:
	using RuntimeClass_t = CWorldNode;
};

class InfoForResourceTypeCVoxelVisibility
{
public:
	using RuntimeClass_t = CVoxelVisibility;
};

using HWorldStrong = CStrongHandle<InfoForResourceTypeWorld_t>;
using HWorldNodeStrong = CStrongHandle<InfoForResourceTypeCWorldNode>;
using HWorldVisStrong = CStrongHandle<InfoForResourceTypeCVoxelVisibility>;
using HWorldNode = CWeakHandle< InfoForResourceTypeCWorldNode >;
using HResourceManifest = void*;

//--------------------------------------------------------------------------------------
// Enum related
//--------------------------------------------------------------------------------------
enum WorldNodeFlags_t
{
	WORLD_NODE_SIMPLIFIED						= 0x0001,					// The geometry is simplified
	WORLD_NODE_UNIQUE_UV						= 0x0002,					// The geometry is uniquely mapped (likely, we're a higher LOD)
	WORLD_NODE_ATLASED							= 0x0004,					// This node was atlased but not uniquely mapped
	WORLD_NODE_KDTREE							= 0x0008,					// Node contains a kd-tree for raycasts
	WORLD_NODE_NODRAW							= 0x0010,					// Node has no actual draw calls... it's just a container for stuff and other nodes
	WORLD_NODE_START_TRAVERSAL					= 0x0020,					// Start a traversal at this node (add a check to ensure that the KDTREE flag also exists with this one)
	WORLD_NODE_CAN_SEE_SKY						= 0x0040,					// Can this node see the sky?
	WORLD_NODE_MOST_DETAILED					= 0x0080,					// Node is the most detailed node containing the original geometry and textures
};

// Schema classes of worldrenderer (fields and offsets from the CS2 schema, both platforms the same)
struct BakedLightingInfo_t
{
	// Which baked shadow channel a light uses on this map
	struct BakedShadowAssignment_t
	{
		uint32 m_nLightHash;
		uint32 m_nMapHash;
		int8 m_nShadowChannel;
	};

	uint32 m_nLightmapVersionNumber;
	uint32 m_nLightmapGameVersionNumber;
	Vector2D m_vLightmapUvScale;
	bool m_bHasLightmaps;
	bool m_bBakedShadowsGamma20;
	bool m_bCompressionEnabled;
	int8 m_nLPVEncoding;
	int8 m_nLightmapEncoding;
	uint8 m_nChartPackIterations;
	uint8 m_nVradQuality;
	// CStrongHandle< InfoForResourceTypeCTextureBase > in the game
	CUtlVector< ResourceHandle_t > m_lightMaps;
	CUtlVector< BakedShadowAssignment_t > m_bakedShadows;
};

struct WorldBuilderParams_t
{
	float m_flMinDrawVolumeSize;
	bool m_bBuildBakedLighting;
	bool m_bAggregateInstanceStreams;
	BakedLightingInfo_t m_bakedLightingInfo;
	uint64 m_nCompileTimestamp;
	uint64 m_nCompileFingerprint;
};

struct NodeData_t
{
	Vector m_vOrigin;
	Vector m_vMinBounds;
	Vector m_vMaxBounds;
	CUtlString m_worldNodePrefix;
};

#include <entity2/entityidentity.h> //TODO: declare CEntityIdentity/CEntityInstance as schema classes here.

#endif // WORLD_SCHEMA_H
