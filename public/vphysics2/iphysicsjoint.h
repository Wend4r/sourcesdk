//===== Copyright © Valve Corporation, All rights reserved. ======//
//
// Purpose: A joint constraining two physics bodies
//
//===========================================================================//

#ifndef IPHYSICSJOINT_H
#define IPHYSICSJOINT_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "tier0/utlstring.h"
#include "tier0/utldelegateimpl.h"
#include "mathlib/vector.h"
#include "mathlib/transform.h"
#include "physicslib/param_types.h"
#include "physicslib/serialize.h"
#include "schemasystem/schematypes.h"

#include "schemacompiler/schemaon.h"

class IPhysicsBody;
class IPhysicsJoint;
class IVPhysics2World;
class CRnJoint;

struct PhysicsJointDesc_t;
struct PhysicsSphericalJointDesc_t;
struct PhysicsRevoluteJointDesc_t;
struct PhysicsPrismaticJointDesc_t;
struct PhysicsConicalJointDesc_t;
struct PhysicsWeldJointDesc_t;
struct PhysicsGenericJointDesc_t;
struct PhysicsSpringJointDesc_t;
struct PhysicsPulleyJointDesc_t;
struct PhysicsSplineJointDesc_t;
struct PhysicsWheelJointDesc_t;

// Called when the joint breaks, with the joint as the argument
typedef CUtlDelegate< void ( IPhysicsJoint * ) > PhysicsJointBreakDelegate_t;

// The zeroed 40-byte struct IPhysicsJoint::unk074 returns; only its size and 8-byte alignment are known (fields at +16, +24 and +32)
struct PhysicsJointUnk074_t
{
	ALIGN8 uint8 m_Data[ 40 ] ALIGN8_POST;
};

