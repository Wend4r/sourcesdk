//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The "CS2 WorldModel" debug overlay window.
//
//=============================================================================//

#ifndef CS_DEBUGOVERLAY_WORLDMODEL_H
#define CS_DEBUGOVERLAY_WORLDMODEL_H
#ifdef _WIN32
#pragma once
#endif

#include "debugoverlay_window.h"

//-----------------------------------------------------------------------------
// Reallocating factory, 48 bytes. On the server it draws nothing (DrawOverlay is empty).
//-----------------------------------------------------------------------------
class CDebugOverlayWorldModel : public CDebugOverlayWindow
{
public:
	DECLARE_GAME_SYSTEM();

	CDebugOverlayWorldModel();
	virtual ~CDebugOverlayWorldModel();

	// Forward to the game system factory
	virtual void SetGameSystemGlobalPtrs( void *pValue ) override;
	virtual bool DoesGameSystemReallocate() override;

	// Empty
	virtual void DrawOverlay( void *pDebugOverlay ) override;

public:
	// Named by what the code does with them: the window's seven option flags; the constructor sets
	// the first and clears the others, nothing else in the server reads or writes them
	bool m_bOptions[ 7 ];
};

#endif // CS_DEBUGOVERLAY_WORLDMODEL_H
