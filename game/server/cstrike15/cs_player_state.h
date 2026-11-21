//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The states of a Counter-Strike player pawn
//
//=============================================================================//

#ifndef CSTRIKE15_CS_PLAYER_STATE_H
#define CSTRIKE15_CS_PLAYER_STATE_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "../../shared/cstrike15/cs_shareddefs.h"

class CCSPlayerPawnBase;
class CCSPlayerPawn;
class CCSObserverPawn;

//-----------------------------------------------------------------------------
// One state of a pawn: the pawn embeds an object of each state it knows, and
// CCSPlayerPawnBase::State_LookupInfo returns the one for a CSPlayerState (null for the others).
// State_Enter sets m_iPlayerState and the current state of the pawn and calls Enter;
// State_Transition calls Leave of the current state first; the pawn calls PreThink of its
// current state every think. No destructor in the table: 3 slots on both platforms. 32 bytes.
//
// CCSPlayerPawnBase holds Welcome, ObserverMode, GungameRespawn, DeathWaitForKey and Dormant,
// CCSPlayerPawn adds Active and DeathAnim (CCSPlayerPawn::State_LookupInfo answers them first),
// CCSObserverPawn adds PickingTeam.
//-----------------------------------------------------------------------------
class CCSPlayerState
{
public:
	// Empty here. Slot 0
	virtual void Enter();
	// Empty here. Slot 1
	virtual void Leave();
	// Empty here. Slot 2
	virtual void PreThink();

public:
	// The state this object implements
	CSPlayerState m_iPlayerState;
	// "STATE_ACTIVE", "STATE_WELCOME", ...: printed by cs_ShowStateTransitions
	const char *m_pStateName;
	// The pawn that owns the state. Named by what it holds
	CCSPlayerPawnBase *m_pPawn;
};

//-----------------------------------------------------------------------------
// STATE_WELCOME, a member of CCSPlayerPawnBase. Same size as CCSPlayerState.
//-----------------------------------------------------------------------------
class CCSPlayerStateWelcome : public CCSPlayerState
{
public:
	void Enter() override;
	// Empty
	void PreThink() override;
};

//-----------------------------------------------------------------------------
// STATE_OBSERVER_MODE, a member of CCSPlayerPawnBase. Same size as CCSPlayerState.
//-----------------------------------------------------------------------------
class CCSPlayerStateObserverMode : public CCSPlayerState
{
public:
	void Enter() override;
	// Empty
	void Leave() override;
	// Empty
	void PreThink() override;
};

//-----------------------------------------------------------------------------
// STATE_GUNGAME_RESPAWN, a member of CCSPlayerPawnBase. Same size as CCSPlayerState.
//-----------------------------------------------------------------------------
class CCSPlayerStateGungameRespawn : public CCSPlayerState
{
public:
	void Enter() override;
	// Respawns the pawn into STATE_ACTIVE, a limited number of pawns per frame
	void PreThink() override;
};

//-----------------------------------------------------------------------------
// STATE_DEATH_WAIT_FOR_KEY, a member of CCSPlayerPawnBase. Same size as CCSPlayerState.
//-----------------------------------------------------------------------------
class CCSPlayerStateDeathWaitForKey : public CCSPlayerState
{
public:
	void Enter() override;
	// Moves the dead player on to observing once the death delay is over
	void PreThink() override;
};

//-----------------------------------------------------------------------------
// STATE_DORMANT, a member of CCSPlayerPawnBase: overrides nothing. Same size as CCSPlayerState.
//-----------------------------------------------------------------------------
class CCSPlayerStateDormant : public CCSPlayerState
{
};

//-----------------------------------------------------------------------------
// STATE_ACTIVE, a member of CCSPlayerPawn. 40 bytes.
//-----------------------------------------------------------------------------
class CCSPlayerPawnStateActive : public CCSPlayerState
{
public:
	void Enter() override;
	// Turns the touch events of the pawn off
	void Leave() override;
	void PreThink() override;

public:
	// The same pawn as m_pPawn. Named by what it holds
	CCSPlayerPawn *m_pPlayerPawn;
};

//-----------------------------------------------------------------------------
// STATE_DEATH_ANIM, a member of CCSPlayerPawn. 40 bytes.
//-----------------------------------------------------------------------------
class CCSPlayerStateDeathAnim : public CCSPlayerState
{
public:
	void Enter() override;
	void PreThink() override;

public:
	// The same pawn as m_pPawn. Named by what it holds
	CCSPlayerPawn *m_pPlayerPawn;
};

//-----------------------------------------------------------------------------
// STATE_PICKINGTEAM, a member of CCSObserverPawn (CCSObserverPawn::State_LookupInfo answers it
// and asks the base for the others). 40 bytes.
//-----------------------------------------------------------------------------
class CCSObserverStatePickingTeam : public CCSPlayerState
{
public:
	void PreThink() override;

public:
	// The same pawn as m_pPawn. Named by what it holds
	CCSObserverPawn *m_pObserverPawn;
};

#endif // CSTRIKE15_CS_PLAYER_STATE_H
