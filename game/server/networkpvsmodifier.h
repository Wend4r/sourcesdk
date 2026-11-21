//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The game system that keeps the entities which add areas to the
//			network PVS of the clients (factory "PVSModifier"). No Valve path
//			names this header.
//
//=============================================================================//

#ifndef NETWORKPVSMODIFIER_H
#define NETWORKPVSMODIFIER_H
#ifdef _WIN32
#pragma once
#endif

#include "igamesystem.h"
#include "ehandle.h"
#include "tier0/utlvector.h"

class CBaseEntity;

//-----------------------------------------------------------------------------
// Static instance, 72 bytes on both platforms. The game declares
// DECLARE_GAME_SYSTEM after the first vector: its member sits between the two.
// The fields are named by what the code does with them.
//-----------------------------------------------------------------------------
class CNetworkPVSModifierEntityManager : public CAutoGameSystem
{
public:
	// Empty
	GS_EVENT( GameInit );
	// Empties m_Modifiers
	GS_EVENT( GameShutdown );
	// Drops the handles of deleted modifiers from m_Modifiers
	GS_EVENT( GameFrameBoundary );

	// Forward to the game system factory
	virtual void SetGameSystemGlobalPtrs( void *pValue ) override;
	virtual bool DoesGameSystemReallocate() override;

public:
	// The entities that add their origin (or their bounds) to the PVS of each client when the
	// server sets up the client's PVS; each entity adds itself once and removes itself on
	// deletion
	CUtlVector< CHandle< CBaseEntity > > m_Modifiers;

	DECLARE_GAME_SYSTEM();

public:
	// Constructed and freed by the destructor only: nothing in the server adds
	// to it, so its element type is not recoverable (trivially destructible, declared like
	// m_Modifiers)
	CUtlVector< CHandle< CBaseEntity > > m_UnusedModifiers;
};

#endif // NETWORKPVSMODIFIER_H
