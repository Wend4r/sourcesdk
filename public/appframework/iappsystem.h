//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: An application framework 
//
// $Revision: $
// $NoKeywords: $
//===========================================================================//

#ifndef IAPPSYSTEM_H
#define IAPPSYSTEM_H

#ifdef COMPILER_MSVC
#pragma once
#endif

#include "interfaces/interfaces.h"
#include "tier0/interface.h"
#include "tier0/utlstring.h"
#include "tier1/utlstringmap.h"

//-----------------------------------------------------------------------------
// Specifies a module + interface name for initialization
//-----------------------------------------------------------------------------
struct AppSystemInfo_t
{
	const char *m_pModuleName;
	const char *m_pInterfaceName;
};


//-----------------------------------------------------------------------------
// Client systems are singleton objects in the client codebase responsible for
// various tasks
// The order in which the client systems appear in this list are the
// order in which they are initialized and updated. They are shut down in
// reverse order from which they are initialized.
//-----------------------------------------------------------------------------
enum InitReturnVal_t
{
	INIT_FAILED = 0,
	INIT_OK,

	INIT_LAST_VAL
};

enum AppSystemTier_t
{
	APP_SYSTEM_TIER0 = 0,
	APP_SYSTEM_TIER1,
	APP_SYSTEM_TIER2,
	APP_SYSTEM_TIER3,
	APP_SYSTEM_TIER4,
	APP_SYSTEM_TIER5,

	APP_SYSTEM_TIER_OTHER
};

enum AppSystemBuildType_t
{
	APP_SYSTEM_BUILD_UNKNOWN = -1,
	APP_SYSTEM_BUILD_DEBUG = 0,
	APP_SYSTEM_BUILD_RELEASE,
	APP_SYSTEM_BUILD_RETAIL,
	APP_SYSTEM_BUILD_PROFILE,
	APP_SYSTEM_BUILD_MIXED,
	APP_SYSTEM_BUILD_MIXED_DEBUG,

	APP_SYSTEM_BUILD_COUNT
};

class KeyValues;
class CTier2Application;
class IUGCAddonPathResolver;

abstract_class IAppSystem
{
public:
	// Here's where the app systems get to learn about each other 
	virtual bool Connect( CreateInterfaceFn factory ) = 0;
	virtual void Disconnect() = 0;

	// Here's where systems can access other interfaces implemented by this object
	// Returns NULL if it doesn't implement the requested interface
	virtual void *QueryInterface( const char *pInterfaceName ) = 0;

	// Init, shutdown
	virtual InitReturnVal_t Init() = 0;
	virtual void Shutdown() = 0;
	virtual void PreShutdown() = 0;

	// Returns all dependent libraries
	virtual const AppSystemInfo_t* GetDependencies() = 0;

	// Returns the tier
	virtual AppSystemTier_t GetTier() = 0;

	// Reconnect to a particular interface
	virtual void Reconnect( CreateInterfaceFn factory, const char *pInterfaceName ) = 0;

	// Is this appsystem a singleton? (returns false if there can be multiple instances of this interface)
	// Returns whether or not the app system is a singleton
	virtual bool IsSingleton() = 0;
	
	virtual AppSystemBuildType_t GetBuildType() = 0;
};


//-----------------------------------------------------------------------------
// Helper empty implementation of an IAppSystem
//-----------------------------------------------------------------------------
template< class IInterface > 
class CBaseAppSystem : public IInterface
{
public:
	// Here's where the app systems get to learn about each other 
	virtual bool Connect( CreateInterfaceFn factory ) { return true; }
	virtual void Disconnect() {}

	// Here's where systems can access other interfaces implemented by this object
	// Returns NULL if it doesn't implement the requested interface
	virtual void *QueryInterface( const char *pInterfaceName ) { return NULL; }

	// Init, shutdown
	virtual InitReturnVal_t Init() { return INIT_OK; }
	virtual void Shutdown() {}

	virtual const AppSystemInfo_t* GetDependencies() { return NULL; }
	virtual AppSystemTier_t GetTier() { return APP_SYSTEM_TIER_OTHER; }

