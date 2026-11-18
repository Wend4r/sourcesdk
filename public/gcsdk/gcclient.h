//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Holds the CGCClient class
//
//=============================================================================

#ifndef GCCLIENT_H
#define GCCLIENT_H
#ifdef _WIN32
#pragma once
#endif

#include "steam/steam_api.h"
#include "steam/isteamgamecoordinator.h"
#include "tier0/utlleanvector.h"
#include "tier0/utlmap.h"
#include "tier0/utlrbtree.h"
#include "tier0/utlvector.h"
#include "jobmgr.h"
#include "sharedobject.h"
#include "gcclient_sharedobjectcache.h"

class ISteamGameCoordinator;
class CTestEvent;
class CMsgServerHello;
enum GCConnectionStatus : int;

namespace GCSDK
{


//-----------------------------------------------------------------------------
// Purpose: base for CGCMsgHandler
//			used only by CGCMsgHandler, shouldn't be used directly
//-----------------------------------------------------------------------------
class CGCClient
{
public:
	CGCClient( bool bGameserver = false );
	virtual ~CGCClient( );

	// Queries the game coordinator, steam user (or game server) and utils interfaces from
	// pSteamClient, starts the job manager and registers the steam callbacks
	bool BInit( uint32 unVersion, ISteamClient *pSteamClient, HSteamUser hSteamUser, HSteamPipe hSteamPipe );
	void Uninit( );
	bool BMainLoop( uint64 ulLimitMicroseconds, uint64 ulFrameTimeMicroseconds = 0 );

	CJobMgr &GetJobMgr() { return m_JobMgr; }

	bool BSendMessage( uint32 unMsgType, const uint8 *pubData, uint32 cubData );
	bool BSendMessage( const CGCMsgBase& msg );
	bool BSendMessage( const CProtoBufMsgBase& msg );

	/// Locate a given shared object from the cache
	CSharedObject *FindSharedObject( const SOID_t &owner, const CSharedObject & soIndex );

	/// Find a shared object cache for the specified owner.  Optionally, the cache will be
	/// created if it doesn't not currently exist.
	CGCClientSharedObjectCache *FindSOCache( const SOID_t &owner, bool bCreateIfMissing = true );

	/// Adds a listener. Listeners are global to the client, not bound to a single cache,
	/// so the listener is notified for every cache and must match the owner SOID itself.
	/// Adding a listener that is already registered is harmlessly ignored.
	bool AddListener( ISharedObjectListener *pListener );

	/// Removes a listener. The listener immediately receives SOCacheUnsubscribed for every
	/// cache it is currently subscribed to. Returns true if it was registered and removed.
	bool RemoveListener( ISharedObjectListener *pListener );

	/// Handles a k_ESOMsg_CacheSubscribed body: finds or creates the cache for the owner
	/// named in the message, parses every object in it, then notifies the listeners.
	void HandleSOCacheSubscribedMsg( const CMsgSOCacheSubscribed &msg );

	void NotifySOCreated( const SOID_t &owner, const CSharedObject *pObject, ESOCacheEvent eEvent );
	void NotifySOUpdated( const SOID_t &owner, const CSharedObject *pObject, ESOCacheEvent eEvent );
	void NotifySODestroyed( const SOID_t &owner, const CSharedObject *pObject, ESOCacheEvent eEvent );
	void NotifySOCacheSubscribed( const SOID_t &owner, CGCClientSharedObjectCache *pCache, ESOCacheEvent eEvent );
	void NotifySOCacheUnsubscribed( const SOID_t &owner, CGCClientSharedObjectCache *pCache, ESOCacheEvent eEvent );

	void OnGCMessageAvailable( GCMessageAvailable_t *pCallback );
	ISteamGameCoordinator *GetSteamGameCoordinator() { return m_pSteamGameCoordinator; }

	virtual void Test_AddEvent( CTestEvent *pEvent )	{}
	virtual void Test_CacheSubscribed( const SOID_t &owner ) {}

	void Dump();

protected:

	ISteamUser *m_pSteamUser;
	ISteamGameServer *m_pSteamGameserver;
	ISteamGameCoordinator *m_pSteamGameCoordinator;
	// "SteamUtils010", not in CS:GO
	ISteamUtils *m_pSteamUtils;
	CUtlLeanVector< uint8 > m_memMsg;

	// local job handling
	CJobMgr m_JobMgr;

	// Shared object caches
	CUtlOrderedMap< SOID_t, CGCClientSharedObjectCache *, CDefLess< SOID_t >, unsigned short > m_mapSOCache;

	// Listeners are global to the client rather than per-cache
	CUtlVector< ISharedObjectListener * > m_vecListeners;

	// Message types that may be sent before a session with the GC exists
	CUtlRBTree< uint32, CDefLess< uint32 >, unsigned short > m_rbAllowedConnectionlessMessages;

	// Named by what it does: a game server's SendHello calls it with the k_EMsgGCServerHello body
	// before sending it (CCSGCServerSystem::PreInitGC installs PopulateGCServerHandshake)
	void ( *m_pfnPopulateServerHello )( CMsgServerHello *pMsg );

	// Named by what it does: hellos sent since the connection status last changed
	int m_nHelloAttempts;
	// Time of the next hello (randomised retry delay)
	uint64 m_timeLastSendHello;
	uint64 m_timeReceivedConnectionStatus;
	uint64 m_timeLoggedOn;
	uint32 m_unVersion;
	// k_GCConnectionStatus_NO_STEAM until the GC answers; not in CS:GO
	GCConnectionStatus m_eConnectionStatus;
	const bool m_bGameserver;
	// CS:GO's bool m_bSimulateGCConnectionFailure became a mode: 0 off, 1 down, 2 receive only
	int m_eSimulateGCConnectionFailure;
	uint32 m_nSessionNeed;
	// last session need state sent / received from the GC
	uint32 m_nLastSessionNeed;
	bool m_bWantSession;
	uint32 m_nLauncherType;
	int m_nLogonQueuePosition;
	int m_nLogonQueueSize;
	uint64 m_timeLogonQueueApproxTimeEnteredQueue;
	uint64 m_timeLogonQueueEstimatedTimeExitQueue;

	CCallback< CGCClient, GCMessageAvailable_t, false > m_callbackGCMessageAvailable;
	CCallback< CGCClient, SteamServersDisconnected_t, false > m_CallbackSteamServersDisconnected;
	CCallback< CGCClient, SteamServerConnectFailure_t, false > m_CallbackSteamServerConnectFailure;
	CCallback< CGCClient, SteamServersConnected_t, false > m_CallbackSteamServersConnected;

};


} // namespace GCSDK

//utility to make defining client jobs more straight forward
#define GC_REG_CLIENT_JOB( JobClass, Msg )	\
	GC_REG_JOB( GCSDK::CGCClient, JobClass, #JobClass, Msg, GCSDK::k_eGCMsgContext_All )

#endif // GCCLIENT_H
