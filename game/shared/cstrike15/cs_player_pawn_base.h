#ifndef CSTRIKE15_CS_PLAYER_PAWN_BASE_H
#define CSTRIKE15_CS_PLAYER_PAWN_BASE_H

#pragma once

#include "../../server/baseplayerpawn.h"
#include "../../server/ai_speech.h"

class CCSPlayerState;
enum CSPlayerState : uint32;

//-----------------------------------------------------------------------------
// CCSPlayerPawnBase adds 8 methods to CAI_ExpresserHost< CBasePlayerPawn >: Linux slots
// 406-413, Windows 405-412.
//-----------------------------------------------------------------------------
template < class TBase = CAI_ExpresserHostVTable< CBasePlayerPawnVTable<> > >
abstract_class CCSPlayerPawnBaseVTable : public TBase
{
public:
	// Returns false; CCSPlayerPawn returns true
	virtual bool IsPlayer() = 0;
	// Returns false; CCSObserverPawn returns true. Asked by AddedToEntityDatabase and SetOrResetInitialState. Named by what it does
	virtual bool IsObserverPawn() = 0;
	// False for MOVETYPE_NONE; CCSPlayerPawn also outside STATE_ACTIVE and STATE_OBSERVER_MODE, while defusing, grabbing a hostage, in a blocking use action, in the freeze period or while arming the bomb. CCSPlayer::CanMove of Counter-Strike: Global Offensive. Asked every tick
	virtual bool CanMove() = 0;
	// Empty; CCSPlayerPawn and CCSObserverPawn override it. Called by CCSPlayerController::Respawn and for every pawn when a round restarts. CCSPlayer::RoundRespawn of Counter-Strike: Global Offensive
	virtual void RoundRespawn() = 0;
	// Traces the hull down from 16 units above the origin, moves the pawn there and sets its ground entity; called for every pawn when a round begins. Never overridden. Named by what it does
	virtual void PlaceOnGround() = 0;
	// Sets m_blindUntilTime and m_blindStartTime; overridden by CCSPlayerPawn
	virtual void Blind( float flHoldTime, float flFadeTime, float flStartingAlpha ) = 0;
	// Empty; CCSPlayerPawn moves the player between states for the new team, CCSObserverPawn enters STATE_OBSERVER_MODE. Called by OnTeamChanged unless the controller is in m_bInSwitchTeam. Named by what it does, after the state part of CCSPlayer::ChangeTeam of Counter-Strike: Global Offensive
	virtual void HandleTeamChange( int nOldTeam, int nNewTeam ) = 0;
	// The CCSPlayerState member of the pawn for a state, null when the pawn has none: Welcome, ObserverMode, GungameRespawn, DeathWaitForKey and Dormant here; CCSPlayerPawn and CCSObserverPawn answer their own states first. Called by State_Enter
	virtual CCSPlayerState *State_LookupInfo( CSPlayerState nState ) = 0;
};

#endif // CSTRIKE15_CS_PLAYER_PAWN_BASE_H
