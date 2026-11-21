//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The "Grenades" debug overlay recorder window.
//
//=============================================================================//

#ifndef CS_DEBUGOVERLAY_GRENADES_H
#define CS_DEBUGOVERLAY_GRENADES_H
#ifdef _WIN32
#pragma once
#endif

#include "debugoverlay_recorder.h"

//-----------------------------------------------------------------------------
// Reallocating factory, 176 bytes on both platforms
//-----------------------------------------------------------------------------
class CDebugOverlayGrenades : public CDebugOverlayRecorderBase
{
public:
	DECLARE_GAME_SYSTEM();

	CDebugOverlayGrenades();
	virtual ~CDebugOverlayGrenades();

	// Forward to the game system factory
	virtual void SetGameSystemGlobalPtrs( void *pValue ) override;
	virtual bool DoesGameSystemReallocate() override;

	// Sets the recorder's convar, then "host_writeconfig"
	virtual void SetEnabled( bool bEnabled ) override;
	// The recorder's convar
	virtual bool IsEnabled() override;
	// Returns 7
	virtual int GetChannelId() override;

public:
	// Named by what the code does with them: seven option flags cleared by the factory, as in
	// CDebugOverlayWorldModel; nothing else in the server reads or writes them
	bool m_bOptions[ 7 ];
};

#endif // CS_DEBUGOVERLAY_GRENADES_H
