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

	// Set while m_Unk080 overrides the audio state (unk028 / unk029)
	bool m_bUnk078;
	CAudioState m_Unk080;

	CThreadFastMutex m_UnkMutex198;
	CUtlVector< IVoiceTarget * > m_Unk1A8;
	uint8 m_pad1C0[ 0x8 ];
	int m_nUnk1C8;

	// Voice capture: input stream, captured samples, encoded Opus frames, encoder and noise suppression state
	IAudioInputStream *m_pUnk1D0;
	CCircularBuffer m_Unk1D8;
	CCircularBuffer m_Unk1F0;
	CUtlLeanVector< uint8 > m_Unk208;
	CUtlLeanVector< uint8 > m_Unk218;

	// Noise gate (SetVoiceThreshold, SetVoiceThresholdDelay, SetVoiceThresholdAttack, SetVoiceThresholdHold)
	struct
	{
		float m_flUnk00;
		float m_flUnk04;
		float m_flUnk08;
		float m_flUnk0C;
		float m_flUnk10;
		int m_nUnk14;
		int m_nUnk18;
		float m_flUnk1C;
		int m_nUnk20;
		int m_nUnk24;
		int m_nUnk28;
		CUtlVector< float > m_Unk30;
	} m_Unk228;

	float m_flUnk270;
	float m_flUnk274;
	float m_flUnk278;
	bool m_bUnk27C;
	CUtlString m_Unk280;
	bool m_bUnk288;
	int m_nUnk28C;
	int m_nUnk290;
	int m_nUnk294;
	double m_flUnk298;
	double m_flUnk2A0;
	bool m_bUnk2A8;
	uint8 m_pad2A9;
	bool m_bUnk2AA;
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
	CUtlVector< RecentlyPlayedNetworkVoice_t > m_Unk2C8;
};

#endif // SOUNDSERVICE_H
