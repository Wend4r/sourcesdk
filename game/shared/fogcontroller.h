//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The fog game system. Factory "FogSystem".
//
//=============================================================================//

#ifndef FOGCONTROLLER_H
#define FOGCONTROLLER_H
#ifdef _WIN32
#pragma once
#endif

#include "igamesystem.h"
#include "ehandle.h"

class CFogController;

//-----------------------------------------------------------------------------
// Static instance, 24 bytes on both platforms.
//-----------------------------------------------------------------------------
class CFogSystem : public CAutoGameSystem
{
public:
	DECLARE_GAME_SYSTEM();

	// Clears m_hMasterController
	GS_EVENT( GameInit );
	// Finds the master fog controller of the new active spawn group
	GS_EVENT( ActiveSpawnGroupChanged );

	// Forward to the game system factory
	virtual void SetGameSystemGlobalPtrs( void *pValue ) override;
	virtual bool DoesGameSystemReallocate() override;

public:
	CHandle< CFogController > m_hMasterController;
};

#endif // FOGCONTROLLER_H
