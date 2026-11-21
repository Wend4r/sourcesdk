//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Resolves the path of a workshop (UGC) addon by its name
//          (IAppSystem::SetUGCAddonPathResolver).
//
//=============================================================================//

#ifndef IUGCADDONPATHRESOLVER_H
#define IUGCADDONPATHRESOLVER_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"

class CBufferString;

// The path is written into a CBufferString, which the implementation
// (CDedicatedServerWorkshopManager) empties first; the addon name is a published file ID. No virtual
// destructor.
abstract_class IUGCAddonPathResolver
{
public:
	virtual bool GetFullAddonPathFromAddonName( const char *pszAddonName, CBufferString *pOutPath ) const = 0;
};

#endif // IUGCADDONPATHRESOLVER_H
