//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Keeps the activity events that logic_activity_event entities fire
//          until they expire and passes each new one to the registered listeners.
//
//=============================================================================//

#ifndef ACTIVITYEVENTGAMESYSTEM_H
#define ACTIVITYEVENTGAMESYSTEM_H
#ifdef _WIN32
#pragma once
#endif

#include "igamesystem.h"
#include "entityhandle.h"

#include <mathlib/vector.h>
#include <tier0/utllinkedlist.h>
#include <tier0/utlsignalslot.h>
#include <tier0/utlvector.h>

//-----------------------------------------------------------------------------
// One fired event (24 bytes). No source names this struct or its members; they are
// named after the CLogicActivityEvent fields that fill them.
//-----------------------------------------------------------------------------
struct ActivityEvent_t
{
	// CLogicActivityEvent::m_nEventType
	int m_nEventType;
	// gpGlobals->curtime + CLogicActivityEvent::m_flDuration; ServerPostEntityThink drops the
	// event once curtime passes it
	float m_flExpireTime;
	// CLogicActivityEvent::m_hSource
	CEntityHandle m_hSource;
	// Named by what it holds: a position passed by value with the event; CLogicActivityEvent passes
	// the zero vector
	Vector m_vecPosition;
};

//-----------------------------------------------------------------------------
// Named by what it does: the element type of CActivityEventGameSystem::m_Listeners. Its single
// virtual method is called with every event the system adds.
//-----------------------------------------------------------------------------
abstract_class IActivityEventListener
{
public:
	virtual void OnActivityEvent( const ActivityEvent_t *pEvent ) = 0;
};

//-----------------------------------------------------------------------------
// The game system behind logic_activity_event (registered as "ActivityEventGameSystem",
// static factory). It adds no virtual methods to IGameSystem.
//-----------------------------------------------------------------------------
class CActivityEventGameSystem : public CAutoGameSystem, public CUtlSlot
{
public:
	DECLARE_GAME_SYSTEM();

	CActivityEventGameSystem();
	virtual ~CActivityEventGameSystem();

	// Returns true
	virtual bool Init() override;
	// Empty
	GS_EVENT( GameInit );
	// Removes the events whose m_flExpireTime is older than gpGlobals->curtime
	GS_EVENT( ServerPostEntityThink );

	// Forward to the game system factory
	virtual void SetGameSystemGlobalPtrs( void *pValue ) override;
	virtual bool DoesGameSystemReallocate() override;

	// Named by what it does: appends an event that expires flDuration seconds from now and passes it
	// to every listener (called by CLogicActivityEvent::InputFireEvent)
	void AddEvent( int nEventType, CEntityHandle hSource, float flDuration, Vector vecPosition );

public:
	// Named by what it holds: the events that did not expire yet
	CUtlLinkedList< ActivityEvent_t > m_Events;
	// Named by what it holds: called by AddEvent for every new event
	CUtlVector< IActivityEventListener * > m_Listeners;
};

#endif // ACTIVITYEVENTGAMESYSTEM_H