schema abstract_class IPhysicsJoint
{
public:
	virtual SchemaMetaInfoHandle_t< CSchemaClassInfo > Schema_DynamicBinding() = 0;

	virtual IVPhysics2World *GetWorld() = 0;
	virtual const IVPhysics2World *GetWorld() const = 0;

	virtual void SetEnabled( bool bEnabled ) = 0;
	virtual bool IsEnabled() const = 0;

	virtual PhysicsJointType_t GetType() const = 0;

	// Each fills the description of the joint when it has that type
	virtual PhysicsSphericalJointDesc_t GetSphericalJointDesc() const = 0;
	virtual PhysicsRevoluteJointDesc_t GetRevoluteJointDesc() const = 0;
	virtual PhysicsPrismaticJointDesc_t GetPrismaticJointDesc() const = 0;
	virtual PhysicsConicalJointDesc_t GetConicalJointDesc() const = 0;
	virtual PhysicsWeldJointDesc_t GetWeldJointDesc() const = 0;
	virtual PhysicsGenericJointDesc_t GetGenericJointDesc() const = 0;
	virtual PhysicsSpringJointDesc_t GetSpringJointDesc() const = 0;
	virtual PhysicsPulleyJointDesc_t GetPulleyJointDesc() const = 0;
	virtual PhysicsJointDesc_t GetNullJointDesc() const = 0;
	virtual PhysicsSplineJointDesc_t GetSplineJointDesc() const = 0;
	virtual PhysicsWheelJointDesc_t GetWheelJointDesc() const = 0;

	// The reference origin and basis are in the space of the reference body, the legacy constraints pass their reference object
	// (the one the ragdoll constraint frame is relative to) as it. The attached origin and basis are in the space of the attached body.
	// GetBody and SetBody take index 0 for the reference body and index 1 for the attached body
	virtual IPhysicsBody *GetBody( int nIndex ) const = 0;
	virtual void SetBody( int nIndex, IPhysicsBody *pBody ) = 0;
	virtual IPhysicsBody *GetReferenceBody() const = 0;
	virtual IPhysicsBody *GetAttachedBody() const = 0;
	virtual IPhysicsBody *GetOtherBody( IPhysicsBody *pBody ) const = 0;
	virtual void SetReferenceBody( IPhysicsBody *pBody ) = 0;
	virtual void SetAttachedBody( IPhysicsBody *pBody ) = 0;

	virtual void WakeBodies() = 0;

	virtual Vector GetWorldReferenceOrigin() const = 0;
	virtual Vector GetLocalReferenceOrigin() const = 0;
	virtual void SetLocalReferenceOrigin( const Vector &vOrigin ) = 0;
	virtual Vector GetWorldAttachedOrigin() const = 0;
	virtual Vector GetLocalAttachedOrigin() const = 0;
	virtual void SetLocalAttachedOrigin( const Vector &vOrigin ) = 0;

	virtual Quaternion GetWorldReferenceBasis() const = 0;
	virtual Quaternion GetLocalReferenceBasis() const = 0;
	virtual void SetLocalReferenceBasis( const Quaternion &qBasis ) = 0;
	virtual Quaternion GetWorldAttachedBasis() const = 0;
	virtual Quaternion GetLocalAttachedBasis() const = 0;
	virtual void SetLocalAttachedBasis( const Quaternion &qBasis ) = 0;

	// nIndex 0 or 1 selects the body
	virtual Vector GetWorldOrigin( int nIndex ) const = 0;
	virtual Vector GetLocalOrigin( int nIndex ) const = 0;
	virtual void SetLocalOrigin( int nIndex, const Vector &vOrigin ) = 0;
	virtual Quaternion GetWorldBasis( int nIndex ) const = 0;
	virtual Quaternion GetLocalBasis( int nIndex ) const = 0;
	virtual void SetLocalBasis( int nIndex, const Quaternion &qBasis ) = 0;

	// Whether the enabled joint locks the degrees of freedom that nFlags selects (bits 3 to 6 choose the linear and angular axes and
	// how many of them must be locked), between its bodies or for one of them; false when pBody is not a body of the joint.
	// MSVC groups these overloads in reverse order
	virtual bool IsConstrained( uint32 nFlags ) = 0;
	virtual bool IsConstrained( uint32 nFlags, IPhysicsBody *pBody ) = 0;

	virtual void SetLinearConstraintEnabled( bool bEnabled ) = 0;
	virtual bool IsLinearConstraintEnabled() const = 0;
	virtual void SetAngularConstraintEnabled( bool bEnabled ) = 0;
	virtual bool IsAngularConstraintEnabled() const = 0;

	// MSVC groups these overloads in reverse order
	virtual float GetLinearImpulse( JointAxis_t nAxis ) const = 0;
	virtual float GetLinearImpulse() const = 0;
	virtual float GetAngularImpulse() const = 0;

	// The joint breaks when an impulse exceeds these; zero disables breaking
	virtual void SetMaxLinearImpulse( float flMaxImpulse ) = 0;
	virtual float GetMaxLinearImpulse() const = 0;
	virtual void SetMaxAngularImpulse( float flMaxImpulse ) = 0;
	virtual float GetMaxAngularImpulse() const = 0;

	virtual void SetMassPriorityShift( int8 nShift ) = 0;
	virtual int GetMassPriorityShift() const = 0;

	virtual void SetUserData( void *pUserData ) = 0;
	virtual void *GetUserData() const = 0;

	// The debug name of the CRnJoint; release builds ignore it and return NULL
	virtual void SetDebugName( const char *pszName ) = 0;
	virtual const char *GetDebugName() const = 0;

	virtual const CUtlString &GetTag() const = 0;
	virtual void SetTag( const char *pszTag ) = 0;

	// A value stored in the CRnJoint for the caller, -1 by default; the library does not read it
	virtual void SetUserIndex( int nIndex ) = 0;
	virtual int GetUserIndex() const = 0;

	// Index of the joint in its world, -1 outside of it
	virtual int GetIndexInWorld() const = 0;

	virtual void SetEnableCollision( bool bEnable ) = 0;
	virtual bool IsCollisionEnabled() const = 0;

	// The transform of pBody that satisfies the joint given the transform of the other body, projected on the linear and the
	// angular degrees of freedom the joint locks (each when its flag is set), optionally clamped to the limits; bodyTransform when
	// the joint is disabled or pBody is not one of its bodies
	virtual CTransform ConstrainTransform( IPhysicsBody *pBody, const CTransform &otherTransform, const CTransform &bodyTransform, bool bLinear, bool bAngular, bool bApplyLimits ) = 0;

	// Calls the break delegate, then disables the joint and wakes the bodies
	virtual void Break() = 0;
	virtual void SetBreakDelegate( const PhysicsJointBreakDelegate_t &breakDelegate ) = 0;

	virtual CRnJoint *GetRnJoint() const = 0;

	// Empty
	virtual void unk073() = 0;

	// Returns zeros
	virtual PhysicsJointUnk074_t unk074() = 0;

	virtual ~IPhysicsJoint() {}
};

#include "schemacompiler/schemaoff.h"

#endif // IPHYSICSJOINT_H
