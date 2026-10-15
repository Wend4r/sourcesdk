//===== Copyright © Valve Corporation, All rights reserved. ======//
//
// Purpose: Descriptions of the joints created by the physics world and returned
//          by IPhysicsJoint
//
//===========================================================================//

#ifndef IPHYSICSJOINTDESC_H
#define IPHYSICSJOINTDESC_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "tier0/utldelegateimpl.h"
#include "mathlib/vector.h"
#include "physicslib/param_types.h"
#include "physicslib/serialize.h"

class IPhysicsBody;
class IPhysicsJoint;

// The derived descriptions start in the tail padding of the base on Itanium, as in the engine
struct PhysicsJointDesc_t
{
	PhysicsJointDesc_t() {}

	PhysicsJointType_t m_nType;

	// The reference body, the space of the reference origin and basis, and the attached body, the space of the attached origin and basis
	IPhysicsBody *m_pReferenceBody;
	IPhysicsBody *m_pAttachedBody;
	Vector m_vLocalReferenceOrigin;
	Quaternion m_qLocalReferenceBasis;
	Vector m_vLocalAttachedOrigin;
	Quaternion m_qLocalAttachedBasis;
	bool m_bIsEnabled;
	bool m_bIsLinearConstraintEnabled;
	bool m_bIsAngularConstraintEnabled;
	bool m_bEnableCollision;
	bool m_bUnk54;

	float m_flFriction;
	float m_flElasticity;
	float m_flPlasticity;

	// Zero disables breaking
	float m_flMaxLinearConstraintImpulse;
	float m_flMaxAngularConstraintImpulse;
	CUtlDelegate< void ( IPhysicsJoint * ) > m_BreakDelegate;

	const char *m_Tag;

	bool m_bUnk90;
};

struct PhysicsSphericalJointDesc_t : public PhysicsJointDesc_t
{
	PhysicsJointMotorMode_t m_nMotorMode;
	Quaternion m_qMotorTargetOrientation;
	float m_flMotorFrequency;
	float m_flMotorDampingRatio;
	Vector m_vMotorTargetVelocity;
	float m_flMotorMaxTorque;
};

struct PhysicsPrismaticJointDesc_t : public PhysicsJointDesc_t
{
	bool m_bUseBlockSolver;
	PhysicsJointMotorMode_t m_nMotorMode;
	float m_flMotorTargetOffset;
	float m_flMotorFrequency;
	float m_flMotorDampingRatio;
	float m_flMotorTargetVelocity;
	float m_flMotorMaxForce;
	bool m_bEnableLimit;
	float m_flMinOffset;
	float m_flMaxOffset;
};

struct PhysicsRevoluteJointDesc_t : public PhysicsJointDesc_t
{
	bool m_bUseBlockSolver;
	PhysicsJointMotorMode_t m_nMotorMode;

	float m_flMotorTargetAngle;
	float m_flMotorFrequency;
	float m_flMotorDampingRatio;
	float m_flMotorTargetVelocity;
	float m_flMotorMaxTorque;

	bool m_bEnableLimit;
	float m_flMinAngle;
	float m_flMaxAngle;
};

struct PhysicsWeldJointDesc_t : public PhysicsJointDesc_t
{
	bool m_bUseBlockSolver;
	float m_flLinearFrequency;
	float m_flLinearDampingRatio;
	float m_flMaxForce;
	float m_flAngularFrequency;
	float m_flAngularDampingRatio;
	float m_flMaxTorque;
};

struct PhysicsSpringJointDesc_t : public PhysicsJointDesc_t
{
	float m_flFrequency;
	float m_flDampingRatio;
	float m_flReferenceMass;
	float m_flMinRestLength;
	float m_flMaxRestLength;
};

struct PhysicsPulleyJointDesc_t : public PhysicsJointDesc_t
{
	Vector m_vReferenceGround;
	Vector m_vAttachedGround;
	float m_flReferenceRestLength;
	float m_flAttachedRestLength;
	float m_flRatio;
	float m_flFrequency;
	float m_flDampingRatio;
	bool m_bIsRigid;
};

struct PhysicsConicalJointDesc_t : public PhysicsJointDesc_t
{
	PhysicsJointMotorMode_t m_nMotorMode;
	Quaternion m_qMotorTargetOrientation;
	float m_flMotorFrequency;
	float m_flMotorDampingRatio;
	Vector m_vMotorTargetVelocity;
	float m_flMotorMaxTorque;

	bool m_bEnableSwingLimit;
	float m_flSwingLimit;
	bool m_bEnableTwistLimit;
	float m_flMinTwistAngle;
	float m_flMaxTwistAngle;
};

struct PhysicsGenericJointDesc_t : public PhysicsJointDesc_t
{
	JointMotion_t m_nLinearMotions[ JOINT_AXIS_COUNT ];
	float m_flLinearFrequencies[ JOINT_AXIS_COUNT ];
	float m_flLinearDampingRatios[ JOINT_AXIS_COUNT ];
	float m_flMaxLinearImpulses[ JOINT_AXIS_COUNT ];
	JointMotion_t m_nAngularMotions[ JOINT_AXIS_COUNT ];
	float m_flAngularFrequencies[ JOINT_AXIS_COUNT ];
	float m_flAngularDampingRatios[ JOINT_AXIS_COUNT ];
	float m_flMaxAngularImpulses[ JOINT_AXIS_COUNT ];
};

struct PhysicsWheelJointDesc_t : public PhysicsJointDesc_t
{
	float m_flSuspensionFrequency;
	float m_flSuspensionDampingRatio;
	float m_flSuspensionRestLoad;
	float m_flSuspensionHeightOffset;
	bool m_bEnableSuspensionLimit;
	float m_flMinSuspensionOffset;
	float m_flMaxSuspensionOffset;

	PhysicsJointMotorMode_t m_nSteeringMotorMode;
	float m_flSteeringMotorFrequency;
	float m_flSteeringMotorDampingRatio;
	float m_flSteeringMotorTargetAngle;
	float m_flSteeringMotorTargetVelocity;
	float m_flSteeringMotorMaxTorque;
	float m_flSteeringMotorFriction;
	bool m_bEnableSteeringLimit;
	float m_flMinSteeringAngle;
	float m_flMaxSteeringAngle;

	bool m_bEnableSpinMotor;
	float m_flSpinMotorTargetVelocity;
	float m_flSpinMotorMaxTorque;
};

#endif // IPHYSICSJOINTDESC_H
