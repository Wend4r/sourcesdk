#ifndef EFFECT_DISPATCH_DATA_H
#define EFFECT_DISPATCH_DATA_H

#pragma once

#include "tier0/platform.h"
#include "schemasystem/schematypes.h"
#include "networksystem/networkvar_vtable.h"

struct NetworkStateChanged_t;
struct NetworkStateChangedRemove_t;

//-----------------------------------------------------------------------------
// Virtual table of CEffectData, the parameters of a dispatched effect. The class carries a
// VTable suffix and declares virtual methods only: the game class of the same name is a
// schema class, so a module declares its own class with the members it needs and derives it
// from this one. Four slots on both platforms, no virtual destructor.
//-----------------------------------------------------------------------------
abstract_class CEffectDataVTable
{
public:
	DECLARE_EMBEDDED_NETWORKVAR_VTABLE()
};

#endif // EFFECT_DISPATCH_DATA_H
