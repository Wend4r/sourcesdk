//===== Copyright (c) 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: Navigation mesh generation app system
//
//===========================================================================//

#ifndef INAVSYSTEM_H
#define INAVSYSTEM_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "appframework/iappsystem.h"

class CNavGenInput;
class CNavGenOutput;
class IVPhysics2World;

abstract_class INavSystem : public IAppSystem
{
public:
	virtual void GenerateNavMesh( void *pUnused, const CNavGenInput &input, const IVPhysics2World *pWorld, const void *pHulls, bool bUnk1, bool bUnk2, bool bSkipSpaceBuild, CNavGenOutput *pOutput ) = 0;

	virtual void LoadNavSettingsFromGameInfo() = 0;
};

#endif // INAVSYSTEM_H
