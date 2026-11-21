//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Keeps particle snapshots alive for a while after their users let them go.
//
//=============================================================================//

#ifndef SERVERTRANSIENTSNAPSHOTCACHE_H
#define SERVERTRANSIENTSNAPSHOTCACHE_H
#ifdef _WIN32
#pragma once
#endif

#include "igamesystem.h"
#include "particles/iparticlesystemmgr.h"
#include "resourcesystem/resourcehandle.h"

#include <tier0/utlvector.h>

//-----------------------------------------------------------------------------
// Registered as "CServerTransientSnapshotCache" (static factory). 48 bytes on both platforms. No
// source has the class; the names are named by what the code does. The breakable prop code adds
// the snapshot of its debris particles for one second (64 ticks).
//-----------------------------------------------------------------------------
class CServerTransientSnapshotCache : public CAutoGameSystem
{
public:
	DECLARE_GAME_SYSTEM();

	struct CachedSnapshot_t
	{
		// A strong reference: released through IResourceHandleUtils when the entry goes
		CStrongHandle< InfoForResourceTypeIParticleSnapshot > m_hSnapshot;
		// gpGlobals->tickcount at which the entry is dropped
		int m_nExpireTick;
	};

	virtual ~CServerTransientSnapshotCache();

	// Releases every snapshot
	GS_EVENT( GameShutdown );
	// Releases the snapshots whose m_nExpireTick has passed
	GS_EVENT( ServerAdvanceTick );

	// Forward to the game system factory
	virtual void SetGameSystemGlobalPtrs( void *pValue ) override;
	virtual bool DoesGameSystemReallocate() override;

public:
	CUtlVector< CachedSnapshot_t > m_Snapshots;
};

#endif // SERVERTRANSIENTSNAPSHOTCACHE_H
