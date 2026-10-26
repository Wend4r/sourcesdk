//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: Rubikon physics worlds and physics data embedded in another resource
//
//===========================================================================//

#ifndef VPHYSICS2_INTERFACE_H
#define VPHYSICS2_INTERFACE_H

#ifdef _WIN32
#pragma once
#endif

#include <functional>

#include "tier0/platform.h"
#include "tier0/threadtools.h"
#include "tier0/utlvector.h"
#include "tier0/utlleanvector.h"
#include "tier0/utlhashtable.h"
#include "tier0/utlstring.h"
#include "tier0/utlstringtoken.h"
#include "tier0/utldelegateimpl.h"
#include "mathlib/vector.h"
#include "mathlib/aabb.h"
#include "mathlib/transform.h"
#include "physicslib/param_types.h"
#include "rubikon/intersection.h"
#include "vphysics2/handle_decl.h"
#include "vphysx/vphysx_interface.h"
#include "resourcefile/resourcetype.h"
#include "gametrace.h"

class IRD_RegisterResourceDataUtils;
class IPhysicsBody;
class IPhysicsShape;
class IPhysicsJoint;
class IPhysicsSoftbody;
class IPhysicsRope;
class IPhysicsParticleRope;
class IPhysicsVehicle;
class IPhysicsCharacterProxy;
class IPhysicsMotionController;
class IPhysAggregateInstance;
class IPhysicsRagdoll;
class IPhysicsRagdollControl;
class IPhysicsWorldEventListener;
class IPhysReflContext;
class CRnWorld;
class CRnQueryCache;
class CRnContact;
class CRnBody;
class Bounds3;
class IMotionEvent;
class IVPhysics2World;
struct PhysicsContact_t;
struct PhysicsAggregateDef_t;
class CUtlBuffer;
class ISceneViewDebugOverlays;
class CCallQueue;
class CCommand;
class CFeModel;
class CFrustum;
class CPhysAggregateData;
class CRayIntervalVector;
class CBufferString;
class CTransform;
class Quaternion;
class ISceneWorld;
struct matrix3x4_t;
struct Ray_t;
struct RnQueryAttr_t;
struct RnCollisionAttr_t;
struct RnToolQueryHit_t;
struct PhysicsClosestPoint_t;
struct PhysicsWorldTraceProfile_t;
struct PhysicsJointDesc_t;
struct PhysicsSphericalJointDesc_t;
struct PhysicsRevoluteJointDesc_t;
struct PhysicsPrismaticJointDesc_t;
struct PhysicsConicalJointDesc_t;
struct PhysicsWeldJointDesc_t;
struct constraint_ballsocketparams_t;
struct constraint_ragdollparams_t;
struct constraint_pulleyparams_t;
struct constraint_lengthparams_t;
struct constraint_slidingparams_t;
struct constraint_fixedparams_t;
struct VPhysXAggregateData_t;
template < class T > class CEmbeddedIntrospectedOrKV3BlockHelper;

// The 2-byte class IVPhysics2World::unk152 returns. Linux returns it through a hidden pointer, so it is not trivially
// copyable; only its size is known, the destructor only reproduces that
struct VPhysics2WorldUnk152_t
{
	~VPhysics2WorldUnk152_t() {}

	uint8 m_Data[ 2 ];
};

// The zeroed 24-byte struct IVPhysics2World::unk156 and unk157 return; only its size is known
struct VPhysics2WorldUnk156_t
{
	uint8 m_Data[ 24 ];
};

// A hard impact the contact solve records for a contact that notifies, when the closing normal speeds of its points scaled by the
// inverse mass of the lighter body exceed 75
struct PhysicsImpactEvent_t
{
	// The lighter body of the contact and its entity id
	IPhysicsBody *m_pBody;

	// Copied from +64 of the CRnContact
	void *m_pContactData;

	uint32 m_nEntityId;
	float m_flImpact;

	// The surface properties of the materials of the two shapes
	CUtlStringToken m_SurfacePropertyA;
	CUtlStringToken m_SurfacePropertyB;
};

// The opacity of the debug shape of the shape with this id, sorted by the id
struct PhysicsShapeDebugOpacity_t
{
	int m_nShapeId;
	float m_flOpacity;
};

// The first hit of a sweep: fraction 1.0, null objects and sub-shape -1 when nothing is hit
struct CSweepResult
{
	IPhysicsBody *m_pBody;
	IPhysicsShape *m_pShape;
	float m_flFraction;

	// The sub-shape that was hit: a compound child and a mesh triangle
	uint32 m_nSubShapeChild;
	uint32 m_nSubShapeTriangle;
};

// The first hit of IVPhysics2World::LinearCast, with the closest points of the two shapes at the time of impact
// (the fields of CSweepResult followed by the points, which start in what is its tail padding)
struct CLinearSweepResult
{
	IPhysicsBody *m_pBody;
	IPhysicsShape *m_pShape;
	float m_flFraction;
	uint32 m_nSubShapeChild;
	uint32 m_nSubShapeTriangle;
	Vector m_vWitnessPoint1;
	Vector m_vWitnessPoint2;
};

// Called with user data 0 of each softbody that IVPhysics2 re-initialises from a replaced softbody model
typedef void ( *SoftbodyModelReplacedFn_t )( void *pUserData );

// The object counts IVPhysics2World::GetObjectCounts returns
struct PhysicsWorldObjectCounts_t
{
	int m_nBodyCount;
	int m_nSoftbodyCount;
	int m_nJointCount;

	// The solver and the report contacts
	int m_nContactCount;

	// Always 0
	int m_nUnk010;
};

// The zeroed 40-byte struct IVPhysics2World::unk212, unk213 and unk214 return; only its size and 8-byte alignment are known (fields at +16, +24 and +32)
struct VPhysics2WorldUnk212_t
{
	ALIGN8 uint8 m_Data[ 40 ] ALIGN8_POST;
};

// Flags of IVPhysics2::CreateWorld; phys_debug_draw selects the worlds with them as client, server and other (neither) worlds
#define PHYSICS_WORLD_CLIENT ( 1 << 0 ) // Body parts with VPhysXBodyPart_t::FLAG_ALWAYS_DYNAMIC_ON_CLIENT become dynamic
#define PHYSICS_WORLD_SERVER ( 1 << 1 )

// How the bodies of an aggregate instance move; the aggregate instance can also change it after its creation
enum PhysicsAggregateMotionType_t
{
	// Keeps the current body types when changed; on creation it is the same as PHYSICS_AGGREGATE_MOTION_FROM_PARTS
	PHYSICS_AGGREGATE_MOTION_UNCHANGED = 0,

	// Static or keyframed by VPhysXBodyPart_t::FLAG_STATIC and FLAG_KINEMATIC of the body part, dynamic otherwise
	PHYSICS_AGGREGATE_MOTION_FROM_PARTS,

	PHYSICS_AGGREGATE_MOTION_STATIC,
	PHYSICS_AGGREGATE_MOTION_KEYFRAMED,
};

