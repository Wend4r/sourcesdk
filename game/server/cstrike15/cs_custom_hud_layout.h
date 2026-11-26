#ifndef CSTRIKE15_CS_CUSTOM_HUD_LAYOUT_H
#define CSTRIKE15_CS_CUSTOM_HUD_LAYOUT_H

#pragma once

#include "../baseentity.h"
#include "igameevents.h"

//-----------------------------------------------------------------------------
// CCSCustomHudLayout adds no method to CBaseEntity. The Linux table has 1 more slot, 239: the override of a method of IGameEventListener2, which the
// Itanium ABI also enters into the primary table and MSVC does not.
// The second bases sit at offsets 8, 16, ... here, not where the game has them: do not convert a pointer to them.
//-----------------------------------------------------------------------------
template < class TBase = CBaseEntityVTable >
abstract_class CCSCustomHudLayoutVTable : public TBase, public IGameEventListener2
{
public:
	// Overrides IGameEventListener2: a slot of this table on Linux only
	virtual void FireGameEvent( IGameEvent *pEvent ) override = 0;
};

#endif // CSTRIKE15_CS_CUSTOM_HUD_LAYOUT_H
