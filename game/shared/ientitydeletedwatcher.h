//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Notification of a deleted entity.
//
//=============================================================================//

#ifndef IENTITYDELETEDWATCHER_H
#define IENTITYDELETEDWATCHER_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"

class CEntityInstance;

//-----------------------------------------------------------------------------
// Told when a watched entity is deleted (CTriggerAsyncQueryComponent).
//-----------------------------------------------------------------------------
abstract_class IEntityDeletedWatcher
{
public:
	virtual ~IEntityDeletedWatcher() {}
	virtual void OnEntityDeleted( CEntityInstance *pEntity ) = 0;
};

#endif // IENTITYDELETEDWATCHER_H
