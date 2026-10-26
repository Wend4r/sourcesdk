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

//-----------------------------------------------------------------------------
// An enumerated audio output device
//-----------------------------------------------------------------------------
struct audio_device_description_t
{
	wchar_t m_deviceName[ 256 ];
	char m_friendlyName[ 256 ];
	uint32 m_nDeviceID;
	uint32 m_nSampleRate;
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
	// Capture devices, the voice input stream takes its device from here
	CUtlVector< audio_device_description_t > m_inputList;
	int m_nDefaultDevice;
};

//-----------------------------------------------------------------------------
// Shares the voice container parameter map of the sound system
//-----------------------------------------------------------------------------
class CVSndParameterTable
{
public:
	// Creates the map through ISoundSystem::CreateVoiceContainerParams on first use
	virtual std::shared_ptr< CUtlOrderedMap< CUtlStringToken, VoiceContainerParamMem_t > > GetParams();

	std::shared_ptr< CUtlOrderedMap< CUtlStringToken, VoiceContainerParamMem_t > > m_pParams;
};

class CSoundSystem : public CTier2AppSystem< ISoundSystem >, public IVConCommDataReceived
{
public:
	// Last guids handed out by GenerateLocalSoundGuid, GenerateServerSoundGuid, GenerateLocalSoundEventGuid and GenerateServerSoundEventGuid
	uint32 m_nLastLocalSoundGuid;
	uint32 m_nLastServerSoundGuid;
	uint32 m_nLastLocalSoundEventGuid;
	uint32 m_nLastServerSoundEventGuid;
	// Set by the VSND message of VConsole; turning it on sends the async wave cache contents to VConsole at once
	bool m_bSendAsyncCacheToVConsole : 1;
	bool m_bNeedsRestart : 1;
	uint8 m_pad041[ 0xF ];

	// Copy of the last audio state passed to Update, extended by the listener data the mixer derives from it
	struct : public CAudioState
	{
		// snd_gain, snd_musicvolume, snd_gamevolume, snd_gamevoicevolume and snd_voipvolume of the frame
		float m_flGain;
		float m_flMusicVolume;
		float m_flGameVolume;
		float m_flGameVoiceVolume;
		float m_flVoipVolume;
		int m_nActiveListenerCount;
		// Set when any active listener is underwater
		bool m_bAnyListenerUnderwater;
		// 0 pauses with m_bPaused, 2 always pauses, other values never pause; SetIgnorePause stores its bool
		uint8 m_nPauseOverride;
		audio_listener_vectors_t m_ListenerVectors[ 4 ];
	} m_AudioState;

	CAudioDeviceList m_DeviceList;
	double m_flDisableMuteUntil;
	uint8 m_pad288[ 0x8 ];
	CSoundDebugVisualizer *m_pDebugVisualizer;
	float m_flUnk298;
	int m_nInitCount;
	CVSndParameterTable m_VSndParameterTable;
	// Guards the networked voice streams
	CThreadFastMutex m_VoiceStreamMutex;
	// Functions queued by QueueFunction
	CTSQueue< std::function< void() > * > m_QueuedFunctions;
	uint8 m_pad310[ 0x2 ];
	// QueueFunction runs the function at once instead of queuing it
	bool m_bRunFunctionsImmediately;
	// Functions run by Update or QueueFunction while it is set are dropped instead
	bool m_bSkipQueuedFunctions;
	// Set while Update runs the queued functions
	bool m_bRunningQueuedFunctions;
};

#endif // SOUNDSYSTEM_H
