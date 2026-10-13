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
	int m_nUnk00;
	int m_nSampleRate;
	int m_nSequence;
	int m_nOffset;
	const char *m_pszUnk10;
	int m_nFrameCount;
	int m_nFrameEnds[ 4 ];
	float m_flUnk2C;
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
	virtual bool PlayNetworkedVoice( uint64 nXUID, CEntityIndex nEntityIndex, const VoiceRecordingDescription_t &desc, const void *pData, int nSize, bool bUnk, VoiceRecordingLevel_t *pLevel ) = 0;

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
