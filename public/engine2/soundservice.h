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
		// Offset of the last played block in the sequence and the offset the next block has to start at
		int m_nLastOffset;
		int m_nNextOffset;
		// Time of the last accepted packet, packets of an older sequence are dropped for 10 seconds after it
		double m_flLastReceiveTime;
		uint16 m_nLevel;
		// Data of the blocks that wait for their turn
		CUtlVector< uint8 > m_BufferedData;
		CUtlString m_sSoundNameOverride;
		CUtlVector< BufferedVoiceData_t > m_BufferedBlocks;
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
		// RMS of the last captured block
		float m_flRMS;
		int m_nHoldCounter;
		int m_nUnk24;
		int m_nUnk28;
		// Delays the captured samples by the attack time (SetVoiceThresholdAttack)
		CUtlVector< float > m_AttackDelayLine;
	} m_NoiseGate;

	float m_flCurrentPeak;
	// RMS of the last captured block in dB (m_flCurrentPeak is its peak in dB)
	float m_flCurrentRMS;

	// Time of the last captured block; a gap longer than voice_input_stallout sets m_bInputStalled
	float m_flLastInputTime;
	bool m_bInputStalled;
	CUtlString m_Unk280;
	// Set by the voice target and networked voice code when the first slot of any registered voice target returns true
	bool m_bVoiceTargetActive;
	// Level of the last encoded block, raw and normalized to [0, 1]
	int m_nVoiceLevel;
	float m_flVoiceLevel;
	int m_nUnk294;
	double m_flLastFocusTime;
	// Set with its time while the level stays below voice_threshold; cleared voice_threshold_delay after it rises above
	double m_flBelowThresholdTime;
	bool m_bBelowThreshold;
	uint8 m_pad2A9;
	bool m_bRecordingVoice;
	bool m_bUnk2AB;
	// The speaking state last reported to ISteamFriends::SetInGameVoiceSpeaking
	bool m_bInGameVoiceSpeaking;
	uint8 m_pad2AD;
	// IInputService::IsAppActive of the last frame, and set between OnLoopActivate and OnLoopDeactivate
	bool m_bAppIsActive;
	bool m_bGameIsActive;
	bool m_bUnk2B0;
	int m_nUnk2B4;
	// Sequence number of the voice packets, skips -1
	int m_nVoiceSequence;
	int m_nUnk2BC;
	bool m_bUnk2C0;
	CUtlVector< RecentlyPlayedNetworkVoice_t > m_RecentlyPlayedNetworkVoices;
};

#endif // SOUNDSERVICE_H
