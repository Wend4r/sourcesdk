//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Picks the master color_correction entity.
//
//=============================================================================//

#ifndef COLORCORRECTIONSYSTEM_H
#define COLORCORRECTIONSYSTEM_H
#ifdef _WIN32
#pragma once
#endif

#include "igamesystem.h"
#include "ehandle.h"

// CGameEventListener
#include <vscript/ccscript.h>

class CColorCorrection;

//-----------------------------------------------------------------------------
// Registered as "CColorCorrectionSystem" (reallocating factory, 40 / 32 bytes). It adds no
// virtual methods to IGameSystem; on Linux the FireGameEvent override also takes slot 66 of
// the primary table.
//-----------------------------------------------------------------------------
class CColorCorrectionSystem : public CAutoGameSystem, public CGameEventListener
{
public:
	DECLARE_GAME_SYSTEM();

	CColorCorrectionSystem();
	virtual ~CColorCorrectionSystem();

	// Clears m_hMasterController and listens to "round_start"
	GS_EVENT( GameInit );
	// InitMasterController, then (single player) applies it to the local player
	GS_EVENT( PostSpawnGroupLoad );

	// Forward to the game system factory
	virtual void SetGameSystemGlobalPtrs( void *pValue ) override;
	virtual bool DoesGameSystemReallocate() override;

	// "round_start": InitMasterController
	virtual void FireGameEvent( IGameEvent *event ) override;

	CColorCorrection *GetMasterColorCorrection();

private:
	void InitMasterController();

	CHandle< CColorCorrection > m_hMasterController;
};

#endif // COLORCORRECTIONSYSTEM_H
