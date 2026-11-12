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

struct RenderDeviceInfo_t;
class IDebugVisualizerMgr;
class ISwitchLoopModeStatusNotify;
class IAddonListChangeNotify;
struct EventClientOutput_t;

// Entry of the table returned by IEngineService::GetServiceDependencies. A NULL name ends the table.
// The type name is not present in the binaries.
struct EngineServiceDependency_t
{
	const char *m_pszServiceName;
	// The engine services use the values 1 to 5; their meaning is not recovered.
	int m_nFlags;
};

abstract_class IEngineService : public IAppSystem
{
public:
	// Returns NULL when the service has no dependencies.
	virtual const EngineServiceDependency_t *GetServiceDependencies( void ) = 0;
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
	// The nId passed to SwitchToLoop for the active loop.
	uint32 m_nId;
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
	// Info of the engine swap chain, as returned by IRenderDevice::GetSwapChainInfo.
	virtual const RenderDeviceInfo_t *GetEngineDeviceInfo( void ) const = 0;
	virtual int			GetEngineDeviceWidth( void ) const = 0;
	virtual int			GetEngineDeviceHeight( void ) const = 0;
	virtual void		GetEngineSwapChainSize( int *pWidth, int *pHeight ) const = 0;
	virtual bool		IsLoopSwitchQueued( void ) const = 0;
	virtual bool		IsLoopSwitchRequested( void ) const = 0;
	// Set by ExitMainLoop.
	virtual bool		IsMainLoopExitRequested( void ) const = 0;
	virtual CEventDispatcher<CEventIDManager_Default> *GetEventDispatcher( void ) = 0;
	virtual IDebugVisualizerMgr *GetDebugVisualizerMgr( void ) = 0;
	virtual int			GetActiveLoopClientServerMode( void ) const = 0;
	virtual void		PrintStatus( void ) = 0;
	// Writes m_bValid = false when no loop is active.
	virtual void		GetActiveLoop( ActiveLoopInfo_t &info ) = 0;
	virtual bool		IsLoadingLevel( void ) const = 0;
	virtual bool		IsInGameLoop( void ) const = 0;
	virtual void		OnFrameRenderingFinished( const EventClientOutput_t & ) = 0;
	virtual void		ChangeVideoMode( RenderDeviceInfo_t & ) = 0;
	// Returns the mode passed to ChangeVideoMode while the change is pending, NULL otherwise.
	virtual RenderDeviceInfo_t *GetVideoModeChange( void ) = 0;
	virtual int			GetAddonCount( void ) const = 0;
	virtual const char	*GetAddon( int ) const = 0;
	virtual bool		IsAddonMounted( const char * ) const = 0;
	virtual const char	*GetAddonsString( void ) const = 0;
	// External addons are tracked separately from the mounted addon list.
	virtual void		AddExternalAddon( const char *pszAddon ) = 0;
	virtual bool		IsExternalAddon( const char *pszAddon ) const = 0;
	// True when at least one mounted addon is not an external addon.
	virtual bool		HasNonExternalAddons( void ) const = 0;
	virtual void		InstallSwitchLoopModeStatusNotify( ISwitchLoopModeStatusNotify * ) = 0;
	virtual void		UninstallSwitchLoopModeStatusNotify( ISwitchLoopModeStatusNotify * ) = 0;
	virtual void		InstallAddonListChangeNotify( IAddonListChangeNotify * ) = 0;
	virtual void		UninstallAddonListChangeNotify( IAddonListChangeNotify * ) = 0;
	virtual void		StartEngineWatchdogThread( void ) = 0;
	virtual void		AddLogCaptureString( const char * ) = 0;
	virtual void		AddLogCaptureStringV( const char *pFormat, va_list args ) = 0;
	virtual void		AddLogCaptureStringF( const char *pFormat, ... ) = 0;
	// Number of prerequisites added through RegisterPrerequisite.
	virtual int			GetPrerequisiteCount( void ) const = 0;
	virtual void		ExitMainLoop( void ) = 0;
	virtual void		RegisterPrerequisite( IPrerequisite * ) = 0;

	// Forwards to the localize system.
	virtual const char *LookupLocalizationToken( const char *pszTokenName ) = 0;

	// Same methods as IVEngineServer2
	virtual void		SetFrameTimeAmnesty( const char *amnesty, int, float frametime ) = 0;
	virtual const char *GetFrameTimeAmnesty( bool check_cvar ) = 0;
	virtual void		SetFramePerformanceTag( const char *pszTag, int nValue, int, float flDuration ) = 0;
};

#endif // IENGINESERVICE_H
