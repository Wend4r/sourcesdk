#ifndef CSTRIKE15_CS_PLAYER_WEAPONSERVICES_H
#define CSTRIKE15_CS_PLAYER_WEAPONSERVICES_H

#pragma once

#include "../player_weaponservices.h"

//-----------------------------------------------------------------------------
// CCSPlayer_WeaponServices adds no virtual method to its primary table. See
// playerpawn_component.h.
//
// The game class also derives from IToolsResourcePreReloadListener and one more
// interface, whose tables sit behind the members and are not described here.
//-----------------------------------------------------------------------------
template < class TBase = CPlayer_WeaponServicesVTable<> >
abstract_class CCSPlayer_WeaponServicesVTable : public TBase
{
};

#endif // CSTRIKE15_CS_PLAYER_WEAPONSERVICES_H
