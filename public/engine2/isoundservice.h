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
#include "entity2/entityindex.h"

class CAudioState;
class IAudioInputStream;
class IVoiceTarget;

//-----------------------------------------------------------------------------
// One block of encoded voice data (the recording side fills it for every
// group of up to four 10 ms Opus frames)
//-----------------------------------------------------------------------------
struct VoiceRecordingDescription_t
{
	// 0: Steam voice data (decompressed through ISteamUser), 1: up to four Opus frames
	int m_nFormat;
	int m_nSampleRate;
	int m_nSequence;
	int m_nOffset;
	const char *m_pszSoundNameOverride;
	int m_nFrameCount;
	int m_nFrameEnds[ 4 ];
	// Playback level in dB (-90 and below is silent)
	float m_flLevelDB;
};

//-----------------------------------------------------------------------------
// Level of a voice stream (GetNetworkedVoiceLevel scales a 16-bit level to 0..1)
//-----------------------------------------------------------------------------
struct VoiceRecordingLevel_t
{
	float m_flLevel;
	int m_nLevel;
	int m_nMaxLevel;
	bool m_bUnk0C;
	bool m_bUnk0D;
	float m_flUnk10;
};

abstract_class ISoundService : public IEngineService
{
public:
	// bIgnoreSequence plays the block at once instead of buffering it until the blocks before it in its sequence arrive
	virtual bool PlayNetworkedVoice( uint64 nXUID, CEntityIndex nEntityIndex, const VoiceRecordingDescription_t &desc, const void *pData, int nSize, bool bIgnoreSequence, VoiceRecordingLevel_t *pLevel ) = 0;

	virtual bool GetNetworkedVoiceLevel( uint64 nXUID, VoiceRecordingLevel_t *pLevel ) = 0;

	virtual void RegisterVoiceTarget( IVoiceTarget *pTarget ) = 0;
	virtual void UnregisterVoiceTarget( IVoiceTarget *pTarget ) = 0;

	virtual bool IsRecordingVoice() = 0;

	// Uses a copy of pAudioState instead of the state built every frame until ClearAudioStateOverride
	virtual void OverrideAudioState( const CAudioState *pAudioState ) = 0;
	virtual void ClearAudioStateOverride() = 0;

	virtual void SetVoiceThreshold( float flThresholdDB ) = 0;
	virtual void SetVoiceThresholdDelay( float flTime ) = 0;
	virtual void SetVoiceThresholdAttack( float flTime ) = 0;
	virtual void SetVoiceThresholdHold( float flTime ) = 0;
	virtual float GetVoiceCurrentPeak() = 0;
	virtual bool IsVoiceActive() = 0;

	virtual void StopVoiceRecording() = 0;
};

#endif // ISOUNDSERVICE_H
