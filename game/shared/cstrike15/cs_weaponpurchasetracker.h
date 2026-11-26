//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The purchase tracking of a player.
//
//=============================================================================//

#ifndef CSTRIKE15_CS_WEAPONPURCHASETRACKER_H
#define CSTRIKE15_CS_WEAPONPURCHASETRACKER_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "schemasystem/schematypes.h"
#include "networksystem/networkvar_vtable.h"

class CKV3TransferLoadContext;
class CKV3TransferSaveContext;
class CNetworkSerializerClassInfo;
struct NetworkStateChanged_t;
struct NetworkStateChangedRemove_t;

//-----------------------------------------------------------------------------
// The virtual table of the schema struct SellbackPurchaseEntry_t. A purchase that can still be sold back,
// networked. The game class has members this declaration does not have.
//-----------------------------------------------------------------------------
abstract_class SellbackPurchaseEntry_tVTable
{
public:
	DECLARE_EMBEDDED_NETWORKVAR_NOTFLATTENED_VTABLE()
};

//-----------------------------------------------------------------------------
// The virtual table of the schema struct ServerAuthoritativeWeaponSlot_t. A weapon slot the server decides,
// networked. The game class has members this declaration does not have.
//-----------------------------------------------------------------------------
abstract_class ServerAuthoritativeWeaponSlot_tVTable
{
public:
	DECLARE_EMBEDDED_NETWORKVAR_NOTFLATTENED_VTABLE()
};

//-----------------------------------------------------------------------------
// The virtual table of the schema struct WeaponPurchaseCount_t. How often an item was bought, networked. The
// game class has members this declaration does not have.
//-----------------------------------------------------------------------------
abstract_class WeaponPurchaseCount_tVTable
{
public:
	DECLARE_EMBEDDED_NETWORKVAR_NOTFLATTENED_VTABLE()
};

//-----------------------------------------------------------------------------
// The virtual table of the schema struct WeaponPurchaseTracker_t. The purchase counts of a player, networked.
// The game class has members this declaration does not have.
//-----------------------------------------------------------------------------
abstract_class WeaponPurchaseTracker_tVTable
{
public:
	DECLARE_EMBEDDED_NETWORKVAR_NOTFLATTENED_VTABLE()
};

#endif // CSTRIKE15_CS_WEAPONPURCHASETRACKER_H
