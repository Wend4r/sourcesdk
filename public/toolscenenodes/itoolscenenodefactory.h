//===== Copyright (c) 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: Factory of the tool scene nodes that render a map document in the tools
//
//===========================================================================//

#ifndef ITOOLSCENENODEFACTORY_H
#define ITOOLSCENENODEFACTORY_H
#ifdef _WIN32
#pragma once
#endif

#include "appframework/iappsystem.h"

class IToolSceneVmapNode;

abstract_class IToolSceneNodeFactory : public IAppSystem
{
public:
	// Named by what it does: allocates a CToolSceneVmap ( a tool scene node that draws a map document through
	// two map views ); both arguments go to the tool scene node constructor
	virtual IToolSceneVmapNode *CreateVmapNode( void *pOwner, void *pContext ) = 0;
	virtual void DestroyVmapNode( IToolSceneVmapNode *pNode ) = 0;
};

#endif // ITOOLSCENENODEFACTORY_H
