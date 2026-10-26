#ifndef IWORLDRENDERERMGR_H
#define IWORLDRENDERERMGR_H

#ifdef COMPILER_MSVC
#pragma once
#endif

#include "worldschema.h"

#include <appframework/iappsystem.h>
#include <resourcefile/resourcetype.h>
#include <tier0/platform.h>

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
class IComputeWorldOriginCallback;

enum EntityLumpLoadType_t
{
	EL_LOAD_TYPE_ASYNC = 0,
	EL_LOAD_TYPE_BLOCKING,
};

struct CreateWorldInfo_t
{
	const char *m_pWorldName;
	const char *m_pFolder;
	ISceneWorld *m_pSceneWorld;
	IComputeWorldOriginCallback *m_pComputeOriginCallback;
	WorldGroupId_t m_worldGroupId;
	SpawnGroupHandle_t m_hSpawnGroup;
	ResourceManifestLoadPriority_t m_manifestLoadPriority;
	bool m_bAsyncLoad;
	bool m_bLoadVis;

	// Looks the world up by spawn group and checks its origin callback; only such worlds are updated for rendering
	// (IWorldReference::ShouldUpdateForRendering).
	bool m_bUpdateForRendering;

	// Looks the world up by spawn group; the world node of the world loads without scene objects.
	bool m_bSkipSceneObjectCreation;

	bool m_bDisableNodeStreaming;
	bool m_bCreateSceneWorld;

	// The world VPK mount does not ask the IWorldVPKOverrideManager list for an override VPK.
	bool m_bSkipVPKOverrides;

	bool m_bRequireMountVPK;

	// Mounts the world VPK before loading.
	bool m_bMountVPK;

	// The lookup of an existing world by name ignores the world group.
	bool m_bFindInAnyWorldGroup;

	// Hides "world_layer_base" once the scene objects of the world node are created.
	bool m_bHideBaseLayer;
};

