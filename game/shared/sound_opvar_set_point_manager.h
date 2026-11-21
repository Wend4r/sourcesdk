//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Keeps the sound opvar set points (CSoundOpvarSetPointBase entities) enabled per player
// and updates their opvars.
//
//=============================================================================//

#ifndef SOUND_OPVAR_SET_POINT_MANAGER_H
#define SOUND_OPVAR_SET_POINT_MANAGER_H
#ifdef _WIN32
#pragma once
#endif

#include "igamesystem.h"

#include <tier0/utlhashtable.h>
#include <tier0/utlvector.h>
#include <tier0/utlsymbollarge.h>
#include <mathlib/vector.h>
#include <mathlib/vectorws.h>
#include <kv3lib/kv3transfer.h>
#include <schemasystem/schematypes.h>

#include "schemacompiler/schemaon.h"

class CSoundOpvarSetPointBase;
// Named by what it holds: the opvar values latched for one player (a pointer owned by
// m_PlayerLatchedOpvars, destroyed with it)
class CSoundOpvarPlayerLatchState;

//-----------------------------------------------------------------------------
// An opvar value latched for a player; saved with the player (schema struct, 48 bytes; field names
// from the schema). It has a vtable and no virtual destructor.
//-----------------------------------------------------------------------------
schema struct sndopvarlatchdata_t
{
public:
	TYPEMETA( MGetKV3ClassDefaults );

	virtual SchemaMetaInfoHandle_t< CSchemaClassInfo > Schema_DynamicBinding();
	// Empty
	virtual void NetworkStateChanged( const NetworkStateChanged_t &data ) {}
	virtual void NetworkStateChangedRemove( const NetworkStateChangedRemove_t &data ) {}
	virtual void NetworkStateChangedLog( const char *pszFieldName, const char *pszInfo ) const {}
	CLASS_USES_KV3TRANSFER_VIRTUAL( sndopvarlatchdata_t );
	virtual const char *GetClassName() const { return "sndopvarlatchdata_t"; }

public:
	CUtlSymbolLarge m_iszStack;
	CUtlSymbolLarge m_iszOperator;
	CUtlSymbolLarge m_iszOpvar;
	float m_flVal;
	VectorWS m_vPos;
};

//-----------------------------------------------------------------------------
// Registered as "Opvar Set Point Manager" (reallocating factory). 152 bytes on both platforms. No
// source has the class; the names are named by what the code does.
//-----------------------------------------------------------------------------
class CSoundOpvarSetPointManager : public CAutoGameSystem
{
public:
	DECLARE_GAME_SYSTEM();

	virtual ~CSoundOpvarSetPointManager();

	// Zeroes m_flLastUpdateTime
	GS_EVENT( GameInit );
	// Empties m_PlayerLatchedOpvars and m_SetPointPlayers
	GS_EVENT( GameShutdown );
	// Updates the set points for every player at once
	GS_EVENT( GameDeactivate );
	// Updates the set points at most every 16 ms ("FrameUpdatePreEntityThink")
	GS_EVENT( ServerPreEntityThink );
	// Gathers each player's latched opvars into m_SaveScratch and hands them to the player
	GS_EVENT( SaveGame );
	// Restores the latched opvars from the players
	GS_EVENT( RestoreGame );

	// Forward to the game system factory
	virtual void SetGameSystemGlobalPtrs( void *pValue ) override;
	virtual bool DoesGameSystemReallocate() override;

public:
	// An opvar value queued for a player: the fields of sndopvarlatchdata_t without its vtable
	struct PendingOpvarValue_t
	{
		CUtlSymbolLarge m_iszStack;
		CUtlSymbolLarge m_iszOperator;
		CUtlSymbolLarge m_iszOpvar;
		float m_flVal;
		Vector m_vPos;
	};

	// The players (slots) each set point is enabled for (the "Enable" input adds them)
	CUtlHashtable< CSoundOpvarSetPointBase *, CUtlVector< int > > m_SetPointPlayers;
	// By player slot: the opvar values latched for the player
	CUtlHashtable< uint32, CSoundOpvarPlayerLatchState * > m_PlayerLatchedOpvars;
	// By player slot: the values the disable path ("SetDisabledValue") queued for the player
	CUtlHashtable< int, CUtlVector< PendingOpvarValue_t > > m_PendingPlayerOpvars;
	// Filled by SaveGame for one player at a time
	CUtlVector< sndopvarlatchdata_t > m_SaveScratch;
	// gpGlobals->curtime of the last update by ServerPreEntityThink
	float m_flLastUpdateTime;
};

//-----------------------------------------------------------------------------
// Registered as "Opvar Set Point Manager (MP)" (reallocating factory). 24 bytes on both platforms:
// no fields besides the DECLARE_GAME_SYSTEM member, and it overrides no event. Not derived from
// CSoundOpvarSetPointManager (RTTI).
//-----------------------------------------------------------------------------
class CSoundOpvarSetPointManager_MP : public CAutoGameSystem
{
public:
	DECLARE_GAME_SYSTEM();

	virtual ~CSoundOpvarSetPointManager_MP();

	// Forward to the game system factory
	virtual void SetGameSystemGlobalPtrs( void *pValue ) override;
	virtual bool DoesGameSystemReallocate() override;
};

#include "schemacompiler/schemaoff.h"

#endif // SOUND_OPVAR_SET_POINT_MANAGER_H
