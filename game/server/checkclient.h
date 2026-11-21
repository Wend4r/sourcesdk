//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Picks one client every few ticks and keeps its PVS for the NPC
//          "can see any client" checks.
//
//=============================================================================//

#ifndef CHECKCLIENT_H
#define CHECKCLIENT_H
#ifdef _WIN32
#pragma once
#endif

#include "igamesystem.h"
#include "entityhandle.h"

//-----------------------------------------------------------------------------
// Registered as "CCheckClient" (reallocating factory, 552 bytes). CS2 dropped the cluster and
// visibility PVS members. It adds no virtual methods to IGameSystem.
//-----------------------------------------------------------------------------
class CCheckClient : public CAutoGameSystem
{
public:
	DECLARE_GAME_SYSTEM();

	CCheckClient();
	virtual ~CCheckClient();

	// Resets m_hCheckClient and m_nLastCheckTick
	GS_EVENT( GameInit );
	// Every 7 ticks: m_lastcheck moves on to the next connected client
	GS_EVENT( ServerPreClientUpdate );

	// Forward to the game system factory
	virtual void SetGameSystemGlobalPtrs( void *pValue ) override;
	virtual bool DoesGameSystemReallocate() override;

public:
	// The PVS of m_hCheckClient (copied in as 0x208 bytes)
	uint32 m_checkPVS[ 130 ];
	// Named by what it holds: the client whose PVS is in m_checkPVS
	CEntityHandle m_hCheckClient;
	// The client ServerPreClientUpdate picked last
	CEntityHandle m_lastcheck;
	// The tick at which the next client is picked
	int m_nLastCheckTick;
};

#endif // CHECKCLIENT_H
