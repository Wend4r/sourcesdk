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

class KeyValues3;
class CUtlBuffer;
class IToolFrameListener;
class CEnvironmentBlock;
class CEngineToolInfo;
class CExternalToolInfo;
class IToolFrameworkTabletInfo;
class CQToolFrameworkApplication;
class CJoystickInput;
class CFGDFromAssetRegistry;
class CToolsConfig;
class CQInputBindManager;
class CSingleAppInstance;
class QWidget;
class QVBoxLayout;
class IToolSystem2;
class IAsset;
class IAssetType;
class IFGDFromAssetListener;
class IToolFrameworkAudioSettingsListener;

abstract_class IToolFramework2 : public IAppSystem
{
public:
	// Starts the tools stall monitor thread
	virtual void unk011() = 0;

	// Fills pTools with the engine tools this installation may run
	virtual void unk012( CUtlVector< CEngineToolInfo * > *pTools ) = 0;

	// Loads the tool and activates it
	virtual void LaunchEngineTool( uint8 nToolIndex ) = 0;

	virtual void QueueToolUnload( uint8 nToolIndex ) = 0;

	// False for a tool that is queued for unloading
	virtual bool IsToolLoaded( uint8 nToolIndex ) = 0;

	// Defers LaunchEngineTool to the next Tools_RunFrame
	virtual void unk016( uint8 nToolIndex ) = 0;

	virtual void Tools_RunFrame() = 0;
	virtual void UnloadQueuedTools() = 0;

	// Returns the first non-zero answer of the loaded tool systems
	virtual int unk019() = 0;

	virtual void AddToolFrameListener( IToolFrameListener *pListener ) = 0;
	virtual void RemoveToolFrameListener( IToolFrameListener *pListener ) = 0;

	// Matches either of the two names of a tool, case-insensitive
	virtual uint8 *FindTool( uint8 *pResult, const char *pName ) = 0;
	virtual const char *GetToolName( uint8 nToolIndex ) = 0;

	// Loads the tool module and connects its "ToolSystem2_001" interface
	virtual IToolSystem2 *LoadTool( uint8 nToolIndex ) = 0;

	virtual CEngineToolInfo *GetEngineToolInfo( uint8 nToolIndex ) = 0;
	virtual CExternalToolInfo *GetExternalToolInfo( uint8 nExternalToolIndex ) = 0;

	// Overloads that open assets in a tool and warn about assets no tool handles; 0xFF picks the tool
	// by asset type. Declared in the Itanium order: MSVC places them in reverse, so the last one is
	// the first slot there. The asset entries are 24 bytes and start with the IAsset; pAssetEntries
	// is a CUtlVector of them
	virtual void unk027( void *pAssetEntries, uint8 nToolIndex ) = 0;

	// Opens the assets the object's ninth method lists
	virtual void unk027( void *pAssets ) = 0;

	virtual void unk027( const CUtlVector< IAsset * > *pAssets, uint8 nToolIndex ) = 0;
	virtual void unk027( const void *pAssetEntry, uint8 nToolIndex ) = 0;
	virtual void unk027( IAsset *pAsset, uint8 nToolIndex ) = 0;

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
	// Reloads tools/stylesheets/qtabbedstyle_colors.vsc and applies the style
	virtual void ReloadTabbedStyle() = 0;

	// Forwards both values to every loaded tool system
	virtual void unk041( bool bUnknown1, bool bUnknown2 ) = 0;

	virtual void MarkSettingsDirty() = 0;

	virtual bool IsMSAASupported() = 0;
	// Whether the multisample mode is available, asked by the options dialog
	virtual bool unk044( uint8 nMultisampleMode ) = 0;

	virtual const char *GetDeveloperHelpURL() = 0;
	virtual void ShowAboutDialog() = 0;

	// Selects the file in Explorer
	virtual void OpenFolderInExplorer( const char *pFullPath ) = 0;

	// Shell "open" of the path
	virtual void unk048( const char *pPath ) = 0;

	// Opens the file in the external tool registered for its extension, otherwise like unk048
	virtual void unk049( const char *pPath, int nUnknown ) = 0;

	// Shared tool settings, saved under "toolframework"
	virtual void SetAllowFlyMode2( bool bAllow ) = 0;
	virtual bool GetAllowFlyMode2() = 0;
	virtual void SetHideCursorDuringCameraManipulation( bool bHide ) = 0;
	virtual bool GetHideCursorDuringCameraManipulation() = 0;

	// True when not running on a customer machine
	virtual bool unk054() = 0;

	// pParentWindow is the native handle of the window the console is opened from
	virtual void OpenVConsole( void *pParentWindow ) = 0;

	virtual void LaunchBuildWatch() = 0;

	// "Render All Windows" in the tools menu
	virtual bool GetAlwaysRenderAllWindows() = 0;
	virtual void SetAlwaysRenderAllWindows( bool bAlways ) = 0;
	virtual CQInputBindManager *unk059() = 0;

	// Remembers the tool window and activates the engine window
	virtual void unk060( QWidget *pWindow ) = 0;

	// Activates and raises the remembered tool window
	virtual void unk061() = 0;

	// <settings dir>/<pName>/settings.txt; bUnknown adds a prefix to the file name
	virtual void LoadSettings( const char *pName, CUtlBuffer &buf, bool bUnknown ) = 0;
	virtual void SaveSettings( const char *pName, CUtlBuffer &buf, bool bUnknown ) = 0;
	virtual void GetSettingsPath( const char *pName, CBufferString *pPath, bool bUnknown ) = 0;

	// Copies the settings to the next free numbered backup
	virtual void BackupSettings() = 0;

	// Engine tool by asset type, external tool by file extension; 0xFF when there is none
	virtual uint8 *unk066( uint8 *pResult, IAsset *pAsset ) = 0;
	virtual uint8 *unk067( uint8 *pResult, IAsset *pAsset ) = 0;
	virtual uint8 *unk068( uint8 *pResult, IAssetType *pAssetType ) = 0;

	// Every engine tool or external tool that handles the asset
	virtual void unk069( IAsset *pAsset, CUtlVector< uint8 > *pTools ) = 0;
	virtual void unk070( IAsset *pAsset, CUtlVector< uint8 > *pExternalTools ) = 0;

	// Notify the audio settings listeners when the value changes
	virtual void SetAutoMuteGameWhenToolsHaveFocus( bool bMute ) = 0;
	virtual bool GetAutoMuteGameWhenToolsHaveFocus() = 0;
	virtual void SetToolsAudioMuted( bool bMuted ) = 0;
	virtual bool GetToolsAudioMuted() = 0;

	// pbMuteGame from the auto mute setting and the engine focus, pbMuteTools from the tools mute
	// setting while no unk078 request is active
	virtual bool unk075( bool *pbMuteGame, bool *pbMuteTools ) = 0;

	virtual void AddAudioSettingsListener( IToolFrameworkAudioSettingsListener *pListener ) = 0;
	virtual void RemoveAudioSettingsListener( IToolFrameworkAudioSettingsListener *pListener ) = 0;

	// Increments a counter for true, decrements it for false; the tools are not muted while it is set
	virtual void unk078( bool bPush ) = 0;

	// Loads the tool and passes it the request; called by the "tool_request" console command and
	// the "tool_request" VToolMessage
	virtual void unk079( const char *pToolName, KeyValues3 *pRequest ) = 0;

	// Queues the request for unk079, dispatched by Tools_RunFrame
	virtual void unk080( const char *pToolName, KeyValues3 *pRequest ) = 0;

	virtual KeyValues3 *unk081( uint8 nToolIndex ) = 0;

	// Saves the tool's settings
	virtual void unk082( uint8 nToolIndex, KeyValues3 *pSettings ) = 0;

	// Load and save the named settings file as KV3, see LoadSettings
	virtual void unk083( const char *pName, KeyValues3 *pSettings, bool bUnknown ) = 0;
	virtual void unk084( const char *pName, KeyValues3 *pSettings, bool bUnknown ) = 0;

	virtual void ShowImportWizard( void *pUnk ) = 0;

	virtual void RegisterResponseListener( void *pListener ) = 0;
	virtual void UnregisterResponseListener( void *pListener ) = 0;

	// Forwards the arguments to every response listener
	virtual void unk088( uint32 nUnk1, uint8 nUnk2, void *pUnk3, void *pUnk4, void *pUnk5, void *pUnk6 ) = 0;

	virtual bool CanToolHighlightSubassets( uint8 nToolIndex ) = 0;

	// Executes "quit"
	virtual void Quit() = 0;

	virtual void AddFGDFromAssetListener( IFGDFromAssetListener *pListener ) = 0;
	virtual void RemoveFGDFromAssetListener( IFGDFromAssetListener *pListener ) = 0;
	virtual void *unk093() = 0;
	virtual void *unk094() = 0;
	virtual bool unk095( void *p ) = 0;
	virtual const char *unk096() = 0;
	// Increments for true, decrements for false the application counter that suppresses Qt shortcut events
	virtual void unk097( bool bPush ) = 0;
};

