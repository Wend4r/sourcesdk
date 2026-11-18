//===== Copyright (c) 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: Factory for the map compile builders (physics, visibility, baked LOD)
//
//===========================================================================//

#ifndef IMAPBUILDERMGR_H
#define IMAPBUILDERMGR_H
#ifdef _WIN32
#pragma once
#endif

#include "appframework/iappsystem.h"

class IMapBuilder;

abstract_class IMapBuilderMgr : public IAppSystem
{
public:
	// Allocates a new builder; release it with DestroyMapBuilder
	virtual IMapBuilder *CreateMapBuilder() = 0;

	virtual void DestroyMapBuilder( IMapBuilder *pBuilder ) = 0;
};

#endif // IMAPBUILDERMGR_H
