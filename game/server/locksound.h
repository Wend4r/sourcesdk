//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The lock sounds of doors and buttons.
//
//=============================================================================//

#ifndef LOCKSOUND_H
#define LOCKSOUND_H
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
// The virtual table of the schema struct locksound_t. The lock and unlock sounds of a door or button. The game
// class has members this declaration does not have.
//-----------------------------------------------------------------------------
abstract_class locksound_tVTable
{
public:
	virtual SchemaMetaInfoHandle_t< CSchemaClassInfo > Schema_DynamicBinding() = 0;
	virtual void KV3TransferSave( CKV3TransferSaveContext *pContext ) const = 0;
	virtual void KV3TransferLoad( CKV3TransferLoadContext *pContext ) = 0;
	virtual const char *GetClassName() const = 0;
};

#endif // LOCKSOUND_H
