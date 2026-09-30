//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: The Counter-Strike navigation mesh.
//
//=============================================================================//

#ifndef NAVLIB_CS_NAV_MESH_H
#define NAVLIB_CS_NAV_MESH_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "tier1/utlvector.h"
#include "entityhandle.h"
#include "cstrike15/bot/cs_bot_timers.h"
#include "nav_mesh.h"
#include "cs_nav_area.h"

//-----------------------------------------------------------------------------
// Creates CCSNavArea areas and loads hiding spots, encounters and approach areas with them.
// Adds no virtuals of its own.
//-----------------------------------------------------------------------------
abstract_class CSNavMesh : public CNavMesh
{
public:
	int GetHidingSpotCount() const { return m_HidingSpots.Count(); }
	HidingSpot *GetHidingSpot( int i ) const { return m_HidingSpots[ i ]; }

public:
	// Random spawn points Update keeps alive for deathmatch; 0 disables it.
	int m_desiredDMSpawns;
	// Placement attempts since the last success; after 100 m_desiredDMSpawns is lowered to the
	// current count.
	int m_consecutiveFailedAttempts;
	CountdownTimer m_refreshDMSpawnTimer;
	// The info_deathmatch_spawn entities created so far.
	CUtlVector< CEntityHandle > m_DMSpawnVector;
	// Areas with NAV_MESH_TRANSIENT, rebuilt as areas are added and removed.
	CUtlVector< CNavArea * > m_transientAreas;
	// Every hiding spot of every area; owned here.
	CUtlVector< HidingSpot * > m_HidingSpots;
};

#endif // NAVLIB_CS_NAV_MESH_H
