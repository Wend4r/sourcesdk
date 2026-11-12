//====== Copyright (c) Valve Corporation, All rights reserved. =======
//
// Purpose: Container that allows client & server access to data in player inventories & loadouts
//
//=============================================================================
#ifndef CSTRIKE15_ITEM_INVENTORY_H
#define CSTRIKE15_ITEM_INVENTORY_H
#ifdef _WIN32
#pragma once
#endif

#include "econ/econ_item_inventory.h"
#include "cstrike15_item_constants.h"

#define LOADOUT_SLOT_USE_BASE_ITEM 0

//-----------------------------------------------------------------------------
// Purpose: One entry of a player's loadout. The equip-slot handler writes the id
//			and the definition index separately.
//			An id at or above k_ulDefaultEquippedDefinitionID names no real item -
//			its low bits are a definition index instead, which is how a slot holds a
//			default skin. The definition is 0xFFFF until the slot is first written.
//-----------------------------------------------------------------------------
const itemid_t k_ulDefaultEquippedDefinitionID = 0xF000000000000000ull;

struct LoadoutSlot_t
{
	itemid_t m_ItemID;
	item_definition_index_t m_ItemDefIndex;
};
COMPILE_TIME_ASSERT( sizeof( LoadoutSlot_t ) == 0x10 );

//-----------------------------------------------------------------------------
// Purpose: A single player's inventory.
//		On the client, the inventory manager contains an instance of this for the local player.
//		On the server, each player contains an instance of this.
//-----------------------------------------------------------------------------
class CCSPlayerInventory : public CPlayerInventory
{
public:
	// Not a game function: writes the slot the way the game's equip-slot handler does.
	void SetItemInLoadout( int iTeam, int iItemLoadoutSlot, itemid_t iItemID, item_definition_index_t iItemDefIndex )
	{
		if ( iTeam < 0 || iTeam >= LOADOUT_COUNT )
		{
			return;
		}

		if ( iItemLoadoutSlot < 0 || iItemLoadoutSlot >= LOADOUT_SLOT_COUNT )
		{
			return;
		}

		// No id means the slot holds a default definition rather than an owned item.
		if ( iItemID == 0 && iItemDefIndex != INVALID_ITEM_DEF_INDEX )
		{
			iItemID = k_ulDefaultEquippedDefinitionID | iItemDefIndex;
		}

		LoadoutSlot_t &slot = m_LoadoutItems[ iTeam ][ iItemLoadoutSlot ];
		slot.m_ItemID = iItemID;
		slot.m_ItemDefIndex = iItemDefIndex;
	}

public:
	int m_nActiveQuestID;
	LoadoutSlot_t m_LoadoutItems[ LOADOUT_COUNT ][ LOADOUT_SLOT_COUNT ];
};
COMPILE_TIME_ASSERT( sizeof( CCSPlayerInventory ) == 0xF10 );

#endif // CSTRIKE15_ITEM_INVENTORY_H
