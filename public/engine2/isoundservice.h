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
// Encoding of a block of voice data, other values are rejected by PlayNetworkedVoice
enum VoiceRecordingFormat_t
{
	// Steam voice data, decompressed through ISteamUser
	VOICE_RECORDING_FORMAT_STEAM = 0,
	// Up to four Opus frames
	VOICE_RECORDING_FORMAT_OPUS = 1,
};

struct VoiceRecordingDescription_t
{
	VoiceRecordingFormat_t m_nFormat;
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
	// Set by every writer that fills the level
	bool m_bValid;
	uint8 m_pad0D[ 0x3 ];
	// The playback writes the 0..1 level of m_flLevel; the client voice status also takes a value of 0 or below as dB
	float m_flVoiceLevel;
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
