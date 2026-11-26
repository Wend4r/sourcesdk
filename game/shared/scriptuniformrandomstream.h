#ifndef SCRIPTUNIFORMRANDOMSTREAM_H
#define SCRIPTUNIFORMRANDOMSTREAM_H

#pragma once

#include "tier0/platform.h"
#include "schemasystem/schematypes.h"

struct ScriptClassDesc_t;

//-----------------------------------------------------------------------------
// Virtual table of CScriptUniformRandomStream, the uniform random number generator the scripts
// get. The class carries a VTable suffix and declares virtual methods only: the game class of
// the same name is a schema class, so a module declares its own class with the members it needs
// and derives it from this one. Windows slots 0-2; on Linux the destructor takes two slots, so
// the last slot is one further.
//-----------------------------------------------------------------------------
abstract_class CScriptUniformRandomStreamVTable
{
public:
	virtual SchemaMetaInfoHandle_t< CSchemaClassInfo > Schema_DynamicBinding() = 0;
	virtual ~CScriptUniformRandomStreamVTable() = 0;

	// The script description of the class ("A uniform random number generator."), built on the first call
	virtual ScriptClassDesc_t *GetScriptDesc() = 0;
};

#endif // SCRIPTUNIFORMRANDOMSTREAM_H
