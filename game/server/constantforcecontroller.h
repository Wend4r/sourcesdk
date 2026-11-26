//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: A constant force on physics bodies.
//
//=============================================================================//

#ifndef CONSTANTFORCECONTROLLER_H
#define CONSTANTFORCECONTROLLER_H
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
// The virtual table of CConstantForceController: IMotionEvent and the schema class virtuals. A constant force
// or torque on physics bodies (phys_thruster, phys_torque); a schema class. The game class has members this
// declaration does not have.
//-----------------------------------------------------------------------------
abstract_class CConstantForceControllerVTable : public IMotionEvent
{
public:
	virtual SchemaMetaInfoHandle_t< CSchemaClassInfo > Schema_DynamicBinding() = 0;
	virtual void KV3TransferSave( CKV3TransferSaveContext *pContext ) const = 0;
	virtual void KV3TransferLoad( CKV3TransferLoadContext *pContext ) = 0;
	virtual const char *GetClassName() const = 0;
};

#endif // CONSTANTFORCECONTROLLER_H
