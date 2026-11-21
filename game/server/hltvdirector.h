//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The SourceTV director: picks the camera shots the HLTV servers
//			broadcast.
//
//=============================================================================//

#ifndef HLTVDIRECTOR_H
#define HLTVDIRECTOR_H
#ifdef _WIN32
#pragma once
#endif

#include "igamesystem.h"
#include "ihltvdirector.h"
#include "entity2/entityindex.h"
#include "mathlib/vector.h"

#include <tier0/utlrbtree.h>
#include <tier0/utlvector.h>

// CGameEventListener
#include <vscript/ccscript.h>

class CBaseEntity;
class CBasePlayerController;
class CBasePlayerPawn;
class IGameEvent;
struct Source1LegacyGameEvent_t;

#define	HLTV_MIN_DIRECTOR_DELAY		3	// minimum delay if director is enabled
#define	HLTV_MAX_DELAY				120	// maximum delay

#define MAX_NUM_CAMERAS				64	// support up to 64 fixed cameras per level

//-----------------------------------------------------------------------------
// A game event the director keeps to build shots from, 24 bytes
//-----------------------------------------------------------------------------
class CHLTVGameEvent
{
public:
	int m_Tick;			// tick of this command
	int m_Priority;		// game event priority
	// Running number of the event (CHLTVDirector::m_iRunningIndex when it was added)
	int64 m_Index;
	IGameEvent *m_Event;
};

// The order of m_EventHistory: by tick (the tree of the game stores no function pointer)
class CHLTVGameEventLess
{
public:
	bool operator()( const CHLTVGameEvent &e1, const CHLTVGameEvent &e2 ) const { return e1.m_Tick < e2.m_Tick; }
};

//-----------------------------------------------------------------------------
// 1248 bytes on both platforms (reallocating factory "HLTVDirector"; CCSHLTVDirector is the
// static instance the server uses). The members CS2 added are named by what the code does.
//
// The primary table is the one of CGameEventListener. On Windows it holds the destructor,
// FireGameEvent and the 18 methods the class adds. On Linux it repeats there, after
// FireGameEvent, every override of the other bases in declaration order: slots 3-5 the
// DECLARE_GAME_SYSTEM trio (YouForgot..., DoesGameSystemReallocate, SetGameSystemGlobalPtrs),
// 6-23 the IHLTVDirector methods, 24-27 Init, Shutdown, ServerPostEntityThink and
// PostSpawnGroupLoad, then the own methods at 28-45 - so they are declared in that order.
// The IHLTVDirector method names are the ones of ihltvdirector.h.
//-----------------------------------------------------------------------------
class CHLTVDirector : public CGameEventListener, public IGameSystem, public IHLTVDirector
{
public:
	DECLARE_GAME_SYSTEM();

	CHLTVDirector();
	virtual ~CHLTVDirector();

	// Forward to the game system factory
	virtual bool DoesGameSystemReallocate() override;
	virtual void SetGameSystemGlobalPtrs( void *pValue ) override;

	// IHLTVDirector
	// True while an HLTV server with a client (type 1) is registered
	virtual bool IsActive( void ) override;
	// Type 1 (an HLTV server with its fake client): adds a record to m_HltvServers, fatal
	// error without the client controller, and listens to the director's events (the ids go
	// to the m_n...EventId members); type 0: adds the server to m_HltvServersWithoutClient.
	// Calls RemoveHLTVServer first
	virtual void AddHLTVServer( int nType, IHLTVServer *hltv ) override;
	virtual void RemoveHLTVServer( int nType, IHLTVServer *hltv ) override;
	virtual int GetHLTVServerCount( int nType ) override;
	virtual IHLTVServer *GetHLTVServer( int nType, int instance ) override;
	// m_nBroadcastTick
	virtual int GetDirectorTick( void ) override;
	// m_iPVSEntity
	virtual int GetPVSEntity( void ) override;
	// m_vPVSOrigin
	virtual Vector GetPVSOrigin( void ) override;
	// m_fDelay
	virtual float GetDelay( void ) override;
	virtual const char **GetModEvents() override;
	// Fires "hltv_versioninfo"
	virtual void OnStartRecording( void *pUnusedArg ) override;
	virtual void OnHLTVClientDisconnect( void *pUnusedArg ) override;
	// Returns -1 here
	virtual int GetDirectorCameraEventId() override;
	virtual void BroadcastLocalTitle( int nPlayerSlot ) override;
	virtual void CreateLocalChatMessage( const CSteamID &steamID, const char *pszName, const char *pszText, INetworkMessageInternal **ppSerializer, CNetMessage **ppMessage ) override;
	// Writes m_pLastCameraEvent into the snapshot
	virtual void OnHLTVUncompressedSnapshot( int nPlayerSlot, bf_write &buf ) override;
	// Keeps a copy of a legacy game event whose id is GetDirectorCameraEventId() in m_pLastCameraEvent
	virtual void OnHLTVNetMessage( const NetMessageInfo_t *pInfo, const CNetMessage *pData ) override;
	// Empty here
	virtual void UpdateDirector( float flDelay ) override;

