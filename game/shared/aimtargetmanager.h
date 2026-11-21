//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Keeps the list of entities that carry FL_AIMTARGET.
//
//=============================================================================//

#ifndef AIMTARGETMANAGER_H
#define AIMTARGETMANAGER_H
#ifdef _WIN32
#pragma once
#endif

#include "igamesystem.h"

#include <tier0/utlvector.h>

class CBaseEntity;

//-----------------------------------------------------------------------------
// Registered as "AimTargetManager" (reallocating factory). It adds no virtual methods to
// IGameSystem.
//-----------------------------------------------------------------------------
class CAimTargetManager : public CAutoGameSystem
{
public:
	DECLARE_GAME_SYSTEM();

	CAimTargetManager();
	virtual ~CAimTargetManager();

	// Both purge m_targetList
	GS_EVENT( GameInit );
	GS_EVENT( GameShutdown );

	// Forward to the game system factory
	virtual void SetGameSystemGlobalPtrs( void *pValue ) override;
	virtual bool DoesGameSystemReallocate() override;

public:
	// An entity is added when FL_AIMTARGET is set on it and removed when the flag
	// is cleared (the flag change handler of CBaseEntity)
	CUtlVector< CBaseEntity * > m_targetList;
};

#endif // AIMTARGETMANAGER_H
