//===== Copyright (c) 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: Server tools info app system
//
//===========================================================================//

#ifndef ISERVERTOOLSINFO_H
#define ISERVERTOOLSINFO_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "appframework/iappsystem.h"

abstract_class IServerToolsInfo : public IAppSystem
{
public:
	virtual ~IServerToolsInfo() {}
};

#endif // ISERVERTOOLSINFO_H
