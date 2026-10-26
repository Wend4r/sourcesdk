//===== Copyright © Valve Corporation, All rights reserved. ======//
//
// Purpose: Serialized Rubikon collision geometry: spheres, capsules, convex hulls,
//          triangle meshes, compounds and their shape descriptions
//
//===========================================================================//

#ifndef PHYSICSLIB_SERIALIZE_H
#define PHYSICSLIB_SERIALIZE_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "tier0/utlleanvector.h"
#include "tier0/utlstring.h"
#include "tier0/utlvector.h"
#include "mathlib/aabb.h"
#include "mathlib/mathlib.h"
#include "mathlib/vector.h"

#include "schemacompiler/schemaon.h"

schema enum JointAxis_t
{
	JOINT_AXIS_X = 0,
	JOINT_AXIS_Y,
	JOINT_AXIS_Z,
	JOINT_AXIS_COUNT,
};

schema enum JointMotion_t
{
	JOINT_MOTION_FREE = 0,
	JOINT_MOTION_LOCKED,
	JOINT_MOTION_COUNT,
};

// Whether a dynamic body uses continuous contacts against other dynamic bodies (RnBodyDesc_t::m_nDynamicContinuousContactBehavior)
schema enum DynamicContinuousContactBehavior_t : uint8
{
	DYNAMIC_CONTINUOUS_ALLOW_IF_REQUESTED_BY_OTHER_BODY = 0,
	DYNAMIC_CONTINUOUS_ALWAYS,
	DYNAMIC_CONTINUOUS_NEVER,
};

// Registered as an atomic type, not as a schema class
struct RnSphere_t
{
	Vector m_vCenter;
	float m_flRadius;
};

schema struct RnCapsule_t
{
	TYPEMETA( MGetKV3ClassDefaults );

	Vector m_vCenter[ 2 ];
	float m_flRadius;
};

schema struct RnPlane_t
{
	TYPEMETA( MGetKV3ClassDefaults );

	Vector m_vNormal;
	float m_flOffset;
};

//-----------------------------------------------------------------------------
// Convex hull: half-edge structure with byte indices
//-----------------------------------------------------------------------------
schema struct RnVertex_t
{
	TYPEMETA( MGetKV3ClassDefaults );

	uint8 m_nEdge;
};

schema struct RnHalfEdge_t
{
	TYPEMETA( MGetKV3ClassDefaults );

	uint8 m_nNext;
	uint8 m_nTwin;
	uint8 m_nOrigin;
	uint8 m_nFace;
};

schema struct RnFace_t
{
	TYPEMETA( MGetKV3ClassDefaults );

	uint8 m_nEdge;
};

// Separating planes of the hull regions, used by large hulls
schema class CRegionSVM
{
public:
	TYPEMETA( MGetKV3ClassDefaults );

	CUtlVector< RnPlane_t > m_Planes;
	CUtlVector< uint32 > m_Nodes;
};

// RnHull_t::m_nFlags
#define RN_HULL_FLAGS_BOX ( ( 1 << 0 ) | ( 1 << 1 ) ) // Set by RnHullCreateBox; the GJK proxy and the hull pair tests take box paths
#define RN_HULL_FLAG_CYLINDER ( 1 << 2 ) // Set by RnHullCreateCylinder

schema struct RnHull_t
{
	TYPEMETA( MGetKV3ClassDefaults );

	Vector m_vCentroid;
	float m_flMaxAngularRadius;
	float m_flMinCentroidRadius;
	AABB_t m_Bounds;
	Vector m_vOrthographicAreas;
	matrix3x4_t m_MassProperties;
	float m_flVolume;
	float m_flSurfaceArea;
	CUtlVector< Vector > m_VertexPositions;
	CUtlVector< RnPlane_t > m_FacePlanes;
	uint32 m_nFlags;
	CRegionSVM *m_pRegionSVM;
	CUtlVector< RnVertex_t > m_Vertices;
	CUtlVector< RnHalfEdge_t > m_Edges;
	CUtlVector< RnFace_t > m_Faces;
};

//-----------------------------------------------------------------------------
// Triangle mesh with its bounding volume hierarchy
//-----------------------------------------------------------------------------
schema struct ALIGN16 RnNode_t
{
	TYPEMETA( MGetKV3ClassDefaults );

	Vector m_vMin;
	uint32 m_nChildren;
	Vector m_vMax;
	uint32 m_nTriangleOffset;
} ALIGN16_POST;

schema struct RnTriangle_t
{
	TYPEMETA( MGetKV3ClassDefaults );

	int32 m_nIndex[ 3 ];
};

schema struct RnWing_t
{
	TYPEMETA( MGetKV3ClassDefaults );

	int32 m_nIndex[ 3 ];
};

// RnMesh_t::m_nFlags
#define RN_MESH_FLAG_CLOSED ( 1 << 0 ) // The mesh has no open edges; the point inside tests need it
#define RN_MESH_FLAG_INVERTED ( 1 << 1 ) // The volume of the closed mesh is negative, which flips its inside tests
#define RN_MESH_FLAG_VERTEX_VELOCITIES ( 1 << 2 ) // Required by IFnPhysicsMeshShape::UpdateVertices with nMode 1
#define RN_MESH_FLAG_HIDDEN_TRIANGLES ( 1 << 3 ) // Set by RnMeshHideTriangles, which also clears RN_MESH_FLAG_CLOSED and RN_MESH_FLAG_INVERTED

