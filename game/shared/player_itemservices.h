#ifndef PLAYER_ITEMSERVICES_H
#define PLAYER_ITEMSERVICES_H

#pragma once

#include "playerpawn_component.h"
#include "mathlib/vector.h"

class CBaseEntity;
class KeyValues;

//-----------------------------------------------------------------------------
// CPlayer_ItemServices, slots 23-27. See playerpawn_component.h.
//-----------------------------------------------------------------------------
template < class TBase = CPlayerPawnComponentVTable >
abstract_class CPlayer_ItemServicesVTable : public TBase
{
public:
	// Spawns the entity and gives it to the pawn. CCSPlayer_ItemServices ignores everything but the name.
	virtual CBaseEntity *GiveNamedItem( const char *pszName, int iSubType = 0, KeyValues *pKeyValues = nullptr ) = 0;

	// Returns whether the item was given. The base class gives nothing.
	virtual bool TryGiveNamedItem( const char *pszName, bool bForce = false ) = 0;

	// Like GiveNamedItem, but spawns the entity at the origin instead of at the pawn
	virtual CBaseEntity *GiveNamedItemAtOrigin( const char *pszName, int iSubType, Vector vecOrigin ) = 0;

	// CCSPlayer_ItemServices drops the active weapon through the weapon services
	virtual void DropActivePlayerWeapon( const Vector &vecForce ) = 0;
	virtual void RemoveAllItems( bool bRemoveSuit ) = 0;
};

#endif // PLAYER_ITEMSERVICES_H
