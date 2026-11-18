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
#include "tier0/utlvector.h"
#include "color.h"
#include "appframework/iappsystem.h"

class QObject;
class KeyValues3;
class CUtlBuffer;
class CEnvironmentBlock;
class CQInputBindManager;
class IToolFrameListener;
class IToolFrameworkTabletInfo;

// Interface of a tool module, exported as "ToolSystem2_001" unless the tool's m_InterfaceName says otherwise
class IToolSystem2;

// Schema classes of toolframework2 read from bin/sdkenginetools.txt ( m_EngineTools / m_ExternalTools )
class CEngineToolInfo;
class CExternalToolInfo;

// A tool is addressed by its uint8 index in m_EngineTools; 0xFF means "no tool" (or the asset browser
// where a tool is shown). The asset arguments are asset system objects (opaque here); an "asset entry"
// is 24 bytes: the asset, its subasset (or NULL) and one more pointer-sized value.

// Line and column of a file opened in an external tool, substituted for {LineNo} / {ColNo} of
// m_ArgsWithLineColumn; passed by value in one register. Named by what it holds.
struct ToolFileLocation_t
{
	int m_nLine;
	int m_nColumn;
};

abstract_class IToolFramework2 : public IAppSystem
{
public:
	// Named by what it does: starts the import socket listener thread (port 24981), hands the main window
	// to the tools stall monitor and resets the asset browser state
	virtual void StartImportSocketListener() = 0;

	// Named by what it does: appends every tool of m_EngineTools that may run here (workshop restrictions,
	// m_LimitToMods / m_ExcludeFromMods) to pTools
	virtual void GetAvailableTools( CUtlVector< CEngineToolInfo * > *pTools ) = 0;

	// Named by what it does: loads the tool and asks its tool system to show itself with no assets
	virtual void ShowTool( uint8 nToolIndex ) = 0;

	// Named by what they do: queue a tool for unloading ( UnloadQueuedTools unloads the queue ) and tell
	// whether a tool is loaded and not queued yet
	virtual void QueueUnloadTool( uint8 nToolIndex ) = 0;
	virtual bool CanUnloadTool( uint8 nToolIndex ) = 0;

	// Named by what it does: the next Tools_RunFrame shows this tool ( 0xFF shows the asset browser )
	virtual void QueueShowTool( uint8 nToolIndex ) = 0;

	virtual void Tools_RunFrame() = 0;

	// Named by what it does: unloads the tools queued by QueueUnloadTool
	virtual void UnloadQueuedTools() = 0;

	// Named by what it does: returns the first non-zero answer of the loaded tool systems to a quit request;
	// the engine quits on 0, cancels the quit on 1 and asks again next frame otherwise
	virtual int QueryToolsQuit() = 0;

	virtual void AddToolFrameListener( IToolFrameListener *pListener ) = 0;
	virtual void RemoveToolFrameListener( IToolFrameListener *pListener ) = 0;

	// Matches m_Name or m_FriendlyName, case-insensitive; *pResult is 0xFF when no tool matches
	virtual uint8 *FindTool( uint8 *pResult, const char *pName ) = 0;

	// Returns m_FriendlyName
	virtual const char *GetToolFriendlyName( uint8 nToolIndex ) = 0;

	// Loads the module ( m_Library ) and initializes its tool system on first use
	virtual IToolSystem2 *LoadTool( uint8 nToolIndex ) = 0;

	// Entries of m_EngineTools / m_ExternalTools
	virtual const CEngineToolInfo *GetToolInfo( uint8 nToolIndex ) = 0;
	virtual const CExternalToolInfo *GetExternalToolInfo( uint8 nExternalToolIndex ) = 0;

	// Named by what they do: open assets in a tool ( 0xFF picks the tool of each asset ) and warn about
	// assets no tool handles
	virtual void OpenAssetInTool( const void *pAsset, uint8 nToolIndex ) = 0;
	virtual void OpenAssetEntryInTool( const void *pAssetEntry, uint8 nToolIndex ) = 0;
	virtual void OpenAssetsInTool( const CUtlVector< const void * > *pAssets, uint8 nToolIndex ) = 0;

	// Named by what it does: opens the assets that pAssetList's ninth virtual collects
	virtual void OpenAssetList( void *pAssetList ) = 0;

	// Named by what it does: pAssetEntries is a CUtlVector of 24-byte asset entries
	virtual void OpenAssetEntriesInTool( const void *pAssetEntries, uint8 nToolIndex ) = 0;

	virtual void ExecuteCommand( const char *pCommand ) = 0;

	// The "multisample" tool setting; reads 0 while MSAA is not supported
	virtual uint8 GetMultisampleType() = 0;
	virtual void SetMultisampleType( uint8 nMultisampleType ) = 0;

	virtual void SetSplashScreenMessage( const char *pMessage ) = 0;

	// Named by what they return: the tablet state of the tools application and the process environment
	// captured in Connect
	virtual IToolFrameworkTabletInfo *GetTabletInfo() = 0;
	virtual CEnvironmentBlock *GetProcessEnvironment() = 0;

	// Named by what it does: adds a bold text line in the given color to the splash screen
	virtual void AddSplashScreenLabel( const char *pText, Color color ) = 0;

	// Named by what it does: reads the pen state from the raw input of the tablet ( 24 bytes: position,
	// pressure and tilt floats ); false when no tablet is present
	virtual bool PollTabletPenState( void *pPenState ) = 0;

	// Named by what it does: applies tools/stylesheets/qtabbedstyle_colors.vsc to the application
	virtual void ReloadToolsStyle() = 0;

	// Named by what it does: forwards both values to the sixteenth virtual of every loaded tool system
	virtual void NotifyLoadedTools( uint8 nFirst, uint8 nSecond ) = 0;

	virtual void MarkSettingsDirty() = 0;

	// From gameinfo Engine2/RenderingPipeline/SupportsMSAA
	virtual bool IsMSAASupported() = 0;

	// Named by what it does: asks the rendering pipeline whether it supports the multisample type
	virtual bool IsMultisampleTypeSupported( uint8 nMultisampleType ) = 0;

	virtual const char *GetDeveloperHelpURL() = 0;
	virtual void ShowAboutDialog() = 0;

	// Named by what they do: select the file in Explorer, open it with the shell, open it in the external
	// tool registered for its extension ( the shell opens it when no tool matches or the line is 0 )
	virtual void ShowFileInExplorer( const char *pFileName ) = 0;
	virtual void ShellOpenFile( const char *pFileName ) = 0;
	virtual void OpenFileInExternalTool( const char *pFileName, ToolFileLocation_t location ) = 0;

	// The "allow_fly_mode_2" tool setting
	virtual void SetAllowFlyMode2( bool bAllow ) = 0;
	virtual bool IsFlyMode2Allowed() = 0;

	// The "hide_cursor_during_camera_manipulation" tool setting
	virtual void SetHideCursorDuringCameraManipulation( bool bHide ) = 0;
	virtual bool ShouldHideCursorDuringCameraManipulation() = 0;

	// Named by what it does: true when not running on a customer machine
	virtual bool IsRunningOnDeveloperMachine() = 0;

	// Named by what it does: IVConComm::MakeVConsoleForeground( hReturnToWindow, 0, true )
	virtual void MakeVConsoleForeground( void *hReturnToWindow ) = 0;

	virtual void LaunchBuildWatch() = 0;

	// The "always_render_all_windows" tool setting
	virtual bool ShouldAlwaysRenderAllWindows() = 0;
	virtual void SetAlwaysRenderAllWindows( bool bAlways ) = 0;

	// Named by what it returns: the input binding manager of the tools ( tools/keybindings )
	virtual CQInputBindManager *GetInputBindManager() = 0;

	// Named by what they do: remember the window a tool made active, then raise it again ( the asset
	// browser when no window is remembered )
	virtual void SetActiveToolWindow( QObject *pWindow ) = 0;
	virtual void ActivateToolWindow() = 0;

	// Named by what they do: read and write the raw settings file
	// <settings dir>/<pName>/[<game>_]settings.txt; bGameSpecific adds the game prefix
	virtual void LoadSettingsFile( const char *pName, CUtlBuffer *pBuffer, bool bGameSpecific ) = 0;
	virtual void SaveSettingsFile( const char *pName, CUtlBuffer *pBuffer, bool bGameSpecific ) = 0;
	virtual void GetSettingsPath( const char *pName, CBufferString *pPath, bool bGameSpecific ) = 0;

	// Copies the settings to the next free numbered backup and blocks every later settings save
	virtual void BackupSettings() = 0;

	// Named by what they do: *pResult gets the index of the first tool ( or external tool ) that handles
	// the asset or asset type, 0xFF when none does
	virtual uint8 *FindToolForAsset( uint8 *pResult, const void *pAsset ) = 0;
	virtual uint8 *FindExternalToolForAsset( uint8 *pResult, const void *pAsset ) = 0;
	virtual uint8 *FindToolForAssetType( uint8 *pResult, const void *pAssetType ) = 0;

	// Named by what they do: append the indices of every tool ( or external tool ) that handles the asset
	virtual void GetToolsForAsset( const void *pAsset, CUtlVector< uint8 > *pToolIndices ) = 0;
	virtual void GetExternalToolsForAsset( const void *pAsset, CUtlVector< uint8 > *pToolIndices ) = 0;

	// The "auto_mute_game_when_tools_have_focus" and "tools_audio_muted" tool settings; changes notify the
	// audio settings listeners
	virtual void SetAutoMuteGameWhenToolsHaveFocus( bool bAutoMute ) = 0;
	virtual bool ShouldAutoMuteGameWhenToolsHaveFocus() = 0;
	virtual void SetToolsAudioMuted( bool bMuted ) = 0;
	virtual bool IsToolsAudioMuted() = 0;

	// Named by what it does: *pMuteGame when the application is active, the engine window is not focused and
	// auto mute is on; *pMuteTools when tools audio is muted and nothing forces it on; returns *pMuteGame
	virtual bool GetAudioMuteState( bool *pMuteGame, bool *pMuteTools ) = 0;

	// Named by what they do: listeners called when an audio setting changes
	virtual void AddAudioSettingsListener( void *pListener ) = 0;
	virtual void RemoveAudioSettingsListener( void *pListener ) = 0;

	// Named by what it does: a counter that keeps tools audio on while above 0 ( true increments,
	// false decrements )
	virtual void PushForceToolsAudio( bool bPush ) = 0;

	// Named by what they do: pass a request to the tool system of the named tool ( loading it ), or queue
	// the request under a lock for a later frame
	virtual void SendRequestToTool( const char *pToolName, KeyValues3 *pRequest ) = 0;
	virtual void QueueRequestToTool( const char *pToolName, const KeyValues3 *pRequest ) = 0;

	// Named by what they do: the settings of a loaded tool; setting them also saves them
	virtual KeyValues3 *GetToolSettings( uint8 nToolIndex ) = 0;
	virtual void SetToolSettings( uint8 nToolIndex, const KeyValues3 *pSettings ) = 0;

	// Named by what they do: the KV3 settings saved under pName ( see LoadSettingsFile )
	virtual bool LoadSettings( const char *pName, KeyValues3 *pSettings, bool bGameSpecific ) = 0;
	virtual void SaveSettings( const char *pName, const KeyValues3 *pSettings, bool bGameSpecific ) = 0;

	// pFileList is split into the files to import
	virtual void ShowImportWizard( const char *pFileList ) = 0;

	virtual void RegisterResponseListener( void *pListener ) = 0;
	virtual void UnregisterResponseListener( void *pListener ) = 0;

	// Named by what it does: calls the first virtual of every response listener with these arguments
	virtual void DispatchResponse( uint32 nResponse, uint8 nToolIndex, void *pArg1, void *pArg2, void *pArg3, void *pArg4 ) = 0;

	// m_bCanHighlightSubassets of the tool
	virtual bool CanToolHighlightSubassets( uint8 nToolIndex ) = 0;

	// Executes "quit"
	virtual void Quit() = 0;

	// Named by what they do: the CFGDFromAssetRegistry ( "fgd_from_asset" game data of assets ) - its
	// listeners, its game data object, the entry of an asset name, whether a pointer refers to its game
	// data object, and the "fgd_from_asset_name" of an entry
	virtual void AddFGDFromAssetListener( void *pListener ) = 0;
	virtual void RemoveFGDFromAssetListener( void *pListener ) = 0;
	virtual void *GetFGDFromAssetGameData() = 0;
	virtual void *FindOrCreateFGDFromAsset( const CBufferString *pAssetName, int nFlags ) = 0;
	virtual bool IsFGDFromAssetGameData( void *const *ppGameData ) = 0;
	virtual const char *GetFGDFromAssetName( const void *pEntry ) = 0;

	// Named by what it does: a counter that suppresses Qt shortcut events while above 0
	virtual void SuppressShortcutEvents( bool bSuppress ) = 0;
};

#endif // ITOOLFRAMEWORK2_H
