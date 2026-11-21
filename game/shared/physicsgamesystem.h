//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The game system that owns the physics worlds of the game and steps them.
//
//=============================================================================//

#ifndef PHYSICSGAMESYSTEM_H
#define PHYSICSGAMESYSTEM_H
#ifdef _WIN32
#pragma once
#endif

#include <functional>

#include "igamesystem.h"
#include "entityhandle.h"
#include "entity2/entityindex.h"
#include "mathlib/transform.h"
#include "mathlib/vector.h"
#include "materialsystem2/imaterial2.h"
#include "resourcesystem/resourcehandle.h"
#include "resourcesystem/resourcehandletypes.h"
#include "soundsystem/isoundopsystem.h"
#include "tier0/tslist.h"
#include "tier0/utlhashtable.h"
#include "tier0/utlleanvector.h"
#include "tier0/utlmap.h"
#include "tier0/utlstringtoken.h"
#include "tier0/utlvector.h"
#include "tier1/utlconcurrentappendvector.h"
#include "vphysics2/iphysicsworldeventlistener.h"

class CSkeletonInstance;
class CResourceManifestPrerequisite;
class IVPhysics2World;
class ISceneWorld;
class IPhysAggregateInstance;
class IPhysicsJoint;
class CSceneAnimatableObject;
class CPhysAggregateData;

// Resource type of the physics aggregate data (.vphys); not declared elsewhere in the SDK
class InfoForResourceTypeCPhysAggregateData
{
public:
	using RuntimeClass_t = CPhysAggregateData;
};

// Named by what it does: the objects CPhysicsGameSystem tells about every impact sound it queues
// (one virtual, no source names it)
class IPhysicsImpactSoundListener;

namespace physicssound
{
	// A queued impact sound. 40 bytes
	struct impactsound_t
	{
		CEntityHandle m_hEntity;
		CEntityIndex m_nEntityIndex;
		float m_flVolume;
		float m_flImpactSpeed;
		// Named by what it does: 500 by default, else a value of the other body (its 38th virtual)
		// when that body's motion type is 2
		float m_flOtherBodyFactor;
		CUtlStringToken m_nSurfaceProps;
		CUtlStringToken m_nSurfacePropsHit;
		Vector m_vOrigin;
	};

	// A queued break sound. 16 bytes
	struct breaksound_t
	{
		Vector m_vOrigin;
		CUtlStringToken m_nSurfacePropsBreak;
	};

	// A running scrape sound: the origin and the whole start info of the sound event. 48 bytes
	struct friction_t
	{
		Vector m_vOrigin;
		CEntityHandle m_hEntity;
		StartSoundEventInfo_t m_nSound;
		float m_flLastUpdateTime;
		float m_flLastEffectTime;
		float m_flLastVolume;
	};
}

//-----------------------------------------------------------------------------
// The step results the physics system fills while it simulates the worlds (the type name is in
// the RTTI of CPhysicsGameSystem::PushDynamicTransformsWorker; the members are named by what the
// code does with them). 72 bytes
//-----------------------------------------------------------------------------
struct VPhysXWorldStepResults
{
	// Passed to the entities' sleep handling (the second pass of the post-step callbacks)
	CUtlVector< IPhysAggregateInstance * > m_SleptAggregates;
	// Passed to the entities' wake handling and to their post-step update
	CUtlVector< IPhysAggregateInstance * > m_WokenAggregates;
	// The aggregates whose bodies moved: PushDynamicTransformsWorker copies their transforms
	CUtlVector< IPhysAggregateInstance * > m_MovedAggregates;
};

//-----------------------------------------------------------------------------
// Registered as "PhysicsGameSystem" (reallocating factory), 1408 bytes on both platforms, aligned
// to 128 (the deleting destructor frees it with the alignment). Second base
// IPhysicsWorldEventListener (+0x10). Members without a known name are named by what the code
// does.
//-----------------------------------------------------------------------------
class CPhysicsGameSystem : public IGameSystem, public IPhysicsWorldEventListener
{
public:
	DECLARE_GAME_SYSTEM();

	// Value of m_PhysicsSpawnGroups. 80 bytes
	struct PhysicsSpawnGroups_t
	{
		IPhysAggregateInstance *m_pLevelAggregateInstance;
		// The spawn group's world physics (.vphys)
		CStrongHandle< InfoForResourceTypeCPhysAggregateData > m_hPhysicsLevel;
		// Named by what it does: the world model the physics came from when it had no .vphys
		CStrongHandle< InfoForResourceTypeCModel > m_hLevelModel;
		bool m_bLevelDebugRenderCreated;
		CSceneAnimatableObject *m_pLevelDebugGeo;
		CTransform m_WorldOffset;
	};

	// Value of m_mapLevelWorlds (destroyed through its virtual destructor). 32 bytes
	struct PhysicsWorld_t
	{
		virtual ~PhysicsWorld_t();

		IVPhysics2World *pPhysicsWorld;
		ISceneWorld *pDebugSceneWorld;
		bool bNeedsDebugDraw;
	};

	// Named by what it does: the joint that ties a skeleton to its parent and the parent bone
	// (-1: the parent's root). Value of m_HierarchyConstraints
	struct HierarchyConstraint_t
	{
		IPhysicsJoint *m_pJoint;
		int m_nParentBone;
	};

	// Element of m_kinematicsToUpdate: the skeleton (which gets m_bMarkedForKinematicUpdate) and a
	// flag that is always true when queued (named by what the code does)
	struct KinematicUpdate_t
	{
		CSkeletonInstance *m_pSkeleton;
		bool m_bQueued;
	};

