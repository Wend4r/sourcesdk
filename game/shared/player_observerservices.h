#ifndef PLAYER_OBSERVERSERVICES_H
#define PLAYER_OBSERVERSERVICES_H

#pragma once

#include "playerpawn_component.h"
#include "playerslot.h"
#include "mathlib/vector.h"

class CBaseEntity;
class CCommand;

//-----------------------------------------------------------------------------
// CPlayer_ObserverServices, slots 23-37. See playerpawn_component.h.
//
// The methods are the observer part of the Source 1 CBasePlayer. The modes are
// ObserverMode_t values, declared as integers so that the header does not tie a
// module to one definition of the enum.
//-----------------------------------------------------------------------------
template < class TBase = CPlayerPawnComponentVTable >
abstract_class CPlayer_ObserverServicesVTable : public TBase
{
public:
	virtual uint32 GetObserverMode() = 0;

	// Clears the target, the mode and the forced mode
	virtual void ResetObserverMode() = 0;
	virtual CBaseEntity *GetObserverTarget() = 0;

	// The target in the in-eye and chase modes, null in the others
	virtual CBaseEntity *GetFollowedObserverTarget() = 0;
	virtual void CalcObserverView( Vector &eyeOrigin, QAngle &eyeAngles, float &fov ) = 0;
	virtual bool SetObserverTarget( CBaseEntity *pTarget ) = 0;
	virtual bool StartObserverMode( uint32 nMode ) = 0;
	virtual void StopObserverMode() = 0;
	virtual bool SetObserverMode( uint32 nMode ) = 0;
	virtual CBaseEntity *FindNextObserverTarget( bool bReverse ) = 0;
	virtual CPlayerSlot GetNextObserverSearchStartPoint( bool bReverse ) = 0;
	virtual bool IsValidObserverTarget( CBaseEntity *pTarget ) = 0;
	virtual void CheckObserverSettings() = 0;
	virtual void ForceObserverMode( uint32 nMode ) = 0;

	// Handles spectate, spec_mode, spec_next, spec_prev, spec_player and spec_goto
	virtual bool ClientCommand( const CCommand &args ) = 0;
};

#endif // PLAYER_OBSERVERSERVICES_H
