#ifndef ENV_WIND_SHARED_H
#define ENV_WIND_SHARED_H

#pragma once

#include "tier0/platform.h"
#include "schemasystem/schematypes.h"

class CGlobalSymbol;
class CKV3TransferLoadContext;
class CKV3TransferSaveContext;
class CNetworkSerializerClassInfo;
struct NetworkStateChanged_t;
struct NetworkStateChangedRemove_t;

//-----------------------------------------------------------------------------
// Virtual table of CEnvWindShared, the wind state CEnvWind embeds (m_EnvWindShared). The
// class carries a VTable suffix and declares virtual methods only: the game class of the same
// name is a schema class, so a module declares its own class with the members it needs and
// derives it from this one. Nine slots on both platforms, no virtual destructor.
//-----------------------------------------------------------------------------
abstract_class CEnvWindSharedVTable
{
public:
	virtual SchemaMetaInfoHandle_t< CSchemaClassInfo > Schema_DynamicBinding() = 0;
	// Returns this when typeName is the symbol of "CEnvWindShared", else null
	virtual void *CastToType( const CGlobalSymbol &typeName ) = 0;

	// The network variable wrapper of CEnvWind forwards the first and the last to the entity
	virtual void NetworkStateChanged( const NetworkStateChanged_t &data ) = 0;
	virtual void NetworkStateChangedRemove( const NetworkStateChangedRemove_t &data ) = 0;
	virtual void NetworkStateChangedLog( const char *pszFieldName, const char *pszInfo ) = 0;

	virtual void KV3TransferSave( CKV3TransferSaveContext *pContext ) const = 0;
	virtual void KV3TransferLoad( CKV3TransferLoadContext *pContext ) = 0;
	// "CEnvWindShared"
	virtual const char *GetKV3TransferClassName() const = 0;

	virtual const CNetworkSerializerClassInfo *GetSerializerClassInfo() const = 0;
};

#endif // ENV_WIND_SHARED_H