	// Element of m_dirtyScales: the members of the scene node's CGameSceneNodeHandle (without its
	// vtable)
	struct DirtyScale_t
	{
		CEntityHandle m_hOwner;
		CUtlStringToken m_name;
	};

	// Element of m_DeferredEntityCallbacks: run at the frame boundary while the entity still exists
	struct DeferredEntityCallback_t
	{
		CEntityHandle m_hEntity;
		std::function< void() > m_Callback;
	};

	// Creates the "SV GameInit" (0) and "SV EngineTime" (1) worlds, registers the manifest prerequisite
	GS_EVENT( GameInit );
	GS_EVENT( GameShutdown );
	// Loads the four debug materials
	GS_EVENT( GameActivate );
	// Drops the deferred entity callbacks
	GS_EVENT( GameDeactivate );
	// Loads the spawn group's world physics into m_PhysicsSpawnGroups
	GS_EVENT( PreSpawnGroupLoad );
	GS_EVENT( PostSpawnGroupUnload );
	// Empty
	GS_EVENT( ActiveSpawnGroupChanged );
	// Simulates
	GS_EVENT( ClientPostUpdate );
	GS_EVENT( ServerPreEntityThink );
	// Simulates
	GS_EVENT( ServerPostEntityThink );
	// Flushes the impact sounds
	GS_EVENT( ServerPreClientUpdate );
	GS_EVENT( ServerPostSimulate );
	GS_EVENT( ClientPostSimulate );
	GS_EVENT( ServerGameBeginAsyncPostTickWork );
	GS_EVENT( ServerPreEndAsyncPostTickWork );
	GS_EVENT( ServerPostEndAsyncPostTickWork );
	// Runs the deferred entity callbacks ("FrameBoundary")
	GS_EVENT( GameFrameBoundary );

	// Forward to the game system factory
	virtual void SetGameSystemGlobalPtrs( void *pValue ) override;
	virtual bool DoesGameSystemReallocate() override;

	virtual ~CPhysicsGameSystem();

	// IPhysicsWorldEventListener: sets bNeedsDebugDraw of the world's entry
	virtual void OnWorldRequiresDebugRendering( IVPhysics2World *pWorld, bool bRequiresDebugRendering ) override;

public:
	// Calls queued from any thread, run (or only destroyed while m_bSkipDeferredCalls) after the
	// post-step callbacks of a simulation step
	CTSQueue< std::function< void() > * > m_DeferredCalls;
	// Named by what the code does: zeroed by the constructor together with the next flag, read nowhere
	bool m_bZeroedAtConstruction[ 3 ];
	bool m_bSkipDeferredCalls;
	bool m_bRunningDeferredCalls;
	// Named by what the code does: neither build initializes, reads or writes these 8 bytes
	void *m_pUntouched;
	// Set while the post-step callbacks run
	bool m_bInPostStepCallbacks;

	// Spawn group handle -> its world physics
	CUtlMap< uint32, PhysicsSpawnGroups_t, unsigned short > m_PhysicsSpawnGroups;
	// World index (0 "SV GameInit", 1 "SV EngineTime") -> world
	CUtlMap< uint32, PhysicsWorld_t, unsigned short > m_mapLevelWorlds;

	HMaterialStrong m_hMatWireframe;
	HMaterialStrong m_hMatHiddenWireframe;
	HMaterialStrong m_hMatDefault;
	HMaterialStrong m_hMatCollisionGeometry;

	// Frame time accumulated since the last impact sound flush (flushed above 0.05)
	float m_flImpactSoundTime;
	CResourceManifestPrerequisite *m_pResourceManifestPrerequisite;
	// Named by what it does: collision events dispatched, wrapped at 0x3FFFFFFF
	int m_nCollisionEventCount;
	// Named by what it does: the change serial of the physics system's surface properties; a new
	// value reloads them
	int m_nSurfacePropertiesSerial;
	int m_nSubStepCount;
	float m_flSubStepInterval;
	// Named by what it does: the step settings GameInit writes (1, 1/64), read nowhere
	int m_nInitialSubStepCount;
	float m_flInitialSubStepInterval;
	// Skeleton -> the joint to its parent
	CUtlHashtable< CSkeletonInstance *, HierarchyConstraint_t > m_HierarchyConstraints;

	// Skeletons queued for their kinematic update (from any thread), preallocated to 128
	CUtlConcurrentAppendVector< KinematicUpdate_t > m_kinematicsToUpdate;
	// Skeletons queued for their hierarchy joint update (m_bMarkedForHierarchyJointUpdate)
	CUtlConcurrentAppendVector< CSkeletonInstance * > m_hierarchyJointsToUpdate;
	// Scene nodes whose scale changed
	CUtlConcurrentAppendVector< DirtyScale_t > m_dirtyScales;

	// Filled by the physics system each step, cleared after the post-step callbacks
	VPhysXWorldStepResults m_StepResults;

	CUtlVector< physicssound::impactsound_t > m_impactSounds;
	CUtlVector< physicssound::breaksound_t > m_breakSounds;
	CUtlVector< physicssound::friction_t > m_scrapeSounds;
	CUtlVector< IPhysicsImpactSoundListener * > m_ImpactSoundListeners;

	// Named by what they do: the ticks simulated this frame and the most in one frame ("Ticks: %d (%d)")
	int m_nSimulatedTicks;
	int m_nMaxSimulatedTicks;

	bool m_bNeedUpdateDebugSceneObjects : 1;

	CUtlLeanVector< DeferredEntityCallback_t > m_DeferredEntityCallbacks;
};

#endif // PHYSICSGAMESYSTEM_H
