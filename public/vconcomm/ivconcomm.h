//===== Copyright (c) 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: Communication channel between the running application and VConsole
//
//===========================================================================//

#ifndef IVCONCOMM_H
#define IVCONCOMM_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "tier0/platwindow.h"
#include "tier0/utlstring.h"
#include "tier1/utlleanvector.h"
#include "appframework/iappsystem.h"

class KeyValues;

abstract_class IVConCommDataReceived
{
public:
	virtual bool OnDataReceived( uint32 nMsgType, uint16 nVersion, uint32 nSize, void *pMessage, uint32 nSourceAddress ) = 0;
};

abstract_class IVConComm : public IAppSystem
{
public:
	// Whether any VConsole connection is established
	virtual bool IsConnected() = 0;

	virtual void ProcessIncomingMessages() = 0;

	virtual bool IsVConsoleActive() = 0;

	virtual bool RegisterMessageHandler( uint32 nMsgType, uint16 nExpectedVersion, IVConCommDataReceived *pHandler ) = 0;
	virtual bool UnregisterMessageHandler( uint32 nMsgType ) = 0;

	virtual bool SendMessageToVConsole( uint32 nMsgType, uint16 nVersion, uint32 nSize, const void *pData, bool bFlag ) = 0;

	virtual int GetMaxPendingOutgoingMessages() = 0;

	// Whether an established connection comes from 127.0.0.1
	virtual bool HasLocalConnection() = 0;

	// hReturnToWindow is an OS window handle
	virtual bool MakeVConsoleForeground( void *hReturnToWindow, int nTimeout, bool bUseHostPath ) = 0;

	virtual void SetAppInfo( PlatWindow_t hWindow, const char *pszName, bool bFlag ) = 0;

	virtual void SetServicePollThreadEnabled( bool bEnabled ) = 0;

	// Whether a VConsole2 window is open on this machine
	virtual bool IsVConsoleWindowOpen() = 0;

	virtual bool OpenSubTool( const char *pszToolName, const char *pszToolArgs, void *hReturnToWindow, int nTimeout, bool bUseHostPath ) = 0;
	virtual bool OpenSubTool( const char *pszToolName, KeyValues *pToolArgs, void *hReturnToWindow, int nTimeout, bool bUseHostPath ) = 0;

	virtual void ClearConsole( bool bClearAll ) = 0;

	// Sends "vc_exit <context>" to VConsole
	virtual void NotifyExit( const char *pszShutdownContext ) = 0;

	virtual void SetAddonList( const CUtlLeanVector< CUtlString > &addons ) = 0;

	virtual bool GetConnectionInfo( uint32 nAddress, CUtlString *pUserName, bool *pbConnected ) = 0;

	virtual ~IVConComm() {}
};

#endif // IVCONCOMM_H
