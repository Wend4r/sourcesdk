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
#include "tier1/keyvalues3.h"
#include "mathlib/vector.h"
#include "color.h"
#include "appframework/iappsystem.h"
#include "tier3/tier3.h"

#include "schemacompiler/schemaon.h"

class CUtlBuffer;
class CEnvironmentBlock;
class CQToolFrameworkApplication;
class CJoystickInput;
class CFGDFromAssetRegistry;
class CQInputBindManager;
class CSingleAppInstance;
class QWidget;
class QLabel;
class QVBoxLayout;
class IAsset;
class IAssetType;

//-----------------------------------------------------------------------------
// Tool list, loaded from bin/enginetools.txt
//-----------------------------------------------------------------------------
schema class CBaseToolInfo
{
public:
	TYPEMETA( MGetKV3ClassDefaults );

	CUtlString m_Name;
	CUtlString m_OverrideToolShortcutName;
	CUtlString m_FriendlyName;
	CUtlString m_ToolIcon;
};

schema class CEngineToolInfo : public CBaseToolInfo
{
public:
	TYPEMETA( MGetKV3ClassDefaults );

	CUtlString m_Library;
	CUtlString m_InterfaceName;
	bool m_bShowInRevisionSubMenu;
	bool m_bShowInToolsMenu;
	bool m_bIsSecondaryTool;
	bool m_bDoNotWarnAboutLargeAssetBatches;
	bool m_bIsWorkshopManagerTool;
	bool m_bIsWorkshopItemTool;
	bool m_bCanHighlightSubassets;
	CUtlVector< CUtlString > m_AssetTypes;
	CUtlVector< CUtlString > m_LimitToMods;
	CUtlVector< CUtlString > m_ExcludeFromMods;

	// Position in m_EngineTools, returned by IToolFramework2::FindTool
	noschema uint8 m_nToolIndex;
};

schema class CExternalToolInfo : public CBaseToolInfo
{
public:
	TYPEMETA( MGetKV3ClassDefaults );

	CUtlString m_Executable;
	CUtlString m_Args;

	// Used instead of m_Args when a line number is given
	CUtlString m_ArgsWithLineColumn;

	CUtlString m_WorkingDir;
	CUtlString m_MatchSystemExecutable;
	CUtlVector< CUtlString > m_SupportedExts;
	CUtlVector< CUtlString > m_PriorityExts;
	bool m_bDebugCommandline;
};

schema class CToolsConfig
{
public:
	TYPEMETA( MGetKV3ClassDefaults );

	CUtlVector< CEngineToolInfo > m_EngineTools;
	CUtlVector< CExternalToolInfo > m_ExternalTools;
	CUtlVector< CUtlString > m_EngineModulesThatReferenceAssets;
};

//-----------------------------------------------------------------------------
// What a tool is asked to open when it is activated
//-----------------------------------------------------------------------------
enum ToolActivationType_t
{
	// Only activate the tool
	TOOL_ACTIVATION_DEFAULT = 0,

	// Open m_Assets
	TOOL_ACTIVATION_OPEN_ASSETS,

	// Open m_Files
	TOOL_ACTIVATION_OPEN_FILES,

	// Open the single asset of m_Assets with m_pAssetContext
	TOOL_ACTIVATION_OPEN_ASSET_WITH_CONTEXT,
};

struct ToolActivationRequest_t
{
	ToolActivationType_t m_nType;
	CUtlVector< IAsset * > m_Assets;
	void *m_pAssetContext;
	CUtlVector< CUtlString > m_Files;
};

// An asset to open in a tool; a non-null m_pContext opens it with TOOL_ACTIVATION_OPEN_ASSET_WITH_CONTEXT
struct ToolOpenAssetEntry_t
{
	IAsset *m_pAsset;
	void *m_pContext;
	uint64 m_nUnk010;
};

