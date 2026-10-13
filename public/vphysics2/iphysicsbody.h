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
#include "tier0/utlstring.h"
#include "tier0/utlstringtoken.h"
#include "tier0/utlvector.h"
#include "mathlib/vector.h"
#include "mathlib/mathlib.h"
#include "mathlib/aabb.h"
#include "mathlib/transform.h"
#include "physicslib/param_types.h"
#include "vphysics2/iphysicsshape.h"
#include "schemasystem/schematypes.h"

class IVPhysics2World;
class IPhysicsShape;
class IPhysicsJoint;
class IPhysicsShadowController;
class IPhysicsFrictionSnapshot;
class CRnBody;
class CSchemaClassInfo;

struct RnSphere_t;
struct RnCapsule_t;
struct RnHull_t;
struct RnMesh_t;
struct RnCompound_t;
struct RnCollisionAttr_t;
struct RnMassProperties_t;

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
	uint32 m_unk140;

	// Shapes by kind: spheres, capsules, hulls, meshes and compounds; the element types (232, 248, 224, 224 and 224 bytes) are not recovered
	CUtlLeanVector< void * > m_SphereShapes;
	CUtlLeanVector< void * > m_CapsuleShapes;
	CUtlLeanVector< void * > m_HullShapes;
	CUtlLeanVector< void * > m_MeshShapes;
	CUtlLeanVector< void * > m_CompoundShapes;
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

	// Shape queries; the first tests a point with a tolerance, the others pass the query to every shape
	virtual bool unk025( const Vector &vPoint, float flTolerance ) const = 0;
	virtual bool unk025( const void *pQuery, const CTransform &transform ) const = 0;
	virtual bool unk025( const void *pQuery ) const = 0;

	virtual Vector GetClosestPoint( const Vector &vPoint ) const = 0;

	// Closest point of the shapes to a segment
	virtual Vector GetClosestPointToSegment( const Vector &vStart, const Vector &vEnd, void *pUnknown ) const = 0;

	// Bounds of the shapes in body space
	virtual AABB_t BuildLocalBounds() const = 0;
	virtual AABB_t BuildBounds() const = 0;

	virtual void SetMass( float flMass, bool bUnknown ) = 0;
	virtual void SetLocalMassCenter( const Vector &vMassCenter ) = 0;
	virtual void SetMassProperties( const RnMassProperties_t &massProperties, bool bUnknown ) = 0;

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
	virtual bool unk044() const = 0;

	// Body space inertia tensor
	virtual matrix3x4_t GetLocalInertia() const = 0;
	virtual Vector unk046() const = 0;

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

	// Drives a dynamic body towards a transform over the given time
	virtual void MoveTo( const CTransform &target, float flTime, float flUnknown1, float flUnknown2 ) = 0;

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

	virtual void unk098( float flUnknown ) = 0;
	virtual float unk099() const = 0;

	virtual void SetLinearFluidDragScale( float flScale ) = 0;
	virtual void SetAngularFluidDragScale( float flScale ) = 0;

	virtual void SetBuoyancyScale( float flScale ) = 0;
	virtual float GetBuoyancyScale() const = 0;

	virtual float unk104( const void *pParams, float flUnknown1, float flUnknown2 ) = 0;

	// True while a joint or a contact references the body
	virtual bool unk105() const = 0;
	virtual bool unk106() const = 0;

	// Iterate the joints of the body, and find the joint to another body
	virtual IPhysicsJoint *GetFirstJoint() const = 0;
	virtual IPhysicsJoint *GetNextJoint( IPhysicsJoint *pJoint ) const = 0;
	virtual IPhysicsJoint *FindJoint( IPhysicsBody *pOther ) const = 0;

	virtual bool unk110() const = 0;

	// MSVC lays these overloads out in reverse order
	virtual bool GetContactPoint( Vector *pContactPoint, IPhysicsBody **ppContactObject ) const = 0;
	virtual bool GetContactPoint( Vector *pContactPoint ) const = 0;

	virtual void unk113() = 0;

	virtual IPhysicsFrictionSnapshot *CreateFrictionSnapshot() = 0;
	virtual void DestroyFrictionSnapshot( IPhysicsFrictionSnapshot *pSnapshot ) = 0;

	// Creates the shadow controller on first use
	virtual IPhysicsShadowController *CreateShadowController() = 0;
	virtual void RemoveShadowController() = 0;
	virtual void unk118( bool bEnable ) = 0;
	virtual IPhysicsShadowController *GetShadowController() const = 0;

	// Forwarded to the shadow controller
	virtual void unk120() = 0;
	virtual void unk121( const Vector &vPosition, const void *pUnknown, bool bUnknown, float flUnknown ) = 0;
	virtual float unk122() = 0;
	virtual bool unk123() = 0;

	virtual CUtlVector< void * > *unk124() = 0;

	// Turning it off on a dynamic body wakes the body
	virtual void unk125( bool bEnable ) = 0;
	virtual bool unk126() const = 0;

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

	virtual void unk146( void *pUnknown ) = 0;
	virtual void *unk147() const = 0;

	virtual const char *GetDebugName() const = 0;

	// Empty in release builds
	virtual void unk149() = 0;

	virtual ~IPhysicsBody() {}
};

