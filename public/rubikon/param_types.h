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

// A contact point; the points are relative to the bodies of shape 1 and shape 2 of the contact
struct ManifoldPoint_t
{
	Vector m_vLocalP1;
	Vector m_vLocalP2;

	// Scaled together with the impulses of the manifold
	float m_flImpulse;

	uint8 m_pad01C[ 12 ];
};

struct Manifold_t
{
	int m_nPointCount;
	uint8 m_pad004[ 12 ];

	// Points from shape 1 to shape 2: the separation of a point is the distance from its point on shape 1 to its point on shape 2
	// along the normal, negative while the shapes penetrate
	Vector m_vNormal;

	// Holds three more floats that are scaled together with the point impulses
	uint8 m_pad01C[ 36 ];

	ManifoldPoint_t m_Points[ 4 ];
};

#endif // RUBIKON_PARAM_TYPES_H