abstract_class IVPhysics2World
{
public:
	// IVPhysics2::SimulateWorlds steps every world with these two, once per substep
	virtual void StepBeginReadOnlySimulation( float flTimeStep, bool bFirstSubstep ) = 0;
	virtual void StepEndReadOnlyGetResults( VPhysXWorldStepResults *pResults ) = 0;

	virtual void SimulateSoftbodies( float flSubstepTime, int nSubsteps ) = 0;

	// Both process the delayed deletion lists of the world
	virtual void ExecuteDelayedDeletions() = 0;

	// The thread that began the current step
	virtual ThreadId_t GetSimulationThreadId() const = 0;

	virtual void CleanupDeleteList() = 0;

	virtual void Clear() = 0;
	virtual void AdvanceTime( float flTimeStep ) = 0;
	virtual void Step( float flTimeStep ) = 0;

	// Empty in release builds; at every game frame boundary the server calls EndFrame and then BeginFrame on each world
	virtual void BeginFrame() = 0;
	virtual void EndFrame() = 0;

	virtual void BeginAsyncPostTickJobs( void *pUnused, bool bHighPriority ) = 0;
	virtual void EndAsyncPostTickJobs() = 0;
	// bHighPriorityOnly queues the job on the list that only the high priority pass runs
	virtual void PostRopeJob( const std::function< void() > &job, bool bHighPriorityOnly ) = 0;
	virtual void UpdateParticleRopes() = 0;
	virtual bool IsInAsyncPostTickJobs() = 0;
	virtual void Collide( float flTimeStep ) = 0;
	// Stops the dynamic bodies whose broadphase proxies leave the world bounds and moves them back inside, then updates the broadphase
	virtual void ClampBodiesToWorldBounds() = 0;

	virtual void SetGravity( const Vector &vGravity ) = 0;
	virtual const VectorAligned &GetGravity() = 0;

	// Air density in kg/m^3 (1.2 by default) that the drag of bodies uses
	virtual void SetAirDensity( float flDensity ) = 0;
	virtual float GetAirDensity() = 0;

	// 1 (the default) enables continuous collision of the bodies that have it enabled, as long as there are TOI iterations
	virtual void SetContinuousCollisionMode( int nMode ) = 0;
	virtual int GetContinuousCollisionMode() = 0;

	virtual void SetPositionIterations( int nIterations ) = 0;
	virtual int GetPositionIterations() = 0;
	virtual void SetVelocityIterations( int nIterations ) = 0;
	virtual int GetVelocityIterations() = 0;

	// Both set the same value, which is clamped to [0, 4]
	virtual void ResetTOIIterations() = 0;
	virtual void SetTOIIterations( int nIterations ) = 0;
	virtual int GetTOIIterations() = 0;

	virtual void ScaleImpulses( float flScale ) = 0;
	// When set (the default), joint-connected groups that reach 25 bodies or joints are tracked separately; the server clears it
	virtual bool IsJointGroupTrackingEnabled() = 0;
	virtual void SetJointGroupTrackingEnabled( bool bEnabled ) = 0;

	// Multithreading of the world step; the server applies phys_multithreading_enabled here
	virtual bool IsMultithreadingEnabled() = 0;
	virtual void SetMultithreadingEnabled( bool bEnabled ) = 0;
	virtual int GetThreadCount() = 0;
	virtual void SetThreadCount( int nThreadCount ) = 0;
	// Priority of the parallel jobs of the step (2 by default); the server uses 3 for its game world
	virtual void SetJobPriority( uint8 nPriority ) = 0;

	virtual float GetSimulationTime() = 0;
	virtual float GetStepStartTime() = 0;

	// The same body as GetSimulationTime on both platforms
	virtual float GetCurrentTime() = 0;
	virtual int GetStepCount() = 0;

	// Store CRnWorld ints that nothing in the library reads: the first (4 by default) precedes the multithreading fields, the third
	// (1 by default) follows the TOI iterations in the iteration block, the second (1 by default) follows it
	virtual void SetUnreadSetting1( int nValue ) = 0;
	virtual int GetUnreadSetting1() = 0;
	virtual void SetUnreadSetting2( int nValue ) = 0;
	virtual int GetUnreadSetting2() = 0;
	virtual void SetUnreadSetting3( int nValue ) = 0;
	virtual int GetUnreadSetting3() = 0;

	// Wind velocity at a position, sampled by the softbody step; the server installs its wind lookup
	virtual void SetWindCallback( Vector ( *pfnGetWind )( const Vector &vPosition ) ) = 0;
	virtual void SetSleepingEnabled( bool bEnabled ) = 0;
	virtual bool IsSleepingEnabled() = 0;
	virtual int GetSleepingBodyCount() = 0;
	virtual void BeginDeferredWake() = 0;
	virtual void EndDeferredWake() = 0;
	virtual bool IsWakeDeferred() = 0;

	virtual int GetBodyCount() = 0;
	virtual IPhysicsBody *GetBody( int nIndex ) = 0;
	virtual const IPhysicsBody *GetBody( int nIndex ) const = 0;

	// True if the body belongs to this world
	virtual bool HasBody( IPhysicsBody *pBody ) = 0;

	// MSVC groups these overloads in reverse order; the declaration order is Linux's
	virtual IPhysicsBody *AddBody() = 0;
	virtual IPhysicsBody *AddBody( const Vector &vPosition, const Quaternion &qOrientation, float flScale ) = 0;
	virtual void RemoveBody( IPhysicsBody *pBody ) = 0;

	virtual int GetAwakeBodyCount() = 0;
	virtual void GetAwakeBodies( CUtlVector< IPhysicsBody * > &bodies ) = 0;

	virtual IPhysicsSoftbody *AddSoftbody( const CFeModel *pFeModel ) = 0;
	virtual void RemoveSoftbody( IPhysicsSoftbody *pSoftbody ) = 0;
	virtual int GetSoftbodyCount() = 0;
	virtual IPhysicsSoftbody *GetSoftbody( int nIndex ) = 0;
	virtual const IPhysicsSoftbody *GetSoftbody( int nIndex ) const = 0;

	// The ground trace raise new softbodies start with (100 by default)
	virtual float GetSoftbodyGroundTraceRaise() = 0;
	virtual void SetSoftbodyGroundTraceRaise( float flRaise ) = 0;

	virtual void SetSoftbodyModelReplacedCallback( SoftbodyModelReplacedFn_t pfnCallback ) = 0;
	virtual SoftbodyModelReplacedFn_t GetSoftbodyModelReplacedCallback() = 0;

	virtual int GetJointCount() = 0;
	virtual IPhysicsJoint *GetJoint( int nIndex ) = 0;
	virtual const IPhysicsJoint *GetJoint( int nIndex ) const = 0;

	// True if the joint belongs to this world
	virtual bool HasJoint( IPhysicsJoint *pJoint ) = 0;

	virtual IPhysicsJoint *AddSphericalJoint( const PhysicsSphericalJointDesc_t &desc ) = 0;
	virtual IPhysicsJoint *AddRevoluteJoint( const PhysicsRevoluteJointDesc_t &desc ) = 0;
	virtual IPhysicsJoint *AddPrismaticJoint( const PhysicsPrismaticJointDesc_t &desc ) = 0;
	virtual IPhysicsJoint *AddConicalJoint( const PhysicsConicalJointDesc_t &desc ) = 0;
	virtual IPhysicsJoint *AddWeldJoint( const PhysicsWeldJointDesc_t &desc ) = 0;
	virtual IPhysicsJoint *AddGenericJoint( const PhysicsJointDesc_t &desc ) = 0;
	virtual IPhysicsJoint *AddSpringJoint( const PhysicsJointDesc_t &desc ) = 0;
	virtual IPhysicsJoint *AddPulleyJoint( const PhysicsJointDesc_t &desc ) = 0;
	virtual IPhysicsJoint *AddNullJoint( const PhysicsJointDesc_t &desc ) = 0;
	virtual IPhysicsJoint *AddSplineJoint( const PhysicsJointDesc_t &desc ) = 0;
	virtual IPhysicsJoint *AddWheelJoint( const PhysicsJointDesc_t &desc ) = 0;

	// Creates the joints of a set of joint descriptions and appends them to joints; bUseCurrentBodyTransforms builds the joint frames
	// from the current body transforms instead of the stored local frames
	virtual void AddJoints( const PhysicsAggregateDef_t &aggregate, const CUtlVector< IPhysicsBody * > &bodies, const void *pUnused1, const void *pUnused2, bool bUseCurrentBodyTransforms, CUtlVector< IPhysicsJoint * > &joints, CUtlVector< int > &parts ) = 0;

	virtual void RemoveJoint( IPhysicsJoint *pJoint ) = 0;

	// Constraints created from the legacy constraint parameters: constraintToReference is relative to the reference body
	virtual IPhysicsJoint *CreateBallsocketConstraint( IPhysicsBody *pReferenceBody, IPhysicsBody *pAttachedBody, const constraint_ballsocketparams_t &ballsocket ) = 0;
	virtual IPhysicsJoint *CreateRagdollConstraint( IPhysicsBody *pReferenceBody, IPhysicsBody *pAttachedBody, const matrix3x4_t &constraintToReference, const constraint_ragdollparams_t &ragdoll ) = 0;
	virtual IPhysicsJoint *CreatePulleyConstraint( IPhysicsBody *pReferenceBody, IPhysicsBody *pAttachedBody, const constraint_pulleyparams_t &pulley ) = 0;
	virtual IPhysicsJoint *CreateDistanceConstraint( IPhysicsBody *pReferenceBody, IPhysicsBody *pAttachedBody, const constraint_lengthparams_t &length ) = 0;
	virtual IPhysicsJoint *CreatePrismaticConstraint( IPhysicsBody *pReferenceBody, IPhysicsBody *pAttachedBody, const constraint_slidingparams_t &sliding, bool bUseBlockSolver ) = 0;
	virtual IPhysicsJoint *CreateFixedConstraint( IPhysicsBody *pReferenceBody, IPhysicsBody *pAttachedBody, const constraint_fixedparams_t &fixed, bool bUseBlockSolver ) = 0;

	virtual void GetConnectedBodies( IPhysicsBody *pBody, CUtlVector< IPhysicsBody * > &bodies ) = 0;
	virtual int GetConnectedBodyCount( IPhysicsBody *pBody ) = 0;

	virtual IPhysicsRope *CreateRope() = 0;

	// Clears the pointer on success
	virtual bool DestroyRope( IPhysicsRope **ppRope ) = 0;

	// The nodes are placed at the points transformed by transform; a value that is not positive selects the default (0.1, 2 and 8).
	// The rope copies the collision attributes
	virtual IPhysicsParticleRope *CreateParticleRope( const RnCollisionAttr_t *pCollisionAttr, const CTransform &transform, const CUtlVector< Vector > &points, float flNodeMass, float flNodeRadius, float flSegmentLength ) = 0;
	virtual bool DestroyParticleRope( IPhysicsParticleRope **ppRope ) = 0;
	virtual int GetParticleRopeCount() = 0;
	virtual IPhysicsParticleRope *GetParticleRope( int nIndex ) = 0;
	virtual const IPhysicsParticleRope *GetParticleRope( int nIndex ) const = 0;

	virtual IPhysicsVehicle *CreateVehicle( const void *pParams ) = 0;
	virtual bool DestroyVehicle( IPhysicsVehicle **ppVehicle ) = 0;
	virtual int GetVehicleCount() = 0;
	virtual IPhysicsVehicle *GetVehicle( int nIndex ) = 0;
	virtual const IPhysicsVehicle *GetVehicle( int nIndex ) const = 0;

	virtual IPhysicsCharacterProxy *CreateCharacterProxy( const void *pParams ) = 0;
	virtual bool DestroyCharacterProxy( IPhysicsCharacterProxy **ppCharacterProxy ) = 0;
	virtual int GetCharacterProxyCount() = 0;
	virtual IPhysicsCharacterProxy *GetCharacterProxy( int nIndex ) = 0;
	virtual const IPhysicsCharacterProxy *GetCharacterProxy( int nIndex ) const = 0;

	// The contacts the solver resolves (INTERSECTION_PAIR_RESOLVE_CONTACTS) and the contacts that only report
	virtual int GetSolverContactCount() = 0;
	virtual void GetSolverContact( int nIndex, PhysicsContact_t &contact ) = 0;
	virtual int GetReportContactCount() = 0;
	virtual void GetReportContact( int nIndex, PhysicsContact_t &contact ) = 0;

	// The contact between the two bodies is looked up from either side, so the order of the bodies does not matter
	virtual void DestroyContacts( IPhysicsBody *pBodyA, IPhysicsBody *pBodyB ) = 0;
	virtual bool HasContact( IPhysicsBody *pBodyA, IPhysicsBody *pBodyB ) = 0;

	// Copies the contact begin and end events (256-byte records) into a CUtlVector; unless bKeepEvents is set, the events are consumed.
	// The first int of a record is its type as IntersectionPairFlags_t bits: INTERSECTION_PAIR_NOTIFY_TOUCH_FOUND (with
	// INTERSECTION_PAIR_RESOLVE_CONTACTS when the contact resolves), INTERSECTION_PAIR_RESOLVE_CONTACTS alone,
	// INTERSECTION_PAIR_NOTIFY_TOUCH_PERSISTS or INTERSECTION_PAIR_NOTIFY_TOUCH_LOST
	virtual void GetContactEvents( void *pEvents, bool bKeepEvents ) = 0;
	virtual void ClearContactEvents() = 0;
	// Moves the impact events recorded since the last call into the vector
	virtual void GetImpactEvents( CUtlVector< PhysicsImpactEvent_t > &events ) = 0;

	virtual IPhysicsMotionController *CreateMotionController( IMotionEvent *pHandler ) = 0;
	virtual void DestroyMotionController( IPhysicsMotionController *pController ) = 0;
	virtual int GetMotionControllerCount() = 0;

	virtual int GetAggregateCount() = 0;
	virtual void GetActiveAggregateInstances( CUtlVector< IPhysAggregateInstance * > &aggregates ) = 0;
	virtual IPhysAggregateInstance *GetAggregate( int nIndex ) = 0;
	// The server passes the entity handle as nEntityId
	virtual IPhysAggregateInstance *CreateAggregateInstance( const CPhysAggregateData *pData, const CTransform &transform, uint32 nEntityId, PhysicsAggregateMotionType_t nMotionType ) = 0;
	virtual void DestroyAggregateInstance( IPhysAggregateInstance *pAggregate ) = 0;

	// Creates a body of nBodyType for each body of the aggregate whose name is a bone of the model; the velocities
	// come from the previous and the current bone transforms flDeltaTime apart, and the bodies get the entity id and the game index
	virtual IPhysicsRagdoll *CreateRagdoll( const void *pModel, const PhysicsAggregateDef_t *pAggregate, PhysicsBodyType_t nBodyType, const CTransform *pPrevBoneTransforms, const CTransform *pBoneTransforms, float flDeltaTime, uint32 nEntityId, uint16 nGameIndex ) = 0;
	virtual void DestroyRagdoll( IPhysicsRagdoll *pRagdoll ) = 0;
	// The first takes a reference-counted pointer to the data, the second a raw one.
	// MSVC groups these overloads in reverse order; the declaration order is Linux's
	virtual IPhysicsRagdollControl *CreateAbsoluteRagdollControl( IPhysicsRagdoll *pRagdoll, void **ppData ) = 0;
	virtual IPhysicsRagdollControl *CreateAbsoluteRagdollControl( IPhysicsRagdoll *pRagdoll, void *pData ) = 0;
	virtual void DestroyRagdollControl( IPhysicsRagdollControl *pControl ) = 0;

	virtual void SetEventListener( IPhysicsWorldEventListener *pListener ) = 0;

	// Callbacks the step runs; each entry is a callback with its context
	virtual void AddPreStepCallback( const void *pCallback ) = 0;
	virtual void RemovePreStepCallback( const void *pCallback ) = 0;
	virtual void AddPostStepCallback( const void *pCallback ) = 0;
	virtual void RemovePostStepCallback( const void *pCallback ) = 0;
	// Called with every resolved contact of a shape that has its contact callback enabled, together with the context
	virtual void SetContactCallback( const CUtlDelegate< void ( const PhysicsContact_t &contact, void *pContext ) > &callback, void *pContext ) = 0;

	// Collision filtering of entity pairs
	virtual bool DisableCollisionsOnEntityPair( uint32 nEntity1, uint32 nEntity2 ) = 0;
	virtual void EnableCollisionsOnEntityPair( uint32 nEntity1, uint32 nEntity2 ) = 0;
	virtual bool IsEntityPairDisabled( uint32 nEntity1, uint32 nEntity2 ) = 0;
	virtual void RemoveAllPairsForEntityId( uint32 nEntity ) = 0;
	virtual bool DoesEntityHaveCollisionPairs( uint32 nEntity ) = 0;
	virtual void GetDisabledEntityPairs( void *pOut ) = 0;

	virtual bool ShouldHit( const IPhysicsShape *pShape, const RnQueryAttr_t &attr ) = 0;

	// The union of the collision bits of every shape: the interaction layers (interacts as, with and exclude), the detail layers or
	// the collision groups
	virtual uint64 GetUsedCollisionMask( IntersectionNameIndex_t nMaskType ) = 0;

	virtual VPhysics2WorldUnk152_t unk152() = 0;

	// Empty
	virtual void unk153() = 0;

	// The bounds of everything in the broadphase
	virtual AABB_t GetBroadphaseBounds() = 0;

	// The bounds of all coordinates the world accepts
	virtual AABB_t GetWorldBounds() = 0;

	// Both return zeros
	virtual VPhysics2WorldUnk156_t unk156() = 0;
	virtual VPhysics2WorldUnk156_t unk157() = 0;

	virtual void CastRayAgainstBody( PhysicsTrace_t *pTrace, const Vector &vStart, const Vector &vDelta, IPhysicsBody *pBody ) = 0;

	// Traces against the world. The Multiple versions collect every hit
	virtual void CastRaySingle( PhysicsTrace_t *pTrace, const Vector &vStart, const Vector &vDelta, const RnQueryAttr_t &attr ) = 0;
	virtual void CastRayMultiple( CTraceVector &results, const Vector &vStart, const Vector &vDelta, const RnQueryAttr_t &attr ) = 0;

	virtual void CastRaySingleUnfiltered( PhysicsTrace_t *pTrace, const Vector &vStart, const Vector &vDelta ) = 0;
	virtual void CastRayMultipleUnfiltered( void *pResults, const Vector &vStart, const Vector &vDelta ) = 0;
	virtual void ToolCastRayMultipleUnfiltered( CUtlVectorFixedGrowable< RnToolQueryHit_t, 8 > &hits, const Vector &vStart, const Vector &vEnd, float flRadius ) = 0;
	// bMergeIntervals joins neighbouring intervals of the same kind
	// flMaxDistance limits the cast from vStart; a negative value casts the whole segment
	virtual void CastRayIntervals( CRayIntervalVector &intervals, const Vector &vStart, const Vector &vEnd, const RnQueryAttr_t &attr, bool bMergeIntervals, float flMaxDistance ) = 0;
	virtual void CastSphereAgainstBody( PhysicsTrace_t *pTrace, const Vector &vStart, const Vector &vDelta, float flRadius, IPhysicsBody *pBody ) = 0;

	// A radius that is not positive casts a ray
	virtual void CastSphereSingle( PhysicsTrace_t *pTrace, const Vector &vStart, const Vector &vDelta, float flRadius, const RnQueryAttr_t &attr ) = 0;
	virtual void CastSphereMultiple( CTraceVector &results, const Vector &vStart, const Vector &vDelta, float flRadius, const RnQueryAttr_t &attr ) = 0;

	virtual void CastBoxAgainstBody( PhysicsTrace_t *pTrace, const Vector &vStart, const Vector &vDelta, const Vector &vExtents, IPhysicsBody *pBody ) = 0;

	// Casts an axis-aligned box of the given half extents
	virtual void CastBoxSingle( PhysicsTrace_t *pTrace, const Vector &vStart, const Vector &vDelta, const Vector &vExtents, const RnQueryAttr_t &attr ) = 0;
	virtual void CastBoxMultiple( CTraceVector &results, const Vector &vStart, const Vector &vDelta, const Vector &vExtents, const RnQueryAttr_t &attr ) = 0;

	// Casts the shape that the ray describes
	virtual bool CastShapeSingle( PhysicsTrace_t *pTrace, const Ray_t &ray, const Vector &vStart, const Vector &vDelta, const RnQueryAttr_t &attr ) = 0;
	virtual bool CastShapeMultiple( CTraceVector &results, const Ray_t &ray, const Vector &vStart, const Vector &vDelta, const RnQueryAttr_t &attr ) = 0;

	// SweepShapeSingle and SweepShapeMultiple are overload sets that MSVC groups in reverse order; the declaration order is Linux's
	// LinearCast moves the ray shape by vDelta from transform; the sweeps move a body, a shape or the ray shape from start to end.
	// bKeepSkin keeps the 1/16 unit separation of the time of impact solve instead of solving to touching
	virtual CLinearSweepResult LinearCast( const CTransform &transform, const Ray_t &ray, const Vector &vDelta, const RnQueryAttr_t &attr, bool bKeepSkin ) = 0;
	virtual CSweepResult SweepBodySingle( const CTransform &start, const CTransform &end, IPhysicsBody *pBody, const RnQueryAttr_t &attr, bool bKeepSkin ) = 0;
	virtual void SweepBodyMultiple( CUtlVectorFixedGrowable< CSweepResult, 128 > &results, const CTransform &start, const CTransform &end, IPhysicsBody *pBody, const RnQueryAttr_t &attr, bool bKeepSkin ) = 0;
	virtual CSweepResult SweepShapeSingle( const CTransform &start, const CTransform &end, IPhysicsShape *pShape, const RnQueryAttr_t &attr, bool bKeepSkin ) = 0;
	virtual void SweepShapeMultiple( CUtlVectorFixedGrowable< CSweepResult, 128 > &results, const CTransform &start, const CTransform &end, IPhysicsShape *pShape, const RnQueryAttr_t &attr, bool bKeepSkin ) = 0;
	virtual CSweepResult SweepShapeSingle( const CTransform &start, const CTransform &end, const Ray_t &ray, const RnQueryAttr_t &attr, bool bKeepSkin ) = 0;
	virtual void SweepShapeMultiple( CUtlVectorFixedGrowable< CSweepResult, 128 > &results, const CTransform &start, const CTransform &end, const Ray_t &ray, const RnQueryAttr_t &attr, bool bKeepSkin ) = 0;

	// Shape queries that collect the shapes overlapping a volume or a swept volume. The game queries spheres, boxes and shapes with these.
	// MSVC groups these overloads in reverse order; the declaration order is Linux's
	virtual void Query( CQueryResult &results, const Vector &vCenter, float flRadius, const RnQueryAttr_t &attr, int nOverlapTest ) = 0;
	virtual void Query( CQueryResult &results, const Bounds3 &bounds, const RnQueryAttr_t &attr, int nOverlapTest ) = 0;
	virtual void Query( CQueryResult &results, const Vector &vStart, const Vector &vDelta, const Vector &vExtents, const RnQueryAttr_t &attr ) = 0;
	virtual void Query( CQueryResult &results, const Vector &vStart, const Vector &vDelta, const RnQueryAttr_t &attr ) = 0;
	virtual void Query( CQueryResult &results, const Vector &vStart, const Vector &vDelta, float flRadius, const RnQueryAttr_t &attr ) = 0;
	virtual void Query( CQueryResult &results, const Ray_t &ray, const Vector &vPosition, const RnQueryAttr_t &attr, int nOverlapTest ) = 0;
	virtual void Query( CQueryResult &results, const Ray_t &ray, const Vector &vStart, const Vector &vDelta, const RnQueryAttr_t &attr ) = 0;
	virtual bool Query( void *pResults, const void *pQuery ) = 0;

	// Runs the query on the broadphase trees whose type bit is in nTreeMask, with the callback context
	virtual bool QueryBroadphaseTrees( const void *pQuery, uint8 nTreeMask, void *pContext ) = 0;

	// Caches the broadphase proxies inside the bounds that pass the query attributes
	virtual CRnQueryCache *CreateQueryCache( const AABB_t &bounds, const RnQueryAttr_t &attr ) = 0;
	virtual void DestroyQueryCache( CRnQueryCache *pCache ) = 0;

	// Closest point of the shapes that pass the filter to a point, within a maximum distance; returns the distance.
	// MSVC groups these overloads in reverse order; the declaration order is Linux's
	virtual float ClosestPoint( const Vector &vPoint, float flMaxDistance, const std::function< bool( IPhysicsShape * ) > &filter, PhysicsClosestPoint_t *pClosest ) = 0;
	virtual float ClosestPoint( const Vector &vPoint, float flMaxDistance, const std::function< bool( IPhysicsShape * ) > &filter, Vector *pClosest ) = 0;

	// flMargin (clamped to 0) widens the bounds the shapes are collected from
	virtual void CollideShape( void *pResults, const CTransform &transform, const Ray_t &ray, const RnQueryAttr_t &attr, bool bKeepSkin, float flMargin ) = 0;

	// A grid of ground heights that softbodies sample for their ground height
	virtual void SetGroundHeightGrid( void *pHeightGrid ) = 0;
	virtual void *GetGroundHeightGrid() = 0;
	// Adds the entity id to a set GetContactEvents checks the entities of the contact events against and returns its index in the set;
	// the server calls it from CBaseEntity::UpdateOnRemove with the handle of the entity
	virtual int AddRemovedEntityId( uint32 nEntityId ) = 0;

	// The PHYSICS_WORLD_* flags IVPhysics2::CreateWorld was called with
	virtual uint32 GetCreateFlags() = 0;
	virtual void SetWorldReferenceBody( IPhysicsBody *pBody ) = 0;
	virtual IPhysicsBody *GetWorldReferenceBody() = 0;

	// Save and restore of object references. A restore translates the saved pointers through the mappings, which the server
	// adds before it restores the objects: a count followed by that many pairs of saved and current pointers
	virtual bool SaveObject( const void *pObject ) = 0;
	virtual void AddRestoreObjectMappings( const void *pMappings ) = 0;
	virtual bool RestoreObject( const void *pObject ) = 0;
	virtual void ClearRestoreObjectMappings() = 0;

	// Returns 0
	virtual int unk204() = 0;

	// Empty in release builds; the server passes the name it creates the world with
	virtual void SetDebugName( const char *pszName ) = 0;
	virtual void unk206() = 0;

	// A CRnWorld member of two hash tables with 24-byte entries that only the constructor and the cleanup touch
	virtual void *GetUnusedHashtables() = 0;

	// Both return 0
	virtual int unk208() = 0;
	virtual int unk209() = 0;

	virtual void DisableDebugRecording( bool bDisable ) = 0;
	virtual PhysicsWorldObjectCounts_t GetObjectCounts() = 0;

	// Each of these returns zeros
	virtual VPhysics2WorldUnk212_t unk212() = 0;
	virtual VPhysics2WorldUnk212_t unk213() = 0;
	virtual VPhysics2WorldUnk212_t unk214() = 0;

	virtual IPhysReflContext *GetReflContext() = 0;

	// Sends the frame recorded for the physics debugger while streaming to it, otherwise drops the recorded data.
	// bStateChanged sets the bit of the CRnWorld flags that starting a stream sets on every world
	virtual void FlushDebugRecording( bool bStateChanged ) = 0;

	virtual void UpdateDebugScene() = 0;

	// Empty
	virtual void unk218() = 0;

	virtual void unk219( void *pUnused1, void *pUnused2, void *pUnused3, void *pUnused4, void *pUnused5 ) = 0;
	// Creates or updates the debug shapes of the shapes that pass the debug draw filter when bEnabled is set
	virtual void UpdateDebugShapes( bool bEnabled ) = 0;
	virtual void DestroyDebugShapes() = 0;

	// Debug draw of the world through the given overlays, also used by IVPhysics2::DebugDrawWorlds
	virtual void Draw( ISceneViewDebugOverlays *pDebugOverlays, const CFrustum *pFrustum ) = 0;
	virtual void UpdateSceneObjects( CCallQueue &callQueue ) = 0;

	// A 16-byte CRnWorld member that only the constructor writes
	virtual void *GetUnusedWorldBlock() = 0;

	// The RnDebugLayers_t bits that Draw and UpdateSceneObjects use
	virtual uint64 GetDebugDrawOptions() = 0;
	virtual void SetDebugDrawOptions( uint64 nOptions ) = 0;

	// The scene world of the debug shapes; changing it destroys them. The server clears it when the game deactivates
	virtual void SetDebugSceneWorld( ISceneWorld *pSceneWorld ) = 0;
	virtual ISceneWorld *GetDebugSceneWorld() = 0;

	// IVPhysics2::SetVisualDebugCameraHint copies the frustum into every world
	virtual void SetVisualDebugCameraHint( int nUnused, const CFrustum &frustum ) = 0;
	virtual const CFrustum &GetVisualDebugCameraHint() = 0;

	// The CRnBody of the body
	virtual CRnBody *GetRnBody( const void *pUnused, IPhysicsBody *pBody ) = 0;

	// The transform and the body of a reference frame of the CRnWorld, which only its constructor sets (to identity and NULL)
	virtual IPhysicsBody *GetReferenceFrame( const void *pUnused, CTransform *pTransform ) = 0;

	// Debug draw filtering; both notify the event listener
	virtual void SetDebugDrawFilter( const void *pParams ) = 0;
	virtual void ClearDebugDrawFilter() = 0;
	virtual bool ShouldDrawCollisionAttributes( const RnCollisionAttr_t &attr ) = 0;
	virtual bool ShouldDrawShape( IPhysicsShape *pShape ) = 0;

	// bTestShapes also requires a shape of the body to pass ShouldDrawShape
	virtual bool ShouldDrawBody( IPhysicsBody *pBody, bool bTestShapes ) = 0;
	virtual bool ShouldDrawSoftbody( IPhysicsSoftbody *pSoftbody ) = 0;
	virtual bool ShouldDrawJoint( IPhysicsJoint *pJoint ) = 0;
	// Sets the opacity of the debug shape of every shape to its entry in the sorted range, 0 for the shapes without one
	virtual void SetDebugShapeOpacities( const PhysicsShapeDebugOpacity_t *pBegin, const PhysicsShapeDebugOpacity_t *pEnd ) = 0;

	// Sends the debug name of the world to the physics debugger
	virtual void SendDebugNameToDebugger() = 0;

	// Returns NULL in release builds
	virtual const char *GetDebugName() = 0;

	// Describes the physics library build, for example "Rubikon Release No-Names non-SIMD"
	virtual const char *GetBuildString() = 0;

	// Handles the physics world console command: snapshots, saving and dumping of the world
	virtual void DumpState( const CCommand &args, float flCurrentTime ) = 0;

	// Dumps the bodies, shapes and joints of the world to a file
	virtual void DumpToJSON( const char *pszFileName, void *pUnused ) = 0;

	virtual void GetBenchmarkPath( CBufferString &path ) = 0;

	// Line segments (40-byte records with the end points at +16 and +28) that RN_DRAW_RECORDED_FORCES draws
	virtual int GetDebugLineCount() = 0;
	virtual void *GetDebugLines() = 0;

	virtual void DescribeShapeCollision( CBufferString &text, IPhysicsShape *pShapeA, IPhysicsShape *pShapeB ) = 0;
	// Describes how the collision attributes of the shape match the query attributes
	virtual void DescribeShape( CBufferString &text, IPhysicsShape *pShape, const RnQueryAttr_t &attr ) = 0;
};


