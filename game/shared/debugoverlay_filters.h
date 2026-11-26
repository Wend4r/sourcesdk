//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Filters of the debug overlay recorders (CDebugOverlayRecorderBase::m_pFilter)
//
//=============================================================================//

#ifndef DEBUGOVERLAY_FILTERS_H
#define DEBUGOVERLAY_FILTERS_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "tier0/threadtools.h"
#include "tier0/utlvector.h"

// The recorder entry that owns a filter (24 bytes: the filter, an int, the recorder)
struct DebugOverlayRecorderEntry_t;

//-----------------------------------------------------------------------------
// The base of the filters of a debug overlay recorder, 56 bytes on both platforms. Eleven slots
// on both platforms, no virtual destructor; the two PassesFilter overloads swap places on
// Windows. The game derives the AI event, combined, entity, pathfinding, schedule, tactical,
// task and text filters (CDebugOverlay<Kind>Filter) from it; none adds a virtual method.
//
// The records are 304-byte structures of the recorder; the UI is the debug overlay UI
// interface. Neither type is described here.
//-----------------------------------------------------------------------------
abstract_class CDebugOverlayFilterBase
{
public:
	// Empty in the base and in every filter
	virtual void Unk_0() = 0;
	// Named by what it does: "" in the base, the filter text in the entity filter
	virtual const char *GetFilterString() = 0;
	// Named by what it does: draws the common controls; the filters call it after their own
	virtual void DrawFilterSettings( void *pUI ) = 0;
	// Named by what they do. The base returns false for a text; the record overload passes the
	// text of the record to the other one (the entity filter matches name and entity index)
	virtual bool PassesFilter( const char *pszText ) = 0;
	virtual bool PassesFilter( const void *pRecord ) = 0;
	// Named by what it does: true when the record or one of its child records passes
	virtual bool PassesFilterRecursive( const void *pRecord ) = 0;
	// Named by what it does: false in the base; the filters test their filter text or value
	virtual bool IsFilterActive() = 0;
	// Named by what it does: clears m_MatchingSnapshots and calls FindAllMatchingSnapshots
	virtual void Reset() = 0;
	// Named by what it does: returns m_MatchingSnapshots
	virtual CUtlVector< int > *GetMatchingSnapshots() = 0;
	// Returns true in the base, false in the combined filter
	virtual bool Unk_9() = 0;
	virtual void FindAllMatchingSnapshots() = 0;

public:
	// The snapshots of the recording that pass the filter, in ascending order; the overlay steps
	// through them. Named by what it holds
	CUtlVector< int > m_MatchingSnapshots;
	// Held while FindAllMatchingSnapshots adds to m_MatchingSnapshots. Named by what it is
	CThreadFastMutex m_MatchingSnapshotsMutex;
	// The entry of the recorder that owns the filter; the filters reach the recorder through it.
	// Named by what it holds
	DebugOverlayRecorderEntry_t *m_pRecorderEntry;
};

#endif // DEBUGOVERLAY_FILTERS_H
