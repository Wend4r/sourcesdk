//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: Client UI state machine driven by the engine
//
//===========================================================================//

#ifndef ICLIENTUI_H
#define ICLIENTUI_H

#ifdef _WIN32
#pragma once
#endif

#include "appframework/iappsystem.h"

class KeyValues;

enum ClientUIState_t : int;

abstract_class IClientUI : public IAppSystem
{
public:
	// Creates the HUD root panel and loads the HUD localization
	virtual void OnPanoramaUIStartup() = 0;

	virtual void OnGameUIActivated() = 0;
	virtual void OnGameUIHidden() = 0;
	virtual void OnLevelLoadingStarted( const char *pszLevelName, KeyValues *pLoadingOptions, bool bShowProgressDialog ) = 0;
	virtual void OnLevelLoadingFinished() = 0;
	virtual void UpdateProgressBar( float flProgress, const char *pszStatusText ) = 0;

	// While the game UI is shown, the engine applies fps_max_ui only when this returns false
	virtual bool unk017() = 0;

	virtual ClientUIState_t GetClientUIState() = 0;

	virtual void AddListener( void *pListener ) = 0;
	virtual void RemoveListener( void *pListener ) = 0;

	// State of the "panorama_ingame" input context
	virtual bool IsInGameInputContextEnabled() = 0;
	virtual void EnableInGameInputContext( bool bEnable ) = 0;
	virtual void *unk023() = 0;

	virtual void SetClientUIState( ClientUIState_t nState ) = 0;
};

#endif // ICLIENTUI_H