class IPhysReflContext
{
public:
	// Returns false
	virtual bool unk000() = 0;

	// The world the context belongs to
	virtual IVPhysics2World *GetWorld() = 0;
};

// The layers of the debug draw of a world. phys_debug_draw lists the names of RN_DRAW_STATIC_BODIES to RN_DRAW_SHAPE_EDGES
// (with RN_DRAW_CONTACTS among them); the layers after them have no name in the binaries and are named after what they draw
enum RnDebugLayers_t : uint64
{
	// Bodies of each type; RN_DRAW_AWAKE_BODIES limits them to the awake ones
	RN_DRAW_STATIC_BODIES = ( 1ull << 0 ),
	RN_DRAW_KEYFRAMED_BODIES = ( 1ull << 1 ),
	RN_DRAW_DYNAMIC_BODIES = ( 1ull << 2 ),

	// The solver contacts
	RN_DRAW_CONTACT_POINTS = ( 1ull << 3 ),
	RN_DRAW_CONTACT_MANIFOLDS = ( 1ull << 4 ),
	RN_DRAW_CONTACT_INDICES = ( 1ull << 5 ),
	RN_DRAW_CONTACTS = RN_DRAW_CONTACT_POINTS | RN_DRAW_CONTACT_MANIFOLDS | RN_DRAW_CONTACT_INDICES,

