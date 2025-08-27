//===== Copyright © 2005-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: A higher level link library for general use in the game and tools.
//
//===========================================================================//


#ifndef TIER2_H
#define TIER2_H

#if defined( _WIN32 )
#pragma once
#endif

#include "tier1/tier1.h"
#include "tier2_logging.h"

struct ResourceManifestDesc_t;
class IRenderDeviceSetup
{
public:
	virtual ~IRenderDeviceSetup() = 0;
};

//-----------------------------------------------------------------------------
// Call this to connect to/disconnect from all tier 2 libraries.
// It's up to the caller to check the globals it cares about to see if ones are missing
//-----------------------------------------------------------------------------
void ConnectTier2Libraries( CreateInterfaceFn *pFactoryList, int nFactoryCount );
void DisconnectTier2Libraries();


//-----------------------------------------------------------------------------
// Call this to get the file system set up to stdio for utilities, etc:
//-----------------------------------------------------------------------------
void InitDefaultFileSystem(void);
void ShutdownDefaultFileSystem(void);


//-----------------------------------------------------------------------------
// for simple utilities using valve libraries, call the entry point below in main(). It will
// init a filesystem for you, init mathlib, and create the command line. Note that this function
// may modify argc/argv because it filters out arguments (like -allowdebug).
//-----------------------------------------------------------------------------
void InitCommandLineProgram( int &argc, char ** &argv );


//-----------------------------------------------------------------------------
// Helper empty implementation of an IAppSystem for tier2 libraries
//-----------------------------------------------------------------------------
template< class IInterface, int ConVarFlag = 0 > 
class CTier2AppSystem : public CTier1AppSystem< IInterface, ConVarFlag >
{
	typedef CTier1AppSystem< IInterface, ConVarFlag > BaseClass;

public:
	virtual bool Connect( CreateInterfaceFn factory ) 
	{
		if ( !BaseClass::Connect( factory ) )
			return false;

		ConnectTier2Libraries( &factory, 1 );
		return true;
	}

	virtual InitReturnVal_t Init()
	{
		InitReturnVal_t nRetVal = BaseClass::Init();
		if ( nRetVal != INIT_OK )
			return nRetVal;

		return INIT_OK;
	}

	virtual AppSystemTier_t GetTier()
	{
		return APP_SYSTEM_TIER2;
	}

	virtual void Shutdown()
	{
		BaseClass::Shutdown();
	}

	virtual void Disconnect() 
	{
		DisconnectTier2Libraries();
		BaseClass::Disconnect();
	}
private:
	CUtlVector<ResourceManifestDesc_t *> m_manualManifests;
	int m_nAppSysTier2LibraryConnects;
};

class CTier2Application : public CTier1Application
{
private:
	CAppSystemDict* m_pDict;
	int m_nAppTier2LibraryConnects;
};

class CTier1AppSystemDict : public CAppSystemDict
{
public:
	bool m_bNoExeCheck;
	int m_nUnk;
	CBufferString unk;
	CBufferString unk1;
	char m_unnk[192];
	CBufferString unk2;
	char m_unnk1[218];
};

class CTier2AppSystemDict : public CTier1AppSystemDict
{
public:
	CUtlStringMap<int> m_unk808;
	CUtlStringMap<int> m_addonRefcounts;
	CUtlVector<void*> m_StartupManifests;
	int m_nStartupManifestSuppressionCount;
	bool m_bStaleStartupResources;
	bool m_bUseModPathBinDir;
	bool m_bLibrariesConnected;
	bool m_bEnsureToolStartupManifests;
	bool m_bEnsureVRStartupManifests;
	bool m_bEnsureGameStartupManifests;
	bool m_bEnsureConsoleStartupManifests;
	bool m_bAddonsOnRestricted;
	CInterlockedInt m_nAllowAddonChanges;
	CUtlString m_addonToRestrictTo;
	IUGCAddonPathResolver *m_pUGCAddonPathResolver;
	CUtlString m_LogFilename;
	CUtlString m_LogFileSuffix;
	bool m_bLogToFile;
	LoggingFileHandle_t m_bLogFileHandle;
	CFileLoggingListener m_LogToFileListener;
};

class CMaterialSystem2AppSystemDict : public CTier2AppSystemDict, public IRenderDeviceSetup
{
public:
	void (*m_pSomeFunc)(void*);
	char m_unnk1168[24];
	CUtlString m_gameName;
	char m_unnk1208[32];
	bool m_bUnk1240;
	bool m_bUnk1241;
	bool m_bUnk1242;
};

//-----------------------------------------------------------------------------
// Distance fade information
//-----------------------------------------------------------------------------
enum FadeMode_t
{
	// These map directly to cpu_level, and g_aFadeData contains settings for each given cpu_level (see videocfg.h CPULevel_t).
	// The exception is 'FADE_MODE_LEVEL', which refers to level-specific values in the map entity.
	FADE_MODE_NONE = 0,
	FADE_MODE_LOW,
	FADE_MODE_MED,
	FADE_MODE_HIGH,
	FADE_MODE_360,
	FADE_MODE_PS3,
	FADE_MODE_LEVEL,

	FADE_MODE_COUNT,
};

struct FadeData_t
{
	float	m_flPixelMin;		// Size (height in pixels) above which objects start to fade in
	float	m_flPixelMax;		// Size (height in pixels) above which objects are fully faded in
	float	m_flWidth;			// Reference screen res w.r.t which the above pixel values were chosen
	float	m_flFadeDistScale;	// Scale factor applied before entity distance-based fade is calculated
};

// see tier2.cpp for data!
extern FadeData_t g_aFadeData[FADE_MODE_COUNT];


//-----------------------------------------------------------------------------
// Used by the resource system for fast resource frame counter
//-----------------------------------------------------------------------------
extern uint32 g_nResourceFrameCount;


#endif // TIER2_H