	virtual void Reconnect( CreateInterfaceFn factory, const char *pInterfaceName )
	{
		ReconnectInterface( factory, pInterfaceName );
	}

	virtual bool IsSingleton() { return true; }
};

enum LanguageType_t
{
	LanguageType_UI = 0x0,
	LanguageType_Audio = 0x1,
};

enum AppSystemErrorPolicy_t
{
	ADD_SYSTEM_ERROR = 0,
	ADD_SYSTEM_WARNING = 1,
	ADD_SYSTEM_SILENT = 2,
};

class CAppSystemDict;
class CBufferString;
class KeyValues3;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class IApplication : public IAppSystem
{
public:
	virtual ~IApplication() {}

	// Each overload returns the added system, or NULL on failure
	virtual IAppSystem *AddSystem( IAppSystem *pAppSystem, const char *pInterfaceName ) = 0;
	virtual IAppSystem *AddSystem( const char *pModuleName, const char *pInterfaceName, AppSystemErrorPolicy_t eErrorPolicy ) = 0;
	virtual IAppSystem *AddSystem( PlatModule_t hModule, const char *pInterfaceName, AppSystemErrorPolicy_t eErrorPolicy ) = 0;
	// Shuts down and disconnects the system before removing it
	virtual void RemoveSystem( IAppSystem *pSystem ) = 0;
	virtual bool AddSystems( int nCount, const AppSystemInfo_t *pSystems ) = 0;
	virtual void *FindSystem( const char *pSystemName ) = 0;
	virtual KeyValues *GetGameInfo() = 0;
	virtual AppSystemBuildType_t GetAppSystemBuildType() = 0;
	virtual const char *GetLanguage( LanguageType_t nType ) = 0;
	virtual const char *GetSubLanguage( LanguageType_t nType ) = 0;
	virtual bool IsInToolsMode() = 0;
	virtual bool IsConsoleApp() = 0;
	virtual void *unk024() = 0;
	virtual bool IsInDeveloperMode() = 0;
	// Full path of the executable module
	virtual const char *GetExecutablePath() = 0;
	virtual const char *GetModGameSubdir() = 0;
	virtual KeyValues *GetApplicationInfo() = 0;
	virtual void *GetAppInstance() = 0;
	virtual const char *GetContentPath() = 0;
	virtual int GetAppSystemFlags() = 0;
	virtual CUtlString GetConsoleLogFilename() = 0;
	virtual void ChangeLogFileSuffix( const char *pSuffix ) = 0;
	virtual IAppSystem *AddSystemDontLoadStartupManifests( const char *pModuleName, const char *pInterfaceName ) = 0;
	virtual int GetGameMode() = 0;
	virtual bool MountAddon( const char *pAddonName ) = 0;
	virtual bool UnmountAddon( const char *pAddonName ) = 0;
	virtual void GetMountedAddons( CUtlVector< CUtlString > &vecAddons ) = 0;
	// Returns the number of names written to ppAddons
	virtual int GetMountedAddons( const char **ppAddons, int nMaxAddons ) = 0;
	virtual bool GetAddonsDirectory( CBufferString &sDirectory ) = 0;
	virtual bool GetAddonsContentDirectory( CBufferString &sDirectory ) = 0;
	virtual bool IsFileInAddon( const char *pFilename ) = 0;
	virtual void GetAvailableAddons( CUtlVector< CUtlString > &vecAddons, int nFlags ) = 0;
	virtual bool GetAddonInfo( KeyValues3 *pAddonInfo, const char *pAddonName ) = 0;
	virtual bool IsRunningOnCustomerMachine() = 0;
	// Set from perforce.inf in the game directory
	virtual bool IsPerforceWorkspace() = 0;
	virtual bool IsLowViolence() = 0;
	virtual void SetLowViolence( bool bLowViolence ) = 0;
	virtual void SetInitializationPhase( int nInitializationPhase ) = 0;
	virtual int GetInitializationPhase() = 0;
	virtual const char *GetRestrictAddonsTo() = 0;
	// Reference counted, every true must be matched by a false
	virtual void SetAllowAddonChanges( bool bAllowAddonChanges ) = 0;
	virtual void SetUGCAddonPathResolver( IUGCAddonPathResolver *pResolver ) = 0;
	virtual CUtlString GetFullAddonPathFromID( uint64 nAddonID ) = 0;
	virtual uint64 GetIDFromAddonName( const char *pAddonName ) = 0;
	virtual CUtlString GetFullAddonPathFromAddonName( const char *pAddonName ) = 0;
	virtual void GetAvailableAddonMaps( CUtlVector< CUtlString > &vecMaps, const char *pAddonName, bool bIncludeFallbackMaps ) = 0;
	virtual void LoadStartupManifestGroup( const char *pManifestGroup ) = 0;
	// Reads source_folder from publish_data.txt next to the addon
	virtual CUtlString GetAddonSourceFolder( const char *pAddonName ) = 0;
	virtual void OnStartupManifestGroupLoaded() = 0;
	virtual void unk061( void *p ) = 0;
	virtual CAppSystemDict *GetAppSystemDict() = 0;

