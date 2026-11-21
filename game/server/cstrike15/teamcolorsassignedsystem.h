//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The game system registered for the assigned team colours.
//
//=============================================================================//

#ifndef CSTRIKE15_TEAMCOLORSASSIGNEDSYSTEM_H
#define CSTRIKE15_TEAMCOLORSASSIGNEDSYSTEM_H
#ifdef _WIN32
#pragma once
#endif

#include "igamesystem.h"

//-----------------------------------------------------------------------------
// Registered as "CTeamColorsAssignedSystem" (reallocating factory). 24 bytes on both platforms: no
// fields besides the DECLARE_GAME_SYSTEM member, and it overrides no event.
//-----------------------------------------------------------------------------
class CTeamColorsAssignedSystem : public CAutoGameSystem
{
public:
	DECLARE_GAME_SYSTEM();

	virtual ~CTeamColorsAssignedSystem();

	// Forward to the game system factory
	virtual void SetGameSystemGlobalPtrs( void *pValue ) override;
	virtual bool DoesGameSystemReallocate() override;
};

#endif // CSTRIKE15_TEAMCOLORSASSIGNEDSYSTEM_H
