//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: Parameters of the physics debug meshes and scene objects
//
//===========================================================================//

#ifndef VPHYSX_INTERFACE_PARAMS_H
#define VPHYSX_INTERFACE_PARAMS_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "tier0/smartptr.h"
#include "mathlib/vector.h"
#include "mathlib/vertexcolor.h"

class CModelSkeleton;
class CPhysAggregateData;
class CTransform;

struct DebugMeshParams_t
{
	const CModelSkeleton *m_pMasterSkeleton;
	CSmartPtr< CPhysAggregateData, CRefCountAccessor > m_pCollisionMesh;
	float m_flConstraintScale;
	int m_nSelectedConstraint;
	int m_nSelectedPart;
	uint8 m_nConstraintAlpha;
	uint8 m_nSelectedConstraintAlpha;
	ALIGN4 VertexColor_t m_vColorCollision ALIGN4_POST;
	VertexColor_t m_vColorSelectedCollision;
	VertexColor_t m_vColorStaticCollision;
	uint m_nFlags;
};

struct DebugSceneObjectParams_t : public DebugMeshParams_t
{
	Vector m_vecSortDir;
	uint m_nSortPoseCount;
	CTransform *m_pSortPose;
	Vector *m_pScaleVector;
};

#endif // VPHYSX_INTERFACE_PARAMS_H