abstract_class IWorldRendererMgr : public IAppSystem
{
public:
	virtual IWorldReference *CreateWorld( const CreateWorldInfo_t &info ) = 0;
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
	virtual void GetBoundsForWorld( WorldGroupId_t hWorldGroupId, Vector &vecMins, Vector &vecMaxs ) const = 0;
	virtual void GetWorldNodeSceneObjects( WorldGroupId_t hWorldGroupId, CUtlVector< CSceneObject * > *pOutSceneObjects ) = 0;

	virtual bool CollectWorldPairs( WorldGroupId_t hWorldGroupId, void *pOut ) = 0;
	// hResource is the world node resource that CWorld keeps at m_hWorldNode.
	virtual bool CollectWorldPairs( ResourceHandle_t hResource, void *pOut ) = 0;

	virtual void FindEntitiesByTargetname( WorldGroupId_t hWorldGroupId, const char *pTargetname, const char *pSearchLump, CUtlVector< const CEntityKeyValues * > &res ) const = 0;

	// World reference
	// pWorldRef NULL uses the first loaded world
	virtual const CUtlVector< const CEntityKeyValues * > *GetEntityList( IWorldReference *pWorldRef, const char *pSearchLump = NULL ) const = 0;
	// pLumpName NULL or empty checks for any entity lump
	virtual bool LoadEntityLump( IWorldReference *pWorldRef, const char *pLumpName, EntityLumpLoadType_t nLoadType ) = 0;
	virtual bool IsEntityLumpLoaded( IWorldReference *pWorldRef, const char *pLumpName ) = 0;

	// Worlds whose spawn group handle masked by nSpawnGroupMask equals nSpawnGroupValue are no longer found by FindWorldReferenceAndAddRef.
	virtual void ExcludeWorldsFromSpawnGroupLookup( int nSpawnGroupMask, int nSpawnGroupValue ) = 0;
	virtual void ServiceWorldRequests() = 0;
	virtual IWorld *FindWorld( WorldGroupId_t hWorldGroupId, const char *pWorldName ) = 0;

	// Load/Unload callback
	virtual void AddWorldLoadHandler( IWorldLoadUnloadCallback *pCallback ) = 0;
	virtual void RemoveWorldLoadHandler( IWorldLoadUnloadCallback *pCallback ) = 0;

	virtual int GetDeletedWorldCount() = 0;

	// VPK manager
	virtual void InstallWorldVPKOverrideManager( IWorldVPKOverrideManager *pVPKMgr ) = 0;
	virtual void RemoveWorldVPKOverrideManager( IWorldVPKOverrideManager *pVPKMgr ) = 0;

	// World VPKs that an override manager replaced when they were mounted, and their overrides
	virtual int GetWorldVPKOverrideCount() = 0;
	virtual const char *GetOverriddenWorldVPKName( int i ) const = 0;
	virtual const char *GetWorldVPKOverrideName( int i ) const = 0;

	virtual void OnClientPreOutput( float flRenderFrameTime, float flRenderTime ) = 0;
	virtual void SetOverrideRenderTime( float flRenderFrameTime, float flRenderTime ) = 0;

	virtual CSceneObject *CreateGrassTileSceneObject( void *pUnk1, const matrix3x4_t &mat, ISceneWorld *pWorld, void *pUnk2, bool bUnk ) = 0;

	// The flattening state keeps the last position and the squared radius; the update
	// flattens along the movement once it exceeds the radius
	virtual void *CreateGrassFlatteningState( float flRadius ) = 0;
	virtual void UpdateGrassFlattening( void *pState, const ISceneWorld *pSceneWorld, const Vector &vecPos, float flRadius, float flStrength, bool bRadial ) = 0;
	virtual void DestroyGrassFlatteningState( void *pState ) = 0;

	// bRadial pushes the grass away from vecPos instead of along vecDir.
	virtual void FlattenGrass( const ISceneWorld *pSceneWorld, const Vector &vecPos, const Vector &vecDir, float flRadius, float flStrength, bool bRadial ) = 0;

	virtual void SetWindForWorld( const ISceneWorld *pSceneWorld, const Vector &vWindDir, float flWindSpeed, float flTime, bool bOverride ) = 0;
	virtual void ForgetWindForWorld( const ISceneWorld *pSceneWorld ) = 0;
	virtual void GetCurrentWindParams( const ISceneWorld *pSceneWorld, Vector &vStrengthFreq, Vector &vOffset, Vector &vWindDir ) = 0;

	// The strengths queued by FlattenGrass are divided by it.
	virtual void SetMaxGrassFlatteningStrength( const ISceneWorld *pSceneWorld, float flMaxStrength ) = 0;

	virtual void GetSceneWorldsForWorldGroup( WorldGroupId_t hWorldGroupId, CUtlVector< ISceneWorld * > &worldsOut ) = 0;

	// Starts the world node loads that UpdateObjectsForRendering queued, for the worlds that have IWorldReference::ShouldUpdateForRendering set.
	virtual void LoadQueuedWorldNodes() = 0;

	// Weather of a scene world, set from the map info entity (CMapInfo).
	virtual void SetRainTraceToSkyEnabled( const ISceneWorld *pSceneWorld, bool bRainTraceToSkyEnabled ) = 0;
	virtual bool IsRainTraceToSkyEnabled( const ISceneWorld *pSceneWorld ) = 0;
	virtual void SetWeatherForWorld( const ISceneWorld *pSceneWorld, float flEnvRainStrength, float flEnvPuddleRippleStrength, float flEnvPuddleRippleDirection, float flEnvWetnessCoverage, float flEnvWetnessDryingAmount ) = 0;
	virtual bool GetWeatherForWorld( const ISceneWorld *pSceneWorld, float *pflEnvRainStrength, float *pflEnvPuddleRippleStrength, float *pflEnvPuddleRippleDirection, float *pflEnvWetnessCoverage, float *pflEnvWetnessDryingAmount ) = 0;
	virtual void ForgetWeatherForWorld( const ISceneWorld *pSceneWorld ) = 0;

	// Releasing the reference unmounts the VPK
	virtual IWorldVPKReference *MountWorldVPK( const char *pVPKName ) = 0;
};

class IWorld;

class IComputeWorldOriginCallback
{
public:
	virtual matrix3x4_t ComputeWorldOrigin(const char *pWorldName, SpawnGroupHandle_t hSpawnGroup, IWorld *pWorld) = 0;
};

#endif // IWORLDRENDERERMGR_H
