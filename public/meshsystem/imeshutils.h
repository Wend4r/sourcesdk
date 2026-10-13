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
#include <tier3/tier3.h>

abstract_class IMeshUtils : public IAppSystem
{
public:
	virtual void CreateSkeletonSceneObject( void *p ) = 0;
};

//-----------------------------------------------------------------------------
// Implementation of IMeshUtils, without members of its own
//-----------------------------------------------------------------------------
class CMeshUtils : public CTier3AppSystem< IMeshUtils >
{
};

#endif // IMESHUTILS_H
