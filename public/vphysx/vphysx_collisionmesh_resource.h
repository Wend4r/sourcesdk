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
#include "tier0/globalsymbol.h"
#include "tier0/utlleanvector.h"
#include "tier0/utlstring.h"
#include "tier0/utlvector.h"
#include "mathlib/mathlib.h"
#include "mathlib/vector.h"
#include "physicslib/serialize.h"

#include "schemacompiler/schemaon.h"

struct PhysShapeMarkup_t;
struct VPhysXConstraint2_t;
struct VPhysXJoint_t;
struct PhysFeModelDesc_t;

// Collision group and interaction layers that shapes of the aggregate data refer to by index
schema struct VPhysXCollisionAttributes_t
{
	TYPEMETA( MGetKV3ClassDefaults );

	int32 m_nIncludeDetailLayerCount;
	uint32 m_CollisionGroup;
	CUtlVector< uint32 > m_InteractAs;
	CUtlVector< uint32 > m_InteractWith;
	CUtlVector< uint32 > m_InteractExclude;
	CUtlVector< uint32 > m_DetailLayers;
	CUtlString m_CollisionGroupString;
	CUtlVector< CUtlString > m_InteractAsStrings;
	CUtlVector< CUtlString > m_InteractWithStrings;
	CUtlVector< CUtlString > m_InteractExcludeStrings;
	CUtlVector< CUtlString > m_DetailLayerStrings;
};

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
		FLAG_STATIC = ( 1 << 0 ),
		FLAG_KINEMATIC = ( 1 << 1 ),
		FLAG_JOINT = ( 1 << 2 ),
		FLAG_MASS = ( 1 << 3 ),
		FLAG_ALWAYS_DYNAMIC_ON_CLIENT = ( 1 << 4 ),
		FLAG_DISABLE_CCD = ( 1 << 5 ),
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

// Hit group of one shape of a body part
schema struct PhysShapeMarkup_t
{
	TYPEMETA( MGetKV3ClassDefaults );

	int32 m_nBodyInAggregate;
	int32 m_nShapeInBody;
	CGlobalSymbol m_sHitGroup;
};

schema struct VPhysXAggregateData_t
{
	TYPEMETA( MGetKV3ClassDefaults );

	enum VPhysXFlagEnum_t
	{
		FLAG_IS_POLYSOUP_GEOMETRY = ( 1 << 0 ),
		FLAG_LEVEL_COLLISION = ( 1 << 4 ),
		FLAG_IGNORE_SCALE_OBSOLETE_DO_NOT_USE = ( 1 << 5 ),
	};

	uint16 m_nFlags;
	uint16 m_nRefCounter;
	bool m_bCompoundsPacked;
	CUtlVector< uint32 > m_bonesHash;
	CUtlVector< CUtlString > m_boneNames;
	CUtlVector< uint16 > m_indexNames;
	CUtlVector< uint16 > m_indexHash;
	CUtlVector< matrix3x4a_t > m_bindPose;
	CUtlVector< VPhysXBodyPart_t > m_parts;
	CUtlVector< PhysShapeMarkup_t > m_shapeMarkups;
	CUtlVector< VPhysXConstraint2_t > m_constraints2;
	CUtlVector< VPhysXJoint_t > m_joints;
	PhysFeModelDesc_t *m_pFeModel;
	CUtlVector< uint16 > m_boneParents;
	CUtlVector< uint32 > m_surfacePropertyHashes;
	CUtlVector< VPhysXCollisionAttributes_t > m_collisionAttributes;
	CUtlVector< CUtlString > m_debugPartNames;
	CUtlString m_embeddedKeyvalues;
};

#include "schemacompiler/schemaoff.h"

#endif // VPHYSX_COLLISIONMESH_RESOURCE_H
