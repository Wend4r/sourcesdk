//===== Copyright © Valve Corporation, All rights reserved. ======//
//
// Purpose: Rubikon physics parameter types shared by shapes, joints and queries
//
//===========================================================================//

#ifndef PHYSICSLIB_PARAM_TYPES_H
#define PHYSICSLIB_PARAM_TYPES_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "tier0/utlstringtoken.h"
#include "mathlib/vector.h"

class CPhysSurfaceProperties;

enum PhysicsBodyType_t
{
	BODY_STATIC = 0,
	BODY_KEYFRAMED,
	BODY_DYNAMIC,

	BODY_TYPE_COUNT
};

enum PhysicsShapeType_t
{
	SHAPE_SPHERE = 0,
	SHAPE_CAPSULE,
	SHAPE_HULL,
	SHAPE_MESH,
	SHAPE_COMPOUND,
};

enum PhysicsJointType_t
{
	NULL_JOINT = 0,
	SPHERICAL_JOINT,
	PRISMATIC_JOINT,
	REVOLUTE_JOINT,
	QUAT_ORTHOTWIST_JOINT,
	MOUSE_JOINT,
	WELD_JOINT,
	SPRING,
	PULLEY,
	GEAR,
	CONICAL_JOINT,
	PRISMATIC_WELD_FUSED_JOINT,
	REVOLUTE_WELD_FUSED_JOINT,
	GENERIC_JOINT,
	BEVEL_GEAR_FAST_JOINT,
	SPLINE_JOINT,
	WHEEL_JOINT,
};

enum PhysicsJointMotorMode_t
{
	JOINT_MOTOR_POSITION_MODE = 0,
	JOINT_MOTOR_VELOCITY_MODE,
	JOINT_MOTOR_POSITION_AND_VELOCITY_MODE,
	JOINT_MOTOR_DISABLED,
};

// Material of a shape, filled from the physics and vehicle values of a surface property
struct RnMaterial_t
{
	// Surface property density converted to mass per cubic inch
	float m_flDensity;
	float m_flFriction;
	float m_flRestitution;
	float m_flThickness;
	float m_flSoftContactFrequency;
	float m_flSoftContactDampingRatio;
	float m_flWheelDrag;
	float m_flWheelFrictionScale;
	// Cleared with the default material, not written from a surface property
	uint32 m_nUnk20;
	CUtlStringToken m_SurfacePropertyToken;
	CPhysSurfaceProperties *m_pUserData;
};

// Result of casting a ray or a swept sphere against a single shape
struct CShapeCastResult
{
	float m_flHitTime;
	Vector m_vHitPoint;
	Vector m_vHitNormal;

	// The child of a compound shape that was hit and the triangle of a mesh shape, -1 otherwise
	int m_nChild;
	int m_nTriangle;
	const RnMaterial_t *m_pMaterial;
	bool m_bStartInSolid;
};

#endif // PHYSICSLIB_PARAM_TYPES_H
