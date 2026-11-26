#ifndef ECON_WEARABLE_H
#define ECON_WEARABLE_H

#pragma once

#include "econ_entity.h"

//-----------------------------------------------------------------------------
// CEconWearable adds 11 methods to CEconEntity: Linux slots 323-333, Windows 315-325.
//-----------------------------------------------------------------------------
template < class TBase = CEconEntityVTable<> >
abstract_class CEconWearableVTable : public TBase
{
public:
	virtual bool IsWearable() = 0;
	virtual void RemoveFrom( CBaseEntity *pEntity ) = 0;
	// Equip asks it first
	virtual bool CanEquip( CBaseEntity *pEntity ) = 0;
	virtual void Equip( CBaseCombatCharacter *pOwner ) = 0;
	// UpdateBodygroups( pOwner, false ), stops following, clears the owner
	virtual void UnEquip( CBaseCombatCharacter *pOwner ) = 0;
	virtual void OnWearerDeath() = 0;
	virtual int GetDropType() = 0;
	// Returns false in every class
	virtual bool Stub_322() = 0;
	// Empty in every class
	virtual void Stub_323() = 0;
	// m_nForceSkin when set (>= 0), otherwise the skin of the item definition. Named by what it does
	virtual int GetSkin() = 0;
	// m_nForceSkin. Named by the field it writes
	virtual void SetForceSkin( int nSkin ) = 0;
};

#endif // ECON_WEARABLE_H
