//===== Copyright (c) 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: Hosts the Workshop Tools: tool list, tool loading, frame loop and shared tool settings
//
//===========================================================================//

#ifndef ITOOLFRAMEWORK2_H
#define ITOOLFRAMEWORK2_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "tier0/bufferstring.h"
#include "tier0/utlstring.h"
#include "tier0/threadtools.h"
#include "tier1/utlvector.h"
#include "appframework/iappsystem.h"
#include "tier3/tier3.h"

class QObject;
class KeyValues3;
class IToolFrameListener;
class CEnvironmentBlock;
class CEngineToolInfo;
class CExternalToolInfo;
class IToolFrameworkTabletInfo;
class CQToolFrameworkApplication;
class CJoystickInput;
class CFGDFromAssetRegistry;

abstract_class IToolFramework2 : public IAppSystem
{
public:
	// Starts the tools stall monitor thread
	virtual void unk011() = 0;

	// Fills pTools with the engine tools this installation may run
	virtual void unk012( CUtlVector< CEngineToolInfo * > *pTools ) = 0;
	virtual void unk013( uint8 nToolIndex ) = 0;

	virtual void QueueToolUnload( uint8 nToolIndex ) = 0;

	// False for a tool that is queued for unloading
	virtual bool IsToolLoaded( uint8 nToolIndex ) = 0;
	virtual void unk016( uint8 nToolIndex ) = 0;

	virtual void Tools_RunFrame() = 0;
	virtual void UnloadQueuedTools() = 0;

	// Returns the first non-zero answer of the loaded tool systems
	virtual void *unk019() = 0;

	virtual void AddToolFrameListener( IToolFrameListener *pListener ) = 0;
	virtual void RemoveToolFrameListener( IToolFrameListener *pListener ) = 0;

	// Matches either of the two names of a tool, case-insensitive
	virtual uint8 *FindTool( uint8 *pResult, const char *pName ) = 0;
	virtual const char *GetToolName( uint8 nToolIndex ) = 0;

	virtual void *LoadTool( uint8 nToolIndex ) = 0;

	virtual CEngineToolInfo *GetEngineToolInfo( uint8 nToolIndex ) = 0;
	virtual CExternalToolInfo *GetExternalToolInfo( uint8 nExternalToolIndex ) = 0;

	// Open assets in a tool and warn about assets no tool handles
	virtual void unk027( void *pAsset, uint8 nToolIndex ) = 0;
	virtual void unk028( void *pAsset, uint8 nToolIndex ) = 0;
	virtual void unk029( void *pAssets, uint8 nToolIndex ) = 0;
	virtual void unk030( void *pAssets ) = 0;
	virtual void unk031( void *pAssets, uint8 nToolIndex ) = 0;

	virtual void ExecuteCommand( const char *pCommand ) = 0;

	// Enabled only while MSAA is supported
	virtual bool IsMSAAEnabled() = 0;
	virtual void SetMSAAEnabled( bool bEnabled ) = 0;

	virtual void SetSplashScreenMessage( const char *pMessage ) = 0;

	virtual IToolFrameworkTabletInfo *GetTabletInfo() = 0;
	virtual CEnvironmentBlock *unk037() = 0;

	// Adds a colored text label
	virtual void unk038( const char *pText, int nUnknown ) = 0;
	// Reads the joystick raw input into the state
	virtual bool unk039( void *pState ) = 0;
	virtual void *unk040() = 0;

	// Forwards both values to every loaded tool system
	virtual void unk041( uint8 nUnknown1, uint8 nUnknown2 ) = 0;

	virtual void MarkSettingsDirty() = 0;

	virtual bool IsMSAASupported() = 0;
	virtual bool unk044( uint8 nUnknown ) = 0;

	virtual const char *GetDeveloperHelpURL() = 0;
	virtual void ShowAboutDialog() = 0;

	// Selects the file in Explorer
	virtual void unk047( const char *pPath ) = 0;

	// Shell "open" of the path
	virtual void unk048( const char *pPath ) = 0;
	virtual void *unk049() = 0;

	// Shared tool settings, saved under "toolframework"
	virtual void SetAllowFlyMode2( bool bAllow ) = 0;
	virtual bool GetAllowFlyMode2() = 0;
	virtual void SetHideCursorDuringCameraManipulation( bool bHide ) = 0;
	virtual bool GetHideCursorDuringCameraManipulation() = 0;

	// True when not running on a customer machine
	virtual bool unk054() = 0;
	virtual void unk055( void *p ) = 0;

	virtual void LaunchBuildWatch() = 0;

	// "Render All Windows" in the tools menu
	virtual bool GetAlwaysRenderAllWindows() = 0;
	virtual void SetAlwaysRenderAllWindows( bool bAlways ) = 0;
	virtual void *unk059() = 0;

	virtual void unk060( QObject *pWindow ) = 0;
	virtual void unk061() = 0;

	// Settings files; pUnk selects the file the path is built from
	virtual void LoadSettings( void *pUnk, KeyValues3 *pSettings ) = 0;
	virtual void SaveSettings( void *pUnk, KeyValues3 *pSettings ) = 0;
	virtual void GetSettingsPath( void *pUnk, CBufferString *pPath ) = 0;

	// Copies the settings to the next free numbered backup
	virtual void BackupSettings() = 0;

	virtual uint8 *unk066( uint8 *pResult, void *pAsset ) = 0;
	virtual uint8 *unk067( uint8 *pResult, void *pFile ) = 0;
	virtual uint8 *unk068( uint8 *pResult, void *pAssetType ) = 0;
	virtual void unk069( void *pAsset, void *pTools ) = 0;

	virtual void *unk070( void *p1, void *p2 ) = 0;

	// Notify the listeners added with unk076 when the value changes
	virtual void SetAutoMuteGameWhenToolsHaveFocus( bool bMute ) = 0;
	virtual bool GetAutoMuteGameWhenToolsHaveFocus() = 0;
	virtual void SetToolsAudioMuted( bool bMuted ) = 0;
	virtual bool GetToolsAudioMuted() = 0;
	// pbMuteGame from the auto mute setting and the engine focus, pbMuteTools from the tools mute
	// setting while no unk078 request is active
	virtual bool unk075( bool *pbMuteGame, bool *pbMuteTools ) = 0;
	virtual void unk076( void *pListener ) = 0;
	virtual void unk077( void *pListener ) = 0;

	// Increments a counter for true, decrements it for false
	virtual void unk078( bool bPush ) = 0;

	virtual void unk079( const char *pToolName, void *pRequest ) = 0;

	// Queues the pair under a lock
	virtual void unk080( const char *pUnk1, void *pUnk2 ) = 0;

	virtual void *unk081( uint8 nToolIndex ) = 0;

	// Saves the tool's settings
	virtual void unk082( uint8 nToolIndex, void *pUnk ) = 0;
	virtual void unk083( void *p1, void *p2, void *p3 ) = 0;
	virtual void unk084( void *p1, void *p2, void *p3 ) = 0;

	virtual void ShowImportWizard( void *pUnk ) = 0;

	virtual void RegisterResponseListener( void *pListener ) = 0;
	virtual void UnregisterResponseListener( void *pListener ) = 0;

	// Forwards the arguments to every response listener
	virtual void unk088( uint32 nUnk1, uint8 nUnk2, void *pUnk3, void *pUnk4, void *pUnk5, void *pUnk6 ) = 0;

	virtual bool CanToolHighlightSubassets( uint8 nToolIndex ) = 0;

	// Executes "quit"
	virtual void Quit() = 0;

	virtual void unk091() = 0;
	virtual void unk092() = 0;
	virtual void *unk093() = 0;
	virtual void *unk094() = 0;
	virtual bool unk095( void *p ) = 0;
	virtual const char *unk096() = 0;
	virtual void unk097( bool bUnknown ) = 0;
};

