//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Render attributes of entities.
//
//=============================================================================//

#ifndef ENTITYRENDERATTRIBUTE_H
#define ENTITYRENDERATTRIBUTE_H
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
// The virtual table of the schema struct EntityRenderAttribute_t. A render attribute of an entity, networked.
// The game class has members this declaration does not have.
//-----------------------------------------------------------------------------
abstract_class EntityRenderAttribute_tVTable
{
public:
	DECLARE_EMBEDDED_NETWORKVAR_VTABLE()
	virtual void KV3TransferSave( CKV3TransferSaveContext *pContext ) const = 0;
	virtual void KV3TransferLoad( CKV3TransferLoadContext *pContext ) = 0;
	virtual const char *GetClassName() const = 0;
	virtual const CNetworkSerializerClassInfo *GetSerializerClassInfo() const = 0;
};

#endif // ENTITYRENDERATTRIBUTE_H
