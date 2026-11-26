//===== Copyright (c) 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: A resource type compiler that lives in its own module
//
//===========================================================================//

#ifndef IRESOURCETYPECOMPILERDYNAMICLOAD_H
#define IRESOURCETYPECOMPILERDYNAMICLOAD_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "appframework/iappsystem.h"

class IResourceTypeCompiler;

// The resource compiler's compiler table names the module and the interface ( "visbuilder",
// "MapVisibilityV001RESOURCE_TYPE_COMPILER_002" ); it adds the module as an app system on first use and
// compiles through the IResourceTypeCompiler it returns
abstract_class IResourceTypeCompilerDynamicLoad : public IAppSystem
{
public:
	// Named by what it does: the implementing object's IResourceTypeCompiler base
	virtual IResourceTypeCompiler *GetResourceTypeCompiler() = 0;

	virtual ~IResourceTypeCompilerDynamicLoad() {}
};

#endif // IRESOURCETYPECOMPILERDYNAMICLOAD_H
