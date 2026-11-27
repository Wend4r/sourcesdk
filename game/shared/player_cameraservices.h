#ifndef PLAYER_CAMERASERVICES_H
#define PLAYER_CAMERASERVICES_H

#pragma once

#include "playerpawn_component.h"
#include "mathlib/vector.h"

class CBaseEntity;
struct audioparams_t;
struct fogparams_t;

//-----------------------------------------------------------------------------
// CPlayer_CameraServices, slots 23-32, on the client 30-45. See
// playerpawn_component.h.
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

#ifdef CLIENT_DLL
	// Returns 0 in every class of the client, client slot 39; no caller found
	virtual int Stub_39() = 0;

	// The near clipping plane of the view, asked by the view while r_nearz is below 1. 7 in
	// the base class; CCSPlayerBase_CameraServices picks one of three console variables by
	// the value passed. Named by its caller
	virtual float GetZNear( float flUnknown ) = 0;

	// The far clipping plane of the view when it is positive, asked by the view while r_farz
	// is below 1 (r_mapextents and the fog otherwise). 0 in every class. Named by its caller
	virtual float GetZFar( float flUnknown ) = 0;

	// 75, or 5 with one player; CCSPlayerBase_CameraServices returns 10
	virtual float GetMinFOV() = 0;

	// Fills three values of the view setup right after CalcView; zero in every class
	virtual void Unk_43( float *pUnknown1, float *pUnknown2, int *pUnknown3 ) = 0;

	// Fills two more values of the view setup after Unk_43; zero and false in every class
	virtual void Unk_44( int *pUnknown, bool *pbUnknown ) = 0;

	// m_CurrentFog
	virtual fogparams_t *GetFogParams() = 0;
#else
	// Pitch to punch the view by for the damage taken
	virtual float GetDamageViewPunchPitch( const CTakeDamageResult &result ) = 0;
#endif
};

#endif // PLAYER_CAMERASERVICES_H