// Also derives from IToolServices (exposed as "ToolServices_002"), a base without a vtable
// whose data starts at m_pToolsConfig; its extent is unknown, so its members are listed here.
class CToolFramework2 : public CTier3AppSystem< IToolFramework2 >
{
public:
	// Engine and external tools, loaded from bin/enginetools.txt
	CToolsConfig *m_pToolsConfig;

	// One 32-byte entry per engine tool: its settings (KeyValues3, returned by unk081), the
	// IToolSystem2 returned by LoadTool and a flag set once loading was attempted
	uint8 m_unk030[ 0x18 ];

	CUtlVector< IToolFrameListener * > m_ToolFrameListeners;
	CUtlVector< IToolFrameworkAudioSettingsListener * > m_AudioSettingsListeners;
	CQToolFrameworkApplication *m_pApplication;

	// Settings, saved under the matching snake_case keys
	bool m_bMultisample;
	bool m_bAlwaysRenderAllWindows;
	bool m_bAllowFlyMode2;
	bool m_bHideCursorDuringCameraManipulation;
	bool m_bToolsAudioMuted;
	bool m_bAutoMuteGameWhenToolsHaveFocus;

	// Set by MarkSettingsDirty, cleared when the settings are saved
	bool m_bSettingsDirty;

	// Tool deferred by unk016, launched by Tools_RunFrame; 0xFF when none
	bool m_bUnk087;
	uint8 m_nUnk088;

