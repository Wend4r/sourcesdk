//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: Runtime form of the physics aggregate data of a resource (VPhysXAggregateData_t)
//
//===========================================================================//

#ifndef PHYSAGGREGATEDATA_H
#define PHYSAGGREGATEDATA_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "tier0/threadtools.h"
#include "tier0/utlleanvector.h"
#include "tier0/utlstring.h"
#include "tier0/utlvector.h"
#include "mathlib/mathlib.h"

class CFeModel;
class CPhysSurfaceProperties;
class CStrongHandleVoid;
struct PhysShapeMarkup_t;
struct RnCollisionAttr_t;
struct VPhysXAggregateData_t;
struct VPhysXBodyPart_t;
struct VPhysXConstraintParams_t;
struct VPhysXJoint_t;

// A VPhysXConstraint2_t of the disk data
struct CPhysConstraintData
{
	uint32 m_nFlags;
	uint16 m_nParent;
	uint16 m_nChild;
	const VPhysXConstraintParams_t *m_pParams;
};

class CPhysAggregateData
{
public:
	CInterlockedUInt m_nRefCounter;
	uint16 m_nFlags;

private:
	uint8 m_pad006[ 2 ];

public:
	// Index of this object in IVPhysics2::m_AggDataRegistry
	int m_nIncrementalVectorIndex;

	CUtlVectorUltraConservative< const char * > m_BoneNames;

	// The lean vectors point into the disk data
	CUtlLeanVector< uint32 > m_BonesHash;
	CUtlLeanVector< uint16 > m_IndexNames;
	CUtlLeanVector< uint16 > m_IndexHash;
	CUtlLeanVector< uint16 > m_BoneParents;
	CUtlLeanVector< matrix3x4a_t > m_BindPose;
	CUtlLeanVector< PhysShapeMarkup_t > m_ShapeMarkups;

	CUtlVectorConservative< const VPhysXBodyPart_t * > m_Parts;
	CUtlVectorUltraConservative< CPhysConstraintData > m_Constraints;
	CUtlVectorUltraConservative< const VPhysXJoint_t * > m_Joints;

	// Built from VPhysXAggregateData_t::m_pFeModel
	CFeModel *m_pFeModel;

	CUtlVectorUltraConservative< RnCollisionAttr_t > m_CollisionAttributes;

	// Looked up from VPhysXAggregateData_t::m_surfacePropertyHashes
	CUtlVectorUltraConservative< const CPhysSurfaceProperties * > m_SurfaceProperties;

	CUtlVectorUltraConservative< const CPhysAggregateData * > m_ParentResources;

	const CUtlString *m_pDebugPartNames;
	const char *m_pLegacyEmbeddedKeyvalues;
	CUtlVectorUltraConservative< CStrongHandleVoid > m_LegacyEmbeddedKeysAdditionalExtRefs;

	// Owned, deleted with this object
	VPhysXAggregateData_t *m_pBackingData;

#ifdef _WIN32
private:
	// Not touched by the constructor, the destructor or InitFromDiskData
	uint8 m_pad0D8[ 8 ];
#endif // _WIN32
};

#endif // PHYSAGGREGATEDATA_H
