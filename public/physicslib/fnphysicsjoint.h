//===== Copyright © Valve Corporation, All rights reserved. ======//
//
// Purpose: Function interfaces over a joint. IVPhysics2::ConstructFnPhysics*Joint
//          builds one in a caller-provided buffer and SetJoint binds it to a joint;
//          the typed interfaces only bind joints of their own type
//
//===========================================================================//

#ifndef FNPHYSICSJOINT_H
#define FNPHYSICSJOINT_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "mathlib/vector.h"
#include "mathlib/vector2d.h"
#include "physicslib/param_types.h"
#include "physicslib/serialize.h"

class CBufferString;
class CRnJoint;
class IPhysicsJoint;

abstract_class IFnPhysicsJoint
{
public:
	// MSVC groups these overloads in reverse order
	virtual void SetJoint( IPhysicsJoint *pJoint ) = 0;
	virtual void SetJoint( CRnJoint *pJoint ) = 0;

	virtual IPhysicsJoint *GetJoint() const = 0;

	virtual Vector GetWorldOrigin1() const = 0;
	virtual Vector GetLocalOrigin1() const = 0;
	virtual Vector GetWorldOrigin2() const = 0;
	virtual Vector GetLocalOrigin2() const = 0;

	virtual Quaternion GetLocalBasis1() const = 0;
	virtual Quaternion GetWorldBasis1() const = 0;
	virtual Quaternion GetLocalBasis2() const = 0;
	virtual Quaternion GetWorldBasis2() const = 0;

	virtual void SetLocalOrigin1( const Vector &vOrigin ) = 0;
	virtual void SetLocalOrigin2( const Vector &vOrigin ) = 0;

	virtual void SetEnableCollision( bool bEnable ) = 0;
	virtual bool IsCollisionEnabled() const = 0;

	// Multiplies the accumulated constraint impulses
	virtual void ScaleImpulses( float flScale ) = 0;

	// Debug text: tag, mass priority, bodies and anchors, then the joint type details
	virtual void Describe( CBufferString &buffer ) const = 0;
	virtual void DescribeState( CBufferString &buffer ) const = 0;

	virtual CRnJoint *GetRnJoint() const = 0;
};

abstract_class IFnPhysicsAnyJoint : public IFnPhysicsJoint
{
};

abstract_class IFnPhysicsRevoluteJoint : public IFnPhysicsJoint
{
public:
	virtual void SetMotorMode( PhysicsJointMotorMode_t nMode ) = 0;
	virtual PhysicsJointMotorMode_t GetMotorMode() const = 0;

	// In degrees
	virtual void SetMotorTargetAngle( float flAngle ) = 0;
	virtual float GetMotorTargetAngle() const = 0;

	virtual void SetMotorFrequency( float flFrequency ) = 0;
	virtual float GetMotorFrequency() const = 0;
	virtual void SetMotorDampingRatio( float flDampingRatio ) = 0;
	virtual float GetMotorDampingRatio() const = 0;
	virtual void SetMotorTargetVelocity( float flVelocity ) = 0;
	virtual float GetMotorTargetVelocity() const = 0;
	virtual void SetMotorMaxTorque( float flMaxTorque ) = 0;
	virtual float GetMotorMaxTorque() const = 0;

	// A velocity motor with zero target velocity; zero or less disables the motor
	virtual void SetFriction( float flFriction ) = 0;

	virtual void SetLimitEnabled( bool bEnabled ) = 0;
	virtual bool IsLimitEnabled() const = 0;

	// Minimum and maximum angle in degrees
	virtual void SetLimitAngles( const Vector2D &vLimits ) = 0;
	virtual Vector2D GetLimitAngles() const = 0;

	virtual void unk036( float flUnknown ) = 0;

	// In degrees and degrees per second
	virtual float GetAngle() const = 0;
	virtual float GetAngularVelocity() const = 0;

	virtual Vector GetWorldAxis() const = 0;

	virtual float unk040() const = 0;
};

