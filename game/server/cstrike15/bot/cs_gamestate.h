//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: What one bot knows about the scenario: the bomb, the bomb sites and the
//          hostages.
//
//=============================================================================//

#ifndef CS_GAMESTATE_H
#define CS_GAMESTATE_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "mathlib/vector.h"
#include "entityhandle.h"
#include "cs_bot_timers.h"

class CCSBot;

//-----------------------------------------------------------------------------
// Embedded in the bot. Bomb site indices are zone indices of the bot manager.
//-----------------------------------------------------------------------------
class CSGameState
{
public:
	enum BombState
	{
		// Carried by a terrorist.
		MOVING,
		LOOSE,
		PLANTED,
		DEFUSED,
		EXPLODED
	};

	enum
	{
		UNKNOWN = -1
	};

	struct HostageInfo
	{
		CEntityHandle hostage;
		Vector knownPos;
		bool isValid;
		bool isAlive;
		// Not escorted by a CT.
		bool isFree;
	};

	bool IsBombPlanted() const { return m_bombState == PLANTED; }
	BombState GetBombState() const { return m_bombState; }
	int GetPlantedBombsite() const { return m_plantedBombsite; }

public:
	CCSBot *m_owner;
	// The round is won or lost but not yet reset.
	bool m_isRoundOver;
	BombState m_bombState;
	IntervalTimer m_lastSawBomber;
	Vector m_bomberPos;
	IntervalTimer m_lastSawLooseBomb;
	Vector m_looseBombPos;
	bool m_isBombsiteClear[ 4 ];
	int m_bombsiteSearchOrder[ 4 ];
	int m_bombsiteCount;
	int m_bombsiteSearchIndex;
	// UNKNOWN until the planted site is known.
	int m_plantedBombsite;
	bool m_isPlantedBombPosKnown;
	Vector m_plantedBombPos;
	HostageInfo m_hostage[ 12 ];
	int m_hostageCount;
	CountdownTimer m_validateInterval;
	bool m_allHostagesRescued;
	// A CT has been seen moving a hostage.
	bool m_haveSomeHostagesBeenTaken;
};

#endif // CS_GAMESTATE_H
