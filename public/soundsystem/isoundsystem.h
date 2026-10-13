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
#include "tier0/platwindow.h"
#include "tier0/utlstringtoken.h"


//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class IAudioDevice;
class IAudioDevice2;
class IAudioInputStream;
class IAudioOutputStream;
class CAudioSource;
class CAudioMixer;
class CAudioState;
class CSfxTable;
class ISoundSystemServices;
struct StartSoundParams_t;
class CVSndParameterTable;
class CUtlBuffer;


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
// Flags for FindAudioSource
//-----------------------------------------------------------------------------
enum FindAudioSourceFlags_t
{
	FINDAUDIOSOURCE_NODELAY = 0x1,
	FINDAUDIOSOURCE_PREFETCH = 0x2,
	FINDAUDIOSOURCE_PLAYONCE = 0x4,
};


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define SOUNDSYSTEM_INTERFACE_VERSION "SoundSystem001"

abstract_class ISoundSystem : public IAppSystem
{
public:
	virtual IAudioDevice2 *CreateDefaultDevice() = 0;
	virtual void DestroyDevice( IAudioDevice2 *pDevice ) = 0;
	virtual void unk013() = 0;
	virtual IAudioDevice2 *CreateNullDevice() = 0;
	virtual IAudioInputStream *unk015( const void *pParams ) = 0;
	virtual void unk016( IAudioInputStream *pInputStream ) = 0;
	virtual IAudioOutputStream *CreateDirectMixOutputStream( uint32 nSampleRate, uint32 nChannels, uint32 nBits ) = 0;
	virtual void unk018() = 0;
	virtual void DestroyOutputStream( IAudioOutputStream *pOutputStream ) = 0;
	virtual void InitSoundSystem( PlatWindow_t hWindow, ISoundSystemServices *pSoundSystemServices ) = 0;
	virtual bool IsSoundEnabled() = 0;
	virtual void Update( const CAudioState *pAudioState, bool bAllowAsync ) = 0;
	virtual void StopAllSounds() = 0;
	virtual void unk024() = 0;
	// Starts a voice from sound parameters and returns its guid
	virtual SoundGuid_t StartSound( StartSoundParams_t &params ) = 0;
	virtual void StopSound( SoundGuid_t guid, bool bForceSync ) = 0;
	virtual void unk027() = 0;
	virtual void unk028() = 0;
	virtual float unk029( SoundGuid_t guid ) = 0;
	virtual bool IsSoundPlaying( SoundGuid_t guid ) = 0;
	virtual void unk032( SoundGuid_t guid, float flValue ) = 0;
	virtual void unk033() = 0;
	virtual CSfxTable *PrecacheSound( const char *pSample ) = 0;
	virtual void PreloadSoundEvent( CSoundEvent *pSoundEvent, uint16 nUnk ) = 0;
	virtual void PreloadSoundEvent( const char *pSoundEventName, uint16 nUnk ) = 0;
	virtual void PreloadSound( CSfxTable *pSfx, uint16 nUnk ) = 0;
	virtual void PreloadSound( const char *pSoundName, uint16 nUnk ) = 0;
	virtual void unk039() = 0;
	virtual void SetMixGroupOfCurrentMixer( const char *pszGroupName, const char *pszParam, float flValue ) = 0;
	// Returns -1 for an unknown mix layer
	virtual int GetMixLayerIndex( const char *pLayerName ) = 0;
	virtual void SetMixLayerLevel( int nLayer, float flLevel ) = 0;
	virtual void unk043( float flValue ) = 0;
	virtual float unk044() = 0;
	virtual void unk045() = 0;
	virtual void unk046() = 0;
	virtual void unk047() = 0;
	virtual void unk048() = 0;
	virtual void ResetMixControlInput( CUtlStringToken nName ) = 0;
	virtual void unk050( CUtlStringToken nName ) = 0;
	virtual void unk051( CUtlStringToken nName ) = 0;
	virtual void unk052( CUtlStringToken nName ) = 0;
	virtual void unk053( CUtlStringToken nName ) = 0;
	virtual void SetMixControlInput( CUtlStringToken nName, float flValue, bool bUnk ) = 0;
	virtual void unk055( CUtlStringToken nName ) = 0;
	virtual void SetMixNameInput( CUtlStringToken nName, uint32 nValue ) = 0;
	virtual uint32 unk057( CUtlStringToken nName ) = 0;
	virtual float GetMixControlInputValue( CUtlStringToken nName ) = 0;
	virtual void unk059() = 0;
	virtual void unk060() = 0;
	virtual void unk061() = 0;
	virtual void unk062() = 0;
	virtual void unk063() = 0;
	virtual void unk064() = 0;
	virtual void unk065() = 0;
	virtual void unk066() = 0;
	virtual void unk067() = 0;
	virtual void unk068() = 0;
	virtual void unk069() = 0;
	virtual void unk070() = 0;
	virtual void unk071() = 0;
	virtual void unk072() = 0;
	virtual void unk073() = 0;
	virtual void unk074() = 0;
	virtual void unk075() = 0;
	virtual void unk076() = 0;
	virtual void unk077() = 0;
	virtual void unk078() = 0;
	virtual void unk079() = 0;
	virtual void unk080() = 0;
	virtual void unk081() = 0;
	virtual void unk082() = 0;
	virtual void unk083() = 0;
	virtual void FadeOutAllSounds( float flFadeTime, sound_voice_layer_t nVoiceLayer, sound_fade_type_t nFadeType ) = 0;
	virtual void unk085() = 0;
	virtual CAudioState *unk086() = 0;
	virtual bool IsSoundPlaying( SoundEventGuid_t guid ) = 0;
	virtual void unk087() = 0;
	virtual void unk088() = 0;
	virtual void unk089() = 0;
	// Plays a wave file through the operating system
	virtual void PlaySystemSound( const char *pWavFile ) = 0;
	virtual void unk091( bool bUnk1, bool bUnk2 ) = 0;
	virtual void unk092() = 0;
	virtual void unk093() = 0;
	virtual void unk094() = 0;
	virtual bool DisableAudioMuting( float flDisableDurationSeconds ) = 0;
	virtual void ResetMainGraphInputs() = 0;
	virtual void unk097() = 0;
	virtual void unk098() = 0;
	virtual void unk099() = 0;
	virtual void unk100() = 0;
	virtual void unk101() = 0;
	virtual void unk102() = 0;
	virtual void unk103() = 0;
	virtual void unk104() = 0;
	virtual void unk105() = 0;
	virtual void unk106() = 0;
	virtual void unk107() = 0;
	virtual void unk108() = 0;
	virtual void unk109() = 0;
	virtual void unk110() = 0;
	virtual void unk111() = 0;
	virtual void unk112() = 0;
	// Writes the state of the active voices and sound events as text
	virtual void OutputDebugInfo( CUtlBuffer &buf ) = 0;
	virtual void unk114() = 0;
	virtual void unk115( bool bUnk ) = 0;
	virtual void unk116() = 0;
	virtual void unk117() = 0;
	virtual void unk118() = 0;
	virtual void unk119() = 0;
	virtual void unk120() = 0;
	virtual void unk121() = 0;
	virtual void unk122() = 0;
	virtual void unk123() = 0;
	virtual void unk124() = 0;
	virtual void unk125() = 0;
	virtual void unk126() = 0;
	virtual void unk127() = 0;
	virtual CVSndParameterTable *unk128() = 0;
	virtual void unk129() = 0;
	virtual void unk130( CSfxTable *pSfx ) = 0;
	virtual void unk130( CSoundEvent *pSoundEvent ) = 0;
	virtual void unk132() = 0;
	virtual void unk133() = 0;
};



#endif // ISOUNDSYSTEM_H
