//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#ifndef IENGINESERVICE_H
#define IENGINESERVICE_H
#ifdef _WIN32
#pragma once
#endif

#include <appframework/iappsystem.h>
#include <inputsystem/inputenums.h>
#include <iloopmode.h>
#include <localize/ilocalize.h>

#include "tier4/tier4.h"
#include "engine2/iengineservicemgr.h"

#include "schemacompiler/schemaon.h"

struct RenderDeviceInfo_t;
class ISwitchLoopModeStatusNotify;
class IAddonListChangeNotify;
class CDebugVisualizerMgr;

schema struct EventClientOutput_t
{
	DECLARE_SCHEMA_DATA_CLASS( EventClientOutput_t );

	EngineLoopState_t m_LoopState;
	float m_flRenderTime;
	float m_flRealTime;
	float m_flRenderFrameTimeUnbounded;
	bool m_bRenderOnly;
};

abstract_class IEngineService : public IAppSystem
{
public:
	virtual const EngineServiceDependencies_t *GetServiceDependencies( void ) = 0;
	virtual const char	*GetName( void ) const = 0;
	virtual EngineServiceActivateType_t ShouldActivate( const char * ) = 0;
	virtual void		OnLoopActivate( const EngineLoopState_t &loopState, CEventDispatcher<CEventIDManager_Default> *pEventDispatcher) = 0;
	virtual void		OnLoopDeactivate( const EngineLoopState_t &loopState, CEventDispatcher<CEventIDManager_Default> *pEventDispatcher) = 0;
	virtual bool		IsActive( void ) const = 0;
	virtual void		SetActive( bool ) = 0;
	virtual void		SetName( const char *pszName ) = 0;
	virtual void		RegisterEventMap( CEventDispatcher<CEventIDManager_Default> *pEventDispatcher, EventMapRegistrationType_t nRegistrationType ) = 0;
	virtual uint16		GetServiceIndex( void ) = 0;
	virtual void		SetServiceIndex( uint16 index ) = 0;
	virtual				~IEngineService() {}
};

struct ActiveLoopInfo_t
{
	CUtlString m_LoopName;
	CUtlString m_AddonName;
	uint32 m_unRequestId;
	KeyValues *m_pLoopOptions;
	bool m_bValid;
};

abstract_class IEngineServiceMgr : public IAppSystem
{
public:
	virtual void		RegisterEngineService( const char *psServiceName, IEngineService *pService ) = 0;
	virtual void		UnregisterEngineService( const char *pszServiceName, IEngineService *pService ) = 0;
	virtual void		RegisterLoopMode( const char *pLoopModeName, ILoopModeFactory *pLoopModeFactory, void **ppGlobalPointer ) = 0;
	virtual void		UnregisterLoopMode( const char *pLoopModeName, ILoopModeFactory *pLoopModeFactory, void **ppGlobalPointer ) = 0;
	virtual void		SwitchToLoop( const char *pszLoopModeName, KeyValues *pkvLoopOptions, uint32 nId, const char *pszAddonName, bool ) = 0;
	virtual const char		*GetActiveLoopName( void ) const = 0;
	virtual IEngineService	*FindService( const char * ) = 0;
	virtual PlatWindow_t	GetEngineWindow( void ) const = 0;
	virtual SwapChainHandle_t	GetEngineSwapChain( void ) const = 0;
	virtual InputContextHandle_t GetEngineInputContext( void ) const = 0;
	// Swap chain info of the engine swap chain, from the render device
	virtual const RenderDeviceInfo_t &GetEngineDeviceInfo( void ) const = 0;
	virtual int			GetEngineDeviceWidth( void ) const = 0;
	virtual int			GetEngineDeviceHeight( void ) const = 0;
	virtual void		GetEngineSwapChainSize( int *pWidth, int *pHeight ) const = 0;
	virtual bool		IsLoopSwitchQueued( void ) const = 0;
	virtual bool		IsLoopSwitchRequested( void ) const = 0;
	// Set by ExitMainLoop; in tools mode the tool framework may still cancel the exit
	virtual bool		IsExitMainLoopRequested( void ) const = 0;
	virtual CEventDispatcher<CEventIDManager_Default> *GetEventDispatcher( void ) = 0;
	virtual CDebugVisualizerMgr *GetDebugVisualizerMgr( void ) = 0;
	virtual int			GetActiveLoopClientServerMode( void ) const = 0;
	virtual void		PrintStatus( void ) = 0;
	// Writes m_bValid = false when no loop is active.
	virtual void		GetActiveLoop( ActiveLoopInfo_t &info ) = 0;
	virtual bool		IsLoadingLevel( void ) const = 0;
	virtual bool		IsInGameLoop( void ) const = 0;
	virtual void		OnFrameRenderingFinished( bool, const EventClientOutput_t & ) = 0;
	virtual void		ChangeVideoMode( RenderDeviceInfo_t & ) = 0;
	virtual void		GetVideoModeChange( void ) = 0;
	virtual int			GetAddonCount( void ) const = 0;
	virtual const char	*GetAddon( int ) const = 0;
	virtual bool		IsAddonMounted( const char * ) const = 0;
	virtual const char	*GetAddonsString( void ) const = 0;
	virtual void		AddExternalAddon( const char *pszAddon ) = 0;
	virtual bool		IsExternalAddon( const char *pszAddon ) const = 0;
	// Whether a mounted addon is not in the external addon list
	virtual bool		HasNonExternalAddons( void ) const = 0;
	virtual void		InstallSwitchLoopModeStatusNotify( ISwitchLoopModeStatusNotify * ) = 0;
	virtual void		UninstallSwitchLoopModeStatusNotify( ISwitchLoopModeStatusNotify * ) = 0;
	virtual void		InstallAddonListChangeNotify( IAddonListChangeNotify * ) = 0;
	virtual void		UninstallAddonListChangeNotify( IAddonListChangeNotify * ) = 0;
	virtual void		StartEngineWatchdogThread( void ) = 0;
	virtual void		AddLogCaptureString( const char * ) = 0;
	virtual void		AddLogCaptureStringV( const char *pFormat, va_list args ) = 0;
	virtual void		AddLogCaptureStringF( const char *pFormat, ... ) = 0;
	// Registered prerequisites the main loop has not finished yet
	virtual int			GetPendingPrerequisiteCount( void ) const = 0;
	virtual void		ExitMainLoop( void ) = 0;
	virtual void		RegisterPrerequisite( IPrerequisite * ) = 0;

	// Forwards to the localize system.
	virtual const char *LookupLocalizationToken( const char *pszTokenName ) = 0;

	// Same methods as IVEngineServer2
	virtual void		SetFrameTimeAmnesty( const char *amnesty, int, float frametime ) = 0;
	virtual const char *GetFrameTimeAmnesty( bool check_cvar ) = 0;
	virtual void		SetFramePerformanceTag( const char *pszTag, int nValue, int, float flDuration ) = 0;
};

#include "schemacompiler/schemaoff.h"

#endif // IENGINESERVICE_H
