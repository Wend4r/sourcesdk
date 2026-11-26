#ifndef CSTRIKE15_CS_PLAYERANIMSTATE_H
#define CSTRIKE15_CS_PLAYERANIMSTATE_H

#pragma once

#include "tier0/platform.h"
#include "schemasystem/schematypes.h"

//-----------------------------------------------------------------------------
// Virtual table of CCSPlayerAnimationState. The class carries a VTable suffix and declares
// virtual methods only: the game class of the same name is a schema class, so a module
// declares its own class with the members it needs and derives it from this one. One slot on
// both platforms, no virtual destructor.
//-----------------------------------------------------------------------------
abstract_class CCSPlayerAnimationStateVTable
{
public:
	virtual SchemaMetaInfoHandle_t< CSchemaClassInfo > Schema_DynamicBinding() = 0;
};

#endif // CSTRIKE15_CS_PLAYERANIMSTATE_H
