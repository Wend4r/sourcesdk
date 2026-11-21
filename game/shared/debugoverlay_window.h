//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Base of the debug overlay windows (game systems that draw a debug
//          window and debug overlays; "Grenades", "CS2Bullets", "CS2 WorldModel").
//
//=============================================================================//

#ifndef DEBUGOVERLAY_WINDOW_H
#define DEBUGOVERLAY_WINDOW_H
#ifdef _WIN32
#pragma once
#endif

#include "igamesystem.h"

class KeyValues3;

//-----------------------------------------------------------------------------
// Abstract; every window registers with the debug overlay game system on GameActivate and
// unregisters on GameDeactivate. No source names the members or the own virtual methods
// (Linux 66-71, Windows 65-70); they are named by what the code does with them.
//-----------------------------------------------------------------------------
abstract_class CDebugOverlayWindow : public CAutoGameSystem
{
public:
	DECLARE_GAME_SYSTEM();

	CDebugOverlayWindow( const char *pszName );
	virtual ~CDebugOverlayWindow();

	// Adds the window to the debug overlay game system's list, then LoadSettings
	GS_EVENT( GameActivate );
	// SaveSettings, forgets m_pSettings, removes the window from the list
	GS_EVENT( GameDeactivate );
	// When the overlays draw this tick: DrawOverlay with the engine's debug overlay
	GS_EVENT( ServerPostAdvanceTick );

	// Forward to the game system factory
	virtual void SetGameSystemGlobalPtrs( void *pValue ) override;
	virtual bool DoesGameSystemReallocate() override;

	// Called for every registered window by the debug overlay game system in ClientPreRender; empty in
	// every server window
	virtual void OnPreRender();
	// Returns false in every server window; nothing in the server calls it
	virtual bool ServerQueryReturnsFalse();
	// Draws the window's overlays (pure here)
	virtual void DrawOverlay( void *pDebugOverlay ) = 0;
	// Called on GameActivate / GameDeactivate: the recorder windows load / save their splitter
	// position in m_pSettings
	virtual void LoadSettings();
	virtual void SaveSettings();
	// Opens the window in the debug UI (pUI slot 311 / 28), sets m_bWindowShown from the convar
	virtual void BeginWindow( void *pUI, const char *pszTitle, void *pFlags );

public:
	// Set by BeginWindow while the overlay convar shows the windows
	bool m_bWindowShown;
	// The name the constructor got ("Grenades", "CS2Bullets", "CS2 WorldModel"); also the key of the
	// window's settings
	const char *m_pszName;
	// The window's settings (looked up by m_pszName in the debug overlay system's settings),
	// cleared on GameDeactivate
	KeyValues3 *m_pSettings;
};

#endif // DEBUGOVERLAY_WINDOW_H
