//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: Rigid body of a physics world and its function table interface
//
//===========================================================================//

#ifndef IPHYSICSBODY_H
#define IPHYSICSBODY_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "tier0/utlleanvector.h"
#include "tier0/utlsortvector.h"
#include "tier0/utlstring.h"
#include "tier0/utlstringtoken.h"
#include "tier0/utlvector.h"
#include "mathlib/vector.h"
#include "mathlib/vector4d.h"
#include "mathlib/mathlib.h"
#include "mathlib/aabb.h"
#include "mathlib/transform.h"
#include "mathlib/matrix3x3.h"
#include "physicslib/param_types.h"
#include "physicslib/serialize.h"
#include "vphysx/vphysx_collisionmesh_resource.h"
#include "vphysics2/iphysicsshape.h"
#include "schemasystem/schematypes.h"

class IVPhysics2World;
class IPhysicsShape;
class IPhysicsJoint;
class IPhysicsShadowController;
class IPhysicsFrictionSnapshot;
class CRnBody;
class CSchemaClassInfo;
class ISceneViewDebugOverlays;

struct RnCollisionAttr_t;
struct RnMassProperties_t;

//-----------------------------------------------------------------------------
// Shapes of a body description, filled from the shape descriptions of a body part.
// The type names are not recovered; the layouts are
//-----------------------------------------------------------------------------
struct PhysicsShapeDef_t
{
	PhysicsShapeType_t m_nType;

	// Taken from VPhysXAggregateData_t::m_surfacePropertyHashes by the surface property index
	uint32 m_nSurfacePropertyHash;

	// A copy of the aggregate data record the collision attribute index selects
	VPhysXCollisionAttributes_t m_CollisionAttributes;
};

struct PhysicsSphereDef_t : public PhysicsShapeDef_t
{
	RnSphere_t m_Sphere;
};

struct PhysicsCapsuleDef_t : public PhysicsShapeDef_t
{
	RnCapsule_t m_Capsule;
};

// The geometry of hulls, meshes and compounds is a heap copy of the aggregate data geometry,
// which the destructor of the description does not release
struct PhysicsHullDef_t : public PhysicsShapeDef_t
{
	RnHull_t *m_pHull;
};

struct PhysicsMeshDef_t : public PhysicsShapeDef_t
{
	RnMesh_t *m_pMesh;
};

struct PhysicsCompoundDef_t : public PhysicsShapeDef_t
{
	RnCompound_t *m_pCompound;
};

// Body description built from a body part of physics aggregate data
struct PhysicsBodyDef_t
{
	CUtlString m_DebugName;
	float m_flMass;
	Vector m_vLocalMassCenter;
	matrix3x4_t m_LocalInertia;
	Vector m_vPosition;
	float m_qOrientation[ 4 ]; // x, y, z, w (not 16-byte aligned)
	Vector m_vLinearVelocity;
	float m_flLinearDamping;
	float m_flLinearDrag;
	Vector m_vAngularVelocity;
	float m_flAngularDamping;
	float m_flAngularDrag;

	// The 4 bytes before the shapes are alignment padding: the default construction, the conversion and the copy assignment never touch them
	CUtlLeanVector< PhysicsSphereDef_t > m_SphereShapes;
	CUtlLeanVector< PhysicsCapsuleDef_t > m_CapsuleShapes;
	CUtlLeanVector< PhysicsHullDef_t > m_HullShapes;
	CUtlLeanVector< PhysicsMeshDef_t > m_MeshShapes;
	CUtlLeanVector< PhysicsCompoundDef_t > m_CompoundShapes;
};

