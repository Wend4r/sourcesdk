//===== Copyright © 2005-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: A higher level link library for general use in the game and tools.
//
//===========================================================================//


#ifndef TIER1_H
#define TIER1_H

#if defined( _WIN32 )
#pragma once
#endif

#include "appframework/iappsystem.h"
#include "tier1/convar.h"

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------

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

struct ResourceManifestDesc_t;

//-----------------------------------------------------------------------------
// Call this to connect to/disconnect from all tier 1 libraries.
// It's up to the caller to check the globals it cares about to see if ones are missing
//-----------------------------------------------------------------------------
void ConnectTier1Libraries( CreateInterfaceFn *pFactoryList, int nFactoryCount );
void DisconnectTier1Libraries();

//-----------------------------------------------------------------------------
// Helper empty implementation of an IAppSystem for tier2 libraries
//-----------------------------------------------------------------------------
template< class IInterface, int ConVarFlag = 0 > 
class CTier1AppSystem : public CTier0AppSystem< IInterface >
{
	typedef CTier0AppSystem< IInterface > BaseClass;

public:
	virtual bool Connect( CreateInterfaceFn factory ) 
	{
		if ( !BaseClass::Connect( factory ) )
			return false;

		ConnectTier1Libraries( &factory, 1 );
		return true;
	}

	virtual void Disconnect() 
	{
		DisconnectTier1Libraries();
		BaseClass::Disconnect();
	}

	virtual InitReturnVal_t Init()
	{
		InitReturnVal_t nRetVal = BaseClass::Init();
		if ( nRetVal != INIT_OK )
			return nRetVal;

		if ( g_pCVar )
		{
			ConVar_Register( ConVarFlag );
		}
		return INIT_OK;
	}

	virtual void Shutdown()
	{
		if ( g_pCVar )
		{
			ConVar_Unregister( );
		}
		BaseClass::Shutdown( );
	}

	virtual AppSystemTier_t GetTier()
	{
		return APP_SYSTEM_TIER1;
	}
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

class CTier1Application : public CTier1AppSystem< IApplication >
{
public:
	virtual ~CTier1Application() = 0;

	virtual void AddSystem( IAppSystem *pAppSystem, const char *pInterfaceName, AppSystemErrorPolicy_t eErrorPolicy ) = 0;
	virtual void AddSystem( const char *pModuleName, const char *pInterfaceName, AppSystemErrorPolicy_t eErrorPolicy ) = 0;
	virtual void AddSystem( IAppSystem *pAppSystem, const char *pInterfaceName ) = 0;
	virtual void RemoveSystem( IAppSystem *pSystem ) = 0;
	virtual bool AddSystems( int nCount, const AppSystemInfo_t **pSystemInfo ) = 0;
	virtual void *FindSystem( const char *pSystemName ) = 0;
	virtual KeyValues *GetGameInfo() = 0;
	virtual AppSystemBuildType_t GetAppSystemBuildType() = 0;
	virtual const char *GetLanguage( LanguageType_t nType ) = 0;
	virtual const char *GetModPath( int nPathType ) = 0;
	virtual bool IsInToolsMode();
	virtual bool IsConsoleApp();
	virtual OpusRepacketizer *GetOpusRepacketizer();
	virtual bool IsInDeveloperMode();
	virtual const char *GetExecutablePath();
	virtual const char *GetModGameSubdir();
	virtual KeyValues *GetApplicationInfo();
	virtual void *GetAppInstance();
	virtual const char *GetContentPath();
	virtual int GetAppSystemFlags();
	virtual CUtlString GetConsoleLogFilename();
	virtual void ChangeLogFileSuffix( const char *pSuffix );
	virtual void AddSystemDontLoadStartupManifests( const char *pModuleName, const char *pInterfaceName );
	virtual const char *GetGameMode();
	virtual bool MountAddon( const char *pAddonName ) = 0;
	virtual bool UnmountAddon( const char *pAddonName ) = 0;
	virtual void GetMountedAddons( CUtlVector< CUtlString > &vecAddons ) = 0;
	virtual int GetMountedAddons( const char **ppAddons, int nMaxAddons ) = 0;
	virtual bool GetAddonsDirectory( CUtlString &sDirectory ) = 0;
	virtual bool GetAddonsContentDirectory( CUtlString &sDirectory ) = 0;
	virtual bool IsFileInAddon( const char *pFilename ) = 0;
	virtual void GetAvailableAddons( CUtlVector< CUtlString > &vecAddons, int nFlags ) = 0;
	virtual bool GetAddonInfo( KeyValues *pAddonInfo, const char *pAddonName ) = 0;
	virtual bool IsRunningOnCustomerMachine() = 0;
	virtual bool IsLowViolence() = 0;
	virtual bool SetLowViolence( bool bLowViolence ) = 0;
	virtual bool SetInitializationPhase( int nInitializationPhase ) = 0;
	virtual int GetInitializationPhase() = 0;
	virtual const char *GetRestrictAddonsTo() = 0;
	virtual void SetAllowAddonChanges( bool bAllowAddonChanges ) = 0;
	virtual void SetUGCAddonPathResolver( IUGCAddonPathResolver *pResolver ) = 0;
	virtual CUtlString GetAddonNameFromID( uint64 nAddonID ) = 0;
	virtual uint64 GetIDFromAddonName( const char *pAddonName ) = 0;
	virtual CUtlString GetFullAddonPathFromAddonName( const char *pAddonName ) = 0;
	virtual void GetAvailableAddonMaps( CUtlVector< CUtlString > &vecMaps, const char *pAddonName, bool bIncludeFallbackMaps ) = 0;
	virtual void LoadStartupManifestGroup( const char *pManifestGroup ) = 0;
	virtual CUtlString GetAddonSourceFolder( const char *pAddonName );
	virtual void OnStartupManifestGroupLoaded();
	virtual void AddStartupManifest( ResourceManifestDesc_t &resourceManifestDesc );
	virtual CAppSystemDict *GetAppSystemDict();

	template < class T > T* FindSystem( const char *pSystemName ) { return static_cast< T* >( FindSystem( pSystemName ) ); }
};

#endif // TIER1_H