abstract_class IFnPhysicsPrismaticJoint : public IFnPhysicsJoint
{
public:
	virtual void SetMotorMode( PhysicsJointMotorMode_t nMode ) = 0;
	virtual PhysicsJointMotorMode_t GetMotorMode() const = 0;
	virtual void SetMotorTargetOffset( float flOffset ) = 0;
	virtual float GetMotorTargetOffset() const = 0;
	virtual void SetMotorFrequency( float flFrequency ) = 0;
	virtual float GetMotorFrequency() const = 0;
	virtual void SetMotorDampingRatio( float flDampingRatio ) = 0;
	virtual float GetMotorDampingRatio() const = 0;
	virtual void SetMotorTargetVelocity( float flVelocity ) = 0;
	virtual float GetMotorTargetVelocity() const = 0;
	virtual void SetMotorMaxForce( float flMaxForce ) = 0;
	virtual float GetMotorMaxForce() const = 0;

	// A velocity motor with zero target velocity; zero or less disables the motor
	virtual void SetFriction( float flFriction ) = 0;

	virtual void SetLimitEnabled( bool bEnabled ) = 0;
	virtual bool IsLimitEnabled() const = 0;

	// Minimum and maximum offset
	virtual void SetLimitOffsets( const Vector2D &vLimits ) = 0;
	virtual Vector2D GetLimitOffsets() const = 0;

	virtual float GetOffset() const = 0;
	virtual float GetVelocity() const = 0;
	virtual Vector GetWorldAxis() const = 0;

	virtual float unk039() const = 0;
};

abstract_class IFnPhysicsWeldJoint : public IFnPhysicsJoint
{
public:
	virtual void SetLinearFrequency( float flFrequency ) = 0;
	virtual float GetLinearFrequency() const = 0;
	virtual void SetLinearDampingRatio( float flDampingRatio ) = 0;
	virtual float GetLinearDampingRatio() const = 0;
	virtual void SetMaxForce( float flMaxForce ) = 0;
	virtual float GetMaxForce() const = 0;
	virtual void SetAngularFrequency( float flFrequency ) = 0;
	virtual float GetAngularFrequency() const = 0;
	virtual void SetAngularDampingRatio( float flDampingRatio ) = 0;
	virtual float GetAngularDampingRatio() const = 0;
	virtual void SetMaxTorque( float flMaxTorque ) = 0;
	virtual float GetMaxTorque() const = 0;

	virtual float unk031() const = 0;
};

abstract_class IFnPhysicsSpringJoint : public IFnPhysicsJoint
{
public:
	virtual void SetFrequency( float flFrequency ) = 0;
	virtual float GetFrequency() const = 0;
	virtual void SetDampingRatio( float flDampingRatio ) = 0;
	virtual float GetDampingRatio() const = 0;
	virtual void SetReferenceMass( float flMass ) = 0;
	virtual float GetReferenceMass() const = 0;

	virtual void SetRestLength( float flMinRestLength, float flMaxRestLength ) = 0;
	virtual float GetMinRestLength() const = 0;
	virtual float GetMaxRestLength() const = 0;

	// Converted from and to the frequency and damping ratio with the masses of both bodies
	virtual void SetStiffnessAndDamping( float flStiffness, float flDamping ) = 0;
	virtual Vector2D GetStiffnessAndDamping() const = 0;
};

abstract_class IFnPhysicsSphericalJoint : public IFnPhysicsJoint
{
public:
	virtual void SetMotorMode( PhysicsJointMotorMode_t nMode ) = 0;
	virtual PhysicsJointMotorMode_t GetMotorMode() const = 0;
	virtual void SetMotorTargetOrientation( const Quaternion &qOrientation ) = 0;
	virtual Quaternion GetMotorTargetOrientation() const = 0;
	virtual void SetMotorFrequency( float flFrequency ) = 0;
	virtual float GetMotorFrequency() const = 0;
	virtual void SetMotorDampingRatio( float flDampingRatio ) = 0;
	virtual float GetMotorDampingRatio() const = 0;
	virtual void SetMotorTargetVelocity( const Vector &vVelocity ) = 0;
	virtual Vector GetMotorTargetVelocity() const = 0;
	virtual void SetMotorMaxTorque( float flMaxTorque ) = 0;
	virtual float GetMotorMaxTorque() const = 0;

	// A velocity motor with zero target velocity; zero or less disables the motor
	virtual void SetFriction( float flFriction ) = 0;
};

abstract_class IFnPhysicsPulleyJoint : public IFnPhysicsJoint
{
public:
	virtual void SetFrequency( float flFrequency ) = 0;
	virtual float GetFrequency() const = 0;
	virtual void SetDampingRatio( float flDampingRatio ) = 0;
	virtual float GetDampingRatio() const = 0;

	virtual Vector GetGround1() const = 0;
	virtual Vector GetGround2() const = 0;
	virtual void SetGround1( const Vector &vGround ) = 0;
	virtual void SetGround2( const Vector &vGround ) = 0;

	virtual float GetRatio() const = 0;
	virtual void SetRatio( float flRatio ) = 0;

	virtual float GetRestLength1() const = 0;
	virtual float GetRestLength2() const = 0;
	virtual void SetRestLength1( float flLength ) = 0;
	virtual void SetRestLength2( float flLength ) = 0;
};

