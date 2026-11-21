//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Collects the entities whose spatial partition entry must be updated
//          and updates them in one pass.
//
//=============================================================================//

#ifndef DIRTYSPATIALPARTITIONENTITYLIST_H
#define DIRTYSPATIALPARTITIONENTITYLIST_H
#ifdef _WIN32
#pragma once
#endif

#include "igamesystem.h"
#include "entityhandle.h"

#include <tier0/threadtools.h>
#include <tier0/utlleanvector.h>

//-----------------------------------------------------------------------------
// Registered as "DirtySpatialPartitionEntityList" (static factory). The dirty entities are a
// lock-free array plus an overflow array, and the hot members sit on their own 128-byte lines.
// Other names are descriptive. It adds no virtual methods to IGameSystem.
//-----------------------------------------------------------------------------
class CDirtySpatialPartitionEntityList : public CAutoGameSystem
{
public:
	DECLARE_GAME_SYSTEM();

	CDirtySpatialPartitionEntityList();
	virtual ~CDirtySpatialPartitionEntityList();

	// Returns true
	virtual bool Init() override;
	// Empty
	virtual void Shutdown() override;
	// m_nProcessedCount = m_nDirtyCount
	GS_EVENT( GameInit );
	// Processes and clears the list
	GS_EVENT( GameShutdown );

	// Forward to the game system factory
	virtual void SetGameSystemGlobalPtrs( void *pValue ) override;
	virtual bool DoesGameSystemReallocate() override;

public:
	// Written lock-free: an adder takes slot m_nDirtyCount++ while it is inside the array, otherwise
	// it appends to m_OverflowEntities under m_OverflowMutex
	alignas( 128 ) CUtlLeanVector< CEntityHandle > m_DirtyEntities;
	CUtlLeanVector< CEntityHandle > m_OverflowEntities;
	CThreadFastMutex m_OverflowMutex;
	// The number of dirty entities (atomic)
	alignas( 128 ) int m_nDirtyCount;
	// -1 while the list is processed, then the count processed; GameInit copies m_nDirtyCount
	int m_nProcessedCount;
	// Held while the list is processed
	alignas( 128 ) CThreadFastMutex m_partitionMutex;
	// Processing is skipped while it is above 0
	int m_nDeferProcessingCount;
};

#endif // DIRTYSPATIALPARTITIONENTITYLIST_H
