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
#include "tier0/utlvector.h"
#include "appframework/iappsystem.h"
#include "inputsystem/inputenums.h"

namespace panorama
{
	class IUIEngine;
	class IUIWindow;
	class CPanel2D;
};

class IEngineConsoleUI;

// The debugger panel on a window; destroying it deletes the panel
abstract_class IPanoramaClientDebugger
{
public:
	virtual ~IPanoramaClientDebugger() {}

	// Starts picking the inspected panel with the mouse; selects the hovered panel of the focused window
	virtual void BeginInspect() = 0;

	// Fraction of the debugger taken by the panel tree, kept between 0.1 and 0.9
	virtual float GetSplitterPosition() = 0;
	virtual void SetSplitterPosition( float flPosition ) = 0;
};

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
