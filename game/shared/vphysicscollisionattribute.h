//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The collision attribute of entities.
//
//=============================================================================//

#ifndef VPHYSICSCOLLISIONATTRIBUTE_H
#define VPHYSICSCOLLISIONATTRIBUTE_H
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
// The virtual table of the schema struct VPhysicsCollisionAttribute_t. The collision attribute of an entity,
// networked. The serializer getter comes first here, unlike in the other networked structs. The game class has
// members this declaration does not have.
//-----------------------------------------------------------------------------
abstract_class VPhysicsCollisionAttribute_tVTable
{
public:
	virtual const CNetworkSerializerClassInfo *GetSerializerClassInfo() const = 0;
	DECLARE_EMBEDDED_NETWORKVAR_VTABLE()
	virtual void KV3TransferSave( CKV3TransferSaveContext *pContext ) const = 0;
	virtual void KV3TransferLoad( CKV3TransferLoadContext *pContext ) = 0;
	virtual const char *GetClassName() const = 0;
};

#endif // VPHYSICSCOLLISIONATTRIBUTE_H
