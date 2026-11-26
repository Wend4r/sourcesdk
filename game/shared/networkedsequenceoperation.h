#ifndef NETWORKEDSEQUENCEOPERATION_H
#define NETWORKEDSEQUENCEOPERATION_H

#pragma once

#include "tier0/platform.h"
#include "schemasystem/schematypes.h"
#include "networksystem/networkvar_vtable.h"

class CKV3TransferLoadContext;
class CKV3TransferSaveContext;
class CNetworkSerializerClassInfo;
struct NetworkStateChanged_t;
struct NetworkStateChangedRemove_t;
struct datamap_t;

//-----------------------------------------------------------------------------
// Virtual table of CNetworkedSequenceOperation, a networked sequence playback operation. The
// class carries a VTable suffix and declares virtual methods only: the game class of the same
// name is a schema class, so a module declares its own class with the members it needs and
// derives it from this one. Nine slots on both platforms, no virtual destructor.
//-----------------------------------------------------------------------------
abstract_class CNetworkedSequenceOperationVTable
{
public:
	DECLARE_EMBEDDED_NETWORKVAR_VTABLE()

	// Named by what it does: the static data description of the class ("CNetworkedSequenceOperation")
	virtual datamap_t *GetDataDescMap() = 0;

	virtual void KV3TransferSave( CKV3TransferSaveContext *pContext ) const = 0;
	virtual void KV3TransferLoad( CKV3TransferLoadContext *pContext ) = 0;
	// "CNetworkedSequenceOperation"
	virtual const char *GetKV3TransferClassName() const = 0;

	virtual const CNetworkSerializerClassInfo *GetSerializerClassInfo() const = 0;
};

#endif // NETWORKEDSEQUENCEOPERATION_H
