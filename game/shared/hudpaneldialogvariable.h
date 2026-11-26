//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Dialog variables of HUD panels.
//
//=============================================================================//

#ifndef HUDPANELDIALOGVARIABLE_H
#define HUDPANELDIALOGVARIABLE_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "schemasystem/schematypes.h"

class CKV3TransferLoadContext;
class CKV3TransferSaveContext;
class CNetworkSerializerClassInfo;
struct NetworkStateChanged_t;
struct NetworkStateChangedRemove_t;

//-----------------------------------------------------------------------------
// The virtual table of the schema struct HUDPanelDialogVariableString_t. A dialog variable of a HUD panel,
// networked. The game class has members this declaration does not have.
//-----------------------------------------------------------------------------
abstract_class HUDPanelDialogVariableString_tVTable
{
public:
	virtual SchemaMetaInfoHandle_t< CSchemaClassInfo > Schema_DynamicBinding() = 0;
	virtual const CNetworkSerializerClassInfo *GetSerializerClassInfo() const = 0;
};

#endif // HUDPANELDIALOGVARIABLE_H
