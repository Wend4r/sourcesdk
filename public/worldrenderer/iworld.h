#ifndef IWORLD_H
#define IWORLD_H

#ifdef COMPILER_MSVC
#pragma once
#endif

#include "worldschema.h"

#include <tier0/platform.h>
#include <tier0/utlvector.h>
#include <mathlib/camera.h>

class ISceneWorld;
class Vector;
class CEntityKeyValues;
class CUtlString;

abstract_class IWorld
{
public:
	// Loading
	virtual void Init( ISceneWorld *pScene, SpawnGroupHandle_t hSpawnGroup ) = 0;
	virtual void Shutdown() = 0;

	// Transform
	virtual void SetWorldTransform( const matrix3x4_t &mat ) = 0;
	virtual const matrix3x4_t &GetWorldTransform() const = 0;

	// Players
	virtual bool IsFullyLoadedForPlayer( WorldGroupId_t hWorldGroupId, CSplitScreenSlot nSlot ) const = 0;

	// Precache
	virtual void PrecacheAllWorldNodes( bool bUnknown, bool bBlockUntilFinished ) = 0;

	// Entities and lighting
	virtual const CUtlVector< const CEntityKeyValues * > *GetEntityList( const char *pSearchLump = NULL ) const = 0; // Can be return NULL value.

	// Searches for "[PR#]" followed by pTargetname when bUnknown is false; the world renderer manager passes false
	virtual void FindEntitiesByTargetname( const char *pTargetname, const char *pSearchLump, CUtlVector< const CEntityKeyValues * > &res, bool bUnknown ) const = 0;

	virtual bool HasLightmaps() const = 0;
	virtual bool HasBakedLighting() const = 0;
	virtual const BakedLightingInfo_t *GetBakedLightingInfo() const = 0;

	// Layers
	virtual void GetWorldLayerNames( CUtlVector< CUtlString > &layerNames ) = 0;
	virtual bool SetWorldLayerVisibility( const char *pLayerName, bool bVisible ) = 0;

	virtual void FindEntitiesByClassname( const char *pClassname, const char *pSearchLump, CUtlVector< const CEntityKeyValues * > &res ) const = 0;
};

#endif // IWORLD_H
