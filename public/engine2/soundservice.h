#ifndef SOUNDSERVICE_H
#define SOUNDSERVICE_H

#ifdef _WIN32
#pragma once
#endif

#include "engine2/isoundservice.h"
#include "engine2/iengineservicemgr.h"
#include "soundsystem/isoundsystem.h"
#include "tier0/circularbuffer.h"
#include "tier0/threadtools.h"
#include "tier0/utlleanvector.h"
#include "tier0/utlstring.h"
#include "tier1/convar.h"
#include "tier1/utlvector.h"

class CSoundService : public CBaseEngineService< ISoundService >
{
public:
	//-----------------------------------------------------------------------------
	// Networked voice of one player, buffered until its packets can be played in order
	//-----------------------------------------------------------------------------
	struct RecentlyPlayedNetworkVoice_t
	{
		struct BufferedVoiceData_t
		{
			VoiceRecordingDescription_t m_desc;
			int m_nDataOffset;
			int m_nDataSize;
			CEntityIndex m_nEntityIndex;
			double m_flTime;
		};

		uint64 m_nXUID;
		int m_nSequence;
		int m_nUnk0C;
		int m_nUnk10;
		double m_flUnk18;
		uint16 m_nLevel;
		CUtlVector< uint8 > m_Unk28;
		CUtlString m_Unk40;
		CUtlVector< BufferedVoiceData_t > m_Unk48;
	};

	CConCommandMemberAccessor< CSoundService > m_PlaySoundCommand;

	// Set while m_AudioState holds the state of OverrideAudioState instead of the one built every frame
	bool m_bAudioStateOverridden;
	CAudioState m_AudioState;

	CThreadFastMutex m_VoiceTargetsMutex;
	CUtlVector< IVoiceTarget * > m_VoiceTargets;
	uint8 m_pad1C0[ 0x8 ];
	int m_nUnk1C8;

	// Voice capture: input stream, captured samples, encoded Opus frames, encoder and noise suppression state
	IAudioInputStream *m_pVoiceInputStream;
	CCircularBuffer m_CapturedSamples;
	CCircularBuffer m_EncodedFrames;
	CUtlLeanVector< uint8 > m_OpusEncoder;
	CUtlLeanVector< uint8 > m_NoiseSuppression;

	// Noise gate (SetVoiceThreshold, SetVoiceThresholdDelay, SetVoiceThresholdAttack, SetVoiceThresholdHold)
	struct
	{
		float m_flEnvelope;
		float m_flThreshold;
		float m_flUnk08;
		float m_flUnk0C;
		float m_flDelayRate;
		int m_nHoldSamples;
		int m_nUnk18;
		float m_flUnk1C;
		int m_nHoldCounter;
		int m_nUnk24;
		int m_nUnk28;
		// Delays the captured samples by the attack time (SetVoiceThresholdAttack)
		CUtlVector< float > m_AttackDelayLine;
	} m_NoiseGate;

	float m_flCurrentPeak;
	float m_flUnk274;
	float m_flUnk278;
	bool m_bUnk27C;
	CUtlString m_Unk280;
	bool m_bUnk288;
	int m_nUnk28C;
	int m_nUnk290;
	int m_nUnk294;
	double m_flLastFocusTime;
	double m_flUnk2A0;
	bool m_bUnk2A8;
	uint8 m_pad2A9;
	bool m_bRecordingVoice;
	bool m_bUnk2AB;
	bool m_bUnk2AC;
	uint8 m_pad2AD;
	bool m_bUnk2AE;
	bool m_bUnk2AF;
	bool m_bUnk2B0;
	int m_nUnk2B4;
	int m_nUnk2B8;
	int m_nUnk2BC;
	bool m_bUnk2C0;
	CUtlVector< RecentlyPlayedNetworkVoice_t > m_RecentlyPlayedNetworkVoices;
};

#endif // SOUNDSERVICE_H
