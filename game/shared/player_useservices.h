#ifndef PLAYER_USESERVICES_H
#define PLAYER_USESERVICES_H

#pragma once

#include "playerpawn_component.h"

class CBaseEntity;

//-----------------------------------------------------------------------------
// CPlayer_UseServices, slots 23-26. See playerpawn_component.h.
//-----------------------------------------------------------------------------
template < class TBase = CPlayerPawnComponentVTable >
abstract_class CPlayer_UseServicesVTable : public TBase
{
public:
	virtual bool IsUseableEntity( CBaseEntity *pEntity, uint32 nRequiredCaps ) = 0;

	// The object the pawn carries. The three below are the Source 1 CBasePlayer methods for
	// carried physics objects and do nothing in the server classes: no object, nothing to drop.
	virtual CBaseEntity *GetHeldObject() = 0;
	virtual void ForceDropOfCarriedPhysObjects( CBaseEntity *pOnlyIfHoldingThis ) = 0;
	virtual bool IsHoldingEntity( CBaseEntity *pEntity ) = 0;
};

#endif // PLAYER_USESERVICES_H
