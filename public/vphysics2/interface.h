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
#include "mathlib/vector.h"
#include "mathlib/aabb.h"
#include "vphysics2/handle_decl.h"
#include "vphysx/vphysx_interface.h"
#include "resourcefile/resourcetype.h"

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
class Bounds3;
class IMotionEvent;
class IVPhysics2World;
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
struct matrix3x4_t;
struct Ray_t;
struct RnQueryAttr_t;
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

// The 28-byte result of IVPhysics2World::LinearCast, SweepBodySingle and SweepShapeSingle (fraction 1.0 and ids -1 when nothing is hit); only its size is known
struct VPhysics2WorldUnk173_t
{
	uint32 m_Data[ 7 ];
};

// The zeroed 40-byte struct IVPhysics2World::unk212, unk213 and unk214 return; only its size and 8-byte alignment are known (fields at +16, +24 and +32)
struct VPhysics2WorldUnk212_t
{
	ALIGN8 uint8 m_Data[ 40 ] ALIGN8_POST;
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

	// Empty in release builds
	virtual void unk009() = 0;
	virtual void unk010() = 0;

	virtual void BeginAsyncPostTickJobs( void *pUnused, bool bUnknown ) = 0;
	virtual void EndAsyncPostTickJobs() = 0;
	virtual void PostRopeJob( const void *pJob, bool bUnknown ) = 0;
	virtual void UpdateParticleRopes() = 0;
	virtual bool IsInAsyncPostTickJobs() = 0;
	virtual void Collide( float flTimeStep ) = 0;
	virtual void unk017() = 0;

	virtual void SetGravity( const Vector &vGravity ) = 0;
	virtual const VectorAligned &GetGravity() = 0;

	virtual void unk020( float flValue ) = 0;
	virtual float unk021() = 0;
	virtual void unk022( int nValue ) = 0;
	virtual int unk023() = 0;
	virtual void SetPositionIterations( int nIterations ) = 0;
	virtual int GetPositionIterations() = 0;
	virtual void SetVelocityIterations( int nIterations ) = 0;
	virtual int GetVelocityIterations() = 0;

	// Both set the same value, which is clamped to [0, 4]
	virtual void ResetTOIIterations() = 0;
	virtual void SetTOIIterations( int nIterations ) = 0;
	virtual int GetTOIIterations() = 0;

	virtual void ScaleImpulses( float flScale ) = 0;
	virtual bool unk032() = 0;
	virtual void unk033( bool bValue ) = 0;

	// Multithreading of the world step; the server applies phys_multithreading_enabled here
	virtual bool IsMultithreadingEnabled() = 0;
	virtual void SetMultithreadingEnabled( bool bEnabled ) = 0;
	virtual int GetThreadCount() = 0;
	virtual void SetThreadCount( int nThreadCount ) = 0;
	virtual void unk038( uint8 nValue ) = 0;

	virtual float GetSimulationTime() = 0;
	virtual float GetStepStartTime() = 0;
	virtual float unk041() = 0;
	virtual int GetStepCount() = 0;

	virtual void unk043( int nValue ) = 0;
	virtual int unk044() = 0;
	virtual void unk045( int nValue ) = 0;
	virtual int unk046() = 0;
	virtual void unk047( int nValue ) = 0;
	virtual int unk048() = 0;

	virtual void unk049( void *pCallback ) = 0;
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

	virtual float unk070() = 0;
	virtual void unk071( float flValue ) = 0;
	virtual void unk072( void *pValue ) = 0;
	virtual void *unk073() = 0;

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

	// Creates the joints of a set of joint descriptions and appends them to joints
	virtual void AddJoints( const void *pJoints, const void *pUnknown1, const void *pUnknown2, const void *pUnknown3, bool bUnknown, CUtlVector< IPhysicsJoint * > &joints, CUtlVector< int > &parts ) = 0;

	virtual void RemoveJoint( IPhysicsJoint *pJoint ) = 0;

