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
#include "soundsystem/isoundopsystem.h"
#include "tier0/utlstringtoken.h"


//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class IAudioDevice;
class CAudioSource;
class CAudioMixer;
class IAudioDevice2;
class IAudioInputStream;
class IAudioOutputStream;
class IAudioWAVRecorder;
class CAudioProcessor;
class CAudioState;
class CAudioDecoderStretch;
class CSfxTable;
class CSoundViewerState;
class CVoiceTraceKV3;
class CUtlBuffer;
class CBufferString;
class CUtlString;
struct StartSoundParams_t;
struct AudioInputStreamParams_t;


//-----------------------------------------------------------------------------
// Sound handle
//-----------------------------------------------------------------------------
typedef unsigned short AudioSourceHandle_t;
enum
{
	AUDIOSOURCEHANDLE_INVALID = (AudioSourceHandle_t)~0
};


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
// Guid of a voice started by ISoundSystem::StartSound (typesafe integer in the game, s&box DWARF)
//-----------------------------------------------------------------------------
class SoundGuid_t
{
public:
	SoundGuid_t() : m_Value( 0 ) {}
	explicit SoundGuid_t( uint32 nValue ) : m_Value( nValue ) {}

	uint32 m_Value;
};

// Reference-counted pointer the game returns by value (one pointer)
struct AudioObjectRef_t
{
	~AudioObjectRef_t() {}

	void *m_pObject;
};

// Reference to a voice container parameter table; a std::shared_ptr in the game (pointer + control block)
struct VoiceContainerParamTableRef_t
{
	~VoiceContainerParamTableRef_t() {}

	void *m_pTable;
	void *m_pRefCount;
};


//-----------------------------------------------------------------------------
// Low level sound system (soundsystem.dll, CSoundSystem). Names from s&box DWARF where the slot is
// there, otherwise named by what the CS2 code does (see re-workspace/vtables/data/proposals/sound_sdk.md).
// "Current voice" functions work on the voice the sound thread is processing (operators call them).
//-----------------------------------------------------------------------------
#define SOUNDSYSTEM_INTERFACE_VERSION "SoundSystem001"

