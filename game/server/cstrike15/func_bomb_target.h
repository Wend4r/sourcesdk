#ifndef CSTRIKE15_FUNC_BOMB_TARGET_H
#define CSTRIKE15_FUNC_BOMB_TARGET_H

#pragma once

#include "../triggers.h"
#include "entity2/entityclass.h"

//-----------------------------------------------------------------------------
// CBombTargetShim adds 2 methods to CBaseTrigger: Linux slots 280-281, Windows 279-280.
// A mix-in without data: the members of CBombTarget start at the end of CBaseTrigger.
//-----------------------------------------------------------------------------
template < class TBase = CBaseTriggerVTable<> >
abstract_class CBombTargetShim : public TBase
{
public:
	// CBombTarget checks the toucher for the C4 and changes a networked field
	virtual void BombTargetTouch( CBaseEntity *pOther ) = 0;
	// CBombTarget prints "BombTargetUse does nothing"
	virtual void BombTargetUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value ) = 0;
};

//-----------------------------------------------------------------------------
// CBombTarget adds 1 method to CBombTargetShim: Linux slot 282, Windows 281.
//-----------------------------------------------------------------------------
template < class TBase = CBombTargetShim<> >
abstract_class CBombTargetVTable : public TBase
{
public:
	// Sets the bomb target up for the round: creates the info_bomb_target_hint_A or _B entity (with "origin" and "angles" keys) nearest to the site, after a shape trace
	virtual void ReInitOnRoundStart() = 0;
};

#endif // CSTRIKE15_FUNC_BOMB_TARGET_H
