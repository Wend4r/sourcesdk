//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: Reverse lookup of registered string tokens
//
//===========================================================================//

#ifndef TIER0_IUTLSTRINGTOKENSYSTEM_H
#define TIER0_IUTLSTRINGTOKENSYSTEM_H
#ifdef _WIN32
#pragma once
#endif

#include "appframework/iappsystem.h"
#include "tier0/utlstringtoken.h"

abstract_class IUtlStringTokenSystem : public IAppSystem
{
public:
	virtual const char *GetStringForToken( CUtlStringToken token ) = 0;
};

#endif // TIER0_IUTLSTRINGTOKENSYSTEM_H
