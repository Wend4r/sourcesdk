//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Listeners the resource system tells about reloaded resources in tools
//			mode (IApplication::IsInToolsMode). Both are registered through
//			IResourceSystem (slot 56 the tools listener, slot 71 the pre-reload
//			listener; the sourcesdk names of those slots differ, see
//			iresourcesystem.h). The game's CBodyGameSystem and
//			CAnimGraphGameSystem derive from both.
//
//=============================================================================//

#ifndef ITOOLSRESOURCELISTENER_H
#define ITOOLSRESOURCELISTENER_H
#ifdef _WIN32
#pragma once
#endif

#include "resourcefile/resourcetype.h"

// Declared in resourcesystem/iresourcesystem.h; RESOURCE_LOAD_RELOAD (2) is the value the
// listeners test
enum ResourceLoadType_t : int;

//-----------------------------------------------------------------------------
// One resource of a status notification, 32 bytes
//-----------------------------------------------------------------------------
struct ResourceStatusChange_t
{
	ResourceId_t m_nResourceId;
	ResourceHandle_t m_hResource;
	ResourceType_t m_nResourceType;
	ResourceLoadType_t m_nLoadType;
};

//-----------------------------------------------------------------------------
// The class name comes from the RTTI of the game; the methods are named by what the resource
// system does with them. No virtual destructor (two slots on both platforms).
//-----------------------------------------------------------------------------
class IToolsResourceListener
{
public:
	// Named by what the resource system does with it: 1 puts the listener on the list that
	// is told at once, 2 on the list that is told in the next resource system update, 0 lets
	// a command line switch decide. Returns 0 here; no game class overrides it
	virtual int GetResourceNotificationMode() { return 0; }
	// Named by the log message of CBodyGameSystem ("CBodyGameSystem::NotifyResourceStatusChange[%s] ...").
	// Called with the resources whose status changed (nCount records)
	virtual void NotifyResourceStatusChange( const ResourceStatusChange_t *pChanges, int64 nCount ) = 0;
};

//-----------------------------------------------------------------------------
// The class name comes from the RTTI of the game; the method from the log message of
// CBodyGameSystem ("CBodyGameSystem::NotifyResourcePreReload[%s] ..."). Called before a
// resource is reloaded (the resource system passes the binding and the type). One slot, no
// virtual destructor.
//-----------------------------------------------------------------------------
class IToolsResourcePreReloadListener
{
public:
	virtual void NotifyResourcePreReload( ResourceHandle_t hResource, ResourceType_t nResourceType ) = 0;
};

#endif // ITOOLSRESOURCELISTENER_H
