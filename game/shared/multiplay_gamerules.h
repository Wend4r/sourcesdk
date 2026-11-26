#ifndef MULTIPLAY_GAMERULES_H
#define MULTIPLAY_GAMERULES_H

#pragma once

#include "gamerules.h"

//-----------------------------------------------------------------------------
// CMultiplayRules adds 1 method to CGameRules: Linux slot 119, Windows 118.
//
// Virtual methods only, in the order of the Linux table; the class has no members.
// Unk_N is a method without a name, Stub_N one that does nothing in every class;
// N is the Windows slot.
//-----------------------------------------------------------------------------
template < class TBase = CGameRulesVTable >
abstract_class CMultiplayRulesVTable : public TBase
{
public:
	// Named by what it does. The method CMultiplayRules adds; no replay of a kill by a bot unless spec_replay_bot is set, and CCSGameRules refuses once m_iRoundWinStatus is set, both
	virtual bool OnReplayPrompt( CBasePlayerPawn *pVictim, CBasePlayerPawn *pScorer ) = 0;
};

#endif // MULTIPLAY_GAMERULES_H
