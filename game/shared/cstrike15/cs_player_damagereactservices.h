#ifndef CSTRIKE15_CS_PLAYER_DAMAGEREACTSERVICES_H
#define CSTRIKE15_CS_PLAYER_DAMAGEREACTSERVICES_H

#pragma once

#include "../playerpawn_component.h"

//-----------------------------------------------------------------------------
// CCSPlayer_DamageReactServices derives straight from
// CPlayerPawnComponent and adds no virtual method. See playerpawn_component.h.
//-----------------------------------------------------------------------------
template < class TBase = CPlayerPawnComponentVTable >
abstract_class CCSPlayer_DamageReactServicesVTable : public TBase
{
};

class CCSPlayerPawn;
class CTakeDamageInfo;

//-----------------------------------------------------------------------------
// A listener of the damage the pawns take. The game nests the interface in
// CCSPlayer_DamageReactServices (CCSPlayer_DamageReactServices::ITakeDamageListener);
// modules declare their own class of that name, so it stands at file scope here.
// Two slots on both platforms, no virtual destructor. A listener adds itself to a
// global list on construction and removes itself on destruction; both methods are
// empty in the interface itself.
//-----------------------------------------------------------------------------
abstract_class ITakeDamageListener
{
public:
	// Called on every listener (newest first) when a pawn is about to take damage; the
	// listener may change the damage (the bullet listener of a shot adds the headshot
	// flag to the later pellets). Listeners that keep this empty default are skipped.
	virtual void OnTakeDamageListenerCallback( CCSPlayerPawn *pVictim, CTakeDamageInfo &infoTweakable ) = 0;
	// Named by what it does: called on every listener once the damage is applied, with
	// the health the pawn lost (the grenade and fire listeners count the enemies hurt)
	virtual void OnTakeDamageApplied( CCSPlayerPawn *pVictim, float flHealthLost ) = 0;
};

#endif // CSTRIKE15_CS_PLAYER_DAMAGEREACTSERVICES_H