	// Constraints created from the legacy constraint parameters
	virtual IPhysicsJoint *CreateBallsocketConstraint( IPhysicsBody *pBody1, IPhysicsBody *pBody2, const constraint_ballsocketparams_t &ballsocket ) = 0;
	virtual IPhysicsJoint *CreateRagdollConstraint( IPhysicsBody *pBody1, IPhysicsBody *pBody2, const matrix3x4_t &constraintToReference, const constraint_ragdollparams_t &ragdoll ) = 0;
	virtual IPhysicsJoint *CreatePulleyConstraint( IPhysicsBody *pBody1, IPhysicsBody *pBody2, const constraint_pulleyparams_t &pulley ) = 0;
	virtual IPhysicsJoint *CreateDistanceConstraint( IPhysicsBody *pBody1, IPhysicsBody *pBody2, const constraint_lengthparams_t &length ) = 0;
	virtual IPhysicsJoint *CreatePrismaticConstraint( IPhysicsBody *pBody1, IPhysicsBody *pBody2, const constraint_slidingparams_t &sliding, bool bUseBlockSolver ) = 0;
	virtual IPhysicsJoint *CreateFixedConstraint( IPhysicsBody *pBody1, IPhysicsBody *pBody2, const constraint_fixedparams_t &fixed, bool bUseBlockSolver ) = 0;

	virtual void GetConnectedBodies( IPhysicsBody *pBody, CUtlVector< IPhysicsBody * > &bodies ) = 0;
	virtual int GetConnectedBodyCount( IPhysicsBody *pBody ) = 0;

	virtual IPhysicsRope *CreateRope() = 0;

	// Clears the pointer on success
	virtual bool DestroyRope( IPhysicsRope **ppRope ) = 0;

	virtual IPhysicsParticleRope *CreateParticleRope( void *pUnknown1, const void *pUnknown2, void *pUnknown3, float flUnknown1, float flUnknown2, float flUnknown3 ) = 0;
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

	virtual int unk116() = 0;
	virtual void unk117( int nIndex, void *pOut ) = 0;
	virtual int unk118() = 0;
	virtual void unk119( int nIndex, void *pOut ) = 0;
	virtual void DestroyContacts( IPhysicsBody *pBody1, IPhysicsBody *pBody2 ) = 0;
	virtual bool HasContact( IPhysicsBody *pBody1, IPhysicsBody *pBody2 ) = 0;
	virtual void unk122( void *pUnknown, bool bUnknown ) = 0;
	virtual void unk123() = 0;
	virtual void unk124( void *pOut ) = 0;

	virtual IPhysicsMotionController *CreateMotionController( IMotionEvent *pHandler ) = 0;
	virtual void DestroyMotionController( IPhysicsMotionController *pController ) = 0;
	virtual int GetMotionControllerCount() = 0;

	virtual int GetAggregateCount() = 0;
	virtual void GetActiveAggregateInstances( CUtlVector< IPhysAggregateInstance * > &aggregates ) = 0;
	virtual IPhysAggregateInstance *GetAggregate( int nIndex ) = 0;
	virtual IPhysAggregateInstance *CreateAggregateInstance( const CPhysAggregateData *pData, const CTransform &transform, uint32 nFlags, int nMotionType ) = 0;
	virtual void DestroyAggregateInstance( IPhysAggregateInstance *pAggregate ) = 0;

	virtual IPhysicsRagdoll *CreateRagdoll( void *pUnknown1, const void *pUnknown2, int nUnknown1, void *pUnknown3, void *pUnknown4, float flUnknown, int nUnknown2, uint16 nUnknown3 ) = 0;
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
	virtual void unk143( const void *pCallbacks, void *pContext ) = 0;

	// Collision filtering of entity pairs
	virtual bool DisableCollisionsOnEntityPair( uint32 nEntity1, uint32 nEntity2 ) = 0;
	virtual void EnableCollisionsOnEntityPair( uint32 nEntity1, uint32 nEntity2 ) = 0;
	virtual bool IsEntityPairDisabled( uint32 nEntity1, uint32 nEntity2 ) = 0;
	virtual void RemoveAllPairsForEntityId( uint32 nEntity ) = 0;
	virtual bool DoesEntityHaveCollisionPairs( uint32 nEntity ) = 0;
	virtual void GetDisabledEntityPairs( void *pOut ) = 0;

