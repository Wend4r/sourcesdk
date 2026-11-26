#ifndef PHYS_CONTROLLER_H
#define PHYS_CONTROLLER_H

#pragma once

#include "tier0/platform.h"
#include "schemasystem/schematypes.h"

class CKV3TransferLoadContext;
class CKV3TransferSaveContext;

//-----------------------------------------------------------------------------
// Virtual table of CMotorController, the motor of a physics motor entity (CPhysMotor::m_motor).
// The class carries a VTable suffix and declares virtual methods only: the game class of the
// same name is a schema class, so a module declares its own class with the members it needs and
// derives it from this one. Four slots on both platforms, no virtual destructor.
//-----------------------------------------------------------------------------
abstract_class CMotorControllerVTable
{
public:
	virtual SchemaMetaInfoHandle_t< CSchemaClassInfo > Schema_DynamicBinding() = 0;

	virtual void KV3TransferSave( CKV3TransferSaveContext *pContext ) const = 0;
	virtual void KV3TransferLoad( CKV3TransferLoadContext *pContext ) = 0;
	// "CMotorController"
	virtual const char *GetKV3TransferClassName() const = 0;
};

#endif // PHYS_CONTROLLER_H