//-----------------------------------------------------------------------------
// Interface of a tool module ("ToolSystem2_001")
//-----------------------------------------------------------------------------
abstract_class IToolSystem2 : public IAppSystem
{
public:
	// Called once the tool module is connected; false makes the framework drop the tool
	virtual bool InitToolSystem( CreateInterfaceFn factory, uint8 nToolIndex ) = 0;

	virtual void ActivateTool( const ToolActivationRequest_t &request ) = 0;

	virtual void RunFrame() = 0;

	// Asked before the engine quits: 0 lets it quit, 1 cancels the quit, any other value asks again
	// on the next frame
	virtual int RequestQuit() = 0;

	virtual void OnGameLoopActiveChanged( bool bServer, bool bActive ) = 0;

	virtual void HandleToolRequest( KeyValues3 *pRequest ) = 0;
};

//-----------------------------------------------------------------------------
// Called around the tool systems' RunFrame by Tools_RunFrame
//-----------------------------------------------------------------------------
abstract_class IToolFrameListener
{
public:
	virtual void RunFrame() = 0;
	virtual void PreFrame() = 0;
	virtual void PostFrame() = 0;
};

//-----------------------------------------------------------------------------
// Pen tablet state, updated from the Qt tablet events
//-----------------------------------------------------------------------------
abstract_class IToolFrameworkTabletInfo
{
public:
	// 1 while the pen is not down
	virtual float GetPressure() = 0;

	virtual bool IsPenDown() = 0;
	virtual bool IsEraser() = 0;

	// X and Y tilt, zero while the pen is not down
	virtual Vector2D GetTilt() = 0;
};

abstract_class IToolFrameworkAudioSettingsListener
{
public:
	virtual void OnToolsAudioSettingsChanged() = 0;
};

// Classes created from assets for the FGD; the registry keeps them in sync with their assets
abstract_class IFGDFromAssetListener
{
public:
	// Called with the classes whose assets changed
	virtual void OnFGDFromAssetClassesChanged( void **ppClasses, int nCount ) = 0;
};

// Input of a multi-axis controller (raw HID input), in units of 1/256
struct MultiAxisControllerInput_t
{
	Vector m_vecTranslation;
	Vector m_vecRotation;
};

