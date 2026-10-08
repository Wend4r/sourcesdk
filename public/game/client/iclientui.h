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
	virtual void unk011() = 0;

	virtual void OnGameUIActivated() = 0;
	virtual void OnGameUIHidden() = 0;
	virtual void OnLevelLoadingStarted( const char *pszLevelName, KeyValues *pLoadingOptions, bool bShowProgressDialog ) = 0;
	virtual void OnLevelLoadingFinished() = 0;
	virtual void UpdateProgressBar( float flProgress, const char *pszStatusText ) = 0;

	virtual bool unk017() = 0;

	virtual ClientUIState_t GetClientUIState() = 0;

	virtual void AddListener( void *pListener ) = 0;
	virtual void RemoveListener( void *pListener ) = 0;

	// Flag toggled by unk022
	virtual bool unk021() = 0;
	virtual void unk022( bool bEnable ) = 0;
	virtual void *unk023() = 0;

	virtual void SetClientUIState( ClientUIState_t nState ) = 0;
};

#endif // ICLIENTUI_H
