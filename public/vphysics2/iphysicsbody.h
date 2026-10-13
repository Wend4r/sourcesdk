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
#include "tier0/utlstringtoken.h"
#include "tier0/utlvector.h"
#include "mathlib/vector.h"
#include "mathlib/mathlib.h"
#include "mathlib/aabb.h"
#include "mathlib/transform.h"
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

enum PhysicsBodyType_t
{
	BODY_STATIC = 0,
	BODY_KEYFRAMED,
	BODY_DYNAMIC,

	BODY_TYPE_COUNT
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
	virtual int unk010( const IPhysicsShape *pShape ) const = 0;

	// bUpdateMass rebuilds the mass properties from the shapes; pMaterials is a count followed by a pointer to RnMaterial_t records
	virtual IPhysicsShape *AddSphereShape( const RnSphere_t &sphere, float flScale, bool bUpdateMass ) = 0;
	virtual IPhysicsShape *AddCapsuleShape( const RnCapsule_t &capsule, float flScale, bool bUpdateMass ) = 0;
	virtual IPhysicsShape *AddHullShape( const RnHull_t *pHull, float flScale, bool bUpdateMass ) = 0;
	virtual IPhysicsShape *AddMeshShape( const RnMesh_t *pMesh, const Vector &vScale, bool bUpdateMass, const void *pMaterials ) = 0;
	virtual IPhysicsShape *AddCompoundShape( const RnCompound_t *pCompound, float flScale, bool bUpdateMass, const void *pMaterials ) = 0;

	// Adds a copy of a shape of another body
	virtual IPhysicsShape *unk016( const IPhysicsShape *pShape ) = 0;

	virtual void RemoveShape( IPhysicsShape *pShape ) = 0;
	virtual void PurgeShapes() = 0;

	// Sets or clears FCOLLISION_FUNC_ENABLE_TOUCH_EVENT on every shape
	virtual void EnableTouchEvents( bool bEnable ) = 0;
	virtual void SetCollisionAttributes( const RnCollisionAttr_t &attributes ) = 0;
	virtual void SetMaterialIndex( CUtlStringToken materialIndex ) = 0;

	// Sum of the surface areas of the shapes
	virtual float unk022() const = 0;

	// Farthest point of the shapes along a direction, at the current or at the given placement
	virtual Vector unk023( const Vector &vDirection ) const = 0;
	virtual Vector unk023( const CTransform &transform, const Vector &vDirection ) const = 0;

	// Shape queries; the first tests a point with a tolerance, the others pass the query to every shape
	virtual bool unk025( const Vector &vPoint, float flTolerance ) const = 0;
	virtual bool unk025( const void *pQuery, const CTransform &transform ) const = 0;
	virtual bool unk025( const void *pQuery ) const = 0;

	virtual Vector GetClosestPoint( const Vector &vPoint ) const = 0;

	// Closest point of the shapes to a segment
	virtual Vector unk029( const Vector &vStart, const Vector &vEnd, void *pUnknown ) const = 0;

	// Bounds of the shapes in body space
	virtual AABB_t unk030() const = 0;
	virtual AABB_t BuildBounds() const = 0;

	virtual void SetMass( float flMass, bool bUnknown ) = 0;
	virtual void SetLocalMassCenter( const Vector &vMassCenter ) = 0;
	virtual void unk034( const RnMassProperties_t &massProperties, bool bUnknown ) = 0;

	// Set while the mass, the mass center and the inertia are overridden instead of built from the shapes
	virtual bool unk035() const = 0;
	virtual bool GetOverrideMassCenter() const = 0;

	virtual float GetMass() const = 0;
	virtual float GetInvMass() const = 0;

	// Multiply and reset the mass scale
	virtual void unk039( float flScale ) = 0;
	virtual void unk040() = 0;

	virtual Vector GetMassCenter() const = 0;
	virtual Vector GetLocalMassCenter() const = 0;
	virtual RnMassProperties_t unk043() const = 0;
	virtual bool unk044() const = 0;

	// Body space inertia tensor
	virtual matrix3x4_t unk045() const = 0;
	virtual Vector unk046() const = 0;

	virtual void ScaleInertia( float flScale ) = 0;

	// Resets the inertia scale
	virtual void unk048() = 0;

	virtual void SetPosition( const Vector &vPosition ) = 0;
	virtual Vector GetPosition() const = 0;
	virtual void SetOrientation( const Quaternion &orientation ) = 0;
	virtual Quaternion GetOrientation() const = 0;
	virtual void SetTransform( const CTransform &transform ) = 0;
	virtual CTransform GetTransform() const = 0;