abstract_class IToolFramework2 : public IAppSystem
{
public:
	// Starts the tools stall monitor thread
	virtual void StartStallMonitor() = 0;

	// Fills pTools with the engine tools this installation may run
	virtual void GetAvailableEngineTools( CUtlVector< CEngineToolInfo * > *pTools ) = 0;

	// Loads the tool and activates it
	virtual void LaunchEngineTool( uint8 nToolIndex ) = 0;

	virtual void QueueToolUnload( uint8 nToolIndex ) = 0;

	// False for a tool that is queued for unloading
	virtual bool IsToolLoaded( uint8 nToolIndex ) = 0;

	// Defers LaunchEngineTool to the next Tools_RunFrame; 0xFF opens the asset browser instead
	virtual void QueueToolLaunch( uint8 nToolIndex ) = 0;

	virtual void Tools_RunFrame() = 0;
	virtual void UnloadQueuedTools() = 0;

	// IToolSystem2::RequestQuit of the loaded tools: the first non-zero answer, otherwise 0
	virtual int RequestToolsQuit() = 0;

	virtual void AddToolFrameListener( IToolFrameListener *pListener ) = 0;
	virtual void RemoveToolFrameListener( IToolFrameListener *pListener ) = 0;

	// Matches either of the two names of a tool, case-insensitive
	virtual uint8 *FindTool( uint8 *pResult, const char *pName ) = 0;
	virtual const char *GetToolName( uint8 nToolIndex ) = 0;

	// Loads the tool module and connects its "ToolSystem2_001" interface
	virtual IToolSystem2 *LoadTool( uint8 nToolIndex ) = 0;

	virtual CEngineToolInfo *GetEngineToolInfo( uint8 nToolIndex ) = 0;
	virtual CExternalToolInfo *GetExternalToolInfo( uint8 nExternalToolIndex ) = 0;

	// Open assets in a tool and warn about assets no tool handles; 0xFF picks the tool by asset type.
	// Declared in the Itanium order: MSVC places the overloads in reverse, so the last one is the first
	// slot there
	virtual void OpenAssetsInTool( const CUtlVector< ToolOpenAssetEntry_t > *pEntries, uint8 nToolIndex ) = 0;

	// Opens the assets the object's ninth method lists
	virtual void OpenAssetsInTool( void *pAssetSource ) = 0;

	virtual void OpenAssetsInTool( const CUtlVector< IAsset * > *pAssets, uint8 nToolIndex ) = 0;
	virtual void OpenAssetsInTool( const ToolOpenAssetEntry_t *pEntry, uint8 nToolIndex ) = 0;
	virtual void OpenAssetsInTool( IAsset *pAsset, uint8 nToolIndex ) = 0;

	virtual void ExecuteCommand( const char *pCommand ) = 0;

	// Enabled only while MSAA is supported
	virtual bool IsMSAAEnabled() = 0;
	virtual void SetMSAAEnabled( bool bEnabled ) = 0;

	virtual void SetSplashScreenMessage( const char *pMessage ) = 0;

	virtual IToolFrameworkTabletInfo *GetTabletInfo() = 0;

	// Environment captured when the tools connected
	virtual CEnvironmentBlock *GetEnvironmentBlock() = 0;

	// Adds a bold label of that color to the loading splash screen
	virtual void AddSplashScreenLabel( const char *pText, Color color ) = 0;

	virtual bool GetMultiAxisControllerInput( MultiAxisControllerInput_t *pInput ) = 0;

	// Reloads tools/stylesheets/qtabbedstyle_colors.vsc and applies the style
	virtual void ReloadTabbedStyle() = 0;

	// Forwards both values to IToolSystem2::OnGameLoopActiveChanged of every loaded tool; called when the
	// server or client game loop mode is activated or deactivated
	virtual void NotifyGameLoopActive( bool bServer, bool bActive ) = 0;

	virtual void MarkSettingsDirty() = 0;

	virtual bool IsMSAASupported() = 0;
	virtual bool IsMultisampleModeSupported( uint8 nMultisampleMode ) = 0;

	virtual const char *GetDeveloperHelpURL() = 0;
	virtual void ShowAboutDialog() = 0;

	// Selects the file in Explorer
	virtual void OpenFolderInExplorer( const char *pFullPath ) = 0;

	// Shell "open" of the path
	virtual void OpenFile( const char *pPath ) = 0;

	// Opens the file in the external tool registered for its extension, at the line when nLine is not 0;
	// without such a tool like OpenFile
	virtual void OpenFileAtLine( const char *pPath, int nLine ) = 0;

	// Shared tool settings, saved under "toolframework"
	virtual void SetAllowFlyMode2( bool bAllow ) = 0;
	virtual bool GetAllowFlyMode2() = 0;
	virtual void SetHideCursorDuringCameraManipulation( bool bHide ) = 0;
	virtual bool GetHideCursorDuringCameraManipulation() = 0;

	virtual bool IsNotRunningOnCustomerMachine() = 0;

	// pParentWindow is the native handle of the window the console is opened from
	virtual void OpenVConsole( void *pParentWindow ) = 0;

	virtual void LaunchBuildWatch() = 0;

	// "Render All Windows" in the tools menu
	virtual bool GetAlwaysRenderAllWindows() = 0;
	virtual void SetAlwaysRenderAllWindows( bool bAlways ) = 0;

	virtual CQInputBindManager *GetInputBindManager() = 0;

	// Remembers the tool window and activates the engine window
	virtual void ActivateEngineWindow( QWidget *pToolWindow ) = 0;

	// Activates and raises the remembered tool window, otherwise the asset browser
	virtual void ReactivateToolWindow() = 0;

	// <settings dir>/<pName>/settings.txt; bModGameSubdirPrefix prefixes the file name with
	// IApplication::GetModGameSubdir() and "_"
	virtual void LoadSettings( const char *pName, CUtlBuffer &buf, bool bModGameSubdirPrefix ) = 0;
	virtual void SaveSettings( const char *pName, CUtlBuffer &buf, bool bModGameSubdirPrefix ) = 0;
	virtual void GetSettingsPath( const char *pName, CBufferString *pPath, bool bModGameSubdirPrefix ) = 0;

	// Copies the settings to the next free numbered backup
	virtual void BackupSettings() = 0;

	// Engine tool by asset type, external tool by file extension; 0xFF when there is none
	virtual uint8 *FindEngineToolForAsset( uint8 *pResult, IAsset *pAsset ) = 0;
	virtual uint8 *FindExternalToolForAsset( uint8 *pResult, IAsset *pAsset ) = 0;
	virtual uint8 *FindEngineToolForAssetType( uint8 *pResult, IAssetType *pAssetType ) = 0;

	// Every engine tool or external tool that handles the asset
	virtual void GetEngineToolsForAsset( IAsset *pAsset, CUtlVector< uint8 > *pTools ) = 0;
	virtual void GetExternalToolsForAsset( IAsset *pAsset, CUtlVector< uint8 > *pExternalTools ) = 0;

	// Notify the audio settings listeners when the value changes
	virtual void SetAutoMuteGameWhenToolsHaveFocus( bool bMute ) = 0;
	virtual bool GetAutoMuteGameWhenToolsHaveFocus() = 0;
	virtual void SetToolsAudioMuted( bool bMuted ) = 0;
	virtual bool GetToolsAudioMuted() = 0;

	// pbMuteGame from the auto mute setting and the engine focus, pbMuteTools from the tools mute
	// setting while no PushToolsAudioUnmute is active; returns *pbMuteGame
	virtual bool GetAudioMuteState( bool *pbMuteGame, bool *pbMuteTools ) = 0;

	virtual void AddAudioSettingsListener( IToolFrameworkAudioSettingsListener *pListener ) = 0;
	virtual void RemoveAudioSettingsListener( IToolFrameworkAudioSettingsListener *pListener ) = 0;

	// Increments a counter for true, decrements it for false; the tools are not muted while it is set
	virtual void PushToolsAudioUnmute( bool bPush ) = 0;

	// Loads the tool and passes the request to IToolSystem2::HandleToolRequest; used by the
	// "tool_request" console command and VToolMessage. Only for the main thread, see QueueToolRequest
	virtual void SendToolRequest( const char *pToolName, KeyValues3 *pRequest ) = 0;

	// Queues the request for SendToolRequest, dispatched by Tools_RunFrame; the game uses it off the
	// main thread
	virtual void QueueToolRequest( const char *pToolName, KeyValues3 *pRequest ) = 0;

	// Settings kept per engine tool; SetToolSettings copies and saves them
	virtual KeyValues3 *GetToolSettings( uint8 nToolIndex ) = 0;
	virtual void SetToolSettings( uint8 nToolIndex, KeyValues3 *pSettings ) = 0;

	// Load and save the named settings file as KV3, see LoadSettings
	virtual void LoadSettingsKV3( const char *pName, KeyValues3 *pSettings, bool bModGameSubdirPrefix ) = 0;
	virtual void SaveSettingsKV3( const char *pName, KeyValues3 *pSettings, bool bModGameSubdirPrefix ) = 0;

	// pFileList is a ';' separated list of the files to import
	virtual void ShowImportWizard( const char *pFileList ) = 0;

	virtual void RegisterResponseListener( void *pListener ) = 0;
	virtual void UnregisterResponseListener( void *pListener ) = 0;

	// Forwards the arguments to the first method of every response listener
	virtual void NotifyResponseListeners( uint32 nUnk1, uint8 nUnk2, void *pUnk3, void *pUnk4, void *pUnk5, void *pUnk6 ) = 0;

	virtual bool CanToolHighlightSubassets( uint8 nToolIndex ) = 0;

	// Executes "quit"
	virtual void Quit() = 0;

	virtual void AddFGDFromAssetListener( IFGDFromAssetListener *pListener ) = 0;
	virtual void RemoveFGDFromAssetListener( IFGDFromAssetListener *pListener ) = 0;

	// Collection that owns the classes of the FGD from asset registry
	virtual void *GetFGDFromAssetClasses() = 0;

	// FGD class of the asset ("fgd_from_asset", named "<path>:<nUnknown>"), created on first use
	virtual void *FindOrCreateFGDFromAssetClass( const CBufferString &sAssetPath, int nUnknown ) = 0;

	// Whether the class belongs to GetFGDFromAssetClasses
	virtual bool IsFGDFromAssetClass( void *pClass ) = 0;

	// "fgd_from_asset_name" of the class
	virtual const char *GetFGDFromAssetClassAssetName( void *pClass ) = 0;

	// Increments for true, decrements for false the application counter that suppresses Qt shortcut events
	virtual void PushSuppressShortcuts( bool bPush ) = 0;
};

