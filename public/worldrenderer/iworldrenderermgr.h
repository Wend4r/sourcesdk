#ifndef IWORLDRENDERERMGR_H
#define IWORLDRENDERERMGR_H

#ifdef COMPILER_MSVC
#pragma once
#endif

#include "worldschema.h"

#include <appframework/iappsystem.h>
#include <tier0/platform.h>
#include <tier0/utlvector.h>
#include <resourcesystem/resourcehandle.h>
#include <resourcesystem/resourcehandletypes.h>

struct CreateWorldInfo_t;
class CThreadRWLock;
class IWorld;
class CEntityKeyValues;
class IWorldReference;
class CWorldVisibility;
class IWorldLoadUnloadCallback;
class IWorldVPKOverrideManager;
class IWorldVPKReference;
class ISceneWorld;
class CSceneObject;

// The renderable pair of one scene object of a world node (SceneObject_t::m_renderableModel / m_renderable),
// copied without taking references
struct WorldNodeRenderable_t
{
	CWeakHandle< InfoForResourceTypeCModel > m_renderableModel;
	// CWeakHandle< InfoForResourceTypeCRenderMesh > in the game
	ResourceHandle_t m_renderable;
};

// Slot order from worldrenderer.dll / libworldrenderer.so 1.41.9.7; names from the s&box DWARF build
// (IWorldRendererMgr) where it has the method, otherwise named by what the method does
abstract_class IWorldRendererMgr : public IAppSystem
{
public:
	virtual IWorldReference *CreateWorld( CreateWorldInfo_t &info ) = 0;
	virtual IWorldReference *GetWorldReference( int iIndex ) const = 0;
	virtual IWorldReference *FindWorldReferenceAndAddRef( SpawnGroupHandle_t hSpawnGroup ) = 0;
	virtual uint32 GetWorldCount( CThreadRWLock *lock = NULL ) = 0;
	virtual uint32 GetPendingWorldCount() = 0;
	virtual const char *GetWorldName( int iIndex ) const = 0;
	virtual IWorld *GetGeometryWorld( int iIndex ) = 0;
	virtual IWorld *GetGeometryWorld( IWorldReference *pWorldRef ) const = 0;
	virtual CWorldVisibility *GetWorldVisibility( int iIndex ) const = 0;

	// World group id
	virtual int FindLoadedWorldIndex( WorldGroupId_t hWorldGroupId, const char *pWorldName, CThreadRWLock *lock = NULL ) const = 0;
	virtual void UpdateObjectsForRendering( WorldGroupId_t hWorldGroupId, int nSkipFlags, const Vector &vCameraPos, float flLODScale, float flFarPlane, float flElapsedTime = 0.0f, const Vector *pCameraPos2 = NULL ) = 0;
	virtual bool IsFullyLoadedForPlayer( WorldGroupId_t hWorldGroupId, CSplitScreenSlot nSlot ) const = 0;
	virtual bool GetBoundsForWorld( WorldGroupId_t hWorldGroupId, Vector &vecMins, Vector &vecMaxs ) const = 0;
	// Appends the scene objects the world nodes of the group created
	virtual void GetWorldNodeSceneObjects( WorldGroupId_t hWorldGroupId, CUtlVector< CSceneObject * > *pSceneObjects ) = 0;

	// Named by what they do: append the renderables of the scene objects of the world nodes of the group,
	// or of one world node. True when every world node was loaded. MSVC orders the two in reverse
	virtual bool GetWorldNodeRenderables( WorldGroupId_t hWorldGroupId, CUtlVector< WorldNodeRenderable_t > *pRenderables ) = 0;
	virtual bool GetWorldNodeRenderables( HWorldNode hWorldNode, CUtlVector< WorldNodeRenderable_t > *pRenderables ) = 0;

	virtual void FindEntitiesByTargetname( WorldGroupId_t hWorldGroupId, const char *pTargetname, const char *pSearchLump, CUtlVector< const CEntityKeyValues * > &res ) const = 0;

	// World reference
	// pWorldRef NULL uses the first loaded world
	virtual const CUtlVector< const CEntityKeyValues * > *GetEntityList( IWorldReference *pWorldRef, const char *pSearchLump = NULL ) const = 0;
	// pLumpName NULL or empty checks for any entity lump
	virtual bool IsEntityLumpLoaded( IWorldReference *pWorldRef, const char *pLumpName ) const = 0;
	// Same lookup, forwarded to CWorld::IsEntityManifestLoaded (s&box name of the CWorld method)
	virtual bool IsEntityManifestLoaded( IWorldReference *pWorldRef, const char *pLumpName ) const = 0;

	// Named by what it does: flags every world whose spawn group handle masked with nSpawnGroupMask equals
	// nSpawnGroupValue, so that CreateWorld no longer shares it with a new world of the same name
	virtual void MarkWorldsNotShareable( uint32 nSpawnGroupMask, uint32 nSpawnGroupValue ) = 0;
	virtual void ServiceWorldRequests() = 0;
	virtual IWorld *FindWorld( WorldGroupId_t hWorldGroupId, const char *pWorldName ) = 0;

	// Load/Unload callback
	virtual void AddWorldLoadHandler( IWorldLoadUnloadCallback *pCallback ) = 0;
	virtual void RemoveWorldLoadHandler( IWorldLoadUnloadCallback *pCallback ) = 0;

	virtual int GetDeletedWorldCount() = 0;

	// VPK manager
	virtual void InstallWorldVPKOverrideManager( IWorldVPKOverrideManager *pVPKMgr ) = 0;
	virtual void RemoveWorldVPKOverrideManager( IWorldVPKOverrideManager *pVPKMgr ) = 0;

	// The world VPKs an override manager redirected: world name and the VPK path it mounted instead
	virtual int GetWorldVPKOverrideCount() = 0;
	virtual const char *GetWorldVPKOverrideWorldName( int i ) const = 0;
	virtual const char *GetWorldVPKOverridePath( int i ) const = 0;

	// Updates the wind of every world; the override times replace the arguments when set
	virtual void OnClientPreOutput( float flFrameTime, float flTime ) = 0;
	// FLT_MAX clears the override
	virtual void SetOverrideRenderTime( float flFrameTime, float flTime ) = 0;

	// Named by what it does: creates a CGrassTileObjectDesc scene object in pSceneWorld at mTransform.
	// pGrassWorldKey selects the grass world (CGrassWorld) of the tile; it, pTileData and bInitFlag are
	// handed to the initialisation of the tile
	virtual CSceneObject *CreateGrassTileSceneObject( const void *pGrassWorldKey, const matrix3x4_t &mTransform, ISceneWorld *pSceneWorld, const void *pTileData, bool bInitFlag ) = 0;

	// Grass flattening (r_grass_allow_flattening). The state keeps the last reported position and the squared radius.
	// An event is a position, a direction, two floats and a flag queued on the grass world of pGrassWorldKey;
	// the meaning of the floats and the flag is not established
	virtual void *CreateGrassFlattenState( float flRadius ) = 0;
	// Queues a flatten event once the position has moved further than the state's radius
	virtual void UpdateGrassFlattenState( void *pState, const void *pGrassWorldKey, const Vector &vPosition, float flEventParam1, float flEventParam2, bool bEventFlag ) = 0;
	virtual void DestroyGrassFlattenState( void *pState ) = 0;
	// Queues a flatten event directly
	virtual void AddGrassFlattenEvent( const void *pGrassWorldKey, const Vector &vPosition, const Vector &vDirection, float flEventParam1, float flEventParam2, bool bEventFlag ) = 0;

	// Wind. flSpeed is in percent; bOverride sets the wind at once, otherwise it blends towards the new target
	virtual void SetWindForWorld( const ISceneWorld *pSceneWorld, const Vector &vWindDir, float flSpeed, float flTime, bool bOverride ) = 0;
	virtual void ForgetWindForWorld( const ISceneWorld *pSceneWorld ) = 0;
	// Falls back to the wind convars when the world has no entry
	virtual void GetCurrentWindParams( const ISceneWorld *pSceneWorld, Vector &vWindDir, Vector &vWindSpeed, Vector &vWindOffset ) = 0;

	// Named by what it does: stores flValue in the grass world of pGrassWorldKey (float at +320)
	virtual void SetGrassWorldValue( const void *pGrassWorldKey, float flValue ) = 0;
	virtual void GetSceneWorldsForWorldGroup( WorldGroupId_t hWorldGroupId, CUtlVector< ISceneWorld * > &sceneWorlds ) = 0;
	// Named by what it does: runs the PrecacheAllWorldNodes that UpdateObjectsForRendering deferred
	virtual void PrecachePendingWorldNodes() = 0;

	// Named by what they do: two sets of pointers that only these methods use: add (bAdd) or remove an entry,
	// test for membership
	virtual void SetInRegisteredSet1( const void *pObject, bool bAdd ) = 0;
	virtual bool IsInRegisteredSet1( const void *pObject ) = 0;
	virtual void SetInRegisteredSet2( const void *pObject, bool bAdd ) = 0;
	virtual bool IsInRegisteredSet2( const void *pObject ) = 0;

	// Weather of a scene world; the client sets the values as the envRainStrength, envPuddleRippleStrength,
	// envPuddleRippleDirection, envWetnessCoverage and envWetnessDryingAmount shader attributes
	virtual int SetWeatherParams( const ISceneWorld *pSceneWorld, float flRainStrength, float flPuddleRippleStrength, float flPuddleRippleDirection, float flWetnessCoverage, float flWetnessDryingAmount ) = 0;
	virtual bool GetWeatherParams( const ISceneWorld *pSceneWorld, float *pRainStrength, float *pPuddleRippleStrength, float *pPuddleRippleDirection, float *pWetnessCoverage, float *pWetnessDryingAmount ) = 0;
	virtual void RemoveWeatherParams( const ISceneWorld *pSceneWorld ) = 0;

	// NULL when the VPK could not be mounted; releasing the reference unmounts it
	virtual IWorldVPKReference *MountWorldVPK( const char *pVPKName ) = 0;
};

class IWorld;

class IComputeWorldOriginCallback
{
public:
	virtual matrix3x4_t ComputeWorldOrigin(const char *pWorldName, SpawnGroupHandle_t hSpawnGroup, IWorld *pWorld) = 0;
};

#endif // IWORLDRENDERERMGR_H
