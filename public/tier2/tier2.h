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
	virtual bool CreateRenderDevice() = 0;
};

//-----------------------------------------------------------------------------
// Call this to connect to/disconnect from all tier 2 libraries.
// It's up to the caller to check the globals it cares about to see if ones are missing
//
// Connects no interface: when g_pResourceManifestRegistry is set, registers
// g_nResourceFrameCount with it (once) and every static resource manifest of
// the module. Calls are counted; the last disconnect unregisters both.
//-----------------------------------------------------------------------------
void ConnectTier2Libraries( CreateInterfaceFn *pFactoryList, int nFactoryCount );
void DisconnectTier2Libraries();

// Unregisters the manifests an app system registered by hand from the resource manifest registry
void DisconnectTier2ManualManifests( ResourceManifestDesc_t **ppManifests, int nManifestCount );


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
template< class IInterface, int64 ConVarFlag = 0 >
class CTier2AppSystem : public CTier1AppSystem< IInterface, ConVarFlag >
{
	typedef CTier1AppSystem< IInterface, ConVarFlag > BaseClass;

public:
	virtual bool Connect( CreateInterfaceFn factory )
	{
		if ( !BaseClass::Connect( factory ) )
			return false;

		ConnectTier2Libraries( &factory, 1 );
		++m_nAppSysTier2LibraryConnects;
		return true;
	}

	virtual void Disconnect()
	{
		DisconnectTier2ManualManifests( m_manualManifests.Base(), m_manualManifests.Count() );
		while ( m_nAppSysTier2LibraryConnects > 0 )
		{
			DisconnectTier2Libraries();
			--m_nAppSysTier2LibraryConnects;
		}
		BaseClass::Disconnect();
	}

	// Overridden at this level in the game, with the behaviour of the base
	virtual InitReturnVal_t Init()
	{
		return BaseClass::Init();
	}

	virtual void Shutdown()
	{
		BaseClass::Shutdown();
	}

	virtual void Reconnect( CreateInterfaceFn factory, const char *pInterfaceName )
	{
		BaseClass::Reconnect( factory, pInterfaceName );
		ConnectTier2Libraries( &factory, 1 );
		++m_nAppSysTier2LibraryConnects;
	}

	virtual AppSystemTier_t GetTier()
	{
		return APP_SYSTEM_TIER2;
	}

	CUtlVector< ResourceManifestDesc_t * > m_manualManifests;
	// Number of ConnectTier2Libraries() calls that Disconnect() has to undo
	int m_nAppSysTier2LibraryConnects = 0;
};

class CTier2Application : public CTier1Application
{
public:
	CAppSystemDict* m_pDict;
	int m_nAppTier2LibraryConnects;
};

//-----------------------------------------------------------------------------
// Used by the resource system for fast resource frame counter
//-----------------------------------------------------------------------------
extern uint32 g_nResourceFrameCount;


#endif // TIER2_H

