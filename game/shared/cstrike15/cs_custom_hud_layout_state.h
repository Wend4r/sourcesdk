#ifndef CSTRIKE15_CS_CUSTOM_HUD_LAYOUT_STATE_H
#define CSTRIKE15_CS_CUSTOM_HUD_LAYOUT_STATE_H

#pragma once

#include "tier0/platform.h"
#include "schemasystem/schematypes.h"
#include "networksystem/networkvar_vtable.h"

class CNetworkSerializerClassInfo;
struct NetworkStateChanged_t;
struct NetworkStateChangedRemove_t;

//-----------------------------------------------------------------------------
// Virtual table of CCSCustomHudLayoutState, the layout state the custom hud entity embeds
// (CCSCustomHudLayout::m_globalLayoutState). The class carries a VTable suffix and declares
// virtual methods only: the game class of the same name is a schema class, so a module
// declares its own class with the members it needs and derives it from this one. Five slots
// on both platforms, no virtual destructor. The network variable wrapper of the embedded
// member forwards NetworkStateChanged and NetworkStateChangedLog to the entity.
//-----------------------------------------------------------------------------
abstract_class CCSCustomHudLayoutStateVTable
{
public:
	DECLARE_EMBEDDED_NETWORKVAR_NOTFLATTENED_VTABLE()
};

#endif // CSTRIKE15_CS_CUSTOM_HUD_LAYOUT_STATE_H
