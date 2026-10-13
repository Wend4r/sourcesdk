//===== Copyright © Valve Corporation, All rights reserved. ======//
//
// Purpose: Physics aggregate data of a collision mesh resource: body parts, their shapes,
//          constraints, joints and collision attributes
//
//===========================================================================//

#ifndef VPHYSX_COLLISIONMESH_RESOURCE_H
#define VPHYSX_COLLISIONMESH_RESOURCE_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "tier0/utlleanvector.h"
#include "tier0/utlstring.h"
#include "tier0/utlvector.h"
#include "mathlib/mathlib.h"
#include "mathlib/vector.h"
#include "physicslib/serialize.h"

#include "schemacompiler/schemaon.h"

schema struct VPhysics2ShapeDef_t
{
	TYPEMETA( MGetKV3ClassDefaults );

	CUtlLeanVector< RnSphereDesc_t > m_spheres;
	CUtlLeanVector< RnCapsuleDesc_t > m_capsules;
	CUtlLeanVector< RnHullDesc_t > m_hulls;
	CUtlLeanVector< RnMeshDesc_t > m_meshes;
	CUtlLeanVector< RnCompoundDesc_t > m_compounds;
	CUtlVector< uint16 > m_CollisionAttributeIndices;
};

schema struct VPhysXBodyPart_t
{
	TYPEMETA( MGetKV3ClassDefaults );

	enum VPhysXFlagEnum_t
	{
		FLAG_STATIC = 1,
		FLAG_KINEMATIC = 2,
		FLAG_JOINT = 4,
		FLAG_MASS = 8,
		FLAG_ALWAYS_DYNAMIC_ON_CLIENT = 16,
		FLAG_DISABLE_CCD = 32,
	};

	uint32 m_nFlags;
	float32 m_flMass;
	VPhysics2ShapeDef_t m_rnShape;
	uint16 m_nCollisionAttributeIndex;
	uint16 m_nReserved;
	float m_flInertiaScale;
	float m_flLinearDamping;
	float m_flAngularDamping;
	float m_flLinearDrag;
	float m_flAngularDrag;
	bool m_bOverrideMassCenter;
	Vector m_vMassCenterOverride;
};

#include "schemacompiler/schemaoff.h"

#endif // VPHYSX_COLLISIONMESH_RESOURCE_H
