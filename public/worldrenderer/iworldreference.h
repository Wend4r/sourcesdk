#ifndef IWORLDREFERENCE_H
#define IWORLDREFERENCE_H

#ifdef COMPILER_MSVC
#pragma once
#endif

#include "worldschema.h"

#include <tier0/platform.h>
#include <mathlib/vectorws.h>

class ISceneWorld;
class Vector;

// A reference counted world (CSingleWorldRep). No virtual destructor in the game
abstract_class IWorldReference
{
public:
	virtual uint32 AddRef() = 0;
	// The world is destroyed when the count reaches zero
	virtual uint32 Release() = 0;
	virtual bool IsWorldLoaded() const = 0;
	virtual bool IsErrorWorld() const = 0;
	virtual bool IsMarkedForDeletion() const = 0;
	virtual SpawnGroupHandle_t GetSpawnGroupHandle() const = 0;
	// FLT_MAX extents when the world is not loaded
	virtual bool GetWorldBounds( Vector &vecMins, Vector &vecMaxs ) const = 0;
	virtual bool HasWorldGeometry() const = 0;
	virtual void ClearComputeWorldOriginCallback() = 0;
	// Named by what it does: true when the world resource is loaded and has at least one world node
	virtual bool HasWorldNodes() const = 0;
	virtual void ForceBlockingLoad() = 0;
	virtual ISceneWorld *GetSceneWorld() const = 0;
	virtual WorldGroupId_t GetWorldGroupId() const = 0;
	virtual bool SetLayerVisiblity( const char *pLayerName, bool bVisible ) = 0;
	virtual void PrecacheAllWorldNodes( bool bBlockUntilLoaded ) = 0;
	// Compile timestamp and fingerprint from the world builder params
	virtual bool GetCompileData( uint64 &nTimestamp, uint64 &nFingerprint ) const = 0;
	// Named by what it does: shows or hides every scene object of the world node
	virtual void SetWorldVisible( bool bVisible ) = 0;
	// Named by what it does: GetWorldBounds into world space vectors
	virtual bool GetWorldBoundsWS( VectorWS &vecMins, VectorWS &vecMaxs ) const = 0;
};

#endif // IWORLDREFERENCE_H
