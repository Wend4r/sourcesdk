//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: The base bot manager: runs every bot once a frame and tracks the grenades
//          the bots know about.
//
//=============================================================================//

#ifndef BOT_MANAGER_H
#define BOT_MANAGER_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "mathlib/vector.h"
#include "tier1/utllinkedlist.h"
#include "cs_bot_timers.h"

class CBaseEntity;
class CBaseGrenade;
class CBasePlayerController;
class CCommand;
class CCSPlayerPawn;

//-----------------------------------------------------------------------------
// A grenade in the world the bots are aware of. AddGrenade allocates one for every
// grenade type below 5 (HE, flashbang, molotov or incendiary, decoy, smoke). StartFrame
// first validates the list: a grenade with an entity follows its origin, one without is
// deleted unless it is smoke that has not reached m_dieTimestamp. With bot debugging on
// it then draws the smoke clouds from m_detonationPosition and m_radius.
//-----------------------------------------------------------------------------
class ActiveGrenade
{
public:
	bool IsSmoke() const { return m_isSmoke; }
	bool IsFlashbang() const { return m_isFlashbang; }
	bool IsMolotov() const { return m_isMolotov; }
	bool IsDecoy() const { return m_isDecoy; }

public:
	// Null once the grenade entity is gone.
	CBaseGrenade *m_entity;
	// The grenade's origin, kept while the entity lives.
	Vector m_detonationPosition;
	// When the smoke cloud goes away after the entity is gone.
	float m_dieTimestamp;
	bool m_isSmoke;
	bool m_isFlashbang;
	// Molotov and incendiary.
	bool m_isMolotov;
	bool m_isDecoy;
	// Cleared by the constructor and never set: no grenade type maps to it.
	bool m_isSensor;
	// 150 for smoke, 100 for the rest.
	float m_radius;
};

//-----------------------------------------------------------------------------
// The singleton that runs the bots. StartFrame gathers every pawn that owns a bot,
// then for each bot calls Upkeep, clears its input and calls Update when its think
// interval is up, and finally RunCommand.
//-----------------------------------------------------------------------------
abstract_class CBotManager
{
public:
	virtual ~CBotManager() = 0;

	// Never called, so the argument type is not confirmed.
	virtual void ClientDisconnect( CBaseEntity *pEntity ) = 0;
	// CCSGameRules::ClientCommand calls ServerCommand with the whole command string first,
	// then this one; true when the command was handled.
	virtual bool ClientCommand( CBasePlayerController *pController, const CCommand &args ) = 0;
	virtual void ServerActivate() = 0;
	virtual void ServerDeactivate() = 0;
	virtual bool ServerCommand( const char *pszCommand ) = 0;

	// Clears the debug message history.
	virtual void RestartRound() = 0;
	virtual void StartFrame() = 0;
	// 0 is the highest priority.
	virtual unsigned int GetPlayerPriority( CCSPlayerPawn *pPlayer ) const = 0;
	// Empty in the base, not overridden and never called, so nothing names it.
	virtual void Unk_9() = 0;

public:
	enum
	{
		MAX_DBG_MSG_SIZE = 1024,
		MAX_DBG_MSGS = 6
	};

	struct DebugMessage
	{
		char m_string[ MAX_DBG_MSG_SIZE ];
		IntervalTimer m_age;
	};

	CUtlLinkedList< ActiveGrenade * > m_activeGrenadeList;

	DebugMessage m_debugMessage[ MAX_DBG_MSGS ];
	int m_debugMessageCount;
	int m_currentDebugMessage;
	// Measures each frame's duration.
	IntervalTimer m_frameTimer;
};

#endif // BOT_MANAGER_H
