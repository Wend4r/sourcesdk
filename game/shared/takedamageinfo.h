#ifndef TAKEDAMAGEINFO_H
#define TAKEDAMAGEINFO_H

#pragma once

#include "tier0/platform.h"
#include "schemasystem/schematypes.h"

class CKV3TransferLoadContext;
class CKV3TransferSaveContext;
struct ScriptClassDesc_t;

//-----------------------------------------------------------------------------
// Virtual table of CTakeDamageInfo, the description of one hit, and of CRadiusDamageInfo
// derived from it (overrides Clone only). The class carries a VTable suffix and declares
// virtual methods only: the game class of the same name is a schema class, so a module
// declares its own class with the members it needs and derives it from this one. Six slots
// on both platforms, no virtual destructor.
//-----------------------------------------------------------------------------
abstract_class CTakeDamageInfoVTable
{
public:
	virtual SchemaMetaInfoHandle_t< CSchemaClassInfo > Schema_DynamicBinding() = 0;

	virtual void KV3TransferSave( CKV3TransferSaveContext *pContext ) const = 0;
	virtual void KV3TransferLoad( CKV3TransferLoadContext *pContext ) = 0;
	// "CTakeDamageInfo"
	virtual const char *GetKV3TransferClassName() const = 0;

	// The script description of the class, built on the first call
	virtual ScriptClassDesc_t *GetScriptDesc() = 0;

	// Named by what it does: allocates a new object of the class (0x118 bytes;
	// CRadiusDamageInfo its own size) and copies this one into it. The game returns the new
	// object through a hidden result, an owning holder of the one pointer whose type is not
	// identified: the pointer return here describes the slot, not the calling convention.
	virtual CTakeDamageInfoVTable *Clone() const = 0;
};

#endif // TAKEDAMAGEINFO_H
