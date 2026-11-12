//====== Copyright (c), Valve Corporation, All rights reserved. =======
//
// Purpose: CEconItem, a shared object for econ items
//
//=============================================================================

#ifndef ECONITEM_H
#define ECONITEM_H
#ifdef _WIN32
#pragma once
#endif

#include "econ_item_constants.h"
#include "econ_item_interface.h"
#include "econ_item_schema.h"
#include "gcsdk/sharedobject.h"

#include <string>

namespace GCSDK
{
class CColumnSet;
};

class CEconItem;
class CSOEconItem;
class CEconItemCustomData;
class CEconSessionItemAudit;

//-----------------------------------------------------------------------------
// Stats tracking for the attributes attached to CEconItem instances.
//-----------------------------------------------------------------------------
struct schema_attribute_stat_bucket_t
{
	const schema_attribute_stat_bucket_t *m_pNext;

	const char *m_pszDesc;
	uint64 m_unLiveInlineCount;
	uint64 m_unLifetimeInlineCount;
	uint64 m_unLiveHeapCount;
	uint64 m_unLifetimeHeapCount;

	void OnAllocateInlineInstance()
	{
		m_unLiveInlineCount++;
		m_unLifetimeInlineCount++;
	}

	void OnFreeInlineInstance()
	{
		m_unLiveInlineCount--;
	}

	void OnAllocateHeapInstance()
	{
		m_unLiveHeapCount++;
		m_unLifetimeHeapCount++;
	}

	void OnFreeHeapInstance()
	{
		Assert( m_unLiveHeapCount );
		m_unLiveHeapCount--;
	}
};

//-----------------------------------------------------------------------------
// Base class interface for attributes of a certain in-memory type.
//-----------------------------------------------------------------------------
template < typename TAttribInMemoryType >
class ISchemaAttributeTypeBase : public ISchemaAttributeType
{
public:
	virtual void ConvertTypedValueToByteStream( const TAttribInMemoryType &typedValue, ::std::string *out_psBytes ) = 0;
	virtual void ConvertByteStreamToTypedValue( const ::std::string &sBytes, TAttribInMemoryType *out_pTypedValue ) = 0;
};

class CEconItem : public GCSDK::CSharedObject, public IEconItemInterface
{
public:
	typedef GCSDK::CSharedObject BaseClass;

	struct attribute_t
	{
		attrib_definition_index_t m_unDefinitionIndex;
		attribute_data_union_t m_value;
	};
	COMPILE_TIME_ASSERT( sizeof( attribute_t ) == 0x10 );

	struct EquippedInstance_t
	{
		EquippedInstance_t()
		 : m_unEquippedClass( 0 ), m_unEquippedSlot( INVALID_EQUIPPED_SLOT )
		{
		}

		EquippedInstance_t( equipped_class_t unClass, equipped_slot_t unSlot )
		 : m_unEquippedClass( unClass ), m_unEquippedSlot( unSlot )
		{
		}

		equipped_class_t m_unEquippedClass;
		equipped_slot_t m_unEquippedSlot;
	};

	struct CustomDataOptimizedObject_t
	{
		// This is preceding the attributes block: the attributes follow immediately, sixteen
		// bytes each. An item has no block at all until it is equipped, gets an attribute or
		// arrives with a style; a new block starts with the slot set to
		// INVALID_EQUIPPED_SLOT_BITPACKED and the second word zeroed.

		uint16 m_equipInstanceSlot1 : 6;     // Equip Instance Slot, if equipped
		uint16 m_equipInstanceClass1 : 3;    // Equip Instance Class
		uint16 m_equipInstanceClass2Bit : 1; // Whether the item is equipped for complementary class

		// Length of following attributes. Six bits next to the three above in CS:GO; eight in CS2,
		// which puts it into a second word.
		uint16 m_numAttributes : 8;

		// CS2 only: CSOEconItem's "style", kept when it is not zero.
		uint16 m_unStyle : 8;

		inline attribute_t *GetAttribute( uint32 j ) { return ( reinterpret_cast< attribute_t * >( this + 1 ) ) + j; }
		inline const attribute_t *GetAttribute( uint32 j ) const { return ( reinterpret_cast< const attribute_t * >( this + 1 ) ) + j; }
	};
	COMPILE_TIME_ASSERT( sizeof( CustomDataOptimizedObject_t ) == 0x4 );

	// Set only the top 16 bits for field ID types! These will be or'd into the index of
	// the field itself and then pulled apart later.
	enum
	{
		kUpdateFieldIDType_FieldID = 0x00000000,     // this must stay as 0 for legacy code
		kUpdateFieldIDType_AttributeID = 0x00010000, // this will modify existing attribute in the database
	};

public:
	// The virtuals below are declared in the game's order. It matters with the Itanium ABI,
	// where every override of an IEconItemInterface method takes a slot of the primary vtable
	// as well; MSVC keeps those in the interface's vtable alone.
	const GameItemDefinition_t *GetItemDefinition() const override = 0;

	// Stamps m_nSOUpdateTime with Plat_RelativeTicks().
	virtual void SetSOUpdateTime() = 0;
	virtual int64 GetSOUpdateTime() const = 0;

	void IterateAttributes( IEconItemAttributeIterator *pIterator ) const override = 0;

	uint64 GetItemID() const override = 0;
	uint32 GetAccountID() const override = 0;
	uint32 GetItemLevel() const override = 0;
	int32 GetQuality() const override = 0;
	int32 GetRarity() const override = 0;
	uint16 GetQuantity() const override = 0;
	bool HasAnyFlags( uint8 unFlags ) const override = 0;
	eEconItemOrigin GetOrigin() const override = 0;

	const char *GetCustomName() const override = 0;
	const char *GetCustomDesc() const override = 0;

	int GetItemSetIndex() const override = 0;
	bool GetInUse() const override = 0;

	bool IsTradable() const override = 0;
	bool IsMarketable() const override = 0;
	bool IsUsableInCrafting() const override = 0;

	using GCSDK::CSharedObject::BParseFromMessage;

	// Both read a serialized CSOEconItem and deserialize the item from it.
	virtual bool BParseFromMessage( const CUtlBuffer &buffer ) = 0;
	virtual bool BParseFromMessage( const std::string &buffer ) = 0;

public:
	// data that is most commonly changed
	uint64 m_ulID;         // Item ID
	uint64 m_ulOriginalID; // Original ID is often different from item ID itself

	// optional data (custom name, additional attributes, etc.)
	CustomDataOptimizedObject_t *m_pCustomDataOptimizedObject;

	uint32 m_unAccountID; // Item Owner
	uint32 m_unInventory; // App managed int representing inventory placement

	item_definition_index_t m_unDefIndex; // Item definition index

	uint16 m_unOrigin : 5;      // Origin (eEconItemOrigin) (0-31)
	uint16 m_nQuality : 4;      // Item quality (0-14)
	uint16 m_unLevel : 2;       // Item Level (0-1-2-3)
	uint16 m_nRarity : 4;       // Item rarity (0-14)
	uint16 m_dirtybitInUse : 1; // In Use

	// The item set index GetItemSetIndex() caches: 255 until it is looked up, 254 when the
	// definition belongs to no set.
	mutable int16 m_iItemSet;

	mutable int64 m_nSOUpdateTime;

	uint8 m_unFlags; // Flags
};
COMPILE_TIME_ASSERT( sizeof( CEconItem ) == 0x48 );

#endif // ECONITEM_H