	RN_DRAW_JOINTS = ( 1ull << 6 ),

	// The broadphase proxy bounds of the shapes
	RN_DRAW_PROXIES = ( 1ull << 7 ),

	// The traces recorded while IFnPhysicsWorld::IsRecordingTraces is set
	RN_DRAW_RECORDED_RAYCASTS = ( 1ull << 8 ),

	// The line segments of IVPhysics2World::GetDebugLines
	RN_DRAW_RECORDED_FORCES = ( 1ull << 9 ),

	RN_DRAW_INERTIA_BOXES = ( 1ull << 10 ),
	RN_DRAW_MESH_FACE_INDICES = ( 1ull << 11 ),
	RN_DRAW_MESH_VERTEX_INDICES = ( 1ull << 12 ),
	RN_DRAW_MESH_EDGE_INDICES = ( 1ull << 13 ),
	RN_DRAW_AWAKE_BODIES = ( 1ull << 14 ),
	RN_DRAW_SOFTBODIES = ( 1ull << 15 ),
	RN_DRAW_SOFTBODY_INDICES = ( 1ull << 16 ),
	RN_DRAW_SOFTBODY_FIELDS = ( 1ull << 17 ),
	RN_DRAW_SOFTBODY_COLLISION = ( 1ull << 18 ),
	RN_DRAW_SOFTBODY_BASES = ( 1ull << 19 ),
	RN_DRAW_SOFTBODY_ANIM_BASES = ( 1ull << 20 ),
	RN_DRAW_SOFTBODY_NODE_NAMES = ( 1ull << 21 ),
	RN_DRAW_SHAPE_EDGES = ( 1ull << 22 ),

