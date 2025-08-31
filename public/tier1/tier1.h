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

class CTier1Application : public CTier1AppSystem<IApplication>
{
public:
	virtual ~CTier1Application() = 0;

	virtual void AddSystem(IAppSystem* pAppSystem, const char * interfaceName, bool errorOut) = 0;
	virtual void AddSystem(const char* unk, const char* interfaceName, bool errorOut) = 0;
	virtual void AddSystem(IAppSystem* pAppSystem, const char* interfaceName) = 0;
	virtual void RemoveSystem(IAppSystem* pSystem) = 0;
	virtual bool AddSystems(int count, const AppSystemInfo_t **) = 0;
	virtual void* FindSystem(const char *pSystemName) = 0;
	virtual KeyValues* GetGameInfo() = 0;
	virtual void unk1() = 0;
	virtual const char* GetLanguage(LanguageType_t) = 0;
	virtual const char* GetModPath(int) = 0;
	virtual bool IsInToolsMode();
	virtual bool IsConsoleApp();
	virtual void unk2();
	virtual bool IsInDeveloperMode();
	virtual const char * GetExecutablePath();
	virtual const char * GetModGameSubdir();
	virtual KeyValues* GetApplicationInfo();
	virtual void* GetAppInstance();
	virtual const char * GetContentPath();
	virtual const char * GetConsoleLogFilename();
	virtual const char * ChangeLogFileSuffix(const char *suffix);
	virtual void unk3();
	virtual void AddSystemDontLoadStartupManifests(const char*,const char*);
	virtual const char * GetGameMode();
	virtual void MountAddon(char const*) = 0;
	virtual void UnmountAddon(char const*) = 0;
	virtual void GetMountedAddons(CUtlVector<CUtlString> & vec) = 0;
	virtual void GetMountedAddons(char const**,int) = 0;
	virtual void GetAddonsContentDirectory() = 0;
	virtual void GetAddonsContentDirectory2() = 0;
	virtual void IsFileInAddon(const char *) = 0;
	virtual void GetAvailableAddons() = 0;
	virtual bool GetAddonInfo() = 0;
	virtual bool IsRunningOnCustomerMachine() = 0;
	virtual bool IsLowViolence() = 0;
	virtual bool SetLowViolence(bool) = 0;
	virtual bool SetInitializationPhase(int) = 0;
	virtual int GetInitializationPhase() = 0;
	virtual const char* GetRestrictAddonsTo() = 0;
	virtual void SetAllowAddonChanges(bool) = 0;
	virtual void SetUGCAddonPathResolver(IUGCAddonPathResolver *) = 0;
	virtual CUtlString GetAddonNameFromID(uint64 id) = 0;
	virtual uint64 GetIDFromAddonName(const char *name) = 0;
	virtual CUtlString GetFullAddonPathFromAddonName(const char *name) = 0;
	virtual void GetAvailableAddonMaps() = 0;
	virtual void LoadStartupManifestGroup() = 0;
	virtual void unk4();
	virtual void unk5();
	virtual void unk6();
	virtual CAppSystemDict* GetAppSystemDict();

	template< class T >
	T* FindSystem(const char *pSystemName) {
		return static_cast<T*>(FindSystem(pSystemName));
	}
};


#endif // TIER1_H