abstract_class ISoundSystem : public IAppSystem
{
public:
	virtual IAudioDevice2 *CreateDefaultDevice() = 0;
	virtual void DestroyDevice( IAudioDevice2 *pDevice ) = 0;
	// Builds a CVMixPresetDSPProcessorDesc and creates its processor
	virtual CAudioProcessor *CreatePresetDSPProcessor( const char *pPresetName, int nPreset, float flMix, int nChannels, int nReserved, bool bStereo ) = 0;
	virtual IAudioDevice2 *CreateNullDevice() = 0;
	// Microphone capture stream (voice chat)
	virtual IAudioInputStream *CreateInputStream( const AudioInputStreamParams_t &params ) = 0;
	virtual void DestroyInputStream( IAudioInputStream *pStream ) = 0;
	virtual IAudioOutputStream *CreateOutputStream( int nSampleRate, int nChannels ) = 0;
	// Output stream fed by the caller (sounds/__attached_output_stream_wrapper_N.vsnd); used by the Panorama movie player
	virtual IAudioOutputStream *CreateAttachedOutputStream( uint32 nSoundEventGuid, int nReserved, int nSampleRate, int nChannels, int nBitsPerSample ) = 0;
	virtual void DestroyOutputStream( IAudioOutputStream *pStream ) = 0;
	virtual void InitSoundSystem( void *hWindow, void *pInitContext ) = 0;
	virtual bool IsSoundEnabled() = 0;
	virtual void Update( const CAudioState *pState, bool bUpdateMixer ) = 0;
	virtual void StopAllSounds() = 0;
	// Stops all sounds and every sound event instance
	virtual void ClearBuffer() = 0;
	virtual SoundGuid_t StartSound( StartSoundParams_t &params ) = 0;
	virtual void StopSound( SoundGuid_t guid ) = 0;
	// Named by what it does: keeps a time for nChannel (0 or 1) for a number of mix samples
	virtual void SetMixHoldTime( double flTime, int nChannel ) = 0;
	virtual void ClearMixHoldTime( int nChannel ) = 0;
	// Position of the voice in seconds (sample position / rate / pitch)
	virtual float GetSoundElapsedTime( SoundGuid_t guid ) = 0;
	// The overload without arguments sits here on Windows only (MSVC groups overloads)
	virtual bool IsSoundStillPlaying( SoundGuid_t guid ) = 0;
	virtual void SetSoundVolume( SoundGuid_t guid, float flVolume ) = 0;
	virtual int GetSoundSampleCount( SoundGuid_t guid ) = 0;
	// Returns NULL on failure; used by the play command
	virtual CSfxTable *PrecacheSound( const char *pVSndName, bool bLoad ) = 0;
	// Sound event preloading: the CSoundEvent overload is first on Linux, MSVC reverses overloads
	virtual void PreloadSoundEvent( CSoundEvent *pSoundEvent, uint16 nPreloadCount ) = 0;
	virtual void PreloadSoundEvent( const char *pSoundEventName, uint16 nPreloadCount ) = 0;
	// Sets the preload count of the sound event and loads or releases its vsnds
	virtual void UpdateSoundEventPreload( CSoundEvent *pSoundEvent, uint16 nPreloadCount ) = 0;
	virtual void UpdateSoundEventPreload( const char *pSoundEventName, uint16 nPreloadCount ) = 0;
	virtual float GetOutputLatency( bool bIncludeDevice ) = 0;
	// "Setting Mixer %s: MixGroup %s: %s : %f" on the active mixer
	virtual void SetMixGroupLevel( const char *pMixGroupName, float flLevel ) = 0;
	// Returns -1 for an unknown mixer
	virtual int GetMixLayerIndex( const char *pLayerName ) = 0;
	virtual void SetMixLayerLevel( int nLayer, float flLevel ) = 0;
	// Named by what it does: a float of the sound system read back by GetOutputGain
	virtual void SetOutputGain( float flGain ) = 0;
	virtual float GetOutputGain() = 0;
	// Destroys the device and creates the default one again
	virtual void RestartDevice( const char *pDeviceId, bool bForce ) = 0;
	// Returns a new CWaveTmpFileWriter; used by movie recording
	virtual IAudioWAVRecorder *CreateWaveTmpFileWriter() = 0;
	virtual bool IsNullDevice() = 0;
	virtual IAudioWAVRecorder *CreateWaveMicrophoneWriter( const char *pFileName ) = 0;
	// Voice container parameters of the current voice, by parameter token
	virtual void FindCurrentVoiceFloatParam( uint32 nParamToken ) = 0;
	virtual bool HasCurrentVoiceFloatParam( uint32 nParamToken ) = 0;
	virtual bool ClearCurrentVoiceResourceParam( uint32 nParamToken ) = 0;
	virtual bool ClearCurrentVoiceEventParam( uint32 nParamToken ) = 0;
	virtual bool HasCurrentVoiceEnumParam( uint32 nParamToken ) = 0;
	virtual bool SetCurrentVoiceParam( uint32 nParamToken, void *pValue, bool bForce ) = 0;
	virtual bool IsCurrentVoiceEventParamSet( uint32 nParamToken ) = 0;
	virtual bool SetCurrentVoiceIntParam( uint32 nParamToken, uint32 nValue ) = 0;
	virtual int64 GetCurrentVoiceIntParam( uint32 nParamToken ) = 0;
	virtual double GetCurrentVoiceFloatParam( uint32 nParamToken ) = 0;
	virtual double GetVoiceParamValue( uint32 nParamToken ) = 0;
	// DSP preset list (entries of the DSP type manager)
	virtual int GetDSPPresetId( int nIndex ) = 0;
	virtual CUtlString GetDSPPresetName( int nIndex ) = 0;
	virtual int GetDSPPresetCount() = 0;
	virtual bool SetCurrentVoiceResourceParam( uint32 nParamToken, void **ppResource, bool bForce ) = 0;
	// Names of the loaded mix graphs, without __toolspreview__
	virtual void GetMixGraphNames( CUtlVector< CUtlString > &names ) = 0;
	virtual CUtlStringToken GetActiveMixGraph() = 0;
	virtual void GetActiveSounds( CUtlVector< void * > &sounds ) = 0;
	// Voice chat streams (core.voip), keyed by the 64-bit id of the speaker
	virtual bool UpdateVoiceStream( uint64 nStreamId, uint32 nSoundEventGuid, void *pParams ) = 0;
	virtual void WriteVoiceStreamData( uint64 nStreamId, uint32 nSoundEventGuid, uint32 nSampleRate, const float *pSamples, int nSamples, void *pParams ) = 0;
	virtual void WriteVoiceStreamEncodedData( uint64 nStreamId, int nSoundEventGuid, int nCodec, const void *pData, int nBytes, void *pParams, bool bReset ) = 0;
	virtual bool IsVoiceStreamFinished( uint64 nStreamId ) = 0;
	// flVolume is clamped to [0, 1]
	virtual void SetVoiceStreamVolume( uint64 nStreamId, float flVolume ) = 0;
	virtual float GetVoiceStreamVolume( uint64 nStreamId ) = 0;
	virtual void ResetVoiceStreamVolumes() = 0;
	// Copies the debug visualizer state when its version changed
	virtual bool UpdateSoundViewerState( CSoundViewerState *pState ) = 0;
	// Guid counters: the first of each pair counts in the negative range, the second in the positive range
	virtual SoundGuid_t GenerateNegativeSoundGuid() = 0;
	virtual SoundGuid_t GeneratePositiveSoundGuid() = 0;
	virtual SoundEventGuid_t GenerateNegativeSoundEventGuid() = 0;
	virtual SoundEventGuid_t GeneratePositiveSoundEventGuid() = 0;
	virtual void PauseAllVoices() = 0;
	virtual void ResumeAllVoices() = 0;
	virtual bool AreAllVoicesPaused() = 0;
	virtual void ApplyVoicePauseState() = 0;
	virtual bool FadeOutSound( SoundGuid_t guid, float flFadeTime, bool bPause ) = 0;
	virtual void StopAllFadingSounds() = 0;
	virtual bool FadeOutSoundsOnLayer( int nLayer, float flFadeTime, bool bPause ) = 0;
	virtual const CAudioState *GetAudioState() = 0;
	// Overload of IsSoundStillPlaying( SoundGuid_t ): asks the sound op system whether its sound event plays
	virtual bool IsSoundStillPlaying() = 0;
	virtual void SetAudioStateEnabled( bool bEnabled ) = 0;
	virtual bool IsAudioStateEnabled() = 0;
	virtual bool IsAudioStateMuted() = 0;
	// Plays a wave file through the operating system
	virtual void PlaySoundAtOSLevel( const char *pWavFile ) = 0;
	// "Addon unloaded, unloading sound resources."
	virtual void OnAddonsChanged( bool bUnloadResources, bool bResetVoiceStreams ) = 0;
	virtual CSfxTable *GetSoundSfx( SoundGuid_t guid ) = 0;
	virtual void RegisterExternalSource( void *pSource, void *pContext ) = 0;
	virtual void UnregisterExternalSource( void *pSource ) = 0;
	virtual bool DisableAudioMuting( float flDuration ) = 0;
	virtual void ResetCurrentVoiceParams() = 0;
	virtual int GetLoadedVSndCount() = 0;
	virtual int64 GetLoadedVSndBytes() = 0;
	virtual CUtlString GetSoundName( SoundGuid_t guid ) = 0;
	virtual void GetRecentSounds( CUtlVector< void * > &sounds ) = 0;
	virtual void ResetMixState() = 0;
	virtual void UpdateMixState() = 0;
	virtual void GetCurrentVoiceChildren( void *pGuidList ) = 0;
	virtual SoundGuid_t GetGuidForLastSoundEmitted() = 0;
	virtual void AttachSoundData( SoundGuid_t guid, void *pData ) = 0;
	virtual void CopyVoiceState( void *pFrom, void *pTo ) = 0;
	virtual void FlushVoiceState() = 0;
	virtual void GetMixBufferInfo( void *pInfo ) = 0;
	virtual CVoiceTraceKV3 *CreateVoiceTrace( void *pTrace ) = 0;
	virtual void QueueRestart() = 0;
	virtual void OutputDebugInfo( CUtlBuffer &buf ) = 0;
	virtual float GetSoundEventElapsedTime( SoundEventGuid_t guid ) = 0;
	virtual void SetDebugOverlayEnabled( bool bEnabled ) = 0;
	virtual void ResetPeakVoiceCount() = 0;
	virtual void EnableMixSnapshot() = 0;
	virtual void DisableMixSnapshot() = 0;
	virtual int GetDeviceState() = 0;
	virtual AudioObjectRef_t GetObjectHandle( void *pObject ) = 0;
	virtual bool ProcessSubmixCommands( void *pCommands ) = 0;
	virtual void ClearSubmixCommands() = 0;
	// Voice recording for voice chat
	virtual void SetVoiceRecordingEnabled( bool bEnabled ) = 0;
	// The engine passes a float buffer of 1024 samples
	virtual int ReadVoiceRecordData( float *pBuffer, int nCount ) = 0;
	virtual int GetVoiceRecordSampleRate() = 0;
	virtual AudioObjectRef_t CreateAudioSource( CSfxTable *pSfx, void *pParams ) = 0;
	virtual AudioObjectRef_t CreateAudioCacheHandle( CSfxTable *pSfx ) = 0;
	virtual CAudioDecoderStretch *CreateStretchDecoder( CAudioSource *pSource, float flStretch ) = 0;
	virtual int GetSfxSampleRate( CSfxTable *pSfx ) = 0;
	virtual const void *GetSfxData( CSfxTable *pSfx, CBufferString *pName ) = 0;
	virtual void *GetVoiceStreamMutex() = 0;
	virtual VoiceContainerParamTableRef_t CreateVoiceContainerParamTable() = 0;
	virtual void PreloadSoundEventVSnds( CSoundEvent *pSoundEvent ) = 0;
	virtual void ReleaseSfxPreload( CSfxTable *pSfx ) = 0;
	virtual AudioObjectRef_t FindVSndResource( const char *pVSndName, void *pContext ) = 0;
	// Runs pFunction (a std::function< void() >) on the audio thread
	virtual void QueueAudioThreadFunction( void *pFunction ) = 0;
};



#endif // ISOUNDSYSTEM_H
