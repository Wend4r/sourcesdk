//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The game system that loads the baked bomb damage data of a map
//			(CS2 game/shared/cstrike15/cs_bomb_damage.cpp). Factory
//			"MapBombDamageGameSystem".
//
//=============================================================================//

#ifndef CSTRIKE15_CS_BOMB_DAMAGE_H
#define CSTRIKE15_CS_BOMB_DAMAGE_H
#ifdef _WIN32
#pragma once
#endif

#include "igamesystem.h"
#include "mathlib/vector.h"
#include "tier0/utlhashtable.h"
#include "tier0/utlstring.h"
#include "tier0/utlvector.h"
#include "tier0/vertexkdtree.h"

//-----------------------------------------------------------------------------
// The data of "<map>_c" (a KeyValues3 file with "header" { "version" } and
// "data" { "bombsites", "positions", "damage_values" } binary blobs), filled by
// the load callback (CMapBombDamageDataLoadCallback). All names are named by
// what the loader does with the members; 168 bytes.
//-----------------------------------------------------------------------------
class CMapBombDamageData
{
public:
	// 28 bytes per bomb site in "bombsites"; the loader widens the bounds by 32 units
	struct BombSite_t
	{
		Vector m_vecMins;
		Vector m_vecMaxs;
		// 3000 until the file's value is copied over it
		float m_flRange;
	};

	// Position -> one value per bomb site from "damage_values" (0xFFFF until set)
	CUtlHashtable< Vector, CUtlVector< uint32 > > m_DamageAtPosition;
	// "positions" (int16 triples in the file)
	CUtlVector< Vector > m_Positions;
	// Built over m_Positions
	CVertexKDTree m_PositionTree;
	// Bounds of m_Positions
	Vector m_vecPositionMins;
	Vector m_vecPositionMaxs;
	CUtlVector< BombSite_t > m_BombSites;
};

//-----------------------------------------------------------------------------
// Static instance, 208 bytes on both platforms.
//-----------------------------------------------------------------------------
class CMapBombDamageGameSystem : public CAutoGameSystem
{
public:
	DECLARE_GAME_SYSTEM();

	// Releases m_pLoadRequest
	GS_EVENT( GameShutdown );
	// On a new map: clears the data and requests "<map>_c" from the map's vpk
	GS_EVENT( GameActivate );

	// Forward to the game system factory
	virtual void SetGameSystemGlobalPtrs( void *pValue ) override;
	virtual bool DoesGameSystemReallocate() override;

public:
	CMapBombDamageData m_Data;
	// Name of the map the data belongs to
	CUtlString m_sMapName;
	// The pending load of "<map>_c" (an engine file loading service request), released by
	// GameShutdown
	void *m_pLoadRequest;
};

#endif // CSTRIKE15_CS_BOMB_DAMAGE_H
