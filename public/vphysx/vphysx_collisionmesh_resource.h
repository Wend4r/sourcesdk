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

#include "schemacompiler/schemaoff.h"

#endif // VPHYSX_COLLISIONMESH_RESOURCE_H
