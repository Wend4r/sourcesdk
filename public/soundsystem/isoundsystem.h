//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: 
//
//===========================================================================//

#ifndef ISOUNDSYSTEM_H
#define ISOUNDSYSTEM_H
#ifdef _WIN32
#pragma once
#endif

#include "appframework/iappsystem.h"
#include "mathlib/vector.h"
#include "resourcesystem/resourcehandle.h"
#include "soundsystem/isoundopsystem.h"
#include "soundsystem/soundflags.h"
#include "tier0/platwindow.h"
#include "tier0/utlbuffer.h"
#include "tier0/utlmap.h"
#include "tier0/utlstring.h"
#include "tier0/utlstringtoken.h"
#include "tier0/utlvector.h"

#include <functional>
#include <memory>


//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class CAudioDeviceList;
class CAudioProcessor;
class IAudioCacheHandle;
class IAudioDecoder;
class IAudioDevice;
class IAudioDevice2;
class IAudioInputStream;
class IAudioOutputStream;
class IAudioStreamRecorder;
class IAudioWAVRecorder;
class CAudioSource;
class CAudioMixer;
class CBufferString;
class CSfxTable;
class ISoundSystemServices;
class ISteamUser;
struct StartSoundParams_t;
class CVSndParameterTable;
class CVoiceTraceKV3;
class CUtlBuffer;
struct ResourceFileHeader_t;
struct VoiceContainerParamMem_t;
struct VSound_t;


//-----------------------------------------------------------------------------
// Sound handle
//-----------------------------------------------------------------------------
typedef unsigned short AudioSourceHandle_t;
enum
{
	AUDIOSOURCEHANDLE_INVALID = (AudioSourceHandle_t)~0
};

struct SoundGuid_t
{
	SoundGuid_t()
		: m_Value( 0 )
	{
	}
	SoundGuid_t( const int32 nValue )
		: m_Value( nValue )
	{
	}

	int32 m_Value;
};

enum sound_voice_layer_t : uint8
{
	VOICE_LAYER_GAME = 0,
	VOICE_LAYER_UI,
	VOICE_LAYER_TOOL,
	VOICE_LAYER_ASYNC_LOAD,
	VOICE_LAYER_VOIP,
	NUM_VOICE_LAYERS,

	VOICE_LAYER_NONE = 0xFE,
	VOICE_LAYER_ALL = 0xFF
};

enum sound_fade_type_t : uint8
{
	SOUND_FADE_EQUAL_POWER = 0,
	SOUND_FADE_LINEAR
};


//-----------------------------------------------------------------------------
// Sound resource handle
//-----------------------------------------------------------------------------
class InfoForResourceTypeVSound_t
{
public:
	using RuntimeClass_t = VSound_t;
};

using HSound = CWeakHandle< InfoForResourceTypeVSound_t >;


//-----------------------------------------------------------------------------
// Flags for FindAudioSource
//-----------------------------------------------------------------------------
enum FindAudioSourceFlags_t
{
	FINDAUDIOSOURCE_NODELAY = 0x1,
	FINDAUDIOSOURCE_PREFETCH = 0x2,
	FINDAUDIOSOURCE_PLAYONCE = 0x4,
};


//-----------------------------------------------------------------------------
// Frame state the engine passes to ISoundSystem::Update: time, mute flags and
// up to four listeners
//-----------------------------------------------------------------------------
class CAudioState
{
public:
	CAudioState()
	{
		m_flAccumulatedTime = 0.0;
		m_flFrameTime = 0.0;
		m_flRealTime = 0.0;
		m_pSteamUser = nullptr;
		m_bUnk20_0 = false;
		m_bUnk20_1 = false;
		m_bMuted = false;
		m_bUnk20_3 = false;
		m_bUnk20_4 = false;
		m_bUnk20_5 = false;

		for ( int i = 0; i < 4; i++ )
		{
			m_Listeners[ i ].m_vecOrigin.Init();
			m_Listeners[ i ].m_angAngles.Init();
			m_Listeners[ i ].m_vecUnk18.Init();
			m_Listeners[ i ].m_vecUnk24.Init();
			m_Listeners[ i ].m_nViewEntity = -1;
			m_Listeners[ i ].m_nUnk34 = -1;
			m_Listeners[ i ].m_bUnk38_0 = false;
			m_Listeners[ i ].m_bActive = false;
		}
	}