	virtual bool ShouldHit( const IPhysicsShape *pShape, const RnQueryAttr_t &attr ) = 0;
	virtual uint64 unk151( uint32 nValue ) = 0;

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
	virtual void ToolCastRayMultipleUnfiltered( CUtlVectorFixedGrowable< RnToolQueryHit_t, 8 > &hits, const Vector &vStart, const Vector &vEnd, float flUnknown ) = 0;
	virtual void CastRayIntervals( CRayIntervalVector &intervals, const Vector &vStart, const Vector &vEnd, const RnQueryAttr_t &attr, bool bUnknown, float flUnknown ) = 0;
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
	virtual VPhysics2WorldUnk173_t LinearCast( const void *pUnknown1, const void *pUnknown2, const void *pUnknown3, const void *pUnknown4, bool bUnknown ) = 0;
	virtual VPhysics2WorldUnk173_t SweepBodySingle( const void *pUnknown1, const void *pUnknown2, IPhysicsBody *pBody, const void *pUnknown3, bool bUnknown ) = 0;
	virtual void SweepBodyMultiple( void *pOut, const void *pUnknown1, const void *pUnknown2, IPhysicsBody *pBody, const void *pUnknown3, bool bUnknown ) = 0;
	virtual VPhysics2WorldUnk173_t SweepShapeSingle( const void *pUnknown1, const void *pUnknown2, IPhysicsShape *pShape, const void *pUnknown3, bool bUnknown ) = 0;
	virtual void SweepShapeMultiple( void *pOut, const void *pUnknown1, const void *pUnknown2, IPhysicsShape *pShape, const void *pUnknown3, bool bUnknown ) = 0;
	virtual VPhysics2WorldUnk173_t SweepShapeSingle( const void *pUnknown1, const void *pUnknown2, const Ray_t &ray, const void *pUnknown3, bool bUnknown ) = 0;
	virtual void SweepShapeMultiple( void *pOut, const void *pUnknown1, const void *pUnknown2, const Ray_t &ray, const void *pUnknown3, bool bUnknown ) = 0;

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

	virtual bool unk188( void *pUnknown1, bool bUnknown, void *pUnknown2 ) = 0;

	virtual CRnQueryCache *CreateQueryCache( void *pUnknown1, void *pUnknown2 ) = 0;
	virtual void DestroyQueryCache( CRnQueryCache *pCache ) = 0;

	// Closest point of the shapes that pass the filter to a point, within a maximum distance; returns the distance.
	// MSVC groups these overloads in reverse order; the declaration order is Linux's
	virtual float ClosestPoint( const Vector &vPoint, float flMaxDistance, const std::function< bool( IPhysicsShape * ) > &filter, PhysicsClosestPoint_t *pClosest ) = 0;
	virtual float ClosestPoint( const Vector &vPoint, float flMaxDistance, const std::function< bool( IPhysicsShape * ) > &filter, Vector *pClosest ) = 0;

	virtual void CollideShape( void *pUnknown1, void *pUnknown2, void *pUnknown3, void *pUnknown4, bool bUnknown, float flUnknown ) = 0;

	virtual void unk194( void *pValue ) = 0;
	virtual void *unk195() = 0;
	virtual int unk196( uint32 nValue ) = 0;

	// The flags IVPhysics2::CreateWorld was called with
	virtual uint32 GetCreateFlags() = 0;
	virtual void SetWorldReferenceBody( IPhysicsBody *pBody ) = 0;
	virtual IPhysicsBody *GetWorldReferenceBody() = 0;

	// Save and restore of object references. MSVC groups these overloads in reverse order; the declaration order is Linux's
	virtual bool unk200( const void *pObject ) = 0;
	virtual bool unk200( const CUtlVector< void * > &objects ) = 0;
	virtual bool RestoreObject( const void *pObject ) = 0;
	virtual void unk203() = 0;

