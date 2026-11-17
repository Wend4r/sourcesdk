//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: Range of floats and a curve made of spline segments, as edited in
//          the tools and stored in KV3
//
//===========================================================================//

#ifndef PIECEWISECURVE_H
#define PIECEWISECURVE_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "tier0/utlvector.h"
#include "mathlib/vector2d.h"

struct Range_t
{
	float m_flMin;
	float m_flMax;
};

class CPiecewiseCurve
{
public:
	// One control point of the curve
	struct Point_t
	{
		float x;
		float y;
		float m_flSlopeIncoming;
		float m_flSlopeOutgoing;
	};

	// How the curve enters and leaves the control point with the same index
	struct TangentPair_t
	{
		int32 m_nIncomingTangent;
		int32 m_nOutgoingTangent;
	};

	CUtlVector< Point_t > m_spline;
	CUtlVector< TangentPair_t > m_tangents;
	Vector2D m_vDomainMins;
	Vector2D m_vDomainMaxs;
};

#endif // PIECEWISECURVE_H
