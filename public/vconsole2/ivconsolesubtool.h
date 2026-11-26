//===== Copyright (c) 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: A tool window hosted by VConsole2 ( built in, or loaded from a subtool module )
//
//===========================================================================//

#ifndef IVCONSOLESUBTOOL_H
#define IVCONSOLESUBTOOL_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"

class QAction;
class KeyValues;
class IVConsoleToolServices;
class IVConsoleDevice;

// VConsole2 keeps one record per subtool of tools/vconsole_subtools.txt; a device is a connected application
abstract_class IVConsoleSubTool
{
public:
	// Called once after the subtool is created: pToolServices is VConsole2's services object, pAction the
	// subtool's menu action
	virtual void Init( IVConsoleToolServices *pToolServices, QAction *pAction ) = 0;
	virtual void Shutdown() = 0;

	// "OpenSubTool": shows the subtool's window; pArgs holds the open arguments
	virtual bool Open( KeyValues *pArgs ) = 0;

	// Named by what they do: the subtool's settings, saved into the "SubTools" group of VConsole2's settings
	// ( the subtool keeps ownership ) and handed back when the subtool is created again
	virtual KeyValues *GetSettings() = 0;
	virtual void RestoreSettings( KeyValues *pSettings ) = 0;

	// Named by what they do: a device connects ( called for every connected device after Init too ),
	// disconnects, becomes the active device, or changes its connection state
	virtual void OnDeviceAdded( IVConsoleDevice *pDevice ) = 0;
	virtual void OnDeviceRemoved( IVConsoleDevice *pDevice ) = 0;
	virtual void SetActiveDevice( IVConsoleDevice *pDevice ) = 0;
	virtual void OnDeviceStateChanged( IVConsoleDevice *pDevice ) = 0;

	// Named by what they do: VConsole2's main loop calls PreFrame before it processes the frame, RunFrame
	// after it, and PostFrame after the Qt events of the frame
	virtual void PreFrame() = 0;
	virtual void RunFrame() = 0;
	virtual void PostFrame() = 0;

	// Named by what it does: false makes VConsole2 ask before closing ( "You have data or important state in
	// the ... Tool" )
	virtual bool CanClose() = 0;

	// Named by what it does: whether the window is shown; visible subtools start with VConsole2
	// ( "SubToolsAutoStart" )
	virtual bool IsVisible() = 0;

	// Named by what it does: a message from another subtool ( or from VConsole2 itself when pSender is NULL )
	virtual void HandleSubToolMessage( IVConsoleSubTool *pSender, int nMessage, void *pData ) = 0;

	// Named by what it does: VConsole2 sleeps between frames only while every subtool answers true
	virtual bool IsIdle() = 0;
};

#endif // IVCONSOLESUBTOOL_H
