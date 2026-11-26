//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Shaking physics bodies.
//
//=============================================================================//

#ifndef PHYSICS_SHAKE_H
#define PHYSICS_SHAKE_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "schemasystem/schematypes.h"
#include "imotionevent.h"

class CKV3TransferLoadContext;
class CKV3TransferSaveContext;
class CNetworkSerializerClassInfo;
struct NetworkStateChanged_t;
struct NetworkStateChangedRemove_t;

//-----------------------------------------------------------------------------
// The virtual table of CPhysicsShake: IMotionEvent and the schema class virtuals. Shakes physics bodies
// (env_shake); a schema class. The game class has members this declaration does not have.
//-----------------------------------------------------------------------------
abstract_class CPhysicsShakeVTable : public IMotionEvent
{
public:
	virtual SchemaMetaInfoHandle_t< CSchemaClassInfo > Schema_DynamicBinding() = 0;
	virtual void KV3TransferSave( CKV3TransferSaveContext *pContext ) const = 0;
	virtual void KV3TransferLoad( CKV3TransferLoadContext *pContext ) = 0;
	virtual const char *GetClassName() const = 0;
};

#endif // PHYSICS_SHAKE_H
