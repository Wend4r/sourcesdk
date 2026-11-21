//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Sends a player's loadout to another player a tick after it is requested.
//
//=============================================================================//

#ifndef CSTRIKE15_SENDPLAYERLOADOUTHELPER_H
#define CSTRIKE15_SENDPLAYERLOADOUTHELPER_H
#ifdef _WIN32
#pragma once
#endif

#include "igamesystem.h"
#include "playerslot.h"

#include <tier0/utlvector.h>

//-----------------------------------------------------------------------------
// Registered as "CSendPlayerLoadoutHelper" (static factory). 48 bytes on both platforms. No
// source has the class; the names are named by what the code does.
//-----------------------------------------------------------------------------
class CSendPlayerLoadoutHelper : public CAutoGameSystem
{
public:
	DECLARE_GAME_SYSTEM();

	// A queued request: send the loadout of m_nLoadoutOwner to m_nRecipient
	struct QueuedLoadout_t
	{
		CPlayerSlot m_nLoadoutOwner;
		CPlayerSlot m_nRecipient;
	};

	virtual ~CSendPlayerLoadoutHelper();

	// Empties m_QueuedLoadouts
	GS_EVENT( GameShutdown );
	// Sends the queued loadouts (honouring the recipient's cl_viewing_vanity_loadout) and empties
	// m_QueuedLoadouts
	GS_EVENT( ServerAdvanceTick );

	// Forward to the game system factory
	virtual void SetGameSystemGlobalPtrs( void *pValue ) override;
	virtual bool DoesGameSystemReallocate() override;

public:
	CUtlVector< QueuedLoadout_t > m_QueuedLoadouts;
};

#endif // CSTRIKE15_SENDPLAYERLOADOUTHELPER_H
