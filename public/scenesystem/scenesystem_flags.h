//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: Scene object flags
//
//===========================================================================//

#ifndef SCENESYSTEM_FLAGS_H
#define SCENESYSTEM_FLAGS_H
#ifdef _WIN32
#pragma once
#endif

#include <tier0/platform.h>

//-----------------------------------------------------------------------------
// Renderable flags of a scene object. Scene layers render the objects whose
// flags contain all of their required flags and none of their excluded ones.
//-----------------------------------------------------------------------------
enum ESceneObjectFlags : uint64
{
	SCENEOBJECTFLAG_NONE = 0x0,

	SCENEOBJECTFLAG_IS_OPAQUE = ( 1ull << 0 ),
	SCENEOBJECTFLAG_IS_TRANSLUCENT = ( 1ull << 1 ),
	SCENEOBJECTFLAG_ALPHA_TESTED = ( 1ull << 2 ),
	SCENEOBJECTFLAG_HAS_OVERLAY = ( 1ull << 3 ),

	// Set by ISceneSystem::SetObjectBoundsInfinite, cleared by ISceneSystem::SetSceneObjectBounds.
	SCENEOBJECTFLAG_INFINITE_BOUNDS = ( 1ull << 5 ),

	SCENEOBJECTFLAG_VIEWMODEL_LAYER = ( 1ull << 8 ),
	SCENEOBJECTFLAG_GAME_OVERLAY_LAYER = ( 1ull << 10 ),
	SCENEOBJECTFLAG_TOOLSCENE_OVERLAY_LAYER = ( 1ull << 16 ),
	SCENEOBJECTFLAG_TOOLS_UNLIT_LAYER = ( 1ull << 17 ),
	SCENEOBJECTFLAG_HAMMER_PREFAB_STENCIL_LAYER = ( 1ull << 19 ),
	SCENEOBJECTFLAG_HAMMER_SELECTION_STENCIL_LAYER = ( 1ull << 20 ),
	SCENEOBJECTFLAG_HAMMER_ENABLED_STENCIL_LAYER = ( 1ull << 21 ),

	// The world renderer sets it for world objects with OBJECT_TYPE_RENDER_TO_CUBEMAPS.
	SCENEOBJECTFLAG_RENDER_TO_CUBEMAPS = ( 1ull << 22 ),

	SCENEOBJECTFLAG_HAS_AO_PROXIES = ( 1ull << 28 ),
	SCENEOBJECTFLAG_ALPHA_TEST_ZPREPASS = ( 1ull << 29 ),
	SCENEOBJECTFLAG_OCCLUSION_RECULLED = ( 1ull << 30 ),
	SCENEOBJECTFLAG_NEEDS_DYNAMIC_REFLECTION_OR_REFRACTION = ( 1ull << 31 ),
	SCENEOBJECTFLAG_REFLECTS = ( 1ull << 32 ),
	SCENEOBJECTFLAG_DOES_NOT_ACCEPT_DECALS = ( 1ull << 33 ),
	SCENEOBJECTFLAG_WANTS_FRAMEBUFFER_COPY_TEXTURE = ( 1ull << 34 ),
	SCENEOBJECTFLAG_ISSUES_QUERIES = ( 1ull << 35 ),
	SCENEOBJECTFLAG_STATIC_OBJECT = ( 1ull << 36 ),

	// The world renderer sets it for world objects with OBJECT_TYPE_NEEDS_DYNAMIC_SHADOWS.
	SCENEOBJECTFLAG_NEEDS_DYNAMIC_SHADOWS = ( 1ull << 37 ),

	SCENEOBJECTFLAG_CAST_SHADOWS_ENABLED = ( 1ull << 38 ),

	// The object casts shadows only with both MATERIAL_SUPPORTS_SHADOWS and CAST_SHADOWS_ENABLED.
	SCENEOBJECTFLAG_MATERIAL_SUPPORTS_SHADOWS = ( 1ull << 39 ),

