//===== Copyright (c) 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: A VConsole2 subtool that lives in its own module ( bin/subtools/*_subtool )
//
//===========================================================================//

#ifndef IEXTERNALVCONSOLESUBTOOL_H
#define IEXTERNALVCONSOLESUBTOOL_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "appframework/iappsystem.h"
#include "tier3/tier3.h"
#include "vconsole2/ivconsolesubtool.h"

// A subtool module registers its tool under VCONSOLE_SUBTOOL_INTERFACE_VERSION_PREFIX followed by the tool's
// name from tools/vconsole_subtools.txt ( "VConsole_SubTool_001_ConvarHelperTool" ). VConsole2 adds the
// module as an app system and uses the IVConsoleSubTool base
abstract_class IExternalSubTool : public IAppSystem, public IVConsoleSubTool
{
};

abstract_class IExternalVConsoleSubTool : public CTier3AppSystem< IExternalSubTool >
{
};

#endif // IEXTERNALVCONSOLESUBTOOL_H
