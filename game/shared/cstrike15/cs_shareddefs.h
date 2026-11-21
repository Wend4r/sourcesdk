//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Shared Counter-Strike definitions
//
//=============================================================================//

#ifndef CSTRIKE15_CS_SHAREDDEFS_H
#define CSTRIKE15_CS_SHAREDDEFS_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"

#include "schemacompiler/schemaon.h"

//-----------------------------------------------------------------------------
// The states of a player pawn (CCSPlayerPawnBase::m_iPlayerState). Each state the pawn knows is a
// CCSPlayerState member of the pawn, see cstrike15/cs_player_state.h.
//-----------------------------------------------------------------------------
schema enum CSPlayerState : uint32
{
	// Running around in the game
	STATE_ACTIVE = 0,
	// The state a player enters the server in
	STATE_WELCOME,
	// Choosing a team
	STATE_PICKINGTEAM,
	// Choosing a class; no pawn has a state for it
	STATE_PICKINGCLASS,
	// Playing the death animation
	STATE_DEATH_ANIM,
	// Done playing the death animation, waiting for a key to go into observer mode
	STATE_DEATH_WAIT_FOR_KEY,
	// Observing
	STATE_OBSERVER_MODE,
	// Respawning in a gun game
	STATE_GUNGAME_RESPAWN,
	// No thinking
	STATE_DORMANT,
	NUM_PLAYER_STATES,
};

#include "schemacompiler/schemaoff.h"

#endif // CSTRIKE15_CS_SHAREDDEFS_H
