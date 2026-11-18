//========= Copyright Valve Corporation, All rights reserved. ============//
//
//
//
//=============================================================================
#ifndef GC_CLIENTSYSTEM_H
#define GC_CLIENTSYSTEM_H
#ifdef _WIN32
#pragma once
#endif

#include <tier0/utlstring.h>
#include <tier0/utlmap.h>

#include <gcsdk_gcmessages.pb.h>

#include "igamesystem.h"
#include "gcsdk/gcclient.h"
#include "gcsdk/gcclient_sharedobjectcache.h"

//-----------------------------------------------------------------------------
// The game system that owns the GC client of a game module. Shared by the
// server (CCSGCServerSystem) and the client (CCSGCClientSystem in client.dll).
// Names follow CS:GO game/shared/gc_clientsystem.h where it has them.
//-----------------------------------------------------------------------------
class CGCClientSystem : public CAutoGameSystem
{
public:
	CGCClientSystem();
	virtual ~CGCClientSystem();

	// Init returns true; Shutdown uninits the GC client and clears m_bInittedGC
	virtual bool Init() override;
	virtual void PostInit() override;
	virtual void Shutdown() override;

	// Calls Update with the frame time of the message
	GS_EVENT( GameFrameBoundary );
	// Forwards to GameFrameBoundary
	GS_EVENT( OutOfGameFrameBoundary );

	// Runs the GC client main loop (1 ms budget) while m_bInittedGC is set. CS:GO names this
	// Update( float frametime ) on the client and GCThink() on the server.
	virtual void Update( float flFrameTime );

	// Named by what it does: returns m_GCClient's GC connection status
	virtual GCConnectionStatus GetGCConnectionStatus() const;

	// Named by what it does: returns the engine's steam universe (IVEngineServer2::GetSteamUniverse
	// on the server)
	virtual EUniverse GetSteamUniverse() const;

protected:
	// Inits m_GCClient with the engine's server version and the steam client interfaces
	virtual void InitGC();
	// The server installs its GC hello populator here
	virtual void PreInitGC() {}
	virtual void PostInitGC() {}

public:
	GCSDK::CGCClient *GetGCClient() { return &m_GCClient; }
	const GCSDK::CGCClient *GetGCClient() const { return &m_GCClient; }

	bool BConnectedtoGC() const { return m_bHasReceivedGCWelcome; }
	const char *GetTxnCountryCode() const { return m_sTxnCountryCode.Get(); }

protected:
	// Set, together with the fields below, by ProcessWelcomeMessage( const CMsgClientWelcome & )
	bool m_bHasReceivedGCWelcome;
	// The GC's rtime32 minus our own at the moment the welcome arrived
	int m_nGCTimeOffset;
	RTime32 m_rtGCTimeAtWelcome;
	double m_dblPlatTimeAtWelcome;
	// ECurrency, k_ECurrencyInvalid (255) until the welcome sets it
	uint32 m_eCurrency;
	CUtlString m_strPartnerAccountBalanceURL;
	CUtlString m_sTxnCountryCode;
	uint32 m_unPartnerAccountBalance;
	bool m_bPartnerAccountBalanceValid;

	CMsgClientWelcome_Location m_location;

	// Named by what it holds: a map from heap-allocated key strings (freed by the destructor) to
	// strings. Not in CS:GO; nothing in server.dll or libserver.so adds to it.
	CUtlMap< char *, CUtlString, int > m_mapKeyedStrings;

private:
	bool m_bInittedGC;
	GCSDK::CGCClient m_GCClient;

	// CCallback< CGCClientSystem, SteamServersConnected_t, false > in client.dll
	CCallback< CGCClientSystem, SteamServersConnected_t, true > m_CallbackLogonSuccess;
};

//-----------------------------------------------------------------------------
// The server's GC client system (server.dll / libserver.so). Its shared object
// listener overrides and its own virtual methods are empty in this build.
//-----------------------------------------------------------------------------
class CCSGCServerSystem : public CGCClientSystem, public GCSDK::ISharedObjectListener
{
public:
	DECLARE_GAME_SYSTEM();

	CCSGCServerSystem();
	virtual ~CCSGCServerSystem();

	virtual void PostInit() override;
	// Removes the shared object listener (when m_steamIDListenedSOCacheOwner is valid), then
	// shuts the base down
	virtual void Shutdown() override;

	// Calls the base, then CSource2Server::UpdateGCInformation
	GS_EVENT( GameFrameBoundary );

	// Forward to the game system factory
	virtual void SetGameSystemGlobalPtrs( void *pValue ) override;
	virtual bool DoesGameSystemReallocate() override;

protected:
	// Installs PopulateGCServerHandshake as m_GCClient's GC hello populator
	virtual void PreInitGC() override;
	virtual void PostInitGC() override;

public:
	// Named by what they do: empty in server.dll and libserver.so, never called through the
	// vtable. Their signatures are not recoverable from an empty body.
	virtual void EmptyServerSystemHook1();
	virtual void EmptyServerSystemHook2();
	virtual void EmptyServerSystemHook3();

	// GCSDK::ISharedObjectListener, all empty
	virtual void SOCreated( GCSDK::SOID_t owner, const GCSDK::CSharedObject *pObject, GCSDK::ESOCacheEvent eEvent ) override;
	virtual void SOUpdated( GCSDK::SOID_t owner, const GCSDK::CSharedObject *pObject, GCSDK::ESOCacheEvent eEvent ) override;
	virtual void SODestroyed( GCSDK::SOID_t owner, const GCSDK::CSharedObject *pObject, GCSDK::ESOCacheEvent eEvent ) override;
	virtual void SOCacheSubscribed( GCSDK::SOID_t owner, GCSDK::CGCClientSharedObjectCache *pCache, GCSDK::ESOCacheEvent eEvent ) override;
	virtual void SOCacheUnsubscribed( GCSDK::SOID_t owner, GCSDK::CGCClientSharedObjectCache *pCache, GCSDK::ESOCacheEvent eEvent ) override;

protected:
	// The fields below are named by what the code does with them; no source names them.

	// Zeroed by the constructor, read nowhere
	uint32 m_unZeroedAtConstruction[ 2 ];
	// Not initialised by the constructor, read nowhere
	uint32 m_unNeverInitialized[ 3 ];
	// Shutdown removes this system from m_GCClient's listeners only while this ID is valid
	CSteamID m_steamIDListenedSOCacheOwner;
	// Default-constructed, read nowhere
	CSteamID m_steamIDNeverRead;
	// Zeroed by the constructor, read nowhere
	uint64 m_ulZeroedAtConstruction;
	// Cleared by the GC welcome handler (CMsgClientWelcome job)
	uint32 m_nClearedOnGCWelcome;
	// Zeroed by the constructor together with m_nClearedOnGCWelcome, read nowhere
	uint32 m_nZeroedWithClearedOnGCWelcome;
	// Incremented by the GC welcome handler ("GC Connection established for server version %d,
	// instance idx %d"); CSource2Server::UpdateGCInformation resends the server info when it changes
	uint32 m_nGCConnectionInstanceIdx;
};

#endif // GC_CLIENTSYSTEM_H