// RnMesh_t::m_nDebugFlags
#define RN_MESH_DEBUG_FLAG_NOT_CLOSED ( 1 << 0 ) // Set by the mesh build instead of RN_MESH_FLAG_CLOSED

schema struct RnMesh_t
{
	TYPEMETA( MGetKV3ClassDefaults );

	Vector m_vMin;
	Vector m_vMax;
	CUtlVector< RnNode_t > m_Nodes;
	CUtlVectorSIMDPaddedVector m_Vertices;
	CUtlVector< RnTriangle_t > m_Triangles;
	CUtlVector< RnWing_t > m_Wings;
	CUtlVector< uint8 > m_TriangleEdgeFlags;
	CUtlVector< uint8 > m_Materials;
	Vector m_vOrthographicAreas;
	uint32 m_nFlags;
	uint32 m_nDebugFlags;
	float m_flSurfaceArea;
};

//-----------------------------------------------------------------------------
// Compound of spheres, capsules, hulls and meshes with a bounding tree
//-----------------------------------------------------------------------------
// RnCompoundTreeNode_t::m_nType of a leaf, whose m_nSubtreeEndOrCompoundId is the index of the compound child; the traversals
// treat the other types as inner nodes, whose m_nSubtreeEndOrCompoundId is the node count of their subtree
#define RN_COMPOUND_TREE_NODE_LEAF 3

schema struct RnCompoundTreeNode_t
{
	TYPEMETA( MGetKV3ClassDefaults );

	Vector m_vMin;
	Vector m_vMax;
	uint32 m_nType : 3;
	uint32 m_nSubtreeEndOrCompoundId : 29;
};

schema struct RnCompoundTree_t
{
	TYPEMETA( MGetKV3ClassDefaults );

	CUtlLeanVector< RnCompoundTreeNode_t > m_Nodes;
	uint32 m_nStartIterationIndex;
};

schema struct RnCompound_t
{
	TYPEMETA( MGetKV3ClassDefaults );

	RnCompoundTree_t m_Tree;
	int32 m_nHullBaseIndex;
	int32 m_nMeshBaseIndex;
	int32 m_nShapeCount;
	CUtlLeanVectorFixedGrowable< RnMesh_t, 1 > m_Meshes;
	CUtlLeanVector< RnHull_t > m_Hulls;
	CUtlLeanVector< RnCapsule_t > m_Capsules;
	CUtlLeanVector< RnSphere_t > m_Spheres;
	CUtlLeanVector< uint8 > m_CompoundMaterialIndices;
	AABB_t m_Bounds;
	Vector m_vOrthographicAreas;
	float m_flSurfaceArea;
	float m_flVolume;
};

//-----------------------------------------------------------------------------
// Shape descriptions of the collision resources
//-----------------------------------------------------------------------------
schema struct RnShapeDesc_t
{
	TYPEMETA( MGetKV3ClassDefaults );

	uint32 m_nCollisionAttributeIndex;
	uint32 m_nSurfacePropertyIndex;
	CUtlString m_UserFriendlyName;
	bool m_bUserFriendlyNameSealed;
	bool m_bUserFriendlyNameLong;
	uint32 m_nToolMaterialHash;
};

schema struct RnSphereDesc_t : public RnShapeDesc_t
{
	TYPEMETA( MGetKV3ClassDefaults );

	RnSphere_t m_Sphere;
};

schema struct RnCapsuleDesc_t : public RnShapeDesc_t
{
	TYPEMETA( MGetKV3ClassDefaults );

	RnCapsule_t m_Capsule;
};

schema struct RnHullDesc_t : public RnShapeDesc_t
{
	TYPEMETA( MGetKV3ClassDefaults );

	RnHull_t m_Hull;
};

schema struct RnMeshDesc_t : public RnShapeDesc_t
{
	TYPEMETA( MGetKV3ClassDefaults );

	RnMesh_t m_Mesh;
};

schema struct RnCompoundDesc_t : public RnShapeDesc_t
{
	TYPEMETA( MGetKV3ClassDefaults );

	RnCompound_t m_Compound;
};

//-----------------------------------------------------------------------------
// Softbody resource data
//-----------------------------------------------------------------------------
schema struct RnSoftbodyParticle_t
{
	TYPEMETA( MGetKV3ClassDefaults );

	float32 m_flMassInv;
};

schema struct RnSoftbodySpring_t
{
	TYPEMETA( MGetKV3ClassDefaults );

	uint16 m_nParticle[ 2 ];
	float32 m_flLength;
};

schema struct RnSoftbodyCapsule_t
{
	TYPEMETA( MGetKV3ClassDefaults );

	Vector m_vCenter[ 2 ];
	float32 m_flRadius;
	uint16 m_nParticle[ 2 ];
};

schema struct ALIGN16 RnBlendVertex_t
{
	TYPEMETA( MGetKV3ClassDefaults );

	uint16 m_nWeight0;
	uint16 m_nIndex0;
	uint16 m_nWeight1;
	uint16 m_nIndex1;
	uint16 m_nWeight2;
	uint16 m_nIndex2;
	uint16 m_nFlags;
	uint16 m_nTargetIndex;
} ALIGN16_POST;

#include "schemacompiler/schemaoff.h"

#endif // PHYSICSLIB_SERIALIZE_H
