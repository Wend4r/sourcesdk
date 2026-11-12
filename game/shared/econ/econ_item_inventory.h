//====== Copyright (c) 1996-2005, Valve Corporation, All rights reserved. =======
//
// Purpose: Container that allows client & server access to data in player inventories & loadouts
//
//=============================================================================

#ifndef ITEM_INVENTORY_H
#define ITEM_INVENTORY_H
#ifdef _WIN32
#pragma once
#endif

#include "econ_item_constants.h"
#include "gcsdk/gcclient_sharedobjectcache.h"
#include "igamesystem.h"

#include "tier0/utlmap.h"
#include "tier0/utlvector.h"

// Only pointers are needed here. econ_item_view.h is left out on purpose: projects that generate
// CEconItemView, CAttributeList and CEconItemAttribute from the schema have their own copies.
class CEconItem;
class CEconItemView;
class CEconEquipSlot;
class CItemSelectionCriteria;
class IInventoryUpdateListener;
struct baseitemcriteria_t;

//-----------------------------------------------------------------------------
// Purpose: A single player's inventory.
//		On the client, the inventory manager contains an instance of this for the local player.
//		On the server, each player contains an instance of this.
//-----------------------------------------------------------------------------
class CPlayerInventory : public GCSDK::ISharedObjectListener
{
	enum EInventoryItemEvent
	{
		k_EInventoryItemCreated,
		k_EInventoryItemUpdated,
		k_EInventoryItemDestroyed,
	};

public:
	virtual ~CPlayerInventory() = 0;
	virtual bool CanPurchaseItems( int iItemCount ) = 0;
	virtual int GetMaxItemCount() = 0;
	virtual CEconItemView *GetItemInLoadout( int iClass, int iSlot ) = 0;
	virtual bool ItemShouldBeIncluded( int iItemPosition ) = 0;
	virtual void DumpInventoryToConsole( bool bRoot ) = 0;
	virtual int ExtractInventorySortPosition( uint32 iBackendPosition ) = 0;
	virtual int GetPreviewItemDef() = 0;
	virtual void SOClear() = 0;

	// CCSPlayerInventory's override calls OnHasNewItems() with both arguments zero.
	virtual void NotifyHasNewItems() = 0;

	// Marks the descriptions of the item's collection dirty, calls ItemIsBeingRemoved() and
	// drops the item from the vector and the map.
	virtual void RemoveItem( itemid_t iItemID ) = 0;

	// Called by SOCreated(), SOUpdated() and SOCacheSubscribed() for each shared object of type
	// k_EEconTypeEquipSlot, which took over from CS:GO's default equipped definition instance.
	// Pure in this class.
	virtual void DefaultEquippedDefinitionHasBeenUpdated( CEconEquipSlot *pEquipSlot ) = 0;

	// Called at the end of SOCreated(), SOUpdated() and SODestroyed().
	virtual void SendInventoryUpdateEvent() = 0;

	// Takes two arguments in CS2; the only caller in the server passes zero for both.
	virtual void OnHasNewItems( void *pArg1, void *pArg2 ) = 0;
	virtual void OnItemChangedPosition( CEconItemView *pItem, uint32 iOldPos ) = 0;
	virtual void ValidateInventoryPositions() = 0;

	// SOUpdated() calls it as ( pItem, false, false, k_EInventoryItemUpdated ).
	virtual void ItemHasBeenUpdated( CEconItemView *pItem, bool bUpdateAckFile, bool bWriteAckFile, EInventoryItemEvent eEventType ) = 0;
	virtual void ItemIsBeingRemoved( CEconItemView *pItem ) = 0;

	// Marks the description of every item of the set dirty; CS:GO calls it
	// MarkSetItemDescriptionsDirty(). In CS2 it is declared last but under the name of the
	// virtual above that takes an item id: the Itanium ABI keeps it in the last slot, while MSVC
	// emits it right in front of that one, which it only does for an overload. The name the two
	// share is not in the binaries, so RemoveItem() stands in for it.
	virtual void RemoveItem( int nItemSetIndex ) = 0;

	void MarkSetItemDescriptionsDirty( int nItemSetIndex ) { RemoveItem( nItemSetIndex ); }

public:
	int m_nTargetRecipe;
	GCSDK::SOID_t m_OwnerID;

	// One item vector, not CS:GO's two: SOCacheSubscribed clears this one and refills it from the
	// econ item type cache.
	CUtlVector< CEconItemView * > m_vecInventoryItems;
	CUtlMap< itemid_t, CEconItemView *, int, CDefLess< itemid_t > > m_mapItemIDToItemView;

	int m_iPendingRequests;
	bool m_bGotItemsFromSteamAtLeastOnce;
	bool m_bCurrentlySubscribedToSteam;

	GCSDK::CGCClientSharedObjectCache *m_pSOCache;
	CUtlVector< GCSDK::ISharedObjectListener * > m_vecListeners;
};

//-----------------------------------------------------------------------------
// Purpose: The game system that keeps the list of player inventories.
//		This is the server's class. The client's one has more virtuals after these and more
//		members between the reference items and m_pInventories.
//		Of the game system events the class handles Init(), PostInit(), Shutdown(),
//		GameActivate() and GameDeactivate(); they add no slots and are not redeclared here.
//-----------------------------------------------------------------------------
class CInventoryManager : public CAutoGameSystem
{
public:
	// Returns the registered inventory whose owner is that individual account.
	virtual CPlayerInventory *GetInventoryForAccount( uint32 iAccountID ) = 0;

	// We're generating a base item. We need to add the game-specific keys to the criteria so that it'll find the right base item.
	// Empty in this class, and CS2 does not override it.
	virtual void AddBaseItemCriteria( baseitemcriteria_t *pCriteria, CItemSelectionCriteria *pSelectionCriteria ) = 0;

	// Zero for a position that is unset or unacknowledged, its low byte otherwise.
	virtual int GetBackpackPositionFromBackend( uint32 iBackendPosition ) = 0;

public:
	struct inventories_t
	{
		CPlayerInventory *pInventory;
		IInventoryUpdateListener *pListener;
	};

	CUtlVector< CEconItemView * > m_pTempReferenceItems;
	CUtlMap< itemid_t, CEconItemView *, int, CDefLess< itemid_t > > m_mapTempReferenceItems;

	// Inventory registry. Shutdown() calls SOClear() on each of these.
	CUtlVector< inventories_t > m_pInventories;
};

#endif // ITEM_INVENTORY_H
