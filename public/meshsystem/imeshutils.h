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

class CModelSkeleton;
class CSceneObject;
class ISceneWorld;
struct VertexColor_t;

abstract_class IMeshUtils : public IAppSystem
{
public:
	// s&box DWARF / Dota 2 (2018) name. CS2 passes the scene object type flags as a separate
	// argument (they go to ISceneSystem::CreateSceneObject) and has no joint filter
	virtual CSceneObject *CreateSkeletonSceneObject( ISceneWorld *pWorld, uint64 nFlags, uint64 nTypeFlags, const matrix3x4_t &transform, const CModelSkeleton *pSkeleton, const VertexColor_t *pBoneColors ) = 0;
};

#endif // IMESHUTILS_H
