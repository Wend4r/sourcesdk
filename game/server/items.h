#ifndef ITEMS_H
#define ITEMS_H

#pragma once

#include "../shared/baseanimgraph.h"

class CBasePlayerPawn;
class CCSPlayerPawn;

//-----------------------------------------------------------------------------
// CItem adds 8 methods to CBaseAnimGraph: Linux slots 307-314, Windows 306-313.
//-----------------------------------------------------------------------------
template < class TBase = CBaseAnimGraphVTable<> >
abstract_class CItemVTable : public TBase
{
public:
	virtual bool HasBloatedCollision() = 0;
	virtual CBaseEntity *Respawn() = 0;
	// Plays Item.Materialize
	virtual void Materialize() = 0;
	virtual bool MyTouch( CCSPlayerPawn *pPlayer ) = 0;
	virtual void HandleTouch( CCSPlayerPawn *pPlayer, CBaseEntity *pOther ) = 0;
	virtual void FallThink() = 0;
	// Fires m_OnPlayerPickup with the player as activator. Named by what it does
	virtual void OnPlayerPickup( CBasePlayerPawn *pPlayer ) = 0;
	virtual void SetupTouchFunction() = 0;
};

//-----------------------------------------------------------------------------
// CItemDogtags adds 1 method to CItem: Linux slot 315, Windows 314.
//-----------------------------------------------------------------------------
template < class TBase = CItemVTable<> >
abstract_class CItemDogtagsVTable : public TBase
{
public:
	// False unless the entity is a live player pawn; then whether mp_dogtag_pickup_rule lets that player pick the dogtags up
	virtual bool ItemCanBeTouchedByPlayer( CBasePlayerPawn *pPlayer ) = 0;
};

//-----------------------------------------------------------------------------
// CItemGeneric adds 2 methods to CItem: Linux slots 315-316, Windows 314-315.
//-----------------------------------------------------------------------------
template < class TBase = CItemVTable<> >
abstract_class CItemGenericVTable : public TBase
{
public:
	// Empty in every class
	virtual void Stub_314() = 0;
	// Plays the pickup effects for the player: m_hPickupParticleEffect and the sounds
	virtual void Unk_315( CBaseEntity *pPlayer ) = 0;
};

#endif // ITEMS_H
