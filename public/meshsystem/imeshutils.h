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

abstract_class IMeshUtils : public IAppSystem
{
public:
	virtual void Unk_CreateSkeletonSceneObject( void *p ) = 0;
};

#endif // IMESHUTILS_H
