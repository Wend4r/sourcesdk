//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Shattering glass.
//
//=============================================================================//

#ifndef SHATTERGLASS_H
#define SHATTERGLASS_H
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
// The virtual table of the schema struct shard_model_desc_t. The description of a glass shard model,
// networked. The game class has members this declaration does not have.
//-----------------------------------------------------------------------------
abstract_class shard_model_desc_tVTable
{
public:
	DECLARE_EMBEDDED_NETWORKVAR_NOTFLATTENED_VTABLE()
};

#endif // SHATTERGLASS_H
