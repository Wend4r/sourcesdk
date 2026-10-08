//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: Client side of the game UI driven by the engine
//
//===========================================================================//

#ifndef ILEGACYGAMEUI_H
#define ILEGACYGAMEUI_H

#ifdef _WIN32
#pragma once
#endif

#include "appframework/iappsystem.h"
#include "network_connection.pb.h"

class KeyValues;

#define LEGACYGAMEUI_INTERFACE_VERSION "LegacyGameUI001"

abstract_class ILegacyGameUI : public IAppSystem
{
public:
	virtual void PostInit() = 0;
	virtual void Start() = 0;
	virtual void RunFrame() = 0;
	virtual void OnPanoramaUIStartup() = 0;

	virtual void OnGameUIActivated() = 0;
	virtual void OnGameUIHidden() = 0;

	virtual void OnLevelLoadingStarted( const char *pszLevelName, KeyValues *pLoadingOptions, bool bShowProgressDialog ) = 0;
	virtual void OnLevelLoadingFinished() = 0;
	virtual bool UpdateProgressBar( float flProgress, const char *pszStatusText ) = 0;
	// Called with the map name when a non-dedicated server starts
	virtual void SetProgressLevelName( const char *pszLevelName ) = 0;
	virtual void OnConnectToServer2( const char *pszGame, int nIP, int nConnectionPort, int nQueryPort ) = 0;
	virtual void *unk022() = 0;
	// Shows a popup for the Steam/VAC related reasons
	virtual void OnDisconnectFromServer( ENetworkDisconnectionReason reason ) = 0;
	virtual void NeedConnectionProblemWaitScreen( float flTimeRemaining ) = 0;
	virtual void *unk025() = 0;
	virtual void *unk026() = 0;
	// True while the main menu or the pause menu is shown
	virtual bool unk027() = 0;

	virtual void CreateCommandMsgBox( const char *pszTitle, const char *pszMessage, bool bShowOk, bool bShowCancel, const char *pszOkCommand, const char *pszCancelCommand, const char *pszClosedCommand, void *pUnk1, void *pUnk2 ) = 0;
	virtual void CreateCommandMsgBoxInSlot( int nSlot, const char *pszTitle, const char *pszMessage, bool bShowOk, bool bShowCancel, const char *pszOkCommand, const char *pszCancelCommand, const char *pszClosedCommand, void *pUnk1 ) = 0;
	virtual void unk030( const char *pszTitle, const char *pszMessage, bool bShowOk, bool bShowCancel, const char *pszOkCommand, const char *pszCancelCommand, const char *pszClosedCommand, void *pUnk1, void *pUnk2 ) = 0;
	virtual void unk031( void *pParentPanel, const char *pszTitle, const char *pszMessage, const char *pszButton1Label, const char *pszButton2Label, const char *pszButton1Command, const char *pszButton2Command, const char *pszHelpCommand ) = 0;
	// unk031 parented to the main menu
	virtual void unk032( const char *pszTitle, const char *pszMessage, const char *pszButton1Label, const char *pszButton2Label, const char *pszButton1Command, const char *pszButton2Command, const char *pszHelpCommand ) = 0;

	virtual bool unk033() = 0;
	virtual bool unk034() = 0;

	// Hides the secondary bar at 1.0
	virtual void SetSecondaryProgressBar( float flProgress ) = 0;
	virtual void SetSecondaryProgressBarText( const wchar_t *pszText ) = 0;
};

#endif // ILEGACYGAMEUI_H
