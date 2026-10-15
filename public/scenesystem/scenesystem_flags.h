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
// Renderable flags of a scene object. Only the internal bits whose use is
// known are listed; the low bits are type, layer and render property bits.
//-----------------------------------------------------------------------------
enum ESceneObjectFlags : uint64
{
	SCENEOBJECTFLAG_NONE = 0x0,

	// Set while the object is in the dirty list; the dirty update clears it.
	SCENEOBJECTFLAG_NEEDS_DIRTY_UPDATE = 0x0800000000000000ull,
	SCENEOBJECTFLAG_IS_DISABLED = 0x1000000000000000ull,
	SCENEOBJECTFLAG_DELETED = 0x4000000000000000ull,
};

//-----------------------------------------------------------------------------
// Type flags given to a scene object at creation.
//-----------------------------------------------------------------------------
enum ESceneObjectTypeFlags : uint64
{
	SCENEOBJECTTYPEFLAG_NONE = 0x0,
	SCENEOBJECTTYPEFLAG_HAS_MESH_INSTANCE_DATA = 0x2,
	SCENEOBJECTTYPEFLAG_DELETE_MESH_INSTANCE_DATA = 0x4,
};

#endif // SCENESYSTEM_FLAGS_H