	// Returns 0
	virtual int unk204() = 0;

	// Empty in release builds; the server passes the name it creates the world with
	virtual void SetDebugName( const char *pszName ) = 0;
	virtual void unk206() = 0;

	virtual void *unk207() = 0;

	// Both return 0
	virtual int unk208() = 0;
	virtual int unk209() = 0;

	virtual void DisableDebugRecording( bool bDisable ) = 0;
	virtual void unk211( void *p ) = 0;

	// Each of these returns zeros
	virtual VPhysics2WorldUnk212_t unk212() = 0;
	virtual VPhysics2WorldUnk212_t unk213() = 0;
	virtual VPhysics2WorldUnk212_t unk214() = 0;

	virtual IPhysReflContext *GetReflContext() = 0;
	virtual void unk216( bool bUnknown ) = 0;
	virtual void UpdateDebugScene() = 0;

	// Empty
	virtual void unk218() = 0;

	virtual void unk219( void *pUnknown1, void *pUnknown2, void *pUnknown3, void *pUnknown4, void *pUnknown5 ) = 0;
	virtual void unk220( bool bUnknown ) = 0;
	virtual void unk221() = 0;

	// Debug draw of the world through the given overlays, also used by IVPhysics2::DebugDrawWorlds
	virtual void Draw( ISceneViewDebugOverlays *pDebugOverlays, const CFrustum *pFrustum ) = 0;
	virtual void UpdateSceneObjects( CCallQueue &callQueue ) = 0;

	virtual void *unk224() = 0;

	// The debug draw options used by Draw and UpdateSceneObjects
	virtual uint64 GetDebugDrawOptions() = 0;
	virtual void SetDebugDrawOptions( uint64 nOptions ) = 0;

	virtual void unk227( void *pValue ) = 0;
	virtual void *unk228() = 0;

	// IVPhysics2::SetVisualDebugCameraHint copies the frustum into every world
	virtual void SetVisualDebugCameraHint( int nUnknown, const CFrustum &frustum ) = 0;
	virtual const CFrustum &GetVisualDebugCameraHint() = 0;

	// Empty in release builds
	virtual void unk231( void *pUnknown1, void *pUnknown2 ) = 0;

	virtual IPhysicsBody *unk232( void *pUnknown, void *pOut ) = 0;

	// Debug draw filtering; both notify the event listener
	virtual void SetDebugDrawFilter( const void *pParams ) = 0;
	virtual void ClearDebugDrawFilter() = 0;
	virtual bool ShouldDrawCollisionAttributes( void *pUnknown ) = 0;
	virtual bool ShouldDrawShape( IPhysicsShape *pShape ) = 0;
	virtual bool ShouldDrawBody( IPhysicsBody *pBody, bool bUnknown ) = 0;
	virtual bool ShouldDrawSoftbody( IPhysicsSoftbody *pSoftbody ) = 0;
	virtual bool ShouldDrawJoint( IPhysicsJoint *pJoint ) = 0;
	virtual void unk240( void *pUnknown1, void *pUnknown2 ) = 0;
	virtual void unk241() = 0;

	// Returns NULL in release builds
	virtual const char *GetDebugName() = 0;

	// Describes the physics library build, for example "Rubikon Release No-Names non-SIMD"
	virtual const char *GetBuildString() = 0;

	// Handles the physics world console command: snapshots, saving and dumping of the world
	virtual void DumpState( const CCommand &args, float flUnknown ) = 0;

	// Dumps the bodies, shapes and joints of the world to a file
	virtual void DumpToJSON( const char *pszFileName, void *pUnknown ) = 0;

	virtual void GetBenchmarkPath( CBufferString &path ) = 0;

	virtual int unk247() = 0;
	virtual void *unk248() = 0;

