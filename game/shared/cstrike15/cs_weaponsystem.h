//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Caches the econ data of the CS weapons by item definition index.
//
//=============================================================================//

#ifndef CS_WEAPONSYSTEM_H
#define CS_WEAPONSYSTEM_H
#ifdef _WIN32
#pragma once
#endif

#include "igamesystem.h"
#include "icsweaponsystem.h"

#include <tier0/utlmap.h>

//-----------------------------------------------------------------------------
// Registered as "CSWeaponSystem" (static factory). No source names its members; they are named by
// what the code does with them. It adds no virtual methods to IGameSystem; on Linux the
// GetWeaponEconData override also takes slot 66 of the primary table.
//-----------------------------------------------------------------------------
class CCSWeaponSystem : public CAutoGameSystem, public ICSWeaponSystem
{
public:
	DECLARE_GAME_SYSTEM();

	CCSWeaponSystem();
	virtual ~CCSWeaponSystem();

	// Both purge m_WeaponEconData and reset m_nItemSchemaVersion (Init after slot 5 of an econ
	// interface, Shutdown before slot 7 of it)
	virtual bool Init() override;
	virtual void Shutdown() override;
	// Precaches the weapons of the item schema into the session manifest
	GS_EVENT( GamePostInit );

	// Forward to the game system factory
	virtual void SetGameSystemGlobalPtrs( void *pValue ) override;
	virtual bool DoesGameSystemReallocate() override;

	virtual CWeaponEconData *GetWeaponEconData( uint16 nItemDefIndex ) override;

public:
	// The cache of GetWeaponEconData, by item definition index
	CUtlMap< uint16, CWeaponEconData *, uint16 > m_WeaponEconData;
	// The item schema version the cache was built for; when the schema's version differs the stale
	// entries are dropped. -1 after Init / Shutdown
	int m_nItemSchemaVersion;
};

#endif // CS_WEAPONSYSTEM_H
