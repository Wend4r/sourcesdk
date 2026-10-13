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
#include "tier0/convar.h"
#include "tier2/tier2.h"

class IMEManagerBase;
class IIMEUIView;
class IIMEUIObject;

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

	virtual void FinalizeCompositionForView( IIMEUIView *pUIView, IIMEUIObject *pObject ) = 0;

	// Acts only when bHasFocus is false
	virtual void FinalizeCompositionOnFocusLost( IIMEUIView *pUIView, bool bHasFocus ) = 0;

	virtual void SetActiveUIView( IIMEUIView *pUIView, bool bActive ) = 0;
	virtual IIMEUIView *GetActiveUIView() = 0;

	virtual LoggingChannelID_t GetLoggingChannel() = 0;

	virtual ~IIMEManager() {}
};

class CIMEManager : public CTier2AppSystem< IIMEManager >
{
public:
	CConCommandMemberAccessor< CIMEManager > ime_info;

	// Platform backend, created by InitForWindow
	IMEManagerBase *m_pUnk058;

	// Read by IsIMESupported
	bool m_bUnk060;

	// Read by IsIMEEnabled
	bool m_bUnk061;
};

#endif // IIMEMANAGER_H
