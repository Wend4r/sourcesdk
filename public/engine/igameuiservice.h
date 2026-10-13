//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: Engine service for the in-game console and UI input layers
//
//=============================================================================//

#ifndef IGAMEUISERVICE_H
#define IGAMEUISERVICE_H
#ifdef _WIN32
#pragma once
#endif

#include <IEngineService.h>
#include <con_nprint.h>
#include <engine2/irenderservice.h>
#include <engine2/iinputservice.h>
#include <tier0/convar.h>
#include <rendersystem/rendersystemtypes.h>

namespace panorama
{
	class CPanel2D;
	class IUIWindow;
};

abstract_class IGameUIService : public IEngineService
{
public:
	virtual bool IsConsoleEnabled() = 0;
	// IsConsoleEnabled without con_enable
	virtual bool IsConsoleEnabledByCommandLine() = 0;

	virtual bool IsConsoleVisible() = 0;
	virtual void ToggleConsole() = 0;
	// Creates the console panel on first show
	virtual void ShowConsole( bool bShow ) = 0;

	virtual void BlockConsoleToggle( bool bBlock ) = 0;

	// Plat_FloatTime() of the last input event of type 5
	virtual double GetLastMouseMoveTime() = 0;

	virtual InputContextHandle_t GetInputContext( int nLayer ) = 0;

	virtual bool HasConsole() = 0;
	// Forwards to the console panel when it exists
	virtual void ClearConsole() = 0;

	// Wraps the window in a procedural layer, then creates its root panel through the panorama client
	virtual panorama::CPanel2D *AddPanoramaView( const char *pszName, panorama::IUIWindow *pWindow, bool bUnk1, bool bUnk2 ) = 0;
	// Returns NULL in this build
	virtual void *unk034() = 0;

	// Per layer (0 to 3) handler lists, sorted by nPriority
	virtual void AddLayerHandler( int nLayer, void *pHandler, int nPriority ) = 0;
	virtual void RemoveLayerHandler( int nLayer, void *pHandler ) = 0;

	virtual void Con_NPrintf( int nRow, const char *pszFormat, ... ) FMTFUNCTION( 3, 4 ) = 0;
	virtual void Con_NPrintf( const con_nprint_t *pInfo, const char *pszFormat, ... ) FMTFUNCTION( 3, 4 ) = 0;
};

class CGameUIService : public CBaseEngineService< IGameUIService >, public IRenderHandler
{
public:
	virtual RHBackColorBuffer_t AddViewsToSceneSystem( const EngineLoopState_t &state, double flRenderTime, double flRealTime, const RenderViewport_t &viewport, const RHBackColorBuffer_t &backColorBuffer ) override = 0;

	// Called by the panorama input handler the service installs
	virtual InputHandlerResult_t HandleInputEvent( const InputEvent_t &event, CSplitScreenSlot nSlot ) = 0;

public:
	uint8 m_unk040[ 8 ];
	CConCommandMemberAccessor< CGameUIService > m_ToggleConsoleCommand;
	CConCommandMemberAccessor< CGameUIService > m_HideConsoleCommand;
	CConCommandMemberAccessor< CGameUIService > m_ShowConsoleCommand;
	CConCommandMemberAccessor< CGameUIService > m_ClearCommand;
	CConCommandMemberAccessor< CGameUIService > m_ClearAllCommand;
	CConCommandMemberAccessor< CGameUIService > m_ConsoleToolCommand;
	uint8 m_unk1C8[ 8 ];
	void *m_pConsole;
	InputContextHandle_t m_hMouseControlContext;
	InputContextHandle_t m_hPanoramaHUDContext;
	InputContextHandle_t m_hGameUIContext;
	InputContextHandle_t m_hPanoramaUIContext;
	RenderViewport_t m_Viewport;
	double m_flLastMouseMoveTime;
	bool m_bUnk220;
	bool m_bUnk221;
	bool m_bUnk222;
	int m_nUnk224;
	int m_nBlockConsoleToggleCount;
	int m_nUnk22C;
	bool m_bUnk230;
	IInputHandler *m_pInputHandler;
	void *m_unk240;
	uint64 m_unk248[ 4 ][ 3 ];
};

#endif // IGAMEUISERVICE_H
