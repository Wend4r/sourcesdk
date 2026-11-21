//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The game system that retires the oldest ragdolls. Factory "RagdollLRURetirement".
//
//=============================================================================//

#ifndef RAGDOLL_SHARED_H
#define RAGDOLL_SHARED_H
#ifdef _WIN32
#pragma once
#endif

#include "igamesystem.h"
#include "ehandle.h"
#include "entitytypes.h"
#include "tier0/utllinkedlist.h"

class CBaseAnimGraph;

//-----------------------------------------------------------------------------
// Static instance, 88 bytes on both platforms. The lists keep the time each ragdoll was added.
//-----------------------------------------------------------------------------
class CRagdollLRURetirement : public CAutoGameSystem
{
public:
	DECLARE_GAME_SYSTEM();

	// Empties both lists and resets m_iMaxRagdolls to -1 (use g_ragdoll_maxcount)
	GS_EVENT( GameInit );
	// Removes the deleted and the oldest ragdolls beyond the limit, counts the others
	GS_EVENT( ClientPostUpdate );
	// Calls ClientPostUpdate
	GS_EVENT( ServerPostEntityThink );

	// Forward to the game system factory
	virtual void SetGameSystemGlobalPtrs( void *pValue ) override;
	virtual bool DoesGameSystemReallocate() override;

public:
	// Named by what it holds
	struct LRURagdoll_t
	{
		CHandle< CBaseAnimGraph > m_hRagdoll;
		// Game time the ragdoll was added; it may be retired once it is older than
		// ragdoll_lru_min_age
		GameTime_t m_flAddedTime;
	};

	CUtlLinkedList< LRURagdoll_t > m_LRU;
	CUtlLinkedList< LRURagdoll_t > m_LRUImportantRagdolls;
	// -1 uses the g_ragdoll_maxcount convar
	int m_iMaxRagdolls;
	// Counted by ClientPostUpdate
	int m_iSimulatedRagdollCount;
	int m_iRagdollCount;
};

#endif // RAGDOLL_SHARED_H