	// The anchors of the joints on their two bodies, except for wheel joints
	RN_DRAW_JOINT_ANCHORS = ( 1ull << 24 ),

	// The body, contact, triangle cache and joint counts as text
	RN_DRAW_STATISTICS = ( 1ull << 25 ),

	// The shapes of the bodies through the debug overlays
	RN_DRAW_BODY_SHAPES = ( 1ull << 26 ),

	// The instances of the debug meshes the world caches, and the bounds of the cache
	RN_DRAW_DEBUG_MESHES = ( 1ull << 27 ),
	RN_DRAW_DEBUG_MESH_BOUNDS = ( 1ull << 28 ),

	RN_DRAW_PARTICLE_ROPES = ( 1ull << 30 ),
	RN_DRAW_VEHICLES = ( 1ull << 31 ),
	RN_DRAW_CHARACTER_PROXIES = ( 1ull << 32 ),

	// Spheres and capsules are drawn by the shape debug draw only with this layer
	RN_DRAW_SPHERES_AND_CAPSULES = ( 1ull << 34 ),

	// Keeps the debug shape scene objects of the bodies updated while no debug draw filter is set
	RN_DRAW_DEBUG_SHAPES = ( 1ull << 36 ),

	// The body islands of the world, each in its own color
	RN_DRAW_ISLANDS = ( 1ull << 37 ),

