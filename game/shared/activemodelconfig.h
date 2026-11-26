#ifndef ACTIVEMODELCONFIG_H
#define ACTIVEMODELCONFIG_H

#pragma once

#include "tier0/platform.h"
#include "schemasystem/schematypes.h"
#include "networksystem/networkvar_vtable.h"

class CKV3TransferLoadContext;
class CKV3TransferSaveContext;
class CNetworkSerializerClassInfo;
struct NetworkStateChanged_t;
struct NetworkStateChangedRemove_t;

//-----------------------------------------------------------------------------
// Virtual table of ActiveModelConfig_t, the active model configuration of an entity: its
// handle, name and associated entities. The class carries a VTable suffix and declares virtual
// methods only: the game class of the same name is a schema class, so a module declares its own
// class with the members it needs and derives it from this one. Eight slots on both platforms,
// no virtual destructor.
//-----------------------------------------------------------------------------
abstract_class ActiveModelConfig_tVTable
{
public:
	DECLARE_EMBEDDED_NETWORKVAR_VTABLE()

	virtual void KV3TransferSave( CKV3TransferSaveContext *pContext ) const = 0;
	virtual void KV3TransferLoad( CKV3TransferLoadContext *pContext ) = 0;
	// "ActiveModelConfig_t"
	virtual const char *GetKV3TransferClassName() const = 0;

	virtual const CNetworkSerializerClassInfo *GetSerializerClassInfo() const = 0;
};

#endif // ACTIVEMODELCONFIG_H
