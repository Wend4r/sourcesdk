#ifndef SOUNDSYSTEM_H
#define SOUNDSYSTEM_H

#ifdef _WIN32
#pragma once
#endif

#include "soundsystem/isoundsystem.h"
#include "tier0/threadtools.h"
#include "tier0/tslist.h"
#include "tier0/utlvector.h"
#include "tier2/tier2.h"
#include "vconcomm/ivconcomm.h"

#include <functional>

class CSoundDebugVisualizer;
class CVMixTypeManager;

//-----------------------------------------------------------------------------
// An enumerated audio output device
//-----------------------------------------------------------------------------
struct audio_device_description_t
{
	wchar_t m_deviceName[ 256 ];
	char m_friendlyName[ 256 ];
	uint32 m_nDeviceID;
	uint32 m_nUnk304;
	uint8 m_nSubsystemId;
	uint8 m_nChannelCount;
	bool m_bIsDefault : 1;
	bool m_bIsAvailable : 1;
};

//-----------------------------------------------------------------------------
// The list of audio output devices of the current audio subsystem
//-----------------------------------------------------------------------------
class CAudioDeviceList
{
public:
	uint32 m_nDeviceStamp;
	CUtlVector< audio_device_description_t > m_list;
	CUtlVector< audio_device_description_t > m_Unk20;
	int m_nDefaultDevice;
};

//-----------------------------------------------------------------------------
// Shares the voice container parameter map of the sound system
//-----------------------------------------------------------------------------
class CVSndParameterTable
{
public:
	virtual std::shared_ptr< CUtlOrderedMap< CUtlStringToken, VoiceContainerParamMem_t > > unk000();

	std::shared_ptr< CUtlOrderedMap< CUtlStringToken, VoiceContainerParamMem_t > > m_pUnk008;
};

class CSoundSystem : public CTier2AppSystem< ISoundSystem >, public IVConCommDataReceived
{
public:
	uint32 m_nUnk030;
	uint32 m_nUnk034;
	uint32 m_nUnk038;
	uint32 m_nUnk03C;
	bool m_bUnk040 : 1;
	bool m_bNeedsRestart : 1;
	uint8 m_pad041[ 0xF ];
	CVMixTypeManager *m_pUnk050;

	// Copy of the last audio state passed to Update, extended by the listener data the mixer derives from it
	struct : public CAudioState
	{
		float m_flUnk118;
		uint32 m_nUnk11C[ 4 ];
		int m_nUnk12C;
		bool m_bUnk130;
		bool m_bUnk131;
		Vector m_vecUnk134[ 4 ][ 4 ];
	} m_AudioState;

	CAudioDeviceList m_DeviceList;
	double m_flDisableMuteUntil;
	uint8 m_pad298[ 0x8 ];
	CSoundDebugVisualizer *m_pUnk2A0;
	float m_flUnk2A8;
	int m_nUnk2AC;
	CVSndParameterTable m_Unk2B0;
	CThreadFastMutex m_UnkMutex2C8;
	CTSQueue< std::function< void() > * > m_Unk2E0;
	uint8 m_pad320[ 0x2 ];
	bool m_bUnk322;
	bool m_bUnk323;
	bool m_bUnk324;
};

#endif // SOUNDSYSTEM_H
