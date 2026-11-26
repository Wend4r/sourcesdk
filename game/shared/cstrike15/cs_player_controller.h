#ifndef CSTRIKE15_CS_PLAYER_CONTROLLER_H
#define CSTRIKE15_CS_PLAYER_CONTROLLER_H

#pragma once

#include "../baseplayercontroller.h"
#include "igameevents.h"

//-----------------------------------------------------------------------------
// CCSPlayerController adds 3 methods to CBasePlayerController: Linux slots 276-278,
// Windows 274-276. FireGameEvent is in the Linux table only, slot 275: the override of
// IGameEventListener2, the second base, which the Itanium ABI also enters into the primary
// table and MSVC does not. Declaring the second base reproduces both without #ifndef; it
// sits at offset 8 here, not where the game has it, so do not convert a pointer to it.
//-----------------------------------------------------------------------------
template < class TBase = CBasePlayerControllerVTable<> >
abstract_class CCSPlayerControllerVTable : public TBase, public IGameEventListener2
{
public:
	// Handles player_disconnect. Overrides IGameEventListener2: a slot of this table on Linux only
	virtual void FireGameEvent( IGameEvent *pEvent ) override = 0;
	virtual void Respawn() = 0;
	// Calls PlaceOnGround of the pawn; never overridden. Named by what it does
	virtual void PlacePawnOnGround() = 0;
	// Clears the per-round state: m_iRoundScore and the per-round data of the action tracking and money services of the controller and of the action tracking services of the pawn. Called for every controller by RestartRound and BeginRound of the game rules
	virtual void ResetPerRoundStats() = 0;
};

#endif // CSTRIKE15_CS_PLAYER_CONTROLLER_H
