//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Debug drawing of the animation code.
//
//=============================================================================//

#ifndef LABELLEDDEBUGHELPER_H
#define LABELLEDDEBUGHELPER_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"

class CTransform;
class CUtlString;
class VectorAligned;
class QuaternionAligned;
struct VertexColor_t;

//-----------------------------------------------------------------------------
// Debug drawing of the animation code (CLabelledDebugHelper draws it as labelled overlays). The server library
// of Windows has no implementation.
//-----------------------------------------------------------------------------
abstract_class IAnimationDebugRenderer
{
public:
	virtual ~IAnimationDebugRenderer() {}
	virtual void Line( const VectorAligned &vStart, const VectorAligned &vEnd, const VertexColor_t &color, const CUtlString &sLabel ) = 0;
	virtual void Sphere( const VectorAligned &vCenter, float flRadius, const VertexColor_t &color, const CUtlString &sLabel ) = 0;
	virtual void Axis( const CTransform &xf, float flScale, const CUtlString &sLabel ) = 0;
	virtual void Box( const VectorAligned &vCenter, const QuaternionAligned &qOrientation, const VectorAligned &vExtents, const VertexColor_t &color, const CUtlString &sLabel ) = 0;
	virtual void Pie( const VectorAligned &vCenter, const VectorAligned &vStart, const VectorAligned &vEnd, float flRadius, float flAngle, const VertexColor_t &color, const CUtlString &sLabel ) = 0;
	virtual void PushTransform( const CTransform &xf ) = 0;
	virtual void PopTransform() = 0;
};

#endif // LABELLEDDEBUGHELPER_H
