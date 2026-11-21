//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The global entity states. Factory "GlobalState".
//
//=============================================================================//

#ifndef GLOBALSTATE_H
#define GLOBALSTATE_H
#ifdef _WIN32
#pragma once
#endif

#include "igamesystem.h"
#include "tier0/utlsymbol.h"
#include "tier0/utlvector.h"

// One byte in globalentity_t
enum GLOBALESTATE : uint8
{
	GLOBAL_OFF = 0,
	GLOBAL_ON = 1,
	GLOBAL_DEAD = 2,
};

// 12 bytes
struct globalentity_t
{
	CUtlSymbol name;
	CUtlSymbol levelName;
	GLOBALESTATE state;
	int counter;
};

struct globalentitydatabase_t
{
	CUtlSymbolTable m_nameList;
	CUtlVector< globalentity_t > m_list;
};

//-----------------------------------------------------------------------------
// The GlobalEntity_* functions as an interface; the CS2-only ones are named by what they do. Invalid
// indices are ignored (0 / NULL), and the setters do nothing while state updates
// are disabled.
//-----------------------------------------------------------------------------
abstract_class IGlobalEntityMgr
{
public:
	virtual void SetState( int globalIndex, GLOBALESTATE state ) = 0;
	virtual void SetMap( int globalIndex, const char *pMapName ) = 0;
	// Returns the index of the existing global of that name or of the added one
	virtual int Add( const char *pGlobalname, const char *pMapName, GLOBALESTATE state ) = 0;
	// Named by what it does: a counter incremented whenever a global is added and on every
	// reset (GameInit too)
	virtual int GetChangeCount() = 0;
	virtual int GetIndex( const char *pGlobalname ) = 0;
	virtual GLOBALESTATE GetState( int globalIndex ) = 0;
	virtual const char *GetMap( int globalIndex ) = 0;
	virtual const char *GetName( int globalIndex ) = 0;
	virtual int GetCounter( int globalIndex ) = 0;
	virtual void SetCounter( int globalIndex, int counter ) = 0;
	virtual int AddToCounter( int globalIndex, int delta ) = 0;
	virtual int GetNumGlobals() = 0;
	virtual void EnableStateUpdates( bool bEnable ) = 0;
	// Empties the list and the name table
	virtual void Reset() = 0;
	// Named by what it does: Reset, then copies the names and the list of pDatabase
	virtual void CopyFrom( const globalentitydatabase_t *pDatabase ) = 0;
	// Named by what it does: returns the database (its name table first)
	virtual globalentitydatabase_t *GetDatabase() = 0;
};

//-----------------------------------------------------------------------------
// Static instance, 160 bytes on both platforms. libserver.so repeats the sixteen
// IGlobalEntityMgr overrides at the end of the main table (slots 66..81).
//-----------------------------------------------------------------------------
class CGlobalState : public CAutoGameSystem, public IGlobalEntityMgr
{
public:
	DECLARE_GAME_SYSTEM();

	// Counts a change and enables the state updates
	GS_EVENT( GameInit );
	// Empty
	GS_EVENT( GameShutdown );
	// Spawn group loads and unloads switch the state updates (m_disableStateUpdates) and mark
	// the level loaded
	GS_EVENT( PreSpawnGroupLoad );
	GS_EVENT( PostSpawnGroupLoad );
	GS_EVENT( PreSpawnGroupUnload );
	GS_EVENT( PostSpawnGroupUnload );

	// Forward to the game system factory
	virtual void SetGameSystemGlobalPtrs( void *pValue ) override;
	virtual bool DoesGameSystemReallocate() override;

	// IGlobalEntityMgr
	virtual void SetState( int globalIndex, GLOBALESTATE state ) override;
	virtual void SetMap( int globalIndex, const char *pMapName ) override;
	virtual int Add( const char *pGlobalname, const char *pMapName, GLOBALESTATE state ) override;
	virtual int GetChangeCount() override;
	virtual int GetIndex( const char *pGlobalname ) override;
	virtual GLOBALESTATE GetState( int globalIndex ) override;
	virtual const char *GetMap( int globalIndex ) override;
	virtual const char *GetName( int globalIndex ) override;
	virtual int GetCounter( int globalIndex ) override;
	virtual void SetCounter( int globalIndex, int counter ) override;
	virtual int AddToCounter( int globalIndex, int delta ) override;
	virtual int GetNumGlobals() override;
	virtual void EnableStateUpdates( bool bEnable ) override;
	virtual void Reset() override;
	virtual void CopyFrom( const globalentitydatabase_t *pDatabase ) override;
	virtual globalentitydatabase_t *GetDatabase() override;

public:
	globalentitydatabase_t m_database;
	bool m_disableStateUpdates;
	bool m_bLevelLoaded;
};

#endif // GLOBALSTATE_H
