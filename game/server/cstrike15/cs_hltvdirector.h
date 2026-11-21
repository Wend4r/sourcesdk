//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The Counter-Strike SourceTV director. The server's static instance,
//			exposed as "Source2GameDirector001" (g_pSource2GameDirector).
//
//=============================================================================//

#ifndef CSTRIKE15_CS_HLTVDIRECTOR_H
#define CSTRIKE15_CS_HLTVDIRECTOR_H
#ifdef _WIN32
#pragma once
#endif

#include "../hltvdirector.h"

//-----------------------------------------------------------------------------
// Static instance, 1304 bytes on both platforms. On Linux the member DECLARE_GAME_SYSTEM()
// adds takes byte 1246, in the tail padding of CHLTVDirector; on Windows it is at 1248 and
// the event ids follow at 1252. The event id members are named by what the code does
// (AddHLTVServer stores the id of each event it listens to, in this order).
//-----------------------------------------------------------------------------
class CCSHLTVDirector : public CHLTVDirector
{
public:
	DECLARE_GAME_SYSTEM();

	CCSHLTVDirector();
	virtual ~CCSHLTVDirector();

	// Forward to the game system factory; SetGameSystemGlobalPtrs also sets the global the
	// Counter-Strike code reads the director from
	virtual bool DoesGameSystemReallocate() override;
	virtual void SetGameSystemGlobalPtrs( void *pValue ) override;

	// CHLTVDirector::AddHLTVServer, then (type 1) listens to the hostage, bomb and vip events
	virtual void AddHLTVServer( int nType, IHLTVServer *hltv ) override;
	// The game events relayed to spectator clients
	virtual const char **GetModEvents() override;
	// Runs the director update of CHLTVDirector, which is not virtual
	virtual void UpdateDirector( float flDelay ) override;

protected:
	// Chase shots for the hostage and bomb events, else CHLTVDirector::CreateShotFromEvent
	virtual void CreateShotFromEvent( CHLTVGameEvent *event ) override;

public:
	int m_nHostageRescuedEventId;
	int m_nHostageKilledEventId;
	int m_nHostageHurtEventId;
	int m_nHostageFollowsEventId;
	int m_nBombPickupEventId;
	int m_nBombDroppedEventId;
	int m_nBombExplodedEventId;
	int m_nBombDefusedEventId;
	int m_nBombPlantedEventId;
	int m_nBombBeginDefuseEventId;
	int m_nBombBeginPlantEventId;
	int m_nVipEscapedEventId;
	int m_nVipKilledEventId;
};

#endif // CSTRIKE15_CS_HLTVDIRECTOR_H