class CPhysicsBody : public IPhysicsBody
{
public:
	CRnBody *m_pBody;
	void *m_pGameData;
	IPhysicsShadowController *m_pShadowController;

	// Returned by unk124 and freed by the destructor; no code reads its elements
	CUtlVector< void * > m_unk020;
};

// The 52-byte result of IFnPhysicsBody::unk027 (thirteen floats built from the inertia of the body); only its size is known
struct FnPhysicsBodyUnk027_t
{
	float m_flData[ 13 ];
};

// The 36-byte result of IFnPhysicsBody::unk029 (nine floats); only its size is known
struct FnPhysicsBodyUnk029_t
{
	float m_flData[ 9 ];
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

	// Continuous collision flags of the body
	virtual void unk005( bool bEnable ) = 0;
	virtual bool unk006() const = 0;
	virtual void unk007( bool bEnable ) = 0;

	// Dynamic continuous contact behavior of the body description
	virtual void SetDynamicContinuousContactBehavior( uint8 nBehavior ) = 0;
	virtual uint8 GetDynamicContinuousContactBehavior() const = 0;

	// Minimum velocity and position solver iterations
	virtual int GetMinVelocityIterations() const = 0;
	virtual int GetMinPositionIterations() const = 0;
	virtual void SetMinIterations( int nVelocityIterations, int nPositionIterations ) = 0;

	virtual bool unk013() const = 0;
	virtual CTransform unk014() const = 0;
	virtual Vector unk015() const = 0;
	virtual Vector unk016() const = 0;
	virtual float unk017() const = 0;
	virtual void unk018() = 0;

	virtual AABB_t unk019() const = 0;
	virtual AABB_t unk020( const CTransform &transform ) const = 0;
	virtual AABB_t unk021() const = 0;

	virtual float unk022() const = 0;

	// Rebuilds the mass properties from the shapes, optionally dropping the overridden mass, mass center and inertia first
	virtual void BuildMassProperties( bool bResetOverrides ) = 0;

	// Adds the shapes, then sets the mass when pMass is given or rebuilds it
	virtual void AddShapes( IPhysicsShape **ppShapes, int nCount, const float *pMass ) = 0;

	// Inverse mass and inertia scales
	virtual float GetInvMassScale() const = 0;
	virtual float GetInvInertiaScale() const = 0;

	virtual FnPhysicsBodyUnk027_t unk027() const = 0;

	// Diagonalizes the inertia tensor scaled by the inverse mass: the rotation to its principal axes and the principal values
	virtual void ComputePrincipalInertia( Quaternion *pRotation, Vector *pPrincipalInertia ) const = 0;

	virtual FnPhysicsBodyUnk029_t unk029() const = 0;

	virtual void unk030( bool bEnable ) = 0;
	virtual void unk031( uint32 nUnknown ) = 0;

	// Writes a one line description of the body and its shapes
	virtual void Describe( CUtlString *pDescription ) const = 0;

	virtual void unk033( void *pUnknown1, void *pUnknown2 ) = 0;
	virtual void unk034( void *pUnknown1, void *pUnknown2, const float *pUnknown3 ) = 0;

	virtual void UpdateShapeSceneObjects() = 0;

	virtual void unk036( void *pUnknown ) = 0;
	virtual void unk037() = 0;
	virtual void unk038() = 0;
	virtual void unk039( bool bUnknown, float flUnknown ) = 0;

	virtual const char *GetDebugName() const = 0;

	// Index of the body in its world, -1 without a body
	virtual int GetIndexInWorld() const = 0;
	virtual bool unk042() const = 0;

	// The sums of IPhysicsShape::unk033 over the shapes of the body
	virtual PhysicsShapeUnk033_t unk043() const = 0;

	virtual void OutputDebugInfo() const = 0;

	// Empty in release builds
	virtual void unk045() = 0;

	// Returns zeros
	virtual FnPhysicsBodyUnk046_t unk046() const = 0;

	virtual void unk047() = 0;
};

#endif // IPHYSICSBODY_H