	// The GJK proxies of the sphere, capsule and hull shapes
	RN_DRAW_GJK_PROXIES = ( 1ull << 38 ),
};

// The debug draw options of a world, a 128-bit mask passed by value to the debug draw functions
struct RnDebugDrawOptions_t
{
	// RnDebugLayers_t bits; GetDebugDrawOptions and SetDebugDrawOptions access them
	uint64 m_nLayers;

	// The upper half of the mask, which the constructor clears and no option test reads
	uint64 m_nExtendedFlags;
};

class CVPhys2World : public IVPhysics2World, public IPhysReflContext
{
public:
	// Itanium also lists these in the primary vtable
	IVPhysics2World *GetWorld() override;
	bool unk000() override;

public:
	// PHYSICS_WORLD_* flags
	uint32 m_nCreateFlags;

	// Taken from a counter that every new world increments
	int m_nWorldId;

	RnDebugDrawOptions_t m_DebugDrawOptions;

	// Cleared by the constructor and not used otherwise
	void *m_pUnk028;
	CRnWorld *m_pRnWorld;

	// Set by the constructor when a command line parameter is present; nothing reads it
	bool m_bUnk038;

	bool m_bDebugRecordingDisabled;

	// When set, StepEndReadOnlyGetResults saves the world to <this path>/<step count>.rnsave after every step
	CUtlString m_sStepSavePath;

