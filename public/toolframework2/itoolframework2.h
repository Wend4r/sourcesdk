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
#include "appframework/iappsystem.h"

class QObject;
class KeyValues3;
class IToolFrameListener;

abstract_class IToolFramework2 : public IAppSystem
{
public:
	// Starts the tools stall monitor thread
	virtual void unk011() = 0;

	virtual void unk012( void *pTools ) = 0;
	virtual void unk013() = 0;

	// Queue tools for unloading; unk018 unloads the queued tools
	virtual void unk014( uint8 nToolIndex ) = 0;
	virtual bool unk015( uint8 nToolIndex ) = 0;
	virtual void unk016( bool bUnknown ) = 0;

	virtual void Tools_RunFrame() = 0;
	virtual void unk018() = 0;

	// Returns the first non-zero answer of the loaded tool systems
	virtual void *unk019() = 0;

	virtual void AddToolFrameListener( IToolFrameListener *pListener ) = 0;
	virtual void RemoveToolFrameListener( IToolFrameListener *pListener ) = 0;

	// Matches either of the two names of a tool, case-insensitive
	virtual uint8 *FindTool( uint8 *pResult, const char *pName ) = 0;
	virtual const char *GetToolName( uint8 nToolIndex ) = 0;

	virtual void *LoadTool( uint8 nToolIndex ) = 0;

	// Return the tool's descriptor entries
	virtual void *unk025( uint8 nToolIndex ) = 0;
	virtual void *unk026( uint8 nToolIndex ) = 0;

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

	virtual void *unk036() = 0;
	virtual void *unk037() = 0;

	// Adds a colored text label
	virtual void unk038( const char *pText, int nUnknown ) = 0;
	virtual void *unk039( void *p ) = 0;
	virtual void *unk040() = 0;

	// Forwards both values to every loaded tool system
	virtual void unk041( uint8 nUnknown1, uint8 nUnknown2 ) = 0;

	virtual void MarkSettingsDirty() = 0;

	virtual bool IsMSAASupported() = 0;
	virtual bool unk044( uint8 nUnknown ) = 0;

	virtual const char *GetDeveloperHelpURL() = 0;
	virtual void ShowAboutDialog() = 0;

	virtual void unk047( void *p ) = 0;
	virtual void unk048( void *p ) = 0;
	virtual void *unk049() = 0;

	// Shared settings flags
	virtual void unk050( bool bValue ) = 0;
	virtual bool unk051() = 0;
	virtual void unk052( bool bValue ) = 0;
	virtual bool unk053() = 0;

	// True when not running on a customer machine
	virtual bool unk054() = 0;
	virtual void unk055( void *p ) = 0;

	virtual void LaunchBuildWatch() = 0;

	virtual bool unk057() = 0;
	virtual void unk058( bool bValue ) = 0;
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

	virtual void unk071( bool bValue ) = 0;
	virtual bool unk072() = 0;
	virtual void unk073( bool bValue ) = 0;
	virtual bool unk074() = 0;
	virtual bool unk075( bool *pUnk1, bool *pUnk2 ) = 0;
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

	virtual bool unk089() = 0;

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

#endif // ITOOLFRAMEWORK2_H
