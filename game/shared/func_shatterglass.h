//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The game system that breaks the queued func_shatterglass panes a few per frame.
//
//=============================================================================//

#ifndef FUNC_SHATTERGLASS_H
#define FUNC_SHATTERGLASS_H
#ifdef _WIN32
#pragma once
#endif

#include "igamesystem.h"

#include <entity2/entityidentity.h>
#include <tier0/utlvector.h>
#include <schemasystem/schematypes.h>

class KeyValues3;

//-----------------------------------------------------------------------------
// Registered as "CShatterGlassGameSystem" (static factory). 64 bytes on both platforms. No source
// has the class; the names are named by what the code does.
//-----------------------------------------------------------------------------
class CShatterGlassGameSystem : public CAutoGameSystem
{
public:
	DECLARE_GAME_SYSTEM();

	virtual ~CShatterGlassGameSystem();

	// Destroys m_pKeyValues
	GS_EVENT( GameShutdown );
	// Passes every pane of m_QueuedPanes to one entity function, empties it and clears m_bGlassSpawned
	GS_EVENT( GamePreShutdown );
	// Zeroes m_nPanesBrokenThisFrame
	GS_EVENT( ServerPreEntityThink );
	// Breaks queued panes while m_bEnabled and m_bGlassSpawned are set, fewer than 150 are queued
	// and fewer than 5 broke this frame
	GS_EVENT( ServerPostEntityThink );
	// Warns once when the global shard count passes 2048 ("Global func_shatterglass shard count has
	// exceeded global max!")
	GS_EVENT( ServerPostSimulate );

	// Forward to the game system factory
	virtual void SetGameSystemGlobalPtrs( void *pValue ) override;
	virtual bool DoesGameSystemReallocate() override;

public:
	// A KeyValues3 (16 bytes) that GameShutdown frees; no writer of it is left in this build
	KeyValues3 *m_pKeyValues;
	// The panes that wait to break; a pane marks itself queued when it is added
	CUtlVector< CEntityHandle > m_QueuedPanes;
	int m_nPanesBrokenThisFrame;
	// Set by the static instance's constructor
	bool m_bEnabled;
	// Set when a func_shatterglass spawns, cleared by GamePreShutdown
	bool m_bGlassSpawned;
};

class CKV3TransferLoadContext;
class CKV3TransferSaveContext;

//-----------------------------------------------------------------------------
// Virtual table of CShatterGlassShard, one shard of a broken pane. The class carries a VTable
// suffix and declares virtual methods only: the game class of the same name is a schema
// class, so a module declares its own class with the members it needs and derives it from
// this one. Four slots on both platforms, no virtual destructor.
//-----------------------------------------------------------------------------
abstract_class CShatterGlassShardVTable
{
public:
	virtual SchemaMetaInfoHandle_t< CSchemaClassInfo > Schema_DynamicBinding() = 0;

	virtual void KV3TransferSave( CKV3TransferSaveContext *pContext ) const = 0;
	virtual void KV3TransferLoad( CKV3TransferLoadContext *pContext ) = 0;
	// "CShatterGlassShard"
	virtual const char *GetKV3TransferClassName() const = 0;
};

//-----------------------------------------------------------------------------
// The shard manager seen by the shard physics. CShatterGlassShardMgr implements it as its
// second base (after CDefSaveRestoreBlockHandler, eight bytes in): on Linux its overrides
// also take slots of the main table, on Windows they stay in this table. Five slots on both
// platforms, no virtual destructor. No name source was found; each comment says what the
// manager's body does. The parameters are not recovered: the declarations describe the
// slots, not how to call them.
//-----------------------------------------------------------------------------
abstract_class IShatterGlassShardMgr
{
public:
	// Reached only through the thunk of the second base; works on the manager
	virtual void Unk_0() = 0;
	// Takes the manager lock and updates the state of a shard by index
	virtual void Unk_1() = 0;
	// Null for index 0, otherwise looks the shard up under the manager lock
	virtual void *Unk_2() = 0;
	// Under the lock: asks the manager's own method first, then reads shard data
	virtual void Unk_3() = 0;
	// A count kept by the manager
	virtual int Unk_4() = 0;
};

#endif // FUNC_SHATTERGLASS_H
