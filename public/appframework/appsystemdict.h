#ifndef APPSYSTEMDICT_H
#define APPSYSTEMDICT_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/utlhashtable.h"
#include "tier0/utlstringmap.h"

#include <functional>

class IAppSystem;
class IApplication;
class ILoggingListener;
class KeyValues;

using FactoryFn = void* (*)(char const*, int*);

abstract_class CAppSystemDict
{
public:
	virtual ~CAppSystemDict() = 0;
	virtual bool Init() = 0;
	// Flags the log file is opened with, 0 unless the dictionary logs to a file
	virtual int GetLogFileFlags() = 0;
	virtual CUtlString GetConsoleLogFilename() const = 0;
	virtual void ChangeLogFileSuffix( const char *pSuffix ) = 0;
	virtual IApplication *CreateApplication() = 0;
	virtual void OnAppSystemLoaded() = 0;

	// Detected during this run, whether or not the user acknowledged the warning
	bool HasInsufficientPagefile() const { return ( m_nMemoryWarnings & 1 ) != 0; }
	bool HasLowAvailableVirtualMemory() const { return ( m_nMemoryWarnings & 2 ) != 0; }

	struct WindowSize_t
	{
		int m_nWidth;
		int m_nHeight;
	};

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
	// Created as 640x480 by a command line option, then set to the resolution the render device uses
	WindowSize_t *m_pWindowSizeOverride;
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
	bool m_bAllowScreensaver;
	int m_nStartupManifestsDisabledCount;
	int m_nAppSystemPhase;
	bool m_bIsRetail;
	bool m_bIsLowViolence;
	bool m_bInvokedPreShutdown;
	// Bit 1: insufficient page file, bit 2: low available virtual memory
	int m_nMemoryWarnings;
	// Warnings the user has not acknowledged yet, kept across runs in the file below
	int m_nUnacknowledgedMemoryWarnings;
	CBufferString m_UnacknowledgedMemoryWarningsFile;
	// The members up to m_Callbacks are named by what we found of them: in engine2 only the
	// constructor and the destructor of CAppSystemDict touch them
	CBufferStringN< 200 > m_UnusedString200;
	CBufferStringN< 128 > m_UnusedString128;
	bool m_bUnused;

	// Hashtables with 16-byte entries that the destructor clears; the key and value types are not
	// known, these keep the layout
	CUtlHashtable< uint32, void * > m_UnusedTable0;
	CUtlHashtable< uint32, void * > m_UnusedTable1;
	void *m_pUnused;

	// std::function objects that IApplication::AddAppSystemDictCallback appends; nothing in engine2
	// calls them, their signature is not known
	CUtlLeanVector< std::function< void() > > m_Callbacks;
	bool m_bIsPerforceWorkspace;
};

#endif // APPSYSTEMDICT_H
