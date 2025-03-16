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

	INIT_LAST_VAL,
};

enum AppSystemTier_t
{
	APP_SYSTEM_TIER0 = 0,
	APP_SYSTEM_TIER1,
	APP_SYSTEM_TIER2,
	APP_SYSTEM_TIER3,

	APP_SYSTEM_TIER_OTHER,
};

enum BuildType_t
{
	kBuildTypeRelease = 2
};


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
	
	virtual BuildType_t	GetBuildType() = 0;
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


//-----------------------------------------------------------------------------
// Helper implementation of an IAppSystem for tier0
//-----------------------------------------------------------------------------
template< class IInterface > 
class CTier0AppSystem : public CBaseAppSystem< IInterface >
{
};

using FactoryFn = void* (*)(char const*, int*);
class KeyValues;
abstract_class CAppSystemDict
{
public:
	virtual ~CAppSystemDict() = 0;
	virtual void Init() = 0;
	virtual CUtlString GetConsoleLogFilename() = 0;
	virtual void ChangeLogFileSuffix(const char* suffix) = 0;
	virtual void CreateApplication() = 0;
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

	CUtlVector<ModuleInfo_t> m_Modules;
	CUtlVector<AppSystem_t> m_Systems;
	CUtlVector<FactoryFn> m_NonAppSystemFactories;
	CUtlStringList m_ModuleSearchPath;
	CUtlStringMap<UtlSymId_t> m_SystemDict; // 104
	int m_nExpectedShutdownLoggingStateIndex; // 256
	ILoggingListener* m_pDefaultLoggingListener; // 264
	KeyValues* m_pGameInfo; // 272
	KeyValues* m_pApplicationInfo; // 280
	void* m_hInstance; // 288
	void* m_pUnknown; // 296
	bool m_bIsConsoleApp; // 304
	bool m_bInToolsMode; // 305
	bool m_bIsInDeveloperMode; // 306
	bool m_bIsGameApp; // 307
	bool m_bIsDedicatedServer; // 308
	CUtlString unk; // 312
	CUtlString m_UILanguage; // 320
	CUtlString m_AudioLanguage; // 328
	CUtlString m_ModPath; // 336
	CUtlString m_ExecutablePath; // 344
	CUtlString m_ModSubDir; // 352
	CUtlString m_ContentPath; // 360
	IApplication* m_pApplication; // 368
	bool m_bInitialized; // 376
	bool m_bSuppressCOMInitialization;
	int m_nAppSystemPhase;
	int unk2;
	bool m_bIsRetail; // 388
	bool m_bIsLowViolence; // 389
	bool m_bInvokedPreShutdown; // 390
	//.... more unknown fields
};

#endif // IAPPSYSTEM_H

