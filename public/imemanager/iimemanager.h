//===== Copyright (c) 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: Input method editor (IME) support for text input in the game window
//
//===========================================================================//

#ifndef IIMEMANAGER_H
#define IIMEMANAGER_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "tier0/platwindow.h"
#include "tier0/logging.h"
#include "appframework/iappsystem.h"

abstract_class IIMEManager : public IAppSystem
{
public:
	virtual bool InitForWindow( PlatWindow_t hWindow ) = 0;

	// Whether the backend was created and initialized successfully
	virtual bool IsIMESupported() = 0;

	virtual void SetIMEEnabled( bool bEnabled ) = 0;
	virtual bool IsIMEEnabled() = 0;

	virtual bool HandleIMEEvent( PlatWindow_t hWindow, uint nMsg, uint64 wParam, int64 lParam ) = 0;
	virtual bool HandleIMEEventPreProcess( PlatWindow_t hWindow, uint nMsg, uint64 wParam, int64 lParam ) = 0;

	virtual void Unk_FinalizeCompositionForView( void *p ) = 0;

	virtual void Unk_FinalizeCompositionOnFocusLost( void *p ) = 0;

	virtual void SetActiveUIView( void *pUIView, bool bActive ) = 0;
	virtual void *GetActiveUIView() = 0;

	virtual LoggingChannelID_t GetLoggingChannel() = 0;

	virtual ~IIMEManager() {}
};

#endif // IIMEMANAGER_H