	// Returns true
	virtual bool Init() override;
	// Deletes m_pLastCameraEvent, empties m_EventHistory and clears m_iRunningIndex
	virtual void Shutdown() override;
	// UpdateSettings, AnalyzeCameras and AnalyzePlayers every 32 ticks (m_nNextAnalyzeTick),
	// then StartDelayMessage or StartNewShot
	GS_EVENT( ServerPostEntityThink );
	// BuildCameraList and resets the shot state
	GS_EVENT( PostSpawnGroupLoad );

	// IGameEventListener2: adds the event to m_EventHistory with its "priority"
	virtual void FireGameEvent( IGameEvent *event ) override;

	// "point_viewcontrol"
	virtual const char *GetFixedCameraEntityName( void );

protected:
	virtual void StartNewShot();
	virtual void StartRandomShot();
	virtual void StartDelayMessage();
	virtual void StartBestFixedCameraShot( bool bForce );
	virtual void StartBestPlayerCameraShot();
	virtual void StartFixedCameraShot( int iCamera, int iTarget );
	virtual void StartChaseCameraShot( CEntityIndex iTarget1, CEntityIndex iTarget2, int distance, int phi, int theta, bool bInEye );
	virtual void UpdateSettings();
	virtual void AnalyzePlayers();
	virtual void AnalyzeCameras();
	virtual bool StartCameraManShot();
	virtual void StartInstantBroadcastShot();
	virtual void FinishCameraManShot();
	// Fills m_pActivePlayers with the living pawns of the playing teams
	virtual void BuildActivePlayerList();
	virtual CHLTVGameEvent *FindBestGameEvent();
	virtual void CreateShotFromEvent( CHLTVGameEvent *ge );
	// Empty here
	virtual void NewEventNotification( CHLTVGameEvent *ge );

public:
	// An HLTV server of type 1 and its fake client
	struct HltvServerRecord_t
	{
		IHLTVServer *m_pHLTVServer;	// interface to servers HLTV object
		CBasePlayerController *m_pHLTVClient; // the HLTV fake client
	};

	int m_nBroadcastTick; // world time that is currently "on the air"
	int m_nNextShotTick;	// time for the next scene cut
	CUtlRBTree< CHLTVGameEvent, CHLTVGameEventLess, int > m_EventHistory;
	float m_fDelay;	// hltv delay in seconds
	// The HLTV servers of type 1 (with a fake client)
	CUtlVector< HltvServerRecord_t > m_HltvServers;
	// Named by what the code does: the HLTV servers of type 0 (without a client)
	CUtlVector< IHLTVServer * > m_HltvServersWithoutClient;
	CEntityIndex m_iPVSEntity;	// entity for PVS center
	Vector m_vPVSOrigin;	// PVS origin if PVS entity is 0
	int m_nNextAnalyzeTick;
	int m_nNumFixedCameras;	//number of cameras in current map
	CBaseEntity *m_pFixedCameras[ MAX_NUM_CAMERAS ]; // fixed cameras (point_viewcontrol)
	int m_nNumActivePlayers;
	CBasePlayerPawn *m_pActivePlayers[ 64 ];
	CEntityIndex m_iCameraManIndex;		// entity index of current camera man or -1
	// Numbers the events of m_EventHistory (CHLTVGameEvent::m_Index)
	int m_iRunningIndex;
	// Copy of the last camera event (OnHLTVNetMessage), written into the snapshots
	Source1LegacyGameEvent_t *m_pLastCameraEvent;
	// Named by what the code does: the ids AddHLTVServer gets when it listens to the events
	int m_nPlayerHurtEventId;
	int m_nPlayerDeathEventId;
	int m_nRoundEndEventId;
	int m_nRoundStartEventId;
	int m_nCameramanEventId;
	int m_nRankEntityEventId;
	int m_nRankCameraEventId;
	// Only the constructors write these two (false); nothing reads them. Named by that
	bool m_bNeverReadA;
	bool m_bNeverReadB;
};

#endif // HLTVDIRECTOR_H
