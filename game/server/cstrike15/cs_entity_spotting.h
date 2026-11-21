//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Updates which entities are spotted by the teams (the radar).
//
//=============================================================================//

#ifndef CS_ENTITY_SPOTTING_H
#define CS_ENTITY_SPOTTING_H
#ifdef _WIN32
#pragma once
#endif

#include "igamesystem.h"

//-----------------------------------------------------------------------------
// Registered as "CCSEntitySpotting" (static factory).
//-----------------------------------------------------------------------------
class CCSEntitySpotting : public CAutoGameSystem
{
public:
	DECLARE_GAME_SYSTEM();

	// The spotting rules of an entity
	enum SpottingRules_T
	{
		SPOT_RULE_ENEMY = ( 1 << 0 ),					// spotted when seen by an enemy
		SPOT_RULE_CT = ( 1 << 1 ),						// spotted when seen by CT
		SPOT_RULE_T = ( 1 << 2 ),						// spotted when seen by T
		SPOT_RULE_ALWAYS_SEEN_BY_FRIEND = ( 1 << 3 ),	// always visible to friend
		SPOT_RULE_ALWAYS_SEEN_BY_CT = ( 1 << 4 ),		// always visible to CT
		SPOT_RULE_ALWAYS_SEEN_BY_T = ( 1 << 5 ),		// always visible to T
	};

	CCSEntitySpotting();
	virtual ~CCSEntitySpotting();

	// Returns true
	virtual bool Init() override;
	// Calls UpdateSpottedEntities every 0.5 s of curtime
	GS_EVENT( ServerPostEntityThink );

	// Forward to the game system factory
	virtual void SetGameSystemGlobalPtrs( void *pValue ) override;
	virtual bool DoesGameSystemReallocate() override;

	// Returns "CCSEntitySpotting"
	virtual const char *Name();

	void UpdateSpottedEntities();

public:
	// curtime of the last UpdateSpottedEntities
	float m_fLastUpdate;
};

#endif // CS_ENTITY_SPOTTING_H
