#ifndef PLAYER_OBSERVERSERVICES_H
#define PLAYER_OBSERVERSERVICES_H

#pragma once

#include "playerpawn_component.h"
#include "playerslot.h"
#include "mathlib/vector.h"

class CBaseEntity;
class CCommand;

//-----------------------------------------------------------------------------
// CPlayer_ObserverServices, slots 23-37, on the client 30-54. See
// playerpawn_component.h.
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

#ifdef CLIENT_DLL
	// Client slot 45. Turns the input of the local observer into commands: +attack into
	// spec_next, +attack2 into spec_prev, +jump into spec_mode; CCSObserver_ObserverServices
	// also handles +duck, the map overview, lastinv, invprev, invnext and slot keys.
	virtual void Unk_45() = 0;

	// Client slot 46. Empty in the base class; CCSObserver_ObserverServices compares the
	// observer mode with the one it last saw and starts or stops the observer interpolation.
	virtual void Unk_46() = 0;

	// Client slot 47. Fires spec_target_updated (userid, target) for the observer target;
	// CCSObserver_ObserverServices updates the HUD panels of the observed player.
	virtual void Unk_47() = 0;

	// OBSERVER_INTERP_NONE (0) in the base class; CCSObserver_ObserverServices returns
	// m_obsInterpState while it interpolates observer changes
	virtual int GetObserverInterpState() = 0;

	// False in the base class; CCSObserver_ObserverServices: m_obsInterpState is not
	// OBSERVER_INTERP_NONE while it interpolates observer changes
	virtual bool IsInObserverInterpolation() = 0;

	// Client slot 50. Returns a position for the target entity: the default view offset when
	// there is no target or it is not a player pawn. Not overridden.
	virtual void Unk_50() = 0;

	// Client slot 51. Returns a trace filter (by value) for traces of the observer camera
	// against the entity passed; CCSObserver_ObserverServices adds a flag to it.
	virtual void Unk_51() = 0;

	// Client slots 52 and 53. Compute an observer view (origin, angles, FOV) from the target;
	// CCSObserver_ObserverServices overrides both.
	virtual void Unk_52( Vector &eyeOrigin, QAngle &eyeAngles, float &fov ) = 0;
	virtual void Unk_53( Vector &eyeOrigin, QAngle &eyeAngles, float &fov ) = 0;

	// Client slot 54. Takes two arguments; CCSObserver_ObserverServices also clears
	// m_obsInterpState.
	virtual void Unk_54() = 0;
#endif
};

#endif // PLAYER_OBSERVERSERVICES_H
