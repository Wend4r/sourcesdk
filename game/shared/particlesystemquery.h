//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The game system that hands the particle system a query object per
//			world. Factory "ParticleSystemQueryGameSystem".
//
//=============================================================================//

#ifndef PARTICLESYSTEMQUERY_H
#define PARTICLESYSTEMQUERY_H
#ifdef _WIN32
#pragma once
#endif

#include "igamesystem.h"
#include "tier0/utllinkedlist.h"
#include "tier0/utlhashtable.h"
#include "tier0/utlstringtoken.h"
#include "tier0/utlvector.h"

// The particle system query of one world: polymorphic, 16 bytes, the world's
// token at offset 8
class CParticleSystemQuery;

//-----------------------------------------------------------------------------
// Static instance, 112 bytes on both platforms.
//-----------------------------------------------------------------------------
class CParticleSystemQueryGameSystem : public CAutoGameSystem
{
public:
	DECLARE_GAME_SYSTEM();

	// Finds or creates the query of the spawn group's world and hands it to the particle
	// systems of that world; a found one gets one more reference
	GS_EVENT( PreSpawnGroupLoad );
	// Empty
	GS_EVENT( PostSpawnGroupLoad );
	// Releases a reference of the spawn group's world query, queueing it for deletion at zero
	GS_EVENT( PostSpawnGroupUnload );
	// Deletes the queued queries
	GS_EVENT( ClientPostRender );

	// Forward to the game system factory
	virtual void SetGameSystemGlobalPtrs( void *pValue ) override;
	virtual bool DoesGameSystemReallocate() override;

public:
	struct QueryInfo_t
	{
		CParticleSystemQuery *m_pQuery;
		int m_nRefCount;
	};

	CUtlLinkedList< QueryInfo_t, int > m_queries;
	// World token -> index into m_queries
	CUtlHashtable< CUtlStringToken, int > m_lookup;
	CUtlVector< CParticleSystemQuery * > m_queuedQueryDeletions;
};

#endif // PARTICLESYSTEMQUERY_H
