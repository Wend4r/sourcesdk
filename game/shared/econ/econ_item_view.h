//====== Copyright (c) 1996-2005, Valve Corporation, All rights reserved. =======
//
// Purpose:
//
//=============================================================================

#ifndef ECON_ITEM_VIEW_H
#define ECON_ITEM_VIEW_H
#ifdef _WIN32
#pragma once
#endif

#include "econ_item_interface.h"

#include "entity2/entityinstance.h"
#include "entity2/entitynetwork.h"
#include "tier0/utlvector.h"
#include "networkvar.h"

#define RGB_INT_RED 12073019
#define RGB_INT_BLUE 5801378
#define ECON_ITEM_GENERATED_ICON_WIDTH 512
#define ECON_ITEM_GENERATED_ICON_HEIGHT 384
#define ECON_ITEM_GENERATED_PINBOARD_ICON_WIDTH 512
#define ECON_ITEM_GENERATED_PINBOARD_ICON_HEIGHT 384
#define ECON_ITEM_GENERATED_ICON_DIR "resource/Flash/econ/weapons/cached/"

#define SCHEMA_BASE_ITEM_MAX 499 // def indices 1-499 are reserved for default aka stock items

class CAttributeManager;
class CEconItem;
class IMaterial;

struct stickerMaterialReference_t
{
	IMaterial *m_pMaterialReferenceFirstPerson;
	IMaterial *m_pMaterialReferenceThirdPerson;
	int m_nSlotIndex;
};

//-----------------------------------------------------------------------------
// Purpose: An attribute that knows how to read itself from a datafile, describe itself to the user,
//			and serialize itself between Servers, Clients, and Steam.
//			Unlike the attributes created in the Game DLL, this attribute doesn't know how to actually
//			do anything in the game, it just knows how to describe itself.
//
//			An element of CAttributeList::m_Attributes: its changes reach the owner entity through
//			the chainer, which is why the three notifications below forward to it.
//-----------------------------------------------------------------------------
/*class CEconItemAttribute
{
public:
	virtual SchemaMetaInfoHandle_t< CSchemaClassInfo > Schema_DynamicBinding() = 0;

	virtual void NetworkStateChanged( const NetworkStateChanged_t &data ) = 0;
	virtual void NetworkStateChangedRemove( const NetworkStateChangedRemove_t &data ) = 0;
	virtual void NetworkStateChangedLog( const char *pszFieldName, const char *pszInfo ) = 0;

	virtual const CNetworkSerializerClassInfo *GetSerializerClassInfo() = 0;

public:
	CNetworkVarChainer __m_pChainEntity;

	attrib_definition_index_t m_iAttributeDefinitionIndex;
	float m_flValue;
	float m_flInitialValue;
	int m_nRefundableCurrency;
	bool m_bSetBonus;
};
COMPILE_TIME_ASSERT( sizeof( CEconItemAttribute ) == 0x48 );

//-----------------------------------------------------------------------------
// Purpose: The attributes of one item. Embedded in CEconItemView, whose NetworkVar_ wrappers
//			override the three notifications; the bodies of this class itself are empty.
//-----------------------------------------------------------------------------
class CAttributeList
{
	DECLARE_CLASS_NOBASE( CAttributeList );

public:
	// Implemented by the game, as is GetSerializerClassInfo(). Not pure, so that the class can
	// be embedded by value.
	virtual SchemaMetaInfoHandle_t< CSchemaClassInfo > Schema_DynamicBinding();

	virtual void NetworkStateChanged( const NetworkStateChanged_t &data ) {}
	virtual void NetworkStateChangedRemove( const NetworkStateChangedRemove_t &data ) {}
	virtual void NetworkStateChangedLog( const char *pszFieldName, const char *pszInfo ) {}

	virtual const CNetworkSerializerClassInfo *GetSerializerClassInfo();

public:
	CNetworkUtlVarEmbedded( CEconItemAttribute, m_Attributes );
	CAttributeManager *m_pManager;
};
COMPILE_TIME_ASSERT( sizeof( CAttributeList ) == 0x78 );

//-----------------------------------------------------------------------------
// Purpose: An item that knows how to read itself from a datafile, describe itself to the user,
//			and serialize itself between Servers, Clients, and Steam.
//-----------------------------------------------------------------------------
class CEconItemView : public IEconItemInterface
{
	DECLARE_CLASS_NOBASE( CEconItemView );

public:
	virtual void NetworkStateChanged( const NetworkStateChanged_t &data ) = 0;
	virtual void NetworkStateChangedRemove( const NetworkStateChangedRemove_t &data ) = 0;
	virtual void NetworkStateChangedLog( const char *pszFieldName, const char *pszInfo ) = 0;

public:
	uint32 GetInventoryPosition() const { return m_iInventoryPosition; }

	// The kill eater cache. Only the constructor touches it in CS2: the first flag is cleared and
	// the frame is set to -2, exactly as CS:GO does. CS:GO keeps a sorted vector of types between
	// the two and a hash table of values after the frame; here each is down to the plain members
	// below, whose names follow that order.
	bool m_bKillEaterTypesCached;
	uint32 m_nCachedKillEaterType;
	int m_nKillEaterValuesCacheFrame;
	bool m_bKillEaterValueCached;
	int32 m_nCachedKillEaterValue;

	CUtlVector< stickerMaterialReference_t > m_pStickerMaterials;

	item_definition_index_t m_iItemDefinitionIndex;
	int m_iEntityQuality;
	uint32 m_iEntityLevel;

	itemid_t m_iItemID;
	uint32 m_iItemIDHigh;
	uint32 m_iItemIDLow;

	uint32 m_iAccountID;
	uint32 m_iInventoryPosition;

	// Owned: the destructor deletes it.
	CEconItem *m_pNonSOEconItem;

	bool m_bInitialized;

	// Each is a NetworkVar_ wrapper derived from CAttributeList with nothing but the three
	// notifications overridden.
	CAttributeList m_AttributeList;
	CAttributeList m_NetworkedDynamicAttributes;

	char m_szCustomName[ MAX_ITEM_CUSTOM_NAME_DATABASE_SIZE ];
	char m_szCustomNameOverride[ MAX_ITEM_CUSTOM_NAME_DATABASE_SIZE ];
	char m_szCustomNameOverride2[ MAX_ITEM_CUSTOM_NAME_DATABASE_SIZE ];
	char m_szCustomNameOverride3[ MAX_ITEM_CUSTOM_NAME_DATABASE_SIZE ];
};
COMPILE_TIME_ASSERT( sizeof( CEconItemView ) == 0x3E8 );*/

#endif // ECON_ITEM_VIEW_H
