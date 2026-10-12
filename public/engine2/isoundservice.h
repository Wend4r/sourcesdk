//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: Engine service for voice chat
//
//=============================================================================//

#ifndef ISOUNDSERVICE_H
#define ISOUNDSERVICE_H
#ifdef _WIN32
#pragma once
#endif

#include <engine/IEngineService.h>

class CAudioState;
class IVoiceTarget;
struct VoiceRecordingLevel_t;

abstract_class ISoundService : public IEngineService
{
public:
	virtual bool Unk_PlayNetworkedVoice( void *p ) = 0;

	virtual bool GetNetworkedVoiceLevel( uint64 nXUID, VoiceRecordingLevel_t *pLevel ) = 0;

	virtual void AddVoiceListener( IVoiceTarget *pListener ) = 0;
	virtual void RemoveVoiceListener( IVoiceTarget *pListener ) = 0;

	virtual bool IsVoiceRecording() = 0;

	virtual void unk028( const CAudioState *pAudioState ) = 0;
	virtual void unk029() = 0;

	virtual void SetVoiceThreshold( float flThresholdDB ) = 0;
	virtual void SetVoiceThresholdDelay( float flTime ) = 0;
	virtual void SetVoiceThresholdAttack( float flTime ) = 0;
	virtual void SetVoiceThresholdHold( float flTime ) = 0;
	virtual float GetVoiceCurrentPeak() = 0;
	virtual bool IsVoiceActive() = 0;

	virtual void StopVoiceRecording() = 0;
};

#endif // ISOUNDSERVICE_H