	CUtlLeanVector< IPhysAggregateInstance * > m_Aggregates;

	// The user data objects of the awake bodies, and of the bodies that woke up or fell asleep since the last
	// StepEndReadOnlyGetResults, which fills the woke up, asleep and awake lists of the results from them
	CUtlHashtable< void * > m_AwakeObjects;
	CUtlHashtable< void * > m_SleepStateChangedObjects;

	CUtlVector< IPhysicsMotionController * > m_MotionControllers;

	// Objects whose deletion is delayed; the count before each list is how many of its entries are ready
	int m_nAggregatesReadyToDelete;
	CUtlVector< IPhysAggregateInstance * > m_AggregatesToDelete;
	IPhysicsWorldEventListener *m_pEventListener;
	int m_nJointsReadyToDelete;
	CUtlVector< IPhysicsJoint * > m_JointsToDelete;
	int m_nBodiesReadyToDelete;
	CUtlVector< IPhysicsBody * > m_BodiesToDelete;
	int m_nShapesReadyToDelete;
	CUtlVector< IPhysicsShape * > m_ShapesToDelete;

	IPhysicsBody *m_pWorldReferenceBody;

	// The job that BeginAsyncPostTickJobs queues
	void *m_pAsyncPostTickJob;

	// Set when a motion controller asks for the asynchronous update
	bool m_bMotionControllersNeedAsyncUpdate;

	// The step count whose results were collected last
	int m_nResultsStepCount;
	ThreadId_t m_nSimulationThreadId;
};

// A result that ReRunRecordedTrace appends; only its size and 8-byte alignment are known
struct RecordedTraceResult_t
{
	ALIGN8 uint8 m_Data[ 88 ] ALIGN8_POST;
};

// The world query of a recorded trace, which IFnPhysicsWorld::ReRunRecordedTrace repeats; each type has its own list of traces.
// The library records no trace itself, the callers of IFnPhysicsWorld::RecordTrace choose the type
enum RecordedTraceType_t
{
	RECORDED_TRACE_CAST_SHAPE = 0, // CastShapeSingle or CastShapeMultiple
	RECORDED_TRACE_SWEEP_SHAPE, // SweepShapeSingle or SweepShapeMultiple with the ray shape
	RECORDED_TRACE_COLLIDE_SHAPE, // CollideShape
	RECORDED_TRACE_LINEAR_CAST, // LinearCast
	RECORDED_TRACE_RAY_INTERVALS, // CastRayIntervals
	RECORDED_TRACE_CLOSEST_POINT, // ClosestPoint
	RECORDED_TRACE_QUERY, // Query with the ray shape swept from m_vStart by m_vDelta
	RECORDED_TRACE_CAST_SHAPE2, // Repeated the same way as RECORDED_TRACE_CAST_SHAPE

	RECORDED_TRACE_TYPE_COUNT
};

// RecordedTrace_t::m_nFlags
#define RECORDED_TRACE_SINGLE ( 1 << 1 ) // The single hit version of a query instead of the one that collects every hit
#define RECORDED_TRACE_KEEP_SKIN ( 1 << 4 ) // The bKeepSkin argument of the sweeps and of LinearCast
#define RECORDED_TRACE_COLLIDE_KEEP_SKIN ( 1 << 6 ) // The bKeepSkin argument of CollideShape

// A world query recorded while IFnPhysicsWorld::IsRecordingTraces is set
struct RecordedTrace_t
{
	// The ray shape: the bounds of a hull, the radius of a sphere or a capsule (whose centers are in m_Points)
	Vector m_vMins;
	Vector m_vMaxs;
	float m_flRadius;

	// Also the list the trace is kept in
	RecordedTraceType_t m_nType;

	RayType_t m_nRayType;

	// RECORDED_TRACE_* flags
	uint32 m_nFlags;