	// Set and get the transform with the body scale in the w component of the position
	virtual void unk055( const CTransform &transform ) = 0;
	virtual CTransform unk056() const = 0;

	// Target transform of the shadow controller while one drives the body, the transform otherwise
	virtual CTransform unk057() const = 0;

	virtual Vector LocalToWorldVector( const Vector &vLocal ) const = 0;
	virtual Vector WorldToLocalVector( const Vector &vWorld ) const = 0;
	virtual Vector LocalToWorld( const Vector &vLocal ) const = 0;
	virtual Vector WorldToLocal( const Vector &vWorld ) const = 0;

	// Rotate an orientation into body space and back
	virtual Quaternion unk062( const Quaternion &world ) const = 0;
	virtual Quaternion unk063( const Quaternion &local ) const = 0;

	virtual void SetLinearVelocity( const Vector &vVelocity ) = 0;
	virtual void AddLinearVelocity( const Vector &vVelocity ) = 0;
	virtual Vector GetLinearVelocity() const = 0;
	virtual Vector GetVelocityAtPoint( const Vector &vPoint ) const = 0;

	// Angular velocities are in degrees per second
	virtual void SetAngularVelocity( const Vector &vAngularVelocity ) = 0;
	virtual void AddAngularVelocity( const Vector &vAngularVelocity ) = 0;
	virtual Vector GetAngularVelocity() const = 0;

	// Set, add and get the angular velocity in body space
	virtual void unk071( const Vector &vAngularVelocity ) = 0;
	virtual void unk072( const Vector &vAngularVelocity ) = 0;
	virtual Vector unk073() const = 0;

	virtual void ApplyLinearImpulse( const Vector &vImpulse ) = 0;
	virtual void ApplyLinearImpulseAtWorldSpace( const Vector &vImpulse, const Vector &vPosition ) = 0;
	virtual void ApplyAngularImpulse( const Vector &vImpulse ) = 0;

	// Drives a dynamic body towards a transform over the given time
	virtual void unk077( const CTransform &target, float flTime, float flUnknown1, float flUnknown2 ) = 0;

	virtual void CalculateVelocityOffset( const Vector &vForce, const Vector &vPosition, Vector *pCenterVelocity, Vector *pCenterAngularVelocity ) const = 0;

	// Applies a linear acceleration (scaled by the mass) as a force
	virtual void unk079( const Vector &vAcceleration, bool bWake ) = 0;

	virtual void ApplyForce( const Vector &vForce, bool bWake ) = 0;
	virtual void ApplyForceAt( const Vector &vForce, const Vector &vPosition, bool bWake ) = 0;

	// Applies an angular acceleration (scaled by the world inertia) as a torque
	virtual void unk082( const Vector &vAcceleration, bool bWake ) = 0;

	virtual void ApplyTorque( const Vector &vTorque, bool bWake ) = 0;
	virtual void ClearForces() = 0;
	virtual void ClearTorque() = 0;

	virtual void EnableDrag( bool bEnable ) = 0;
	virtual bool IsDragEnabled() const = 0;

	// Linear and angular drag coefficients used by the drag calculations below; setting one enables drag
	virtual void unk088( float flDrag ) = 0;
	virtual float unk089() const = 0;
	virtual void unk090( float flDrag ) = 0;
	virtual float unk091() const = 0;

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
	virtual IPhysicsJoint *unk107() const = 0;
	virtual IPhysicsJoint *unk108( IPhysicsJoint *pJoint ) const = 0;
	virtual IPhysicsJoint *unk109( IPhysicsBody *pOther ) const = 0;

	virtual bool unk110() const = 0;

	// MSVC lays these overloads out in reverse order
	virtual bool GetContactPoint( Vector *pContactPoint, IPhysicsBody **ppContactObject ) const = 0;
	virtual bool GetContactPoint( Vector *pContactPoint ) const = 0;

	virtual void unk113() = 0;

	virtual IPhysicsFrictionSnapshot *CreateFrictionSnapshot() = 0;
	virtual void DestroyFrictionSnapshot( IPhysicsFrictionSnapshot *pSnapshot ) = 0;

	// Creates the shadow controller on first use
	virtual IPhysicsShadowController *unk116() = 0;
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

#endif // IPHYSICSBODY_H