	SCENEOBJECTFLAG_NEEDS_STATIC_ENVMAP_BINDING = ( 1ull << 40 ),
	SCENEOBJECTFLAG_WANTS_PROBE_LIGHTING = ( 1ull << 41 ),
	SCENEOBJECTFLAG_NO_Z_PREPASS = ( 1ull << 42 ),
	SCENEOBJECTFLAG_FORWARD_LAYER_ONLY = ( 1ull << 43 ),
	SCENEOBJECTFLAG_NO_MOTION_VECTORS = ( 1ull << 44 ),
	SCENEOBJECTFLAG_CAN_RENDER_INTO_SST = ( 1ull << 45 ),

	// Set by ISceneSystem::SceneObject_UpdateFlagsBasedOnMaterial: no material of the object sets the NoZPrepass material
	// attribute without AlphaTestZPrepass, or every material does.
	SCENEOBJECTFLAG_ALL_MATERIALS_Z_PREPASS = ( 1ull << 46 ),
	SCENEOBJECTFLAG_ALL_MATERIALS_NO_Z_PREPASS = ( 1ull << 47 ),

	SCENEOBJECTFLAG_PIPELINE_SPECIFIC_0 = ( 1ull << 48 ),
	SCENEOBJECTFLAG_PIPELINE_SPECIFIC_1 = ( 1ull << 49 ),
	SCENEOBJECTFLAG_PIPELINE_SPECIFIC_2 = ( 1ull << 50 ),
	SCENEOBJECTFLAG_PIPELINE_SPECIFIC_3 = ( 1ull << 51 ),
	SCENEOBJECTFLAG_PIPELINE_SPECIFIC_4 = ( 1ull << 52 ),
	SCENEOBJECTFLAG_PIPELINE_SPECIFIC_5 = ( 1ull << 53 ),
	SCENEOBJECTFLAG_PIPELINE_SPECIFIC_6 = ( 1ull << 54 ),

	// Set while the object is in the dirty list; the dirty update clears it.
	SCENEOBJECTFLAG_NEEDS_DIRTY_UPDATE = ( 1ull << 59 ),
	SCENEOBJECTFLAG_IS_DISABLED = ( 1ull << 60 ),
	SCENEOBJECTFLAG_IS_LOADED = ( 1ull << 61 ),
	SCENEOBJECTFLAG_DELETED = ( 1ull << 62 ),
	SCENEOBJECTFLAG_DEBUG_BREAK = ( 1ull << 63 ),
};

//-----------------------------------------------------------------------------
// Type flags given to a scene object at creation.
//-----------------------------------------------------------------------------
enum ESceneObjectTypeFlags : uint64
{
	SCENEOBJECTTYPEFLAG_NONE = 0x0,

	// Deleting the object returns it to the pool it was allocated from.
	SCENEOBJECTTYPEFLAG_FROM_POOL = ( 1ull << 0 ),

	SCENEOBJECTTYPEFLAG_HAS_MESH_INSTANCE_DATA = ( 1ull << 1 ),
	SCENEOBJECTTYPEFLAG_DELETE_MESH_INSTANCE_DATA = ( 1ull << 2 ),

	// ISceneSystem::CreateSceneObject allocates the PVS data, whose cluster bits the culling tests;
	// the world renderer leaves it out for OBJECT_TYPE_DISABLE_VIS_CULLING.
	SCENEOBJECTTYPEFLAG_HAS_PVS_DATA = ( 1ull << 3 ),

	// World aggregates with a vertex emissive stream and OBJECT_TYPE_HAS_EMISSIVE_GI.
	SCENEOBJECTTYPEFLAG_HAS_EMISSIVE_VERTEX_STREAM = ( 1ull << 11 ),

	// Light, barn light, environment map and light probe volume objects.
	SCENEOBJECTTYPEFLAG_IS_LIGHT = ( 1ull << 12 ),
	SCENEOBJECTTYPEFLAG_IS_LIGHT_VOLUME = ( 1ull << 14 ),
	SCENEOBJECTTYPEFLAG_IS_ENVMAP = ( 1ull << 17 ),
	SCENEOBJECTTYPEFLAG_IS_DIRECT_LIGHT = ( 1ull << 18 ),
	SCENEOBJECTTYPEFLAG_IS_INDIRECT_LIGHT = ( 1ull << 19 ),

	// Taken from the "mapbuilder.blocklight" material attribute.
	SCENEOBJECTTYPEFLAG_BLOCK_LIGHT = ( 1ull << 20 ),

