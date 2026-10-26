//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: Mesh system utilities
//
//===========================================================================//

#ifndef IMESHUTILS_H
#define IMESHUTILS_H
#ifdef _WIN32
#pragma once
#endif

#include <appframework/iappsystem.h>
#include <mathlib/mathlib.h>
#include <tier3/tier3.h>

class CModelSkeleton;
class CSceneSkeletonObject;
class ISceneWorld;
struct VertexColor_t;

abstract_class IMeshUtils : public IAppSystem
{
public:
	// pColor is the color of every joint
	virtual CSceneSkeletonObject *CreateSkeletonSceneObject( ISceneWorld *pSceneWorld, uint64 nFlags, uint64 nTypeFlags, const matrix3x4_t &rootToWorld, const CModelSkeleton *pSkeleton, VertexColor_t *pColor ) = 0;
};

//-----------------------------------------------------------------------------
// Implementation of IMeshUtils, without members of its own
//-----------------------------------------------------------------------------
class CMeshUtils : public CTier3AppSystem< IMeshUtils >
{
};

#endif // IMESHUTILS_H
