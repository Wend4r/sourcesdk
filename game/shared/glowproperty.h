#ifndef GLOWPROPERTY_H
#define GLOWPROPERTY_H

#pragma once

#include "tier0/platform.h"
#include "schemasystem/schematypes.h"

class CKV3TransferLoadContext;
class CKV3TransferSaveContext;
class CNetworkSerializerClassInfo;
struct NetworkStateChanged_t;
struct NetworkStateChangedRemove_t;

//-----------------------------------------------------------------------------
// Virtual table of CGlowProperty, the glow settings an entity embeds
// (CBaseModelEntity::m_Glow). The class carries a VTable suffix and declares virtual methods
// only: the game class of the same name is a schema class, so a module declares its own class
// with the members it needs and derives it from this one. Eight slots on both platforms, no
// virtual destructor; the save / load pair comes before the network notifications here.
//-----------------------------------------------------------------------------
abstract_class CGlowPropertyVTable
{
public:
	virtual SchemaMetaInfoHandle_t< CSchemaClassInfo > Schema_DynamicBinding() = 0;

	virtual void KV3TransferSave( CKV3TransferSaveContext *pContext ) const = 0;
	virtual void KV3TransferLoad( CKV3TransferLoadContext *pContext ) = 0;
	// "CGlowProperty"
	virtual const char *GetKV3TransferClassName() const = 0;

	// The network variable wrapper of the embedded member forwards the first and the last to
	// the owning entity
	virtual void NetworkStateChanged( const NetworkStateChanged_t &data ) = 0;
	virtual void NetworkStateChangedRemove( const NetworkStateChangedRemove_t &data ) = 0;
	virtual void NetworkStateChangedLog( const char *pszFieldName, const char *pszInfo ) = 0;

	virtual const CNetworkSerializerClassInfo *GetSerializerClassInfo() const = 0;
};

#endif // GLOWPROPERTY_H
