//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The game system that tracks how long each player speaks on voice.
//
//=============================================================================//

#ifndef PLAYER_VOICE_LISTENER_H
#define PLAYER_VOICE_LISTENER_H
#ifdef _WIN32
#pragma once
#endif

#include "igamesystem.h"

//-----------------------------------------------------------------------------
// Static instance (factory "PlayerVoiceListener"), 1304 bytes on both platforms.
// The arrays are indexed by player slot (0..63).
//-----------------------------------------------------------------------------
class CPlayerVoiceListener : public CAutoGameSystem
{
public:
	DECLARE_GAME_SYSTEM();

	// Both log their name ("CPlayerVoiceListener::GameInit()") and zero the arrays
	GS_EVENT( GameInit );
	GS_EVENT( GameShutdown );
	// Logs "CPlayerVoiceListener::PostSpawnGroupUnload()"
	GS_EVENT( PostSpawnGroupUnload );

	// Forward to the game system factory
	virtual void SetGameSystemGlobalPtrs( void *pValue ) override;
	virtual bool DoesGameSystemReallocate() override;

public:
	// Named by what it does: Plat_GetTime() of the first voice data of the player, set
	// when the slot is zero
	double m_flFirstSpeechPlatTime[ 64 ];
	float m_flLastPlayerSpeechTime[ 64 ];
	float m_flPlayerSpeechDuration[ 64 ];
	float m_flPlayerSpeechTotalDuration[ 64 ];
};

#endif // PLAYER_VOICE_LISTENER_H
