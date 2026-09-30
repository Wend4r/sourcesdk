//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: The Counter-Strike nav area: places, hiding spots, encounters, approach areas,
//			danger and player counts.
//
//=============================================================================//

#ifndef CS_NAV_AREA_H
#define CS_NAV_AREA_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "tier1/utlvector.h"
#include "cstrike15/bot/cs_bot_timers.h"
#include "nav_area.h"

//-----------------------------------------------------------------------------
// What a bot statement or search is about: a nav place, an entity or anything. Wider than an
// area's Place, which is only the place index.
//-----------------------------------------------------------------------------
class CCSPlaceId
{
public:
	uint32 m_nValue;
	// 0 for nowhere, 1 when m_nValue is a Place, 2 when it is an entity handle (GetRandomSpot
	// then picks a nav mesh spot inside that entity's bounds), -1 for anywhere. A place name
	// resolves to a named entity first and to a nav mesh place otherwise.
	int8 m_nKind;
};

//-----------------------------------------------------------------------------
// Adds the place and the per-team game state interface to the base area.
//-----------------------------------------------------------------------------
abstract_class CNavAreaExtended : public CNavArea
{
public:
	virtual void IncrementPlayerCount( int nTeamID, int nEntIndex ) = 0;
	virtual void DecrementPlayerCount( int nTeamID, int nEntIndex ) = 0;
	// Whether continuous damage (fire) is in the area.
	virtual bool IsDamaging() const = 0;
	virtual void MarkAsDamaging( float flDuration ) = 0;
	// Danger lost per second.
	virtual float GetDangerDecayRate() const = 0;

public:
	Place m_place;
};

//-----------------------------------------------------------------------------
// The areas of the Counter-Strike mesh. Hiding spots are also listed by the mesh; the mesh owns
// them.
//-----------------------------------------------------------------------------
abstract_class CCSNavArea : public CNavAreaExtended
{
public:
	// A way into this area on the path from a spawn, for map learning.
	struct ApproachInfo
	{
		CNavConnect here;
		CNavConnect prev;
		CNavConnect next;
		uint16 prevToHereHow;
		uint16 hereToNextHow;
	};

	enum { MAX_APPROACH_AREAS = 16 };

	const CUtlVector< HidingSpot * > &GetHidingSpots() const { return m_hidingSpots; }

	int GetSpotEncounterCount() const { return m_spotEncounters.m_pData ? m_spotEncounters.m_pData->m_Size : 0; }
	const SpotEncounter *GetSpotEncounter( int i ) const { return m_spotEncounters.m_pData->m_Elements[ i ]; }

	int GetApproachInfoCount() const { return m_approachCount; }
	const ApproachInfo &GetApproachInfo( int i ) const { return m_approach[ i ]; }

	uint8 GetPlayerCount( int nTeamID ) const { return m_playerCount[ nTeamID % NAV_MAX_TEAMS ]; }
	float GetEarliestOccupyTime( int nTeamID ) const { return m_earliestOccupyTime[ nTeamID % NAV_MAX_TEAMS ]; }
	// The team index is the team number minus one, as the bots pass it.
	float GetClearedTimestamp( int nTeamID ) const { return m_clearedTimestamp[ nTeamID % NAV_MAX_TEAMS ]; }

	// The stored danger, without the decay the game applies before reading it.
	float GetRawDanger( int nTeamID ) const { return m_danger[ nTeamID % NAV_MAX_TEAMS ]; }

public:
	// The same layout as a connection vector, holding encounter pointers.
	struct SpotEncounterVector
	{
		struct Data_t
		{
			int m_Size;
			SpotEncounter *m_Elements[ 1 ];
		};

		// Null when there are none.
		Data_t *m_pData;
	};

	SpotEncounterVector m_spotEncounters;
	// Least time for each team to reach the area from its spawn.
	float m_earliestOccupyTime[ NAV_MAX_TEAMS ];
	uint8 m_playerCount[ NAV_MAX_TEAMS ];
	// IsDamaging() while gpGlobals->tickcount has not passed it.
	int m_damagingTickCount;
	float m_danger[ NAV_MAX_TEAMS ];
	// Game time each team's danger was last decayed.
	float m_dangerTimestamp[ NAV_MAX_TEAMS ];
	// Game time a bot of each team last entered the area; the hunt state prefers areas not
	// cleared for a while.
	float m_clearedTimestamp[ NAV_MAX_TEAMS ];
	CUtlVector< HidingSpot * > m_hidingSpots;
	ApproachInfo m_approach[ MAX_APPROACH_AREAS ];
	uint8 m_approachCount;

private:
	// Rounds the area's own members up to a multiple of 128 bytes; the size is computed from
	// the base area's size (0xB8 bytes), not from CNavAreaExtended's.
	uint8 m_paddingToAlignTo128[ 0x47 ];

public:
	// Throttles UpdateBlocked, which traces the area for blocking entities and fires
	// nav_blocked when the result changes.
	CountdownTimer m_blockedTimer;
};

#endif // CS_NAV_AREA_H
