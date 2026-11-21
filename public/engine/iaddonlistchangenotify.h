//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Notification about a change of the mounted addon list
//          (IEngineService::InstallAddonListChangeNotify).
//
//=============================================================================//

#ifndef IADDONLISTCHANGENOTIFY_H
#define IADDONLISTCHANGENOTIFY_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"

// No virtual destructor.
abstract_class IAddonListChangeNotify
{
public:
	virtual void OnAddonListChanged() = 0;
};

#endif // IADDONLISTCHANGENOTIFY_H
