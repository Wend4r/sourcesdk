//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Runs the asynchronous touch queries of the triggers.
//
//=============================================================================//

#ifndef TRIGGERASYNCQUERYSYSTEM_H
#define TRIGGERASYNCQUERYSYSTEM_H
#ifdef _WIN32
#pragma once
#endif

#include "igamesystem.h"

#include <tier0/utlvector.h>

class CBaseTrigger;

//-----------------------------------------------------------------------------
// Registered as "CTriggerAsyncQuerySystem" (static factory). 48 bytes on both platforms. No source
// has the class; the names are named by what the code does.
//-----------------------------------------------------------------------------
class CTriggerAsyncQuerySystem : public CAutoGameSystem
{
public:
	DECLARE_GAME_SYSTEM();

	virtual ~CTriggerAsyncQuerySystem();

	// Empties m_Triggers
	GS_EVENT( GameShutdown );
	// Runs the touch query of every registered trigger
	GS_EVENT( ServerPreEntityThink );

	// Forward to the game system factory
	virtual void SetGameSystemGlobalPtrs( void *pValue ) override;
	virtual bool DoesGameSystemReallocate() override;

public:
	CUtlVector< CBaseTrigger * > m_Triggers;
};

#endif // TRIGGERASYNCQUERYSYSTEM_H
