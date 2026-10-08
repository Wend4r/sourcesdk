//===== Copyright (c) 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: Game-side panorama client: engine setup, root panels and engine console
//
//===========================================================================//

#ifndef IPANORAMAUICLIENT_H
#define IPANORAMAUICLIENT_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "tier0/platwindow.h"
#include "tier1/utlvector.h"
#include "appframework/iappsystem.h"
#include "inputsystem/InputEnums.h"

namespace panorama
{
	class IUIEngine;
	class IUIWindow;
	class CPanel2D;
};

class IPanoramaClientDebugger;
class IEngineConsoleUI;

abstract_class IPanoramaUIClient : public IAppSystem
{
public:
	virtual panorama::IUIEngine *SetupUIEngine( const char *pszLanguage, PlatWindow_t hWindow, bool bAllowDebugger, bool bAsyncFontRegistration ) = 0;

	virtual void WaitForFontRegistration() = 0;

	virtual void ShutdownUIEngine() = 0;

	virtual bool HandleInputEvent( const InputEvent_t &event, const CUtlVector< panorama::IUIWindow * > &vecWindows, bool bRequireFocus ) = 0;

	// Creates a root panel on the window
	virtual panorama::CPanel2D *CreatePanel2D( panorama::IUIWindow *pWindow, const char *pszID ) = 0;
	virtual IPanoramaClientDebugger *CreateDebugger( panorama::IUIWindow *pWindow, const char *pszID ) = 0;

	virtual IEngineConsoleUI *CreateEngineConsole() = 0;
};

#endif // IPANORAMAUICLIENT_H
