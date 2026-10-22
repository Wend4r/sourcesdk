#ifndef IWORLDREFERENCE_H
#define IWORLDREFERENCE_H

#ifdef COMPILER_MSVC
#pragma once
#endif

#include "worldschema.h"

#include <tier0/platform.h>

class ISceneWorld;
class Vector;

abstract_class IWorldReference
{
public:
	virtual uint32 AddRef() = 0;
	virtual uint32 Release() = 0;
	virtual bool IsWorldLoaded() const = 0;
	virtual bool IsErrorWorld() const = 0;
	virtual bool IsMarkedForDeletion() const = 0;
	virtual SpawnGroupHandle_t GetSpawnGroupHandle() const = 0;
	virtual bool GetWorldBounds( Vector &vecMins, Vector &vecMaxs ) const = 0;

	// Reads a flag taken from the world creation info; only worlds with it set are
	// updated for rendering and checked by IWorldRendererMgr::IsFullyLoadedForPlayer.
	virtual bool unk007() const = 0;

	virtual void ClearComputeWorldOriginCallback() = 0;
	virtual bool HasWorldGeometry() const = 0;
	virtual void ForceBlockingLoad() = 0;
	virtual ISceneWorld *GetSceneWorld() const = 0;
	virtual WorldGroupId_t GetWorldGroupId() const = 0;
	virtual bool SetWorldLayerVisibility( const char *pLayerName, bool bVisible ) = 0;
	virtual void PrecacheAllWorldNodes( bool bBlockUntilFinished ) = 0;
	virtual bool GetCompileFingerprint( uint64 *pOutTime, uint64 *pOutFingerprint ) const = 0;

	// Shows or hides the scene objects of every world layer
	virtual void SetAllWorldLayersVisibility( bool bVisible ) = 0;

	// Same result as GetWorldBounds, copied out through temporaries.
	virtual bool unk017( Vector &vecMins, Vector &vecMaxs ) const = 0;
};

#endif // IWORLDREFERENCE_H
