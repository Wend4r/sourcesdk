//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The sounds of the physics constraints (hinges, sliders) and their profile.
//
//=============================================================================//

#ifndef PHYSCONSTRAINT_SOUNDS_H
#define PHYSCONSTRAINT_SOUNDS_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "schemasystem/schematypes.h"

class CKV3TransferLoadContext;
class CKV3TransferSaveContext;

//-----------------------------------------------------------------------------
// Both classes are schema classes embedded in the constraints; these classes declare their
// virtual methods only (see playerpawn_component.h for the VTable pattern): the schema-class
// prologue of a class with KV3 transfer. No virtual destructor.
//-----------------------------------------------------------------------------
abstract_class SimpleConstraintSoundProfileVTable
{
public:
	virtual SchemaMetaInfoHandle_t< CSchemaClassInfo > Schema_DynamicBinding() = 0;
	virtual void KV3TransferSave( CKV3TransferSaveContext *pContext ) const = 0;
	virtual void KV3TransferLoad( CKV3TransferLoadContext *pContext ) = 0;
	virtual const char *GetClassName() const = 0;
};

abstract_class ConstraintSoundInfoVTable
{
public:
	virtual SchemaMetaInfoHandle_t< CSchemaClassInfo > Schema_DynamicBinding() = 0;
	virtual void KV3TransferSave( CKV3TransferSaveContext *pContext ) const = 0;
	virtual void KV3TransferLoad( CKV3TransferLoadContext *pContext ) = 0;
	virtual const char *GetClassName() const = 0;
};

#endif // PHYSCONSTRAINT_SOUNDS_H