// Also derives from IToolServices (exposed as "ToolServices_002"), a base without a vtable
// whose data starts at m_pToolsConfig; its extent is unknown, so its members are listed here.
class CToolFramework2 : public CTier3AppSystem< IToolFramework2 >
{
public:
	struct ToolEntry_t
	{
		// See GetToolSettings
		KeyValues3 m_Settings;

		// Returned by LoadTool
		IToolSystem2 *m_pToolSystem;

		// Set once loading was attempted
		bool m_bLoadAttempted;
	};

	struct SplashScreenLabel_t
	{
		// Null until the splash screen exists
		QLabel *m_pLabel;

		Color m_Color;

		// Kept until the splash screen exists
		CUtlString m_sText;
	};

	struct QueuedToolRequest_t
	{
		CUtlString m_sToolName;
		KeyValues3 m_Request;
	};

	// Engine and external tools, loaded from bin/enginetools.txt
	CToolsConfig *m_pToolsConfig;

	// One entry per engine tool
	CUtlVector< ToolEntry_t > m_Tools;

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

	// Tool deferred by QueueToolLaunch, launched by Tools_RunFrame
	bool m_bLaunchQueuedTool;
	uint8 m_nQueuedToolIndex;

	// Tools queued for unloading
	CUtlVector< uint8 > m_ToolsToUnload;