abstract_class IFnPhysicsConicalJoint : public IFnPhysicsJoint
{
public:
	virtual void SetMotorMode( PhysicsJointMotorMode_t nMode ) = 0;
	virtual PhysicsJointMotorMode_t GetMotorMode() const = 0;
	virtual void SetMotorTargetOrientation( const Quaternion &qOrientation ) = 0;
	virtual Quaternion GetMotorTargetOrientation() const = 0;
	virtual void SetMotorFrequency( float flFrequency ) = 0;
	virtual float GetMotorFrequency() const = 0;
	virtual void SetMotorDampingRatio( float flDampingRatio ) = 0;
	virtual float GetMotorDampingRatio() const = 0;
	virtual void SetMotorTargetVelocity( const Vector &vVelocity ) = 0;
	virtual Vector GetMotorTargetVelocity() const = 0;
	virtual void SetMotorMaxTorque( float flMaxTorque ) = 0;
	virtual float GetMotorMaxTorque() const = 0;

	// A velocity motor with zero target velocity; zero or less disables the motor
	virtual void SetFriction( float flFriction ) = 0;

	virtual void SetTwistLimitEnabled( bool bEnabled ) = 0;
	virtual bool IsTwistLimitEnabled() const = 0;

	// Minimum and maximum twist angle in degrees
	virtual void SetTwistLimits( const Vector2D &vLimits ) = 0;
	virtual Vector2D GetTwistLimits() const = 0;

	// In degrees
	virtual float GetTwistAngle() const = 0;

	// Impulse of the twist limit over the time step
	virtual float GetTwistLimitTorque( float flTimeStep ) const = 0;

	virtual void SetSwingLimitEnabled( bool bEnabled ) = 0;
	virtual bool IsSwingLimitEnabled() const = 0;

	// In degrees
	virtual void SetSwingLimit( float flLimit ) = 0;
	virtual float GetSwingLimit() const = 0;
	virtual float GetSwingAngle() const = 0;

	// Impulse of the swing limit over the time step
	virtual float GetSwingLimitTorque( float flTimeStep ) const = 0;
};

abstract_class IFnPhysicsGenericJoint : public IFnPhysicsJoint
{
public:
	virtual void SetLinearMotion( JointAxis_t nAxis, JointMotion_t nMotion ) = 0;
	virtual JointMotion_t GetLinearMotion( JointAxis_t nAxis ) const = 0;
	virtual void SetLinearFrequency( JointAxis_t nAxis, float flFrequency ) = 0;
	virtual float GetLinearFrequency( JointAxis_t nAxis ) const = 0;
	virtual void SetLinearDampingRatio( JointAxis_t nAxis, float flDampingRatio ) = 0;
	virtual float GetLinearDampingRatio( JointAxis_t nAxis ) const = 0;
	virtual float GetLinearImpulse( JointAxis_t nAxis ) const = 0;

	virtual void SetAngularMotion( JointAxis_t nAxis, JointMotion_t nMotion ) = 0;
	virtual JointMotion_t GetAngularMotion( JointAxis_t nAxis ) const = 0;
	virtual void SetAngularFrequency( JointAxis_t nAxis, float flFrequency ) = 0;
	virtual float GetAngularFrequency( JointAxis_t nAxis ) const = 0;
	virtual void SetAngularDampingRatio( JointAxis_t nAxis, float flDampingRatio ) = 0;
	virtual float GetAngularDampingRatio( JointAxis_t nAxis ) const = 0;

	virtual void SetMaxLinearImpulse( JointAxis_t nAxis, float flMaxImpulse ) = 0;
	virtual float GetMaxLinearImpulse( JointAxis_t nAxis ) const = 0;
	virtual void SetMaxAngularImpulse( JointAxis_t nAxis, float flMaxImpulse ) = 0;
	virtual float GetMaxAngularImpulse( JointAxis_t nAxis ) const = 0;

	virtual float unk036() const = 0;
};

// The 32-byte result of IFnPhysicsSplineJoint::unk036 (16-byte vector at +16); only its size and alignment are known
struct FnPhysicsSplineJointUnk036_t
{
	ALIGN16 uint8 m_Data[ 32 ] ALIGN16_POST;
};