	virtual void DescribeShapeCollision( CBufferString &text, IPhysicsShape *pShape1, IPhysicsShape *pShape2 ) = 0;
	virtual void DescribeShape( CBufferString &text, IPhysicsShape *pShape, void *pUnknown ) = 0;
};


class IPhysReflContext
{
public:
	virtual bool unk000() = 0;
	virtual void *unk001() = 0;
};

// The debug draw options of a world, passed by value to the debug draw functions
struct RnDebugDrawOptions_t
{
	uint64 m_nUnk000;
	uint64 m_nUnk008;
};

class CVPhys2World : public IVPhysics2World, public IPhysReflContext
{
public:
	// Itanium also lists these in the primary vtable
	void *unk001() override;
	bool unk000() override;

public:
	uint32 m_nCreateFlags;

	// Taken from a counter that every new world increments
	int m_nWorldId;

	RnDebugDrawOptions_t m_DebugDrawOptions;
	void *m_pUnk028;
	CRnWorld *m_pRnWorld;

	// Set when a command line parameter is present
	bool m_bUnk038;

	bool m_bDebugRecordingDisabled;

	// Two hash tables, one of them is filled from the body map of StepEndReadOnlyGetResults
	ALIGN8 uint8 m_unk040[88] ALIGN8_POST;

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

// The zeroed 40-byte struct IFnPhysicsWorld::unk025 returns; only its size and 8-byte alignment are known (fields at +16, +24 and +32)
struct FnPhysicsWorldUnk025_t
{
	ALIGN8 uint8 m_Data[ 40 ] ALIGN8_POST;
};

// Function table over a world, placed into caller memory by IVPhysics2::ConstructFnPhysicsWorld
abstract_class IFnPhysicsWorld
{
public:
	virtual void SetWorld( IVPhysics2World *pWorld ) = 0;
	virtual void SetRnWorld( CRnWorld *pRnWorld ) = 0;
	virtual IVPhysics2World *GetWorld() = 0;

	virtual void *unk003( int nIndex ) = 0;
	virtual void *unk004( int nIndex ) = 0;
	virtual void unk005( const void *pCallback ) = 0;
	virtual void unk006( void *pUnknown, float flUnknown1, float flUnknown2, float flUnknown3 ) = 0;
	virtual void unk007( const void *pUnknown1, void *pUnknown2 ) = 0;
	virtual void *unk008( int nIndex, uint32 nList ) = 0;
	virtual void *unk009( int nIndex ) = 0;
	virtual int unk010() = 0;
	virtual int unk011() = 0;
	virtual bool IsRecordingTraces() = 0;
	virtual bool unk013() = 0;
	virtual bool unk014() = 0;
	virtual bool unk015() = 0;
	virtual int unk016() = 0;
	virtual void UpdateDebugScene() = 0;

	// Empty
	virtual void unk018() = 0;
	virtual void unk019() = 0;

	// Broadphase proxies
	virtual IPhysicsShape *GetProxyShape( uint32 nProxy ) = 0;
	virtual AABB_t GetProxyBounds( uint32 nProxy ) = 0;

	virtual float unk022( const void *pUnknown1, const void *pUnknown2, const void *pUnknown3, void *pUnknown4 ) = 0;
	virtual bool unk023( void *pObject ) = 0;
	virtual void DestroyContact( void *pContact ) = 0;

	// Returns zeros
	virtual FnPhysicsWorldUnk025_t unk025() = 0;

	virtual void unk026() = 0;
	virtual bool unk027( uint32 nValue ) = 0;
	virtual void unk028() = 0;
	virtual void unk029( void *pObject, void *pUnknown ) = 0;
	virtual CRnQueryCache *CreateQueryCache( void *pUnknown ) = 0;

	// Empty
	virtual void unk031() = 0;

	// Forwards to the same function of the world
	virtual void unk032( bool bUnknown ) = 0;

	// Returns 0
	virtual int unk033() = 0;

	virtual int unk034( void *pObject ) = 0;

	// Returns 0
	virtual int unk035() = 0;

	virtual void unk036( void *pUnknown ) = 0;
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
