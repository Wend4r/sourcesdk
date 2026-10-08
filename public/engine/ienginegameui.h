//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: Engine side of the game UI: shows the UI and wraps the client's legacy game UI
//
//=============================================================================//

#ifndef IENGINEGAMEUI_H
#define IENGINEGAMEUI_H
#ifdef _WIN32
#pragma once
#endif

#include <appframework/iappsystem.h>

abstract_class IEngineGameUI : public IAppSystem
{
public:
	virtual ~IEngineGameUI() {}

	// Cleared by gameui_hide, set by ActivateGameUI and on Init
	virtual bool IsGameUIActive() = 0;
	// Does nothing while gameui_preventescapetoshow blocks it
	virtual void ActivateGameUI() = 0;
	// True between Init and Shutdown
	virtual bool IsInitialized() = 0;
};

#endif // IENGINEGAMEUI_H