	// The flMaxDistance argument of CastRayIntervals
	float m_flMaxDistance;

	// The start transform of the sweeps and the transform of LinearCast and CollideShape (m_vStart and m_qStartOrientation),
	// and the end transform of the sweeps
	VectorAligned m_vStart;
	Quaternion m_qStartOrientation;
	CTransform m_EndTransform;

	Vector m_vDelta;
	RnQueryAttr_t m_QueryAttr;

	// Index of the trace in its list, and the world that recorded it
	int m_nIndex;
	CRnWorld *m_pWorld;

	CUtlLeanVectorFixedGrowable< RecordedTraceResult_t, 1 > m_Results;

	// The ray points: the center of a line or a sphere, the two centers of a capsule
	CUtlLeanVectorFixedGrowable< Vector, 2 > m_Points;

	// Up to 12 keys of debug strings, zero terminated, that the debug draw filter matches against its name; release builds leave
	// them zero
	uint64 m_nDebugStringKeys[ 12 ];
};

// The zeroed 40-byte struct IFnPhysicsWorld::unk025 returns; only its size and 8-byte alignment are known (fields at +16, +24 and +32)
struct FnPhysicsWorldUnk025_t
{
	ALIGN8 uint8 m_Data[ 40 ] ALIGN8_POST;
};

// Flags the physics debugger sets with its messages (CVPhysics2ConComm::m_nDebuggerFlags), which every world gets a copy of while
// streaming. The debugger messages also set bit 1, bit 4, bit 8 and bit 0x20, which nothing in the library reads apart from
// IFnPhysicsWorld::IsDebuggerFlag4Set and IsDebuggerFlag8Set
#define PHYSICS_DEBUGGER_FLAG_RECORD_TRACES ( 1 << 1 ) // IFnPhysicsWorld::IsRecordingTraces
#define PHYSICS_DEBUGGER_FLAG_KEEP_AWAKE_BODY_LIST ( 1 << 4 ) // IFnPhysicsWorld::ClearAwakeBodies keeps the awake body list

// Function table over a world, placed into caller memory by IVPhysics2::ConstructFnPhysicsWorld
abstract_class IFnPhysicsWorld
{
public:
	virtual void SetWorld( IVPhysics2World *pWorld ) = 0;
	virtual void SetRnWorld( CRnWorld *pRnWorld ) = 0;
	virtual IVPhysics2World *GetWorld() = 0;

	// The solver and the report contact lists of the world
	virtual CRnContact *GetSolverContact( int nIndex ) = 0;
	virtual CRnContact *GetReportContact( int nIndex ) = 0;

	// Calls the callback for every trace recorded while IsRecordingTraces is set; the traces are kept in one list per RecordedTraceType_t
	virtual void ForEachRecordedTrace( const std::function< void( const RecordedTrace_t & ) > &callback ) = 0;

	// Appends a copy of the trace to its list and returns its index, -1 while traces are not recorded
	virtual int RecordTrace( const RecordedTrace_t &trace ) = 0;

	virtual void ReRunRecordedTrace( const RecordedTrace_t &trace, CUtlLeanVectorFixedGrowable< RecordedTraceResult_t, 1 > *pResults ) = 0;

	// A recorded trace of the list of a type, null when out of range
	virtual RecordedTrace_t *GetRecordedTrace( int nIndex, RecordedTraceType_t nList ) = 0;

	// The records of the frame recorded for the physics debugger: a byte buffer and an offset per record, cleared with the recording
	virtual void *GetDebugRecord( int nIndex ) = 0;
	virtual int GetDebugRecordCount() = 0;
	virtual int GetDebugRecordBufferSize() = 0;

	// PHYSICS_DEBUGGER_FLAG_RECORD_TRACES; the next two test the debugger flags 4 and 8, which nothing in the library reads
	virtual bool IsRecordingTraces() = 0;
	virtual bool IsDebuggerFlag4Set() = 0;
	virtual bool IsDebuggerFlag8Set() = 0;

	// PHYSICS_DEBUGGER_FLAG_KEEP_AWAKE_BODY_LIST
	virtual bool IsAwakeBodyListKept() = 0;

	// The PHYSICS_DEBUGGER_FLAG_* flags the physics debugger sets, unless the world overrides them
	virtual int GetDebuggerFlags() = 0;
	virtual void UpdateDebugScene() = 0;

	// Empty
	virtual void unk018() = 0;
	virtual void unk019() = 0;

	// Broadphase proxies
	virtual IPhysicsShape *GetProxyShape( uint32 nProxy ) = 0;
	virtual AABB_t GetProxyBounds( uint32 nProxy ) = 0;

	virtual float unk022( const void *pUnknown1, const void *pUnknown2, const void *pUnknown3, void *pUnknown4 ) = 0;
	// Whether the solver or the report contact list holds the contact at its index
	virtual bool IsContactInWorld( CRnContact *pContact ) = 0;
	virtual void DestroyContact( CRnContact *pContact ) = 0;

	// Returns zeros
	virtual FnPhysicsWorldUnk025_t unk025() = 0;

	// Clears IPhysicsSoftbody::SetManualStep of every softbody, updates the debug scene, cleans up the delete list and clears the CRnBody pointer that
	// IPhysicsBody::SetUserPointer sets on every body
	virtual void ClearDebugState() = 0;

	// False without an active debug draw filter; with an id list, whether the id is in it
	virtual bool PassesDebugDrawFilter( uint32 nId ) = 0;
	// Empties the awake body list, unless PHYSICS_DEBUGGER_FLAG_KEEP_AWAKE_BODY_LIST is set
	virtual void ClearAwakeBodies() = 0;
	virtual void unk029( void *pObject, void *pUnknown ) = 0;
	// Reads a query cache from the buffer
	virtual CRnQueryCache *CreateQueryCache( CUtlBuffer &buffer ) = 0;

	// Empty
	virtual void unk031() = 0;

	// Forwards to the same function of the world
	virtual void FlushDebugRecording( bool bStateChanged ) = 0;

	// Returns 0
	virtual int unk033() = 0;

	virtual int unk034( void *pObject ) = 0;

	// Returns 0
	virtual int unk035() = 0;

	// Calls the callback for every node of the broadphase trees with its bounds and depth, and the shape of a leaf (NULL for
	// the inner nodes)
	virtual void ForEachBroadphaseNode( const std::function< void( const AABB_t &bounds, int nDepth, IPhysicsShape *pShape ) > &callback ) = 0;
	virtual void Profile( PhysicsWorldTraceProfile_t &profile ) = 0;
};

struct EmbeddedPhysicsInfo_t
{
	ResourceHandle_t m_hRootResource;
	IRD_RegisterResourceDataUtils *m_pResourceRegisterUtils;
	const char *m_pModelName;
	CEmbeddedIntrospectedOrKV3BlockHelper< VPhysXAggregateData_t > *m_pBlockHandler;
};

#endif // VPHYSICS2_INTERFACE_H
