//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: Results of physics world traces and queries
//
//===========================================================================//

#ifndef VPHYSICS2_HANDLE_DECL_H
#define VPHYSICS2_HANDLE_DECL_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "mathlib/vector.h"
#include "tier0/utlvector.h"

class IPhysicsBody;
class IPhysicsShape;
class CPhysSurfaceProperties;

// Result of a single trace against a physics world
struct PhysicsTrace_t
{
	PhysicsTrace_t()
	{
		m_HitObject = nullptr;
		m_HitShape = nullptr;
		m_vHitPoint.Init();
		m_vHitNormal.Init();
		m_pSurfaceProperties = nullptr;
		m_flHitOffset = 0.0f;
		m_flFraction = 1.0f;
		m_nHitChild = -1;
		m_nHitTriangle = -1;
		m_bStartInSolid = false;
	}

	IPhysicsBody *m_HitObject;
	IPhysicsShape *m_HitShape;
	Vector m_vHitPoint;
	Vector m_vHitNormal;

	// The surface properties of the material that was hit
	const CPhysSurfaceProperties *m_pSurfaceProperties;

	// Distance of the hit plane from the origin, the dot product of m_vHitPoint and m_vHitNormal
	float m_flHitOffset;

	float m_flFraction;

	// The sub-shape that was hit: the child of a compound shape and the triangle of a mesh shape, -1 otherwise
	int m_nHitChild;
	int m_nHitTriangle;

	bool m_bStartInSolid;
};

// Results of a trace that collects every hit
typedef CUtlVectorFixedGrowable< PhysicsTrace_t, 128 > CTraceVector;

// Results of a shape query against a physics world
typedef CUtlVectorFixedGrowable< IPhysicsShape *, 128 > CQueryResult;

#endif // VPHYSICS2_HANDLE_DECL_H
