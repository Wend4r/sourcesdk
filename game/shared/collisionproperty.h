#ifndef COLLISIONPROPERTY_H
#define COLLISIONPROPERTY_H

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
// Virtual table of CCollisionProperty, the collision description an entity embeds
// (CBaseModelEntity::m_Collision). The class carries a VTable suffix and declares virtual
// methods only: the game class of the same name is a schema class, so a module declares its
// own class with the members it needs and derives it from this one. Eight slots on both
// platforms, no virtual destructor.
//
// The network variable wrapper of the embedded member overrides NetworkStateChanged and
// NetworkStateChangedLog to forward them to the owning entity.
//-----------------------------------------------------------------------------
abstract_class CCollisionPropertyVTable
{
public:
	// The "CCollisionProperty" serializer registered with the network serializer class dictionary
	DECLARE_EMBEDDED_NETWORKVAR_NOTFLATTENED_VTABLE()

	virtual void KV3TransferSave( CKV3TransferSaveContext *pContext ) const = 0;
	virtual void KV3TransferLoad( CKV3TransferLoadContext *pContext ) = 0;
	// "CCollisionProperty"
	virtual const char *GetKV3TransferClassName() const = 0;
};

#endif // COLLISIONPROPERTY_H