	// Sum of the frame times, the last frame time and the time the state was built
	double m_flAccumulatedTime;
	double m_flFrameTime;
	double m_flRealTime;
	ISteamUser *m_pSteamUser;
	bool m_bUnk20_0 : 1;
	bool m_bUnk20_1 : 1;
	bool m_bMuted : 1;
	bool m_bUnk20_3 : 1;
	bool m_bUnk20_4 : 1;
	bool m_bUnk20_5 : 1;

	// One listener per split screen slot
	struct
	{
		Vector m_vecOrigin;
		QAngle m_angAngles;
		Vector m_vecUnk18;
		Vector m_vecUnk24;
		int m_nViewEntity;
		int m_nUnk34;
		bool m_bUnk38_0 : 1;
		bool m_bActive : 1;
	} m_Listeners[ 4 ];
};


//-----------------------------------------------------------------------------
// Direction vectors the sound system derives from the angles of a listener,
// and the listening position next to its origin
//-----------------------------------------------------------------------------
struct audio_listener_vectors_t
{
	Vector m_vecForward;
	Vector m_vecLeft;
	Vector m_vecUp;
	Vector m_vecPosition;
};


//-----------------------------------------------------------------------------
// Description of a playing voice (ISoundSystem::GetActiveSounds)
//-----------------------------------------------------------------------------
struct SndInfo_t
{
	SoundGuid_t m_nGuid;
	// File name of the sfx of the voice
	FileNameHandle_t m_hFileName;
	int m_nUnk08;
	int m_nUnk0C;
	float m_flVolume;
	// Percent
	int m_nPitch;
	Vector m_vecOrigin;
	float m_flUnk24;
};


//-----------------------------------------------------------------------------
// Parameters of the capture stream created by ISoundSystem::CreateInputStream
//-----------------------------------------------------------------------------
struct AudioInputStreamParams_t
{
	int m_nSampleRate;
	uint32 m_nDeviceID;
	std::function< void( IAudioInputStream *pStream, int nUnk ) > m_callback;
};


//-----------------------------------------------------------------------------
// PCM samples of a decoded sound (ISoundSystem::ToolLoadVSnd)
//-----------------------------------------------------------------------------
class CSimplePCMWaveData
{
public:
	uint16 m_nFormat;
	uint32 m_nSampleRate;
	uint32 m_nChannels;
	uint32 m_nSampleCount;
	uint32 m_nBits;
	float m_flDuration;
	CUtlBuffer m_pcmData;
};


//-----------------------------------------------------------------------------
// Voice settings of ISoundSystem::StartVoice and UpdateVoice. The names are the keys the voice trace
// writes them under.
//-----------------------------------------------------------------------------
struct SoundVoiceParams_t
{
	float m_flMasterVolume;
	float m_flPitch;
	float m_flSeekTime;
	float m_flMaxVolumeChangePerBuffer;
	float m_flSystemVolumeFadeRate;
	float m_flTimeScale;
	// A start sample other than 0 schedules the start on the sample clock
	uint32 m_nStartSample;
	uint8 m_nChannelSelect;
	sound_voice_layer_t m_nLayer;
	bool m_bShouldPause;
};

//-----------------------------------------------------------------------------
// What ISoundSystem::StartVoice and UpdateVoice apply, either part may be null
//-----------------------------------------------------------------------------
struct SoundVoicePlayback_t
{
	SoundVoiceParams_t *m_pVoice;
	// Origin, room and facing DSP sends, volume, send list and mix groups of the voice (layout not recovered)
	void *m_pStack;
};


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define SOUNDSYSTEM_INTERFACE_VERSION "SoundSystem001"

