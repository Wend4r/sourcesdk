//========= Copyright (c), Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
// $NoKeywords: $
//=============================================================================//

#ifndef CSTRIKE15_ITEM_SCHEMA_H
#define CSTRIKE15_ITEM_SCHEMA_H
#ifdef _WIN32
#pragma once
#endif

#include "econ/econ_item_schema.h"
#include "cstrike15_item_constants.h"

#include "tier0/bitvec.h"
#include "tier0/utlmap.h"
#include "tier0/utlsymbollarge.h"

#include <string.h>

// The names the schema itself gives the loadout positions. IEconItemSchema::GetLoadoutSlotCount,
// ::GetLoadoutSlotName and ::GetLoadoutSlot are `return 0` stubs in the CS2 server, so asking
// the schema what a slot is called answers nothing and this table is what a definition's slot
// reads through. It is the table the schema's constructor registers, in its own order.
inline const char *LoadoutPositionName( int iPosition )
{
	static const char *const s_pszLoadoutPositionNames[ LOADOUT_POSITION_COUNT ] =
	{
		"melee",
		"c4",
		"secondary0",
		"secondary1",
		"secondary2",
		"secondary3",
		"secondary4",
		"secondary5",
		"smg0",
		"smg1",
		"smg2",
		"smg3",
		"smg4",
		"smg5",
		"rifle0",
		"rifle1",
		"rifle2",
		"rifle3",
		"rifle4",
		"rifle5",
		"heavy0",
		"heavy1",
		"heavy2",
		"heavy3",
		"heavy4",
		"heavy5",
		"grenade0",
		"grenade1",
		"grenade2",
		"grenade3",
		"grenade4",
		"grenade5",
		"equipment0",
		"equipment1",
		"equipment2",
		"equipment3",
		"equipment4",
		"equipment5",
		"customplayer",
		"clothing1",
		"clothing2",
		"clothing_hands",
		"clothing4",
		"clothing5",
		"clothing6",
		"clothing7",
		"clothing8",
		"misc0",
		"misc1",
		"misc2",
		"misc3",
		"misc4",
		"misc5",
		"misc6",
		"musickit",
		"flair0",
		"spray0",
		"pet",
	};

	if ( iPosition < 0 || iPosition >= LOADOUT_POSITION_COUNT )
	{
		return NULL;
	}

	return s_pszLoadoutPositionNames[ iPosition ];
}

inline int LoadoutPositionByName( const char *pszName )
{
	if ( !pszName )
	{
		return LOADOUT_POSITION_INVALID;
	}

	for ( int iPosition = 0; iPosition < LOADOUT_POSITION_COUNT; ++iPosition )
	{
		if ( !strcmp( LoadoutPositionName( iPosition ), pszName ) )
		{
			return iPosition;
		}
	}

	return LOADOUT_POSITION_INVALID;
}

class CCStrike15ItemDefinition : public CEconItemDefinition
{
public:
	bool CanBeUsedByTeam( int iTeam ) const { return m_vbClassUsability.IsBitSet( iTeam ); }
	int GetDefaultLoadoutSlot() const { return m_iDefaultLoadoutSlot; }

	// The one team the definition is limited to, or 0 when both teams or neither can use it.
	int GetUsedByTeam() const
	{
		if ( CanBeUsedByTeam( 2 ) && CanBeUsedByTeam( 3 ) )
		{
			return 0;
		}

		if ( CanBeUsedByTeam( 2 ) )
		{
			return 2;
		}

		if ( CanBeUsedByTeam( 3 ) )
		{
			return 3;
		}

		return 0;
	}

public:
	CUtlSymbolLarge m_pszLoadoutCategory;

	int m_iDefaultLoadoutSlot;
	int m_iGearSlot;
	int m_iGearSlotPosition;
	int m_iAnimationSlot;

	// The .mdl file used for this item when it's being carried by a player.
	const char *m_pszPlayerDisplayModel[ LOADOUT_COUNT ];

	// Specifies which class can use this item.
	CBitVec< LOADOUT_COUNT > m_vbClassUsability;

	int m_iLoadoutSlots[ LOADOUT_COUNT ]; // Slot that each class places the item into.
	bool m_bIsSupplyCrate : 1;
	bool m_bItemSharesEquipSlot : 1;

	CUtlMap< const char *, void *, int > m_mapPaintData;

	const char *m_pszVoPrefix;
	const char *m_pszDefCheer;
	const char *m_pszDefDefeat;
	const char *m_pszAnimClass;
};
COMPILE_TIME_ASSERT( sizeof( CCStrike15ItemDefinition ) == 0x3C8 );

#endif // CSTRIKE15_ITEM_SCHEMA_H
