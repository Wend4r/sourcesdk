//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The game journal game system and its null implementation
//			(CGameJournal, CNullGameJournal; factory "CNullGameJournal"). No
//			Valve path names this header.
//
//			Only the null journal exists in the game: no code fills the journal, no code calls its own
//			virtual methods, and the entry class is never instantiated. The
//			members and methods are named by what the code does with them.
//
//=============================================================================//

#ifndef GAMEJOURNAL_H
#define GAMEJOURNAL_H
#ifdef _WIN32
#pragma once
#endif

#include "igamesystem.h"
#include "tier0/utlqueue.h"
#include "tier0/utlrbtree.h"

// Reference counted journal entry: a virtual destructor, the reference count at
// offset 8 (released with an interlocked decrement), 24 bytes
class CGameJournalEntry;

//-----------------------------------------------------------------------------
// Abstract base, 96 bytes on both platforms. It has no DECLARE_GAME_SYSTEM.
//-----------------------------------------------------------------------------
class CGameJournal : public CAutoGameSystem
{
public:
	// The value type of GetLastEntryTick: four bytes, returned through a hidden pointer by
	// MSVC; the null journal returns 0x7FFFFFFF
	struct EntryTick_t
	{
		int m_nTick;
	};

	// The node data of m_EntryTree, named by its size only
	struct EntryTreeData_t
	{
		uint64 m_nFirst;
		uint64 m_nSecond;
	};

	struct EntryTreeLess_t
	{
		bool operator()( const EntryTreeData_t &lhs, const EntryTreeData_t &rhs ) const { return lhs.m_nFirst < rhs.m_nFirst; }
	};

	// Releases the queued entries and m_pActiveEntry, then sets m_bEnabled when the session
	// configuration exists and does not disable it
	GS_EVENT( GameInit );
	// Clears m_bEnabled
	GS_EVENT( GameShutdown );

	// The null journal returns 0
	virtual int GetEntryCount() = 0;
	// The null journal returns NULL
	virtual CGameJournalEntry *GetActiveEntry() = 0;
	// The null journal returns 0x7FFFFFFF
	virtual EntryTick_t GetLastEntryTick() = 0;

public:
	// Entries waiting to be written; GameInit and the destructor release them
	CUtlQueue< CGameJournalEntry * > m_PendingEntries;
	// Released and cleared together with m_PendingEntries
	CGameJournalEntry *m_pActiveEntry;
	bool m_bEnabled;
	// Neither constructed nor accessed by any code in the server or the client
	uint64 m_nNeverAccessed;
	// Constructed empty and freed by the destructor (its nodes need no destructor); nothing
	// inserts into it, so its 16 bytes of node data are not recoverable
	CUtlRBTree< EntryTreeData_t, EntryTreeLess_t, int > m_EntryTree;
};

//-----------------------------------------------------------------------------
// Static instance, 104 bytes on both platforms (constant-initialized on Windows).
//-----------------------------------------------------------------------------
class CNullGameJournal : public CGameJournal
{
public:
	DECLARE_GAME_SYSTEM();

	// Empty
	GS_EVENT( GameInit );
	// Empty
	GS_EVENT( ServerPostSimulate );

	// Forward to the game system factory
	virtual void SetGameSystemGlobalPtrs( void *pValue ) override;
	virtual bool DoesGameSystemReallocate() override;

	virtual int GetEntryCount() override;
	virtual CGameJournalEntry *GetActiveEntry() override;
	virtual EntryTick_t GetLastEntryTick() override;
};

#endif // GAMEJOURNAL_H