// Also derives from IToolServices (exposed as "ToolServices_002"), a base without a vtable
// whose data starts at m_pUnk028; its extent is unknown, so its members are listed here.
class CToolFramework2 : public CTier3AppSystem< IToolFramework2 >
{
public:
	// Tool descriptor list
	void *m_pUnk028;

	// Vector of 32-byte entries of the loaded tool systems
	uint8 m_unk030[ 0x18 ];

	uint8 m_unk048[ 0x18 ];
	uint8 m_unk060[ 0x18 ];
	CQToolFrameworkApplication *m_pUnk078;

	// Settings, saved under the matching snake_case keys
	bool m_bMultisample;
	bool m_bAlwaysRenderAllWindows;
	bool m_bAllowFlyMode2;
	bool m_bHideCursorDuringCameraManipulation;
	bool m_bToolsAudioMuted;
	bool m_bAutoMuteGameWhenToolsHaveFocus;

	// Set by MarkSettingsDirty, cleared when the settings are saved
	bool m_bSettingsDirty;

	bool m_bUnk087;
	uint8 m_nUnk088;

	// Tools queued for unloading
	CUtlVector< uint8 > m_vecUnk090;

	// CEnvironmentBlock returned by unk037
	uint8 m_unk0A8[ 0x30 ];

	void *m_pUnk0D8;
	void *m_pUnk0E0;
	uint8 m_unk0E8[ 0x18 ];
	int m_nUnk100;
	// Created on the first unk039 call
	CJoystickInput *m_pUnk108;
	uint8 m_unk110[ 0x10 ];
	CBufferStringN< 200 > m_sUnk120;

	// QPointer< QObject > of unk060
	uint8 m_unk1F0[ 0x10 ];

	bool m_bUnk200;

	// Stall monitor thread
	ThreadHandle_t *m_pUnk208;

	int m_nUnk210;
	uint8 m_unk214[ 4 ];
	uint8 m_unk218[ 0x18 ];
	uint8 m_unk230[ 0x18 ];
	bool m_bUnk248;
	CFGDFromAssetRegistry *m_pUnk250;
	CBufferString m_sUnk258;
	uint8 m_unk268[ 0x28 ];
	CUtlString m_sUnk290;
	int m_nUnk298;
	bool m_bUnk29C;
};

#endif // ITOOLFRAMEWORK2_H