// Methods with an unknown name that MSVC lays out as one overload group keep one shared unkNNN name
abstract_class IPhysicsBody
{
public:
	virtual SchemaMetaInfoHandle_t< CSchemaClassInfo > Schema_DynamicBinding() = 0;

	virtual IVPhysics2World *GetWorld() = 0;
	virtual const IVPhysics2World *GetWorld() const = 0;

	virtual void SetType( PhysicsBodyType_t nType ) = 0;
	virtual PhysicsBodyType_t GetType() const = 0;

	// Disabled bodies are taken out of the broadphase
	virtual void SetEnabled( bool bEnabled ) = 0;
	virtual bool IsEnabled() const = 0;

	virtual int GetShapeCount() const = 0;
	virtual IPhysicsShape *GetShape( int nIndex ) = 0;
	virtual const IPhysicsShape *GetShape( int nIndex ) const = 0;

	// Index of the shape in this body, -1 when it belongs to another body
	virtual int GetShapeIndex( const IPhysicsShape *pShape ) const = 0;

	// bUpdateMass rebuilds the mass properties from the shapes; pMaterials is a count followed by a pointer to RnMaterial_t records
	virtual IPhysicsShape *AddSphereShape( const RnSphere_t &sphere, float flScale, bool bUpdateMass ) = 0;
	virtual IPhysicsShape *AddCapsuleShape( const RnCapsule_t &capsule, float flScale, bool bUpdateMass ) = 0;
	virtual IPhysicsShape *AddHullShape( const RnHull_t *pHull, float flScale, bool bUpdateMass ) = 0;
	virtual IPhysicsShape *AddMeshShape( const RnMesh_t *pMesh, const Vector &vScale, bool bUpdateMass, const void *pMaterials ) = 0;
	virtual IPhysicsShape *AddCompoundShape( const RnCompound_t *pCompound, float flScale, bool bUpdateMass, const void *pMaterials ) = 0;

	// Adds a copy of a shape of another body
	virtual IPhysicsShape *AddShape( const IPhysicsShape *pShape ) = 0;

	virtual void RemoveShape( IPhysicsShape *pShape ) = 0;
	virtual void PurgeShapes() = 0;

	// Sets or clears FCOLLISION_FUNC_ENABLE_TOUCH_EVENT on every shape
	virtual void EnableTouchEvents( bool bEnable ) = 0;
	virtual void SetCollisionAttributes( const RnCollisionAttr_t &attributes ) = 0;
	virtual void SetMaterialIndex( CUtlStringToken materialIndex ) = 0;

	// Sum of the surface areas of the shapes
	virtual float GetSurfaceArea() const = 0;

	// Farthest point of the shapes along a direction, at the current or at the given placement
	virtual Vector GetFarthestPoint( const Vector &vDirection ) const = 0;
	virtual Vector GetFarthestPoint( const CTransform &transform, const Vector &vDirection ) const = 0;

	// Overlap tests of the shapes; the first tests a point with a tolerance, the others pass the query to every shape
	virtual bool TestOverlap( const Vector &vPoint, float flTolerance ) const = 0;
	virtual bool TestOverlap( const void *pQuery, const CTransform &transform ) const = 0;
	virtual bool TestOverlap( const void *pQuery ) const = 0;

	virtual Vector GetClosestPoint( const Vector &vPoint ) const = 0;

	// Closest point of the shapes to a segment
	virtual Vector GetClosestPointToSegment( const Vector &vStart, const Vector &vEnd, void *pUnused ) const = 0;

	// Bounds of the shapes in body space
	virtual AABB_t BuildLocalBounds() const = 0;
	virtual AABB_t BuildBounds() const = 0;

	// bRecomputeMassProperties rebuilds the mass center and the inertia from the shapes
	virtual void SetMass( float flMass, bool bRecomputeMassProperties ) = 0;
	virtual void SetLocalMassCenter( const Vector &vMassCenter ) = 0;
	virtual void SetMassProperties( const RnMassProperties_t &massProperties, bool bUnused ) = 0;

	// Set while the mass, the mass center and the inertia are overridden instead of built from the shapes
	virtual bool GetOverrideMassProperties() const = 0;
	virtual bool GetOverrideMassCenter() const = 0;

	virtual float GetMass() const = 0;
	virtual float GetInvMass() const = 0;

	// Multiply and reset the mass scale
	virtual void ScaleMass( float flScale ) = 0;
	virtual void ResetMassScale() = 0;

	virtual Vector GetMassCenter() const = 0;
	virtual Vector GetLocalMassCenter() const = 0;
	virtual RnMassProperties_t GetMassProperties() const = 0;
	virtual bool GetOverrideInertia() const = 0;

	// Body space inertia tensor
	virtual matrix3x4_t GetLocalInertia() const = 0;

	// PhysicsInertiaBox_t::m_vPrincipalInertia of the inertia box of the body
	virtual Vector GetPrincipalInertia() const = 0;

	virtual void ScaleInertia( float flScale ) = 0;

	// Resets the inertia scale
	virtual void ResetInertiaScale() = 0;

	virtual void SetPosition( const Vector &vPosition ) = 0;
	virtual Vector GetPosition() const = 0;
	virtual void SetOrientation( const Quaternion &orientation ) = 0;
	virtual Quaternion GetOrientation() const = 0;
	virtual void SetTransform( const CTransform &transform ) = 0;
	virtual CTransform GetTransform() const = 0;

	// Set and get the transform with the body scale in the w component of the position
	virtual void SetTransformAndScale( const CTransform &transform ) = 0;
	virtual CTransform GetTransformAndScale() const = 0;

	// Target transform of the shadow controller while one drives the body, the transform otherwise
	virtual CTransform GetTargetTransform() const = 0;

	virtual Vector LocalToWorldVector( const Vector &vLocal ) const = 0;
	virtual Vector WorldToLocalVector( const Vector &vWorld ) const = 0;
	virtual Vector LocalToWorld( const Vector &vLocal ) const = 0;
	virtual Vector WorldToLocal( const Vector &vWorld ) const = 0;

	// Rotate an orientation into body space and back
	virtual Quaternion WorldToLocalRotation( const Quaternion &world ) const = 0;
	virtual Quaternion LocalToWorldRotation( const Quaternion &local ) const = 0;

	virtual void SetLinearVelocity( const Vector &vVelocity ) = 0;
	virtual void AddLinearVelocity( const Vector &vVelocity ) = 0;
	virtual Vector GetLinearVelocity() const = 0;
	virtual Vector GetVelocityAtPoint( const Vector &vPoint ) const = 0;

	// Angular velocities are in degrees per second
	virtual void SetAngularVelocity( const Vector &vAngularVelocity ) = 0;
	virtual void AddAngularVelocity( const Vector &vAngularVelocity ) = 0;
	virtual Vector GetAngularVelocity() const = 0;

	// Set, add and get the angular velocity in body space
	virtual void SetLocalAngularVelocity( const Vector &vAngularVelocity ) = 0;
	virtual void AddLocalAngularVelocity( const Vector &vAngularVelocity ) = 0;
	virtual Vector GetLocalAngularVelocity() const = 0;

	virtual void ApplyLinearImpulse( const Vector &vImpulse ) = 0;
	virtual void ApplyLinearImpulseAtWorldSpace( const Vector &vImpulse, const Vector &vPosition ) = 0;
	virtual void ApplyAngularImpulse( const Vector &vImpulse ) = 0;

	// Drives a dynamic body towards a transform over the given time; the body wakes when the resulting linear or angular
	// velocity exceeds the threshold
	virtual void MoveTo( const CTransform &target, float flTime, float flLinearWakeThreshold, float flAngularWakeThreshold ) = 0;

	virtual void CalculateVelocityOffset( const Vector &vForce, const Vector &vPosition, Vector *pCenterVelocity, Vector *pCenterAngularVelocity ) const = 0;

	// Applies a linear acceleration (scaled by the mass) as a force
	virtual void ApplyLinearAcceleration( const Vector &vAcceleration, bool bWake ) = 0;

	virtual void ApplyForce( const Vector &vForce, bool bWake ) = 0;
	virtual void ApplyForceAt( const Vector &vForce, const Vector &vPosition, bool bWake ) = 0;

	// Applies an angular acceleration (scaled by the world inertia) as a torque
	virtual void ApplyAngularAcceleration( const Vector &vAcceleration, bool bWake ) = 0;

	virtual void ApplyTorque( const Vector &vTorque, bool bWake ) = 0;
	virtual void ClearForces() = 0;
	virtual void ClearTorque() = 0;

	virtual void EnableDrag( bool bEnable ) = 0;
	virtual bool IsDragEnabled() const = 0;

	// Linear and angular drag coefficients used by the drag calculations below; setting one enables drag
	virtual void SetLinearDrag( float flDrag ) = 0;
	virtual float GetLinearDrag() const = 0;
	virtual void SetAngularDrag( float flDrag ) = 0;
	virtual float GetAngularDrag() const = 0;

	virtual float CalculateAngularDrag( const Vector &vAxis ) const = 0;
	virtual float CalculateLinearDrag( const Vector &vDirection ) const = 0;

	virtual void SetLinearDamping( float flDamping ) = 0;
	virtual float GetLinearDamping() const = 0;
	virtual void SetAngularDamping( float flDamping ) = 0;
	virtual float GetAngularDamping() const = 0;

	// Scales the friction of the materials of the body in its contacts
	virtual void SetFrictionScale( float flScale ) = 0;
	virtual float GetFrictionScale() const = 0;

	virtual void SetLinearFluidDragScale( float flScale ) = 0;
	virtual void SetAngularFluidDragScale( float flScale ) = 0;

	virtual void SetBuoyancyScale( float flScale ) = 0;
	virtual float GetBuoyancyScale() const = 0;

	// Applies the buoyancy and the drag of a fluid to the body and returns the submerged volume. pFluid holds the collision mask of
	// the fluid shapes at +8, the fluid type token, the density, the neutrally buoyant gravity, linear and angular damping and flag
	// at +16 to +36, the fluid body at +88, a surface plane flag, normal and distance at +96 to +112 and a flow callback at +120.
	// The Linux implementation does not read the floats
	virtual float ApplyFluidForces( const void *pFluid, float flUnused1, float flUnused2 ) = 0;

	// True while the joint list GetFirstJoint walks is not empty, and while the CRnLink list of the body is not empty
	virtual bool HasJoints() const = 0;
	virtual bool HasLinks() const = 0;

	// Iterate the joints of the body, and find the joint to another body
	virtual IPhysicsJoint *GetFirstJoint() const = 0;
	virtual IPhysicsJoint *GetNextJoint( IPhysicsJoint *pJoint ) const = 0;
	virtual IPhysicsJoint *FindJoint( IPhysicsBody *pOther ) const = 0;

	// True when a shape of the body has a contact
	virtual bool HasContacts() const = 0;

	// MSVC lays these overloads out in reverse order
	virtual bool GetContactPoint( Vector *pContactPoint, IPhysicsBody **ppContactObject ) const = 0;
	virtual bool GetContactPoint( Vector *pContactPoint ) const = 0;

	// Inserts the entity ids of the bodies the shapes of the body touch
	virtual void GetContactEntityIds( CUtlSortVector< uint32 > &entityIds ) = 0;

	virtual IPhysicsFrictionSnapshot *CreateFrictionSnapshot() = 0;
	virtual void DestroyFrictionSnapshot( IPhysicsFrictionSnapshot *pSnapshot ) = 0;

	// Creates the shadow controller on first use
	virtual IPhysicsShadowController *CreateShadowController() = 0;
	virtual void RemoveShadowController() = 0;
	// A keyframed body always has a shadow controller; this keeps one on a dynamic body too
	virtual void EnableDynamicShadowController( bool bEnable ) = 0;
	virtual IPhysicsShadowController *GetShadowController() const = 0;

	// Forwarded to the shadow controller
	virtual void unk120() = 0;
	virtual void UpdateShadow( const Vector &vTargetPosition, const Quaternion &targetOrientation, bool bUnused, float flTimeOffset ) = 0;
	virtual float unk122() = 0;
	// False without a shadow controller
	virtual bool GetShadowPosition( Vector *pPosition, QAngle *pAngles ) = 0;

	// A vector the body owns for the caller; the library never reads its elements and frees it with the body
	virtual CUtlVector< void * > *GetUserVector() = 0;

	// Turning it off on a dynamic body wakes the body
	virtual void EnableSleep( bool bEnable ) = 0;
	virtual bool IsSleepEnabled() const = 0;

	virtual void Sleep() = 0;
	virtual void Wake() = 0;
	virtual bool IsSleeping() const = 0;

	virtual void SetGravityScale( float flScale ) = 0;
	virtual float GetGravityScale() const = 0;
	virtual void SetGravity( const Vector &vGravity, bool bWake ) = 0;
	virtual Vector GetGravity() const = 0;

	virtual float GetTimeScale() const = 0;
	virtual void SetTimeScale( float flScale ) = 0;

	virtual void SetMassPriority( int8 nPriority ) = 0;
	virtual int8 GetMassPriority() const = 0;

	virtual void SetGameData( void *pGameData ) = 0;
	virtual void *GetGameData() const = 0;

	virtual uint32 GetGameFlags() const = 0;
	virtual void SetGameFlags( uint32 nFlags ) = 0;

	// Copied into RnCollisionAttr_t::m_nEntityId of every shape
	virtual void SetEntityId( uint32 nEntityId ) = 0;
	virtual uint32 GetEntityId() const = 0;

	virtual void SetGameIndex( uint16 nIndex ) = 0;
	virtual uint16 GetGameIndex() const = 0;

	// A pointer stored in the CRnBody for the caller; IFnPhysicsWorld::ClearDebugState clears it on every body
	virtual void SetUserPointer( void *pPointer ) = 0;
	virtual void *GetUserPointer() const = 0;

	virtual const char *GetDebugName() const = 0;

	// The debug name of the CRnBody; release builds ignore it
	virtual void SetDebugName( const char *pszName ) = 0;

	virtual ~IPhysicsBody() {}
};

