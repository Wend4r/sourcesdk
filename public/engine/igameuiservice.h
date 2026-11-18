//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: Engine service for the in-game console and UI input layers
//
//=============================================================================//

#ifndef IGAMEUISERVICE_H
#define IGAMEUISERVICE_H
#ifdef _WIN32
#pragma once
#endif

#include <IEngineService.h>
#include <con_nprint.h>

namespace panorama
{
	class IUIWindow;
};

abstract_class IGameUIService : public IEngineService
{
public:
	virtual bool IsConsoleEnabled() = 0;
	// IsConsoleEnabled without con_enable (Dota 2 2018 name)
	virtual bool IsConsoleEnabledByCommandLine() = 0;

	virtual bool IsConsoleVisible() = 0;
	virtual void ToggleConsole() = 0;
	// Creates the console panel on first show
	virtual void ShowConsole( bool bShow ) = 0;

	virtual void BlockConsoleToggle( bool bBlock ) = 0;

	// Plat_FloatTime() of the last input event of type 5, the mouse move (Dota 2 2018 name)
	virtual double GetLastMouseMoveTime() = 0;

	virtual InputContextHandle_t GetInputContext( int nLayer ) = 0;

	virtual bool HasConsole() = 0;
	// Forwards to the console panel when it exists (Dota 2 2018 name)
	virtual void ClearConsole() = 0;

	// Draws the panorama window as a layer of the game UI and joins it to the UI input dispatch;
	// bFullscreen resizes the window to the main window and applies its scale factor
	virtual void *AddProceduralLayer( const char *pszName, panorama::IUIWindow *pWindow, bool, bool bFullscreen ) = 0;
	// AMNOTE: Only creates the panorama layer singleton and returns NULL; no source names it
	virtual void *EmptyProceduralLayerQuery() = 0;

	// Per layer (0 to 3) handler lists, sorted by nPriority
	virtual void AddLayerHandler( int nLayer, void *pHandler, int nPriority ) = 0;
	virtual void RemoveLayerHandler( int nLayer, void *pHandler ) = 0;

	virtual void Con_NPrintf( int nRow, const char *pszFormat, ... ) FMTFUNCTION( 3, 4 ) = 0;
	virtual void Con_NPrintf( const con_nprint_t *pInfo, const char *pszFormat, ... ) FMTFUNCTION( 3, 4 ) = 0;
};

#endif // IGAMEUISERVICE_H
