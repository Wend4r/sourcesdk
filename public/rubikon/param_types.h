//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: Rubikon contact manifolds
//
//===========================================================================//

#ifndef RUBIKON_PARAM_TYPES_H
#define RUBIKON_PARAM_TYPES_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "mathlib/vector.h"

// A contact point; the points are relative to the bodies of shape A and shape B of the contact
struct ManifoldPoint_t
{
	Vector m_vLocalPointA;
	Vector m_vLocalPointB;

	// Scaled together with the impulses of the manifold
	float m_flImpulse;

	uint8 m_pad01C[ 12 ];
};

struct Manifold_t
{
	int m_nPointCount;
	uint8 m_pad004[ 12 ];

	// Points from shape A to shape B: the separation of a point is the distance from its point on shape A to its point on shape B
	// along the normal, negative while the shapes penetrate
	Vector m_vNormal;

	// Holds three more floats that are scaled together with the point impulses
	uint8 m_pad01C[ 36 ];

	ManifoldPoint_t m_Points[ 4 ];
};

// The space the animated ctrl transforms of a softbody are given in
enum PhysicsSoftbodyAnimSpace_t
{
	SOFTBODY_ANIM_SPACE_WORLD = 0,
	SOFTBODY_ANIM_SPACE_LOCAL,
};

enum PhysicsSoftbodyEnergyTypeEnum_t
{
	SOFTBODY_ENERGY_ELASTIC = 0,
	SOFTBODY_ENERGY_KINEMATIC,
	SOFTBODY_ENERGY_POTENTIAL,

	// The sum of the three
	SOFTBODY_ENERGY_TOTAL,

	SOFTBODY_ENERGY_COUNT
};

// Flags of IPhysicsSoftbody::GetParticleTransforms
enum PhysicsSoftbodySimTransformsFlags_t
{
	// Also writes the transforms of the static ctrls
	SOFTBODY_SIM_TRANSFORMS_INCLUDE_STATIC = ( 1 << 0 ),

	// Skips turning ctrls toward their paired ctrls; added while the simulation is disabled or locked to the transform
	SOFTBODY_SIM_TRANSFORMS_SKIP_CTRL_AIM = ( 1 << 1 ),
};

// IFnPhysicsSoftbody::SetDebugDrawTreeFlags
enum PhysicsSoftbodyDebugDrawFlags_t
{
	SOFTBODY_DEBUG_DRAW_TREE_BOTTOM_UP = ( 1 << 0 ),
	SOFTBODY_DEBUG_DRAW_TREE_TOP_DOWN = ( 1 << 1 ),
	SOFTBODY_DEBUG_DRAW_TREE_CLUSTERS = ( 1 << 2 ),
};

// What IPhysicsSoftbody::SetAbsTransform does with the ground
enum PhysicsSoftbodyGroundMode_t
{
	SOFTBODY_GROUND_NONE = 0,

	// Moves the ground point to the origin
	SOFTBODY_GROUND_ORIGIN,

	// Traces the ground below the origin
	SOFTBODY_GROUND_TRACE,
};

// IFnPhysicsSoftbody::SetSimulationMode
enum PhysicsSoftbodySimulationMode_t
{
	SOFTBODY_SIMULATION_MODE_SIMULATE = 0,

	// Holds the nodes at the animated pose
	SOFTBODY_SIMULATION_MODE_ANIM_POSE,

	// Skips the simulation and leaves the nodes as they are
	SOFTBODY_SIMULATION_MODE_HOLD,
};

// The blended cloth parameters of IPhysicsSoftbody::SetClothParam, by the names IFnPhysicsSoftbody::Describe prints for them.
// The fifth parameter (index 4) has no name there
enum PhysicsSoftbodyClothParam_t
{
	SOFTBODY_CLOTH_PARAM_PAS = 0, // 1 by default
	SOFTBODY_CLOTH_PARAM_PAB, // 0 by default; IPhysicsSoftbody::IsSimulating is false once it reaches 1 for the whole softbody
	SOFTBODY_CLOTH_PARAM_VAS, // 1 by default
	SOFTBODY_CLOTH_PARAM_VAB, // 0 by default

	SOFTBODY_CLOTH_PARAM_COUNT = 5
};

// The effects of a softbody model, by the classes that implement them
enum PhysicsSoftbodyEffectType_t
{
	SOFTBODY_EFFECT_WIND = 1, // CSoftbodyEffectWind
	SOFTBODY_EFFECT_PRESSURIZE, // CSoftbodyEffectPressurize
	SOFTBODY_EFFECT_STIFFEN, // CSoftbodyEffectStiffen
	SOFTBODY_EFFECT_GRAVITY, // CSoftbodyEffectGravity
	SOFTBODY_EFFECT_SKINNED_COLLIDER, // CSoftbodyEffectSkinnedCollider
	SOFTBODY_EFFECT_DAMPEN_VELOCITY, // CSoftbodyEffectDampenVelocity
	SOFTBODY_EFFECT_SPEEDUP, // CSoftbodyEffectSpeedup
	SOFTBODY_EFFECT_BLOB_ANCHOR, // CSoftbodyEffectBlobAnchor
};

#endif // RUBIKON_PARAM_TYPES_H