	// CEnvironmentBlock returned by GetEnvironmentBlock, captured by Connect
	uint8 m_EnvironmentBlock[ 0x30 ];

	// Labels added by AddSplashScreenLabel to the loading splash screen
	QWidget *m_pSplashScreen;
	QVBoxLayout *m_pSplashScreenLayout;
	CUtlVector< SplashScreenLabel_t > m_SplashScreenLabels;

	// Plat_MSTime of the last blink of the labels, the top bit toggles their background
	int m_nSplashScreenLabelsBlinkTime;

	// Created on the first GetMultiAxisControllerInput call
	CJoystickInput *m_pJoystickInput;

	CQInputBindManager *m_pInputBindManager;

	// Warns about other tools instances
	CSingleAppInstance *m_pSingleAppInstance;

	// GAMEROOT path, the root of the settings files
	CBufferStringN< 200 > m_sGameRoot;

	// Window of ActivateEngineWindow, a QPointer< QWidget >: its shared reference count and the widget
	void *m_pToolWindowRefCount;
	QWidget *m_pToolWindow;

	// Stall monitor, started by StartStallMonitor
	bool m_bStallMonitorEnabled;
	ThreadHandle_t *m_pStallMonitorThread;

	// Counter of PushToolsAudioUnmute
	int m_nToolsAudioUnmuteCount;
	uint8 m_unk214[ 4 ];

	// "VToolMessage" notification channel
	uint8 m_VToolMessageChannel[ 0x18 ];

	// Listeners of RegisterResponseListener
	CUtlVector< void * > m_ResponseListeners;

	// Set by BackupSettings: later saves are blocked
	bool m_bBlockSettingsSave;

	CFGDFromAssetRegistry *m_pFGDFromAssetRegistry;
	CBufferString m_sUnk258;

	// Requests of QueueToolRequest, guarded by the mutex
	CUtlVector< QueuedToolRequest_t > m_QueuedToolRequests;
	CAtomicMutex m_ToolRequestsMutex;

	// Command line options "-generatethumbnails", "-generatethumbnailssubstr",
	// "-generatethumbnailsmaxtime" and "-generatethumbnailsclear": Tools_RunFrame passes them to
	// IAssetPreviewSystem::GenerateThumbnails and quits, and no deferred tool is launched meanwhile
	bool m_bGenerateThumbnails;
	CUtlString m_sGenerateThumbnailsSubstr;
	int m_nGenerateThumbnailsMaxTime;
	bool m_bGenerateThumbnailsClear;
};

#include "schemacompiler/schemaoff.h"

#endif // ITOOLFRAMEWORK2_H