class CPhysicsBody : public IPhysicsBody
{
public:
	CRnBody *m_pBody;
	void *m_pGameData;
	IPhysicsShadowController *m_pShadowController;

	// Returned by GetUserVector and freed by the destructor; no code reads its elements
	CUtlVector< void * > m_UserVector;
};

// The box with the mass and the principal inertia of a body
struct PhysicsInertiaBox_t
{
	// Rotation to the principal axes of the inertia: x, y, z, w (not 16-byte aligned)
	float m_qOrientation[ 4 ];

	// The mass center
	Vector m_vCenter;

	// Half extents along the principal axes of a box of the same mass with the same principal inertia
	Vector m_vHalfExtents;

	Vector m_vPrincipalInertia;
};

// The zeroed 40-byte struct IFnPhysicsBody::unk046 returns; only its size and 8-byte alignment are known (fields at +16, +24 and +32)
struct FnPhysicsBodyUnk046_t
{
	ALIGN8 uint8 m_Data[ 40 ] ALIGN8_POST;
};

// Function table over a body, constructed in caller memory by IVPhysics2::ConstructFnPhysicsBody
abstract_class IFnPhysicsBody
{
public:
	// MSVC lays these overloads out in reverse order
	virtual void SetBody( IPhysicsBody *pBody ) = 0;
	virtual IPhysicsBody *GetBody() const = 0;
	virtual void SetBody( CRnBody *pBody ) = 0;

	virtual void SetRnBody( CRnBody *pBody ) = 0;
	virtual CRnBody *GetRnBody() const = 0;

	// RnBodyDesc_t::m_bIsContinuousEnabled
	virtual void SetContinuousEnabled( bool bEnable ) = 0;
	virtual bool IsContinuousEnabled() const = 0;

	// While this body is keyframed, the dynamic bodies touching it can use continuous contacts against it
	virtual void SetKeyframedContinuousEnabled( bool bEnable ) = 0;

	// Dynamic continuous contact behavior of the body description
	virtual void SetDynamicContinuousContactBehavior( uint8 nBehavior ) = 0;
	virtual uint8 GetDynamicContinuousContactBehavior() const = 0;

	// Minimum velocity and position solver iterations
	virtual int GetMinVelocityIterations() const = 0;
	virtual int GetMinPositionIterations() const = 0;
	virtual void SetMinIterations( int nVelocityIterations, int nPositionIterations ) = 0;

	// The transform and velocities the step stores in the body before it integrates; putting the body to sleep clears them (the
	// position becomes FLT_MAX). HasStepStartState tells whether they are set, the others return them, or the current transform
	// and velocities while they are not set
	virtual bool HasStepStartState() const = 0;
	virtual CTransform GetStepStartTransform() const = 0;
	virtual Vector GetStepStartVelocity() const = 0;

	// In degrees per second
	virtual Vector GetStepStartAngularVelocity() const = 0;

	// Time the body has been resting; putting the body to sleep resets it
	virtual float GetSleepTimer() const = 0;

	// The solver puts the body to sleep after the next step whatever its velocity; the step clears the request
	virtual void RequestSleep() = 0;

	// Union of the broadphase proxy bounds of the shapes
	virtual AABB_t GetProxyBounds() const = 0;

	// Union of the bounds of the shapes placed at the transform
	virtual AABB_t BuildBounds( const CTransform &transform ) const = 0;

	// Computes the same union of the proxy bounds as GetProxyBounds
	virtual AABB_t ComputeProxyBounds() const = 0;

	// Smallest centroid radius of the shapes, FLT_MAX without shapes
	virtual float GetMinCentroidRadius() const = 0;

	// Rebuilds the mass properties from the shapes, optionally dropping the overridden mass, mass center and inertia first
	virtual void BuildMassProperties( bool bResetOverrides ) = 0;

	// Adds the shapes, then sets the mass when pMass is given or rebuilds it
	virtual void AddShapes( IPhysicsShape **ppShapes, int nCount, const float *pMass ) = 0;

	// Inverse mass and inertia scales
	virtual float GetInvMassScale() const = 0;
	virtual float GetInvInertiaScale() const = 0;

	virtual PhysicsInertiaBox_t GetInertiaBox() const = 0;

	// Diagonalizes the inertia tensor scaled by the inverse mass: the rotation to its principal axes and the principal values
	virtual void ComputePrincipalInertia( Quaternion &rotation, Vector &vPrincipalInertia ) const = 0;

	// Inverse inertia in body space, zero unless the body is dynamic
	virtual Matrix3x3 GetLocalInvInertia() const = 0;

	// Fluids apply the physical buoyancy of the displaced volume to the body even when they are neutrally buoyant, and skip their
	// neutrally buoyant damping; the server sets it for func_physbox
	virtual void SetIgnoreNeutralBuoyancy( bool bIgnore ) = 0;

	// True when a joint of the body (IPhysicsJoint::IsConstrained) or one of its links locks the degrees of freedom nFlags selects
	virtual bool IsConstrainedByJoints( uint32 nFlags ) = 0;

	// Writes a one line description of the body and its shapes
	virtual void Describe( CUtlString &description ) const = 0;

	// Debug draw of the shapes of the body, and of the axes at the mass center of a dynamic body; DrawAtTransform places the body at the transform when one is given
	virtual void Draw( ISceneViewDebugOverlays *pDebugOverlays, uint64 nDebugDrawOptions ) = 0;
	virtual void DrawAtTransform( ISceneViewDebugOverlays *pDebugOverlays, uint64 nDebugDrawOptions, const CTransform *pTransform ) = 0;

	virtual void UpdateShapeSceneObjects() = 0;

	// Call IFnPhysicsShape::SetDebugShapeColor, SetDebugShapeOpacity and SetDebugShapeAlphaFactor on the shapes of the body
	virtual void SetDebugShapeColor( const Vector4D &vColor ) = 0;
	virtual void SetDebugShapeOpacity( float flOpacity ) = 0;
	virtual void SetDebugShapeAlphaFactor( int nIndex, float flValue ) = 0;

	// Creates or updates the debug shapes of the shapes that pass the debug draw filter when bEnabled is set, otherwise destroys
	// them; IVPhysics2World::UpdateDebugShapes calls it for every body. The Linux implementation does not read the float
	virtual void UpdateDebugShapes( bool bEnabled, float flUnused ) = 0;

	virtual const char *GetDebugName() const = 0;

	// Index of the body in its world, -1 without a body
	virtual int GetIndexInWorld() const = 0;

	// Set when the body is removed from its world and waits in the deletion list
	virtual bool IsPendingDestruction() const = 0;

	// The sums of IPhysicsShape::GetContactCounts over the shapes of the body
	virtual PhysicsShapeContactCounts_t GetContactCounts() const = 0;

	virtual void OutputDebugInfo() const = 0;

	// Empty in release builds
	virtual void unk045() = 0;

	// Returns zeros
	virtual FnPhysicsBodyUnk046_t unk046() const = 0;

	virtual void unk047() = 0;
};

#endif // IPHYSICSBODY_H