abstract_class IFnPhysicsSplineJoint : public IFnPhysicsJoint
{
public:
	// Anchor of the first body in the tangent space of the spline
	virtual Vector GetTangentSpaceAnchor() const = 0;
	virtual void SetTangentSpaceAnchor( const Vector &vAnchor ) = 0;
	virtual Vector ComputeTangentSpaceAnchor( const Vector &vWorldAnchor ) const = 0;

	virtual void SetMotorEnabled( bool bEnabled ) = 0;
	virtual bool IsMotorEnabled() const = 0;

	// A motor with zero target velocity; zero or less disables the motor
	virtual void SetFriction( float flFriction ) = 0;

	virtual void SetMotorTargetVelocity( float flVelocity ) = 0;
	virtual float GetMotorTargetVelocity() const = 0;
	virtual void SetMotorMaxForce( float flMaxForce ) = 0;
	virtual float GetMotorMaxForce() const = 0;

	// Motor impulse over the time step
	virtual float GetMotorForce( float flTimeStep ) const = 0;

	virtual void SetLateralConstraintEnabled( bool bEnabled ) = 0;
	virtual void SetVerticalConstraintEnabled( bool bEnabled ) = 0;

	virtual void SetLimitEnabled( bool bEnabled ) = 0;
	virtual bool IsLimitEnabled() const = 0;

	virtual float unk034() const = 0;
	virtual void unk035() = 0;

	virtual FnPhysicsSplineJointUnk036_t unk036( float flUnknown ) = 0;
};

abstract_class IFnPhysicsWheelJoint : public IFnPhysicsJoint
{
public:
	virtual void SetSuspensionFrequency( float flFrequency ) = 0;
	virtual float GetSuspensionFrequency() const = 0;
	virtual void SetSuspensionDampingRatio( float flDampingRatio ) = 0;
	virtual float GetSuspensionDampingRatio() const = 0;

	virtual void SetSuspensionLimitEnabled( bool bEnabled ) = 0;
	virtual bool IsSuspensionLimitEnabled() const = 0;

	// Minimum and maximum suspension offset
	virtual void SetSuspensionLimits( Vector2D vLimits ) = 0;
	virtual Vector2D GetSuspensionLimits() const = 0;
	virtual float GetSuspensionOffset() const = 0;

	virtual void SetSteeringMotorMode( PhysicsJointMotorMode_t nMode ) = 0;
	virtual PhysicsJointMotorMode_t GetSteeringMotorMode() const = 0;
	virtual void SetSteeringMotorFrequency( float flFrequency ) = 0;
	virtual float GetSteeringMotorFrequency() const = 0;
	virtual void SetSteeringMotorDampingRatio( float flDampingRatio ) = 0;
	virtual float GetSteeringMotorDampingRatio() const = 0;

	// In degrees and degrees per second
	virtual void SetSteeringMotorTargetAngle( float flAngle ) = 0;
	virtual float GetSteeringMotorTargetAngle() const = 0;
	virtual void SetSteeringMotorTargetVelocity( float flVelocity ) = 0;
	virtual float GetSteeringMotorTargetVelocity() const = 0;

	virtual void SetSteeringMotorMaxTorque( float flMaxTorque ) = 0;
	virtual float GetSteeringMotorMaxTorque() const = 0;

	// A velocity motor with zero target velocity; zero or less disables the motor
	virtual void SetSteeringFriction( float flFriction ) = 0;

	virtual void SetSteeringLimitEnabled( bool bEnabled ) = 0;
	virtual bool IsSteeringLimitEnabled() const = 0;

	// Minimum and maximum steering angle in degrees
	virtual void SetSteeringLimits( Vector2D vLimits ) = 0;
	virtual Vector2D GetSteeringLimits() const = 0;
	virtual float GetSteeringAngle() const = 0;

	virtual void SetSpinMotorEnabled( bool bEnabled ) = 0;
	virtual bool IsSpinMotorEnabled() const = 0;

	// In degrees per second
	virtual void SetSpinMotorTargetVelocity( float flVelocity ) = 0;
	virtual float GetSpinMotorTargetVelocity() const = 0;

	virtual void SetSpinMotorMaxTorque( float flMaxTorque ) = 0;
	virtual float GetSpinMotorMaxTorque() const = 0;

	// A spin motor with zero target velocity; zero or less disables the motor
	virtual void SetSpinFriction( float flFriction ) = 0;
};

#endif // FNPHYSICSJOINT_H
