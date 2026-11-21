//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The entity notification system: entities watching other entities.
//			Factory "NotifyManager".
//
//=============================================================================//

#ifndef NOTIFYMANAGER_H
#define NOTIFYMANAGER_H
#ifdef _WIN32
#pragma once
#endif

#include "igamesystem.h"
#include "entity2/ientitylistener.h"
#include "tier0/utlvector.h"

class CEntityInstance;
struct notify_teleport_params_t;
struct notify_destroy_params_t;
struct notify_death_params_t;
struct notify_enemy_params_t;
struct notify_child_created_params_t;

enum notify_system_event_t
{
	NOTIFY_EVENT_TELEPORT = 0,
	NOTIFY_EVENT_DEATH,
	NOTIFY_EVENT_DESTROY,
	NOTIFY_EVENT_ENEMY_SEEN,
	NOTIFY_EVENT_ENEMY_UNSEEN,
	NOTIFY_EVENT_CHILD_NPC_CREATED,
};

// The parameters of the event, one pointer
struct notify_system_event_params_t
{
	union
	{
		const notify_teleport_params_t *pTeleport;
		const notify_destroy_params_t *pDestroy;
		const notify_death_params_t *pDeath;
		const notify_enemy_params_t *pEnemy;
		const notify_child_created_params_t *pChild;
	};
};

// pNotify gets the events of pWatched
struct entitynotify_t
{
	CEntityInstance *pNotify;
	CEntityInstance *pWatched;
};

abstract_class INotify
{
public:
	// Add notification for an entity
	virtual void AddEntity( CEntityInstance *pNotify, CEntityInstance *pWatched ) = 0;

	// Remove notification for an entity
	virtual void RemoveEntity( CEntityInstance *pNotify, CEntityInstance *pWatched ) = 0;

	// Call the named input in each entity who is watching pEvent's status
	virtual void ReportNamedEvent( CEntityInstance *pEntity, const char *pEventName ) = 0;

	// System events don't make sense as inputs, so are handled through a generic notify function
	virtual void ReportSystemEvent( CEntityInstance *pEntity, notify_system_event_t eventType, const notify_system_event_params_t &params ) = 0;

	// Remove this entity from the notify list
	virtual void ClearEntity( CEntityInstance *pNotify ) = 0;
};

//-----------------------------------------------------------------------------
// Reallocating game system, 64 bytes on both platforms. libserver.so repeats the
// seven overrides of the two interfaces at the end of the main table (slots
// 66..72) in the order of the declarations below.
//-----------------------------------------------------------------------------
class CNotifyManager : public CAutoGameSystem, public INotify, public IEntityListener
{
public:
	DECLARE_GAME_SYSTEM();

	// Adds this system as a listener of the entity system
	GS_EVENT( GameActivate );
	// Removes it and purges m_notifyList
	GS_EVENT( GameDeactivate );

	// Forward to the game system factory
	virtual void SetGameSystemGlobalPtrs( void *pValue ) override;
	virtual bool DoesGameSystemReallocate() override;

	// INotify
	virtual void AddEntity( CEntityInstance *pNotify, CEntityInstance *pWatched ) override;
	virtual void RemoveEntity( CEntityInstance *pNotify, CEntityInstance *pWatched ) override;
	virtual void ReportNamedEvent( CEntityInstance *pEntity, const char *pEventName ) override;
	virtual void ClearEntity( CEntityInstance *pNotify ) override;
	virtual void ReportSystemEvent( CEntityInstance *pEntity, notify_system_event_t eventType, const notify_system_event_params_t &params ) override;

	// IEntityListener
	virtual void OnEntityCreated( CEntityInstance *pEntity ) override;
	virtual void OnEntityDeleted( CEntityInstance *pEntity ) override;

public:
	CUtlVector< entitynotify_t > m_notifyList;
};

#endif // NOTIFYMANAGER_H