	// Tools queued for unloading
	CUtlVector< uint8 > m_ToolsToUnload;

	// CEnvironmentBlock returned by unk037, captured by Connect
	uint8 m_unk0A8[ 0x30 ];

	// Labels added by unk038 to the loading splash screen: the widget, its layout and the 24-byte
	// entries (QLabel, the int argument, the text kept until the widget exists)
	QWidget *m_pUnk0D8;
	QVBoxLayout *m_pUnk0E0;
	uint8 m_unk0E8[ 0x18 ];

	int m_nUnk100;

	// Created on the first unk039 call
	CJoystickInput *m_pJoystickInput;

	// Returned by unk059
	CQInputBindManager *m_pInputBindManager;

	// Warns about other tools instances
	CSingleAppInstance *m_pSingleAppInstance;

	// GAMEROOT path, the root of the settings files
	CBufferStringN< 200 > m_sGameRoot;

	// QPointer< QWidget > of unk060
	uint8 m_unk1F0[ 0x10 ];

	// Stall monitor, started by unk011
	bool m_bStallMonitorEnabled;
	ThreadHandle_t *m_pStallMonitorThread;

	// Counter of unk078
	int m_nUnk210;
	uint8 m_unk214[ 4 ];

	// "VToolMessage" notification channel
	uint8 m_unk218[ 0x18 ];

	// Listeners of RegisterResponseListener
	CUtlVector< void * > m_ResponseListeners;

	// Set by BackupSettings: later saves are blocked
	bool m_bBlockSettingsSave;

	CFGDFromAssetRegistry *m_pFGDFromAssetRegistry;
	CBufferString m_sUnk258;

	// 24-byte requests queued by unk080, guarded by the mutex
	uint8 m_unk268[ 0x18 ];
	CAtomicMutex m_ToolRequestsMutex;

	// Command line options "-generatethumbnails", "-generatethumbnailssubstr",
	// "-generatethumbnailsmaxtime" and "-generatethumbnailsclear": Tools_RunFrame passes them to
	// IAssetPreviewSystem::GenerateThumbnails and quits, and no deferred tool is launched meanwhile
	bool m_bGenerateThumbnails;
	CUtlString m_sGenerateThumbnailsSubstr;
	int m_nGenerateThumbnailsMaxTime;
	bool m_bGenerateThumbnailsClear;
};

#endif // ITOOLFRAMEWORK2_H
