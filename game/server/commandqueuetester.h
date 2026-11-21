//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Debug system that cycles the player command queue settings
//          ("SV: changing command queue to %s").
//
//=============================================================================//

#ifndef COMMANDQUEUETESTER_H
#define COMMANDQUEUETESTER_H
#ifdef _WIN32
#pragma once
#endif

#include "igamesystem.h"

#include <tier0/utlvector.h>

//-----------------------------------------------------------------------------
// Registered as "CCommandQueueTester" (static factory). No source names its members; they are named
// by what the code does with them. It adds no virtual methods to IGameSystem.
//-----------------------------------------------------------------------------
class CCommandQueueTester : public CAutoGameSystem
{
public:
	DECLARE_GAME_SYSTEM();

	// One setting of the cycle: GameInit fills { "enabled,q=1", true, 1 } and { "disabled,q=1", false, 1 }
	struct CommandQueueSetting_t
	{
		const char *m_pszName;
		// Written to the first convar of the setting
		bool m_bEnabled;
		// Written to the second convar of the setting
		int m_nQueueValue;
	};

	CCommandQueueTester();
	virtual ~CCommandQueueTester();

	// Fills m_Settings and applies the next one while the tester convar is on
	GS_EVENT( GameInit );
	// Applies the next setting once the interval convar (at least 1 s of Plat_FloatTime) passed
	GS_EVENT( ServerPostSimulate );

	// Forward to the game system factory
	virtual void SetGameSystemGlobalPtrs( void *pValue ) override;
	virtual bool DoesGameSystemReallocate() override;

public:
	CUtlVector< CommandQueueSetting_t > m_Settings;
	// The setting applied next (wraps around m_Settings)
	int m_nNextSetting;
	// Plat_FloatTime of the last change
	float m_flLastChangeTime;
};

#endif // COMMANDQUEUETESTER_H
