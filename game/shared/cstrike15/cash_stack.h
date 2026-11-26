#ifndef CSTRIKE15_CASH_STACK_H
#define CSTRIKE15_CASH_STACK_H

#pragma once

#include "../basemodelentity.h"

//-----------------------------------------------------------------------------
// CCashStack adds 1 method to CBaseModelEntity: Linux slot 272, Windows 271.
//-----------------------------------------------------------------------------
template < class TBase = CBaseModelEntityVTable<> >
abstract_class CCashStackVTable : public TBase
{
public:
	// When pOther->IsPlayerPawn() gives the cash to that pawn (touch handler). Named by what it does
	virtual void CashTouch( CBaseEntity *pOther ) = 0;
};

#endif // CSTRIKE15_CASH_STACK_H
