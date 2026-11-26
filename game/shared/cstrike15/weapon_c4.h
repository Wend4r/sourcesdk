#ifndef CSTRIKE15_WEAPON_C4_H
#define CSTRIKE15_WEAPON_C4_H

#pragma once

#include "weapon_csbase.h"
#include "../baseanimgraph.h"
#include "../econ/ihasattributes.h"

class CAttributeContainer;
class CAttributeList;
class CAttributeManager;
class CCSPlayerPawn;
class CGameTrace;

//-----------------------------------------------------------------------------
// CC4 adds 2 methods to CCSWeaponBase: Linux slots 448-449, Windows 437-438.
//-----------------------------------------------------------------------------
template < class TBase = CCSWeaponBaseVTable<> >
abstract_class CC4VTable : public TBase
{
public:
	// Empty in every class
	virtual void Stub_437() = 0;
	// Empty in every class
	virtual void Stub_438() = 0;
};

//-----------------------------------------------------------------------------
// CPlantedC4 adds 4 methods to CBaseAnimGraph, Windows slots 306-309.
// The Linux table has 9, slots 307-315: 5 override methods of IHasAttributes, which
// the Itanium ABI also enters into the primary table and MSVC does not.
// The second bases sit at offsets 8, 16, ... here, not where the game has them: do not convert a pointer to them.
//-----------------------------------------------------------------------------
template < class TBase = CBaseAnimGraphVTable<> >
abstract_class CPlantedC4VTable : public TBase, public IHasAttributes
{
public:
	// Overrides the method of CEntityInstance and the one of the second base: no slot of its own
	virtual SchemaMetaInfoHandle_t< CSchemaClassInfo > Schema_DynamicBinding() override = 0;
	// Overrides the method of CEntityInstance and the one of IHasAttributes: no slot of its own
	virtual const CNetworkSerializerClassInfo *GetSerializerClassInfo() override = 0;
	// IHasAttributes entry 2; returns this + m_AttributeManager; Windows has it only in the IHasAttributes table. Overrides IHasAttributes: a slot of this table on Linux only
	virtual CAttributeManager *GetAttributeManager() override = 0;
	// IHasAttributes entry 3; Windows has it only in the IHasAttributes table. Overrides IHasAttributes: a slot of this table on Linux only
	virtual CAttributeContainer *GetAttributeContainer() override = 0;
	// IHasAttributes entry 4; returns this; Windows has it only in the IHasAttributes table. Overrides IHasAttributes: a slot of this table on Linux only
	virtual CBaseEntity *GetAttributeOwner() override = 0;
	// IHasAttributes entry 5; Windows has it only in the IHasAttributes table. Overrides IHasAttributes: a slot of this table on Linux only
	virtual CAttributeList *GetAttributeList() override = 0;
	// IHasAttributes entry 6; empty; Windows has it only in the IHasAttributes table. Overrides IHasAttributes: a slot of this table on Linux only
	virtual void ReapplyProvision() override = 0;
	virtual void ActivateSetTimerLength( float flTimerLength ) = 0;
	// The bomb explodes (BombExplode)
	virtual void OnExplode( CGameTrace *pGroundTrace ) = 0;
	virtual void OnDefuse( CCSPlayerPawn *pDefuser ) = 0;
	// Radio.GetOutOfThere
	virtual void RadioAboutToExplode( int nTeamNumber ) = 0;
};

#endif // CSTRIKE15_WEAPON_C4_H
