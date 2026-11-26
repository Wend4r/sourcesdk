//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The networked environment structs of a player: fog, 3D skybox, sound.
//
//=============================================================================//

#ifndef PLAYERNET_VARS_H
#define PLAYERNET_VARS_H
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
// The virtual table of the schema struct audioparams_t. The sound environment of a player, networked. The game
// class has members this declaration does not have.
//-----------------------------------------------------------------------------
abstract_class audioparams_tVTable
{
public:
	DECLARE_EMBEDDED_NETWORKVAR_VTABLE()
	virtual void KV3TransferSave( CKV3TransferSaveContext *pContext ) const = 0;
	virtual void KV3TransferLoad( CKV3TransferLoadContext *pContext ) = 0;
	virtual const char *GetClassName() const = 0;
	virtual const CNetworkSerializerClassInfo *GetSerializerClassInfo() const = 0;
};

//-----------------------------------------------------------------------------
// The virtual table of the schema struct fogparams_t. Fog parameters, networked. The game class has members
// this declaration does not have.
//-----------------------------------------------------------------------------
abstract_class fogparams_tVTable
{
public:
	DECLARE_EMBEDDED_NETWORKVAR_VTABLE()
	virtual void KV3TransferSave( CKV3TransferSaveContext *pContext ) const = 0;
	virtual void KV3TransferLoad( CKV3TransferLoadContext *pContext ) = 0;
	virtual const char *GetClassName() const = 0;
	virtual const CNetworkSerializerClassInfo *GetSerializerClassInfo() const = 0;
};

//-----------------------------------------------------------------------------
// The virtual table of the schema struct fogplayerparams_t. The fog of a player, networked. The game class has
// members this declaration does not have.
//-----------------------------------------------------------------------------
abstract_class fogplayerparams_tVTable
{
public:
	DECLARE_EMBEDDED_NETWORKVAR_VTABLE()
	virtual void KV3TransferSave( CKV3TransferSaveContext *pContext ) const = 0;
	virtual void KV3TransferLoad( CKV3TransferLoadContext *pContext ) = 0;
	virtual const char *GetClassName() const = 0;
	virtual const CNetworkSerializerClassInfo *GetSerializerClassInfo() const = 0;
};

//-----------------------------------------------------------------------------
// The virtual table of the schema struct sky3dparams_t. The 3D skybox of a player, networked. The game class
// has members this declaration does not have.
//-----------------------------------------------------------------------------
abstract_class sky3dparams_tVTable
{
public:
	DECLARE_EMBEDDED_NETWORKVAR_VTABLE()
	virtual void KV3TransferSave( CKV3TransferSaveContext *pContext ) const = 0;
	virtual void KV3TransferLoad( CKV3TransferLoadContext *pContext ) = 0;
	virtual const char *GetClassName() const = 0;
	virtual const CNetworkSerializerClassInfo *GetSerializerClassInfo() const = 0;
};

#endif // PLAYERNET_VARS_H