abstract_class ISoundSystem : public IAppSystem
{
public:
	virtual IAudioDevice2 *CreateDefaultDevice() = 0;
	virtual void DestroyDevice( IAudioDevice2 *pDevice ) = 0;
	// Creates a CPresetDSPAudioProcessor through the sound system processor factory, nUnused is read by neither build
	virtual CAudioProcessor *CreatePresetDSPProcessor( const char *pName, int nUnk1, float flCrossFade, int nChannels, int nUnused, bool bUnk ) = 0;
	virtual IAudioDevice2 *CreateNullDevice() = 0;
	// Opens a capture stream of the input device (voice chat)
	virtual IAudioInputStream *CreateInputStream( AudioInputStreamParams_t &params ) = 0;
	virtual void DestroyInputStream( IAudioInputStream *pInputStream ) = 0;
	virtual IAudioOutputStream *CreateDirectMixOutputStream( uint32 nSampleRate, uint32 nChannels, uint32 nBits ) = 0;
	// Creates an output stream that plays through a field of a playing sound event
	virtual IAudioOutputStream *CreateAttachedOutputStream( SoundEventGuid_t guid, const char *pFieldName, uint32 nSampleRate, uint32 nChannels, uint32 nBits ) = 0;
	virtual void DestroyOutputStream( IAudioOutputStream *pOutputStream ) = 0;
	virtual void InitSoundSystem( PlatWindow_t hWindow, ISoundSystemServices *pSoundSystemServices ) = 0;
	virtual bool IsSoundEnabled() = 0;
	virtual void Update( const CAudioState *pAudioState, bool bAllowAsync ) = 0;
	virtual void StopAllSounds() = 0;
	// Stops every voice and every sound event stack
	virtual void StopAllSoundsAndSoundEvents() = 0;
	// Starts a voice from sound parameters and returns its guid
	virtual SoundGuid_t StartSound( StartSoundParams_t &params ) = 0;
	virtual void StopSound( SoundGuid_t guid, bool bForceSync ) = 0;
	// Returns the mixer sample time of a host time, keeping the offset of the clock stable between calls
	virtual uint32 SyncSampleClock( double flTime, int nClock ) = 0;
	virtual void ResetSampleClock( int nClock ) = 0;
	virtual float GetSoundDuration( SoundGuid_t guid ) = 0;
	virtual bool IsSoundPlaying( SoundGuid_t guid ) = 0;
	virtual void SetSoundVolume( SoundGuid_t guid, float flVolume ) = 0;
	virtual bool IsSoundLooped( SoundGuid_t guid ) = 0;
	virtual CSfxTable *PrecacheSound( const char *pSample ) = 0;
	virtual void PreloadSoundEvent( CSoundEvent *pSoundEvent, int16 nPriority ) = 0;
	virtual void PreloadSoundEvent( const char *pSoundEventName, int16 nPriority ) = 0;
	virtual void PreloadSound( CSfxTable *pSfx, int16 nPriority ) = 0;
	virtual void PreloadSound( const char *pSoundName, int16 nPriority ) = 0;
	// snd_gain attenuated by the distance multiplier of the sound level, flFoliageLoss adds a loss in dB per 1200 units
	virtual float GetGainFromSoundLevel( soundlevel_t nSoundLevel, float flDistance, float flFoliageLoss ) = 0;
	virtual void SetMixGroupOfCurrentMixer( const char *pszGroupName, const char *pszParam, float flValue ) = 0;
	// Returns -1 for an unknown mix layer
	virtual int GetMixLayerIndex( const char *pLayerName ) = 0;
	virtual void SetMixLayerLevel( int nLayer, float flLevel ) = 0;
	virtual void unk043( float flValue ) = 0;
	virtual float unk044() = 0;
	// Adding the first recorder switches the output to the audio capture device, removing the last one restores the default device
	virtual void RegisterAudioStreamRecorder( IAudioStreamRecorder *pRecorder, bool bRegister ) = 0;
	virtual IAudioWAVRecorder *CreateWAVRecorder() = 0;
	virtual bool IsUsingAudioCaptureDevice() = 0;
	virtual IAudioWAVRecorder *CreateMicrophoneWAVRecorder( const char *pszFileName ) = 0;
	virtual void ResetMixControlInput( CUtlStringToken nName ) = 0;
	virtual void ClearMixControlInputOverride( CUtlStringToken nName ) = 0;
	virtual void ClearMixImpulseResponseInputOverride( CUtlStringToken nName ) = 0;
	virtual void ClearMixControlTransientInputOverride( CUtlStringToken nName ) = 0;
	virtual void ClearMixFloatArrayInputOverride( CUtlStringToken nName ) = 0;
	// An override value ignores the later calls without bOverride until its override is cleared
	virtual void SetMixControlInput( CUtlStringToken nName, float flValue, bool bOverride ) = 0;
	virtual bool TriggerMixControlTransientInput( CUtlStringToken nName ) = 0;
	virtual void SetMixNameInput( CUtlStringToken nName, uint32 nValue ) = 0;
	virtual uint32 GetMixNameInputValue( CUtlStringToken nName ) = 0;
	virtual float GetMixControlInputValue( CUtlStringToken nName ) = 0;
	// 0 when the mixer has no entry for the value
	virtual float unk059( uint32 nUnk ) = 0;
	virtual uint32 GetDSPPresetNameToken( int nIndex ) = 0;
	virtual CUtlString GetDSPPresetName( int nIndex ) = 0;
	virtual int GetDSPPresetCount() = 0;
	// pImpulseResponse points to an intrusive reference to the impulse response object
	virtual void SetMixImpulseResponseInput( CUtlStringToken nName, void *pImpulseResponse, bool bOverride ) = 0;
	virtual void GetMixGraphNames( CUtlVector< CUtlString > &vecOut ) = 0;
	virtual SoundGuid_t GetGuidForLastSoundEmitted() = 0;
	virtual void GetActiveSounds( CUtlVector< SndInfo_t > &vecSounds ) = 0;
	// Networked voice: one stream per player, created on the first data at 48 kHz
	virtual bool PlayVoiceStream( uint64 nXUID, CEntityIndex nEntityIndex, const char *pszSoundName ) = 0;
	virtual void QueueVoiceSamples( uint64 nXUID, CEntityIndex nEntityIndex, uint32 nSampleRate, const float *pSamples, int nSampleCount, const char *pszSoundName ) = 0;
	virtual void QueueVoiceFrames( uint64 nXUID, CEntityIndex nEntityIndex, const void **ppFrames, const int *pFrameSizes, int nFrameCount, const char *pszSoundName, bool bCreateStream ) = 0;
	// True without a stream, or when its buffer ran low and no data came for voice_stall_ms
	virtual bool IsVoiceStreamStalled( uint64 nXUID ) = 0;
	virtual void SetVoiceVolume( uint64 nXUID, float flVolume ) = 0;
	virtual float GetVoiceVolume( uint64 nXUID ) = 0;
	virtual void ResetVoiceVolumes() = 0;
	// Copies the device list when its stamp differs from the one in pDeviceList
	virtual bool GetAudioDeviceList( CAudioDeviceList *pDeviceList ) = 0;
	// Guids for sounds and sound events started here (negative range) and by the server (positive range)
	virtual SoundGuid_t GenerateLocalSoundGuid() = 0;
	virtual SoundGuid_t GenerateServerSoundGuid() = 0;
	virtual SoundEventGuid_t GenerateLocalSoundEventGuid() = 0;
	virtual SoundEventGuid_t GenerateServerSoundEventGuid() = 0;
	// Records the voices that start and stop while enabled
	virtual void EnableSubvoiceTrace() = 0;
	virtual void DisableSubvoiceTrace() = 0;
	virtual bool IsSubvoiceTraceEnabled() = 0;
	virtual void GetSubvoiceTrace( void *pStarted, void *pStopped, bool bClear ) = 0;
	virtual bool FadeOutSound( SoundGuid_t guid, float flFadeTime, sound_fade_type_t nFadeType ) = 0;
	virtual void FadeOutAllSounds( float flFadeTime, sound_voice_layer_t nVoiceLayer, sound_fade_type_t nFadeType ) = 0;
	virtual bool unk085( int nUnk, float flFadeTime, sound_fade_type_t nFadeType ) = 0;
	virtual CAudioState *GetAudioState() = 0;
	virtual bool IsSoundPlaying( SoundEventGuid_t guid ) = 0;
	virtual void unk087( bool bUnk ) = 0;
	virtual bool unk088() = 0;
	virtual bool IsMuted() = 0;
	// Plays a wave file through the operating system
	virtual void PlaySystemSound( const char *pWavFile ) = 0;
	virtual void UnloadAddonSoundResources( bool bAddonsUnmounted, bool bAddonsMounted ) = 0;
	virtual CSfxTable *GetSoundSfx( SoundGuid_t guid ) = 0;
	virtual void LoadSteamAudioScene( void *pKey, const char *pszName ) = 0;
	virtual void UnloadSteamAudioScene( void *pKey ) = 0;
	virtual bool DisableAudioMuting( float flDisableDurationSeconds ) = 0;
	virtual void ResetMainGraphInputs() = 0;
	// Builds the VSound_t of a compiled vsnd resource in pBuffer and returns it
	virtual VSound_t *AllocVSound( const ResourceFileHeader_t *pHeader, CUtlVector< uint8 > *pBuffer ) = 0;
	// Reads the compiled vsnd file of pszName and decodes its samples
	virtual bool ToolLoadVSnd( CSimplePCMWaveData *pWaveData, const char *pszName ) = 0;
	virtual CUtlString GetSoundName( SoundGuid_t guid ) = 0;
	virtual void unk100( void *pUnk ) = 0;
	virtual void StartVoiceTrace() = 0;
	virtual void StopVoiceTrace() = 0;
	// Copies the recorded voice trace into pOut and clears it
	virtual void GetVoiceTrace( void *pOut ) = 0;
	// Voices driven directly, recorded by the voice trace as "startVoice", "updateVoice", "graph", "startSubvoice",
	// "stopSubvoice", "listener" and "mainGraph". pInputs holds the control and name inputs of a graph.
	virtual SoundGuid_t StartVoice( FileNameHandle_t hVSndName, CUtlStringToken nGraphName, const SoundVoicePlayback_t *pPlayback ) = 0;
	virtual void UpdateVoice( SoundGuid_t guid, const SoundVoicePlayback_t *pPlayback ) = 0;
	virtual void SetVoiceGraphInputs( SoundGuid_t guid, const void *pInputs ) = 0;
	virtual void StartSubvoice( SoundGuid_t guid, const void *pSubvoice ) = 0;
	virtual void StopSubvoice( SoundGuid_t guid, uint16 nSubvoiceId ) = 0;
	// pListener holds the origin, angles, left and right hand positions and a flag
	virtual void SetListenerAndMainGraphInputs( const void *pListener, const void *pInputs ) = 0;
	// Time of the last frame, a count summed over the frames and the frame count of the active voice trace
	virtual void GetVoiceTraceSummary( void *pSummary ) = 0;
	// Writes a trace taken with GetVoiceTrace as KeyValues3 frames
	virtual CVoiceTraceKV3 *CreateVoiceTraceKV3( const void *pTrace ) = 0;
	// Wakes the mixer of the current device
	virtual void OnFrameBoundary() = 0;
	// Writes the state of the active voices and sound events as text
	virtual void OutputDebugInfo( CUtlBuffer &buf ) = 0;
	virtual float GetSoundEventElapsedTime( SoundEventGuid_t guid ) = 0;
	virtual void unk115( bool bUnk ) = 0;
	// The sample clock base of StartVoice with a start sample resyncs on the next such start
	virtual void ResetVoiceStartSampleBase() = 0;
	// While enabled the output device keeps the peak of the samples it writes
	virtual void EnableOutputPeakMeter() = 0;
	virtual void DisableOutputPeakMeter() = 0;
	// Output peak in dB of the device, 0 for the null device
	virtual float GetOutputPeakLevel() = 0;
	virtual std::unique_ptr< CAudioMixer > CreateAudioMixer( CAudioSource *pSource ) = 0;
	virtual int unk121( void *pUnk ) = 0;
	virtual void unk122( int nUnk ) = 0;
	virtual std::unique_ptr< CAudioSource > CreateAudioSource( CSfxTable *pSfx, VSound_t *pSound ) = 0;
	virtual std::unique_ptr< IAudioCacheHandle > CreateAudioCacheHandle( CSfxTable *pSfx ) = 0;
	virtual IAudioDecoder *CreateStretchDecoder( IAudioDecoder *pDecoder, float flStretch ) = 0;
	virtual uint32 GetSfxSampleCount( CSfxTable *pSfx ) = 0;
	virtual const char *GetSfxFileName( CSfxTable *pSfx, CBufferString *pBuf ) = 0;
	virtual CVSndParameterTable *GetVSndParameterTable() = 0;
	virtual std::shared_ptr< CUtlOrderedMap< CUtlStringToken, VoiceContainerParamMem_t > > CreateVoiceContainerParams() = 0;
	virtual void unk130( CSfxTable *pSfx ) = 0;
	virtual void unk130( CSoundEvent *pSoundEvent ) = 0;
	virtual HSound CreateProceduralSound( const char *pName, void *pUnk ) = 0;
	// Runs the function now or queues it for the next update
	virtual void QueueFunction( std::function< void() > func ) = 0;
};



#endif // ISOUNDSYSTEM_H
