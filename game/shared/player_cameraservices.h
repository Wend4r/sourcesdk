#ifndef PLAYER_CAMERASERVICES_H
#define PLAYER_CAMERASERVICES_H

#pragma once

#include "playerpawn_component.h"
#include "mathlib/vector.h"

class CBaseEntity;
struct audioparams_t;

//-----------------------------------------------------------------------------
// CPlayer_CameraServices, slots 23-32. See playerpawn_component.h.
//-----------------------------------------------------------------------------
template < class TBase = CPlayerPawnComponentVTable >
abstract_class CPlayer_CameraServicesVTable : public TBase
{
public:
	// m_hViewEntity
	virtual CBaseEntity *GetViewEntity() = 0;

	// m_audio
	virtual audioparams_t &GetAudioParams() = 0;

	// The last valid entity of m_PostProcessingVolumes
	virtual CBaseEntity *GetActivePostProcessingVolume() = 0;

	// Uses the observer services' CalcObserverView while the pawn observes, CalcPlayerView otherwise
	virtual void CalcView( Vector &eyeOrigin, QAngle &eyeAngles, float &zNear, float &zFar, float &fov ) = 0;
	virtual void CalcPlayerView( Vector &eyeOrigin, QAngle &eyeAngles, float &fov ) = 0;
	virtual float GetFOV() = 0;

	// The FOV the player's controller asks for, 90 without one
	virtual float GetDefaultFOV() = 0;

	// Run from Reset and Event_Killed. CCSPlayerBase_CameraServices drops the zoom here.
	virtual void ResetFOV() = 0;
	virtual void ViewPunch( const QAngle &angleOffset ) = 0;

	// Pitch to punch the view by for the damage taken
	virtual float GetDamageViewPunchPitch( const CTakeDamageResult &result ) = 0;
};

#endif // PLAYER_CAMERASERVICES_H