	// World aggregates that are not models (OBJECT_TYPE_MODEL clear).
	SCENEOBJECTTYPEFLAG_WORLD_GEOMETRY = ( 1ull << 21 ),

	SCENEOBJECTTYPEFLAG_DISTANCE_FIELD_INSTANCE = ( 1ull << 22 ),
	SCENEOBJECTTYPEFLAG_INSTANCED_MESH = ( 1ull << 25 ),

	// Objects the world renderer creates from a world node; the scene world keeps them in a separate list.
	SCENEOBJECTTYPEFLAG_WORLD_NODE_OBJECT = ( 1ull << 26 ),

	SCENEOBJECTTYPEFLAG_NOT_BATCHABLE = ( 1ull << 32 ),

	// The world renderer sets it for world objects with OBJECT_TYPE_DISABLED_IN_LOW_QUALITY.
	SCENEOBJECTTYPEFLAG_DISABLED_IN_LOW_QUALITY = ( 1ull << 35 ),

	// Rendering the object stores the current frame counter in CSceneObject::m_nGameRenderCounter.
	SCENEOBJECTTYPEFLAG_UPDATE_GAME_RENDER_COUNTER = ( 1ull << 37 ),
};

//-----------------------------------------------------------------------------
// Flags of a scene layer.
//-----------------------------------------------------------------------------
enum SceneLayerFlags_t : uint64
{
	LAYERFLAGS_NEEDS_FULLSORT = ( 1ull << 0 ),
	LAYERFLAGS_DO_NOT_SORT = ( 1ull << 1 ),
	LAYERFLAGS_NEVER_REMOVE = ( 1ull << 2 ),
	LAYERFLAGS_DEBUGDRAW = ( 1ull << 3 ),
	LAYERFLAGS_DEBUGDRAW_UI = ( 1ull << 4 ),
	LAYERFLAGS_DEBUGDRAW_2D = ( 1ull << 5 ),
	LAYERFLAGS_DEBUG_BREAK_ON_RENDERING = ( 1ull << 6 ),
	LAYERFLAGS_DEBUG_BREAK_ON_SUBMISSION = ( 1ull << 7 ),
	LAYERFLAGS_DISCARD_COLOR_BUFFERS = ( 1ull << 9 ),
	LAYERFLAGS_DISCARD_DEPTH_STENCIL_BUFFER = ( 1ull << 10 ),
	LAYERFLAGS_PRESERVE_COLOR_BUFFERS = ( 1ull << 11 ),
	LAYERFLAGS_PRESERVE_DEPTH_BUFFER = ( 1ull << 12 ),
	LAYERFLAGS_PRESERVE_STENCIL_BUFFER = ( 1ull << 13 ),
	LAYERFLAGS_READONLY_DEPTH_STENCIL = ( 1ull << 14 ),
	LAYERFLAGS_READONLY_DEPTH_STENCIL_NORESOLVE = ( 1ull << 15 ),
	LAYERFLAGS_DOESNT_MODIFY_COLOR_BUFFERS = ( 1ull << 16 ),
	LAYERFLAGS_DOESNT_MODIFY_DEPTH_STENCIL_BUFFER = ( 1ull << 17 ),
	LAYERFLAGS_MATCH_TARGET_VIEWPORT_SIZE = ( 1ull << 18 ),
	LAYERFLAGS_LIGHT_BINNER_SETUP_LAYER = ( 1ull << 20 ),
	LAYERFLAGS_NEEDS_PER_VIEW_LIGHTING_CONSTANTS = ( 1ull << 22 ),
	LAYERFLAGS_IS_DEPTH_RENDERING_PASS = ( 1ull << 24 ),
	LAYERFLAGS_IGNORE_LAYER_MATCH_ID = ( 1ull << 26 ),

	// Set by ISceneUtils::AddComputeSignalSemaphoreLayerToView for the layer it puts on the compute queue.
	LAYERFLAGS_ASYNC_COMPUTE = ( 1ull << 27 ),

	LAYERFLAGS_DISCARD_COLOR_BUFFERS_STORE = ( 1ull << 30 ),
	LAYERFLAGS_DISCARD_DEPTH_STENCIL_BUFFER_STORE = ( 1ull << 31 ),
};

#endif // SCENESYSTEM_FLAGS_H