	template < class T > T* FindSystem( const char *pSystemName ) { return static_cast< T* >( FindSystem( pSystemName ) ); }
};

//-----------------------------------------------------------------------------
// Helper implementation of an IAppSystem for tier0
//-----------------------------------------------------------------------------
template< class IInterface > 
class CTier0AppSystem : public CBaseAppSystem< IInterface >
{
public:
	virtual AppSystemTier_t GetTier()
	{
		return APP_SYSTEM_TIER0;
	}
};

using FactoryFn = void* (*)(char const*, int*);

abstract_class CAppSystemDict
{
public:
	virtual ~CAppSystemDict() = 0;
	virtual void Init() = 0;
	virtual int GetSomeFlags() = 0;
	virtual CUtlString GetConsoleLogFilename() = 0;
	virtual void ChangeLogFileSuffix(const char* suffix) = 0;
	virtual CTier2Application* CreateApplication() = 0;
	virtual void OnAppSystemLoaded() = 0;

	struct ModuleInfo_t
	{
		const char* m_pModuleName;
		PlatModule_t m_hModule;
		int m_nRefCount;
	};

	struct AppSystem_t
	{
		const char* m_pModuleName;
		const char* m_pInterfaceName;
		IAppSystem* m_pSystem;
		PlatModule_t m_hModule;
		int m_nPhase;
		bool m_bInvisible;
	};

	CUtlLeanVector<ModuleInfo_t> m_Modules;
	CUtlLeanVector<AppSystem_t> m_Systems;
	CUtlLeanVector<FactoryFn> m_NonAppSystemFactories;
	CUtlStringList m_ModuleSearchPath;
	CUtlStringMap<UtlSymId_t> m_SystemDict;
	int m_nExpectedShutdownLoggingStateIndex;
	ILoggingListener* m_pDefaultLoggingListener;
	KeyValues* m_pGameInfo;
	KeyValues* m_pApplicationInfo;
	void* m_hInstance;
	void *m_pUnk240;
	bool m_bIsConsoleApp;
	bool m_bInToolsMode;
	bool m_bIsInDeveloperMode;
	bool m_bIsGameApp;
	bool m_bIsDedicatedServer;
	CUtlString m_UILanguage;
	CUtlString m_AudioLanguage;
	CUtlString m_UISubLanguage;
	CUtlString m_AudioSubLanguage;
	CUtlString m_ExecutablePath;
	CUtlString m_ModSubDir;
	CUtlString m_ContentPath;
	IApplication* m_pApplication;
	bool m_bInitialized;
	bool m_bSuppressCOMInitialization;
	int m_nStartupManifestsDisabledCount;
	int m_nAppSystemPhase;
	bool m_bIsRetail;
	bool m_bIsLowViolence;
	bool m_bInvokedPreShutdown;
};

#endif // IAPPSYSTEM_H

