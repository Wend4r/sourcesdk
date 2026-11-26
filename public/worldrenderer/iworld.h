#ifndef IWORLD_H
#define IWORLD_H

#ifdef COMPILER_MSVC
#pragma once
#endif

#include "worldschema.h"

#include <tier0/platform.h>
#include <tier0/utlstring.h>
#include <tier0/utlvector.h>
#include <mathlib/camera.h>
#include <resourcefile/resourcetype.h>

class ISceneWorld;
class Vector;
class CEntityKeyValues;
class CSceneSystemBakedLightingInfo;

// The geometry of a loaded world (CWorld). No virtual destructor in the game
abstract_class IWorld
{
public:
	// Loading. nInitParam is stored in the world and never read by the world renderer
	virtual void Init( ISceneWorld *pSceneWorld, int nInitParam ) = 0;
	// Releases the world node and its pending manifests and takes the baked lighting out of the scene world
	virtual void Shutdown() = 0;

	// Transform
	virtual void SetWorldTransform( const matrix3x4a_t &mat ) = 0;
	virtual const matrix3x4a_t &GetWorldTransform() const = 0;

	// Players
	// True when the world has no nodes or when the last update for this world group and slot found it loaded
	virtual bool IsFullyLoadedForPlayer( WorldGroupId_t hWorldGroupId, CSplitScreenSlot nSlot ) const = 0;

	// Precache
	virtual void PrecacheAllWorldNodes( ResourceManifestLoadBehavior_t eLoadBehavior, bool bBlockUntilLoaded ) = 0;

	// Entities
	// Can return NULL
	virtual const CUtlVector< const CEntityKeyValues * > *GetEntityList( const char *pSearchLump = NULL ) const = 0;
	// Without bUseRawName the target name is resolved against the prefab ("[PR#]") naming first
	virtual void FindEntitiesByTargetname( const char *pTargetname, const char *pSearchLump, CUtlVector< const CEntityKeyValues * > &res, bool bUseRawName ) const = 0;

	// Baked lighting
	// The baked lighting of the map is out of date or its lightmap textures are missing
	virtual bool HasInvalidBakedLighting() const = 0;
	// The map was compiled without baked lighting while the map builders require it
	virtual bool HasPreviewBakedLighting() const = 0;
	virtual const CSceneSystemBakedLightingInfo *GetBakedLightingInfo() const = 0;

	// Layers of the world node
	virtual void GetLayerNames( CUtlVector< CUtlString > *pLayerNames ) const = 0;
	virtual bool SetLayerVisiblity( const char *pLayerName, bool bVisible ) = 0;

	virtual void FindEntitiesByClassname( const char *pClassname, const char *pSearchLump, CUtlVector< const CEntityKeyValues * > &res ) const = 0;
};

#endif // IWORLD_H
