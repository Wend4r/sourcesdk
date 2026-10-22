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
	// Voice status changes the capture code queued for ISource2Client::VoiceStatus, reported on the next frame
	int m_nPendingVoiceStatusCount;

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
		// Envelope steps towards a higher and a lower peak
		float m_flAttackRate;
		// Peak of the last four samples
		float m_flPeak;
		float m_flDelayRate;
		int m_nHoldSamples;
		// Read position of m_AttackDelayLine relative to m_nAttackDelayIndex, the write position
		int m_nAttackDelay;
		// RMS of the last captured block
		float m_flRMS;
		int m_nHoldCounter;
		int m_nUnk24;
		int m_nAttackDelayIndex;
		// Delays the captured samples by the attack time (SetVoiceThresholdAttack)
		CUtlVector< float > m_AttackDelayLine;
	} m_NoiseGate;

	float m_flCurrentPeak;
	// RMS of the last captured block in dB (m_flCurrentPeak is its peak in dB)
	float m_flCurrentRMS;

	// Time of the last captured block; a gap longer than voice_input_stallout sets m_bInputStalled
	float m_flLastInputTime;
	bool m_bInputStalled;
	// voice_device_override the input stream was created for
	CUtlString m_sVoiceDeviceOverride;
	// Set by the voice target and networked voice code when the first slot of any registered voice target returns true
	bool m_bVoiceTargetActive;
	// Level of the last encoded block, raw and normalized to [0, 1]
	int m_nVoiceLevel;
	float m_flVoiceLevel;
	// ISteamUser::GetVoiceOptimalSampleRate, the rate Steam voice is decompressed at
	int m_nVoiceSampleRate;
	double m_flLastFocusTime;
	// Set with its time while the level stays below voice_threshold; cleared voice_threshold_delay after it rises above
	double m_flBelowThresholdTime;
	bool m_bBelowThreshold;
	// Talking state of the queued voice status changes, set while Steam voice returns decompressed data
	bool m_bVoiceStatusTalking;
	bool m_bRecordingVoice;
	// Set once ISteamUser::GetAvailableVoice reports that recording ended, starts the next voice sequence
	bool m_bVoiceRecordingFinished;
	// The speaking state last reported to ISteamFriends::SetInGameVoiceSpeaking
	bool m_bInGameVoiceSpeaking;
	// The speaking state last reported while voice_in_process is set
	bool m_bInProcessVoiceSpeaking;
	// IInputService::IsAppActive of the last frame, and set between OnLoopActivate and OnLoopDeactivate
	bool m_bAppIsActive;
	bool m_bGameIsActive;
	// Creating the input stream failed, it is retried once voice_device_override changes
	bool m_bInputStreamFailed;
	// Scheduled function that StopVoiceRecording and Shutdown cancel, -1 for none
	int m_hScheduledFunction;
	// Sequence number of the voice packets, skips -1
	int m_nVoiceSequence;
	// Encoded bytes sent in the current voice sequence
	int m_nVoiceOffset;
	// Latches m_bVoiceTargetActive, keeps the microphone sampled once a voice target was active
	bool m_bVoiceTargetWasActive;
	CUtlVector< RecentlyPlayedNetworkVoice_t > m_RecentlyPlayedNetworkVoices;
};

#endif // SOUNDSERVICE_H
