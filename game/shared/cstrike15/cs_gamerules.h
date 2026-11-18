#ifndef CS_GAMERULES_H
#define CS_GAMERULES_H

#pragma once

#include "../multiplay_gamerules.h"

//-----------------------------------------------------------------------------
// CCSGameRules adds 11 methods to CMultiplayRules: Linux slots 120-130, Windows 119-129.
//
// The game has CTeamplayRules between the two classes, but no table of its own for it:
// these slots are the methods CTeamplayRules and CCSGameRules add together.
//
// Virtual methods only, in the order of the Linux table; the class has no members.
// The game has no symbols for these classes: a name comes from the table of
// CTeamplayRules of Dota 2 2018, which kept the order, from gamedata of other
// projects or from the strings of the method, and the comment says which.
// Unk_N is a method without a name, Stub_N one that does nothing in every class;
// N is the Windows slot.
//-----------------------------------------------------------------------------
template < class TBase = CMultiplayRulesVTable<> >
abstract_class CCSGameRulesVTable : public TBase
{
public:
	// Returns true in every class
	virtual bool Stub_119() = 0;
	// Named by the sources of Counter-Strike: Global Offensive, what it does. Returns m_bGameRestart
	virtual bool IsGameRestarting() = 0;
	// Named by the sources of Counter-Strike: Global Offensive, what it does. Queued matchmaking (sv_mmqueue_reservation) outside the warmup
	virtual bool IgnorePlayerKillCommand() = 0;
	// Named by the table of Dota 2 2018, its strings. CHANGE LEVEL
	virtual void ChangeLevel() = 0;
	// Named by the sources of Counter-Strike: Global Offensive, what it does. Returns d7NSuLq2
	virtual const unsigned char *GetEncryptionKey() = 0;
	// Named by the table of Dota 2 2018, CS2_VibeSignatures, its strings. Game_player_equip
	virtual void PlayerSpawn( CBasePlayerPawn *pPawn ) = 0;
	// Named by the sources of Counter-Strike: Global Offensive, what it does. Mp_endwarmup_player_count, not above the number of players of the game mode
	virtual int NumberOfPlayersRequiredToEndWarmupEarly() = 0;
	// Named by the sources of Counter-Strike: Global Offensive, what it does. Compares the time with a delay made of the respawn wave convars of the team
	virtual bool HasPassedMinRespawnTime( CBasePlayerPawn *pPlayer ) = 0;
	// Named by what it does. Once, m_flIntermissionStartTime becomes the game time and m_flIntermissionEndTime adds mp_chattime
	virtual void StartIntermissionTimer() = 0;
	// Named by the table of Dota 2 2018, its strings. Invalid map included in map cycle. The body writes the name into the buffer of the given size and reads the flag
	virtual void GetNextLevelName( char *pszNextMap, int nBufferSize, bool bRandom ) = 0;
	// Named by the table of Dota 2 2018, CS2_VibeSignatures, its strings. Going to intermission
	virtual void GoToIntermission() = 0;
};

#endif // CS_GAMERULES_H
