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
#include "tier0/platwindow.h"
#include "tier0/utlmap.h"
#include "tier0/utlstring.h"
#include "tier0/utlstringtoken.h"
#include "tier0/utlvector.h"

#include <functional>
#include <memory>


//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
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
struct StartSoundParams_t;
class CVSndParameterTable;
class CUtlBuffer;
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
		m_nUnk00 = 0;
		m_flUnk08 = 0.0;
		m_nUnk10 = 0;
		m_nUnk18 = 0;
		m_bUnk20_0 = false;
		m_bUnk20_1 = false;
		m_bUnk20_2 = false;
		m_bUnk20_3 = false;
		m_bUnk20_4 = false;
		m_bUnk20_5 = false;

		for ( int i = 0; i < 4; i++ )
		{
			m_Unk24[ i ].m_vecUnk00.Init();
			m_Unk24[ i ].m_angUnk0C.Init();
			m_Unk24[ i ].m_vecUnk18.Init();
			m_Unk24[ i ].m_vecUnk24.Init();
			m_Unk24[ i ].m_nUnk30 = -1;
			m_Unk24[ i ].m_nUnk34 = -1;
			m_Unk24[ i ].m_bUnk38_0 = false;
			m_Unk24[ i ].m_bUnk38_1 = false;
		}
	}

	uint64 m_nUnk00;
	double m_flUnk08;
	uint64 m_nUnk10;
	uint64 m_nUnk18;
	bool m_bUnk20_0 : 1;
	bool m_bUnk20_1 : 1;
	bool m_bUnk20_2 : 1;
	bool m_bUnk20_3 : 1;
	bool m_bUnk20_4 : 1;
	bool m_bUnk20_5 : 1;

	struct
	{
		Vector m_vecUnk00;
		QAngle m_angUnk0C;
		Vector m_vecUnk18;
		Vector m_vecUnk24;
		int m_nUnk30;
		int m_nUnk34;
		bool m_bUnk38_0 : 1;
		bool m_bUnk38_1 : 1;
	} m_Unk24[ 4 ];
};


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define SOUNDSYSTEM_INTERFACE_VERSION "SoundSystem001"

abstract_class ISoundSystem : public IAppSystem
{
public:
	virtual IAudioDevice2 *CreateDefaultDevice() = 0;
	virtual void DestroyDevice( IAudioDevice2 *pDevice ) = 0;
	virtual void *unk013( const char *pName, int nUnk1, float flUnk, int nUnk2, int nUnk3, bool bUnk ) = 0;
	virtual IAudioDevice2 *CreateNullDevice() = 0;
	virtual IAudioInputStream *unk015( const void *pParams ) = 0;
	virtual void unk016( IAudioInputStream *pInputStream ) = 0;
	virtual IAudioOutputStream *CreateDirectMixOutputStream( uint32 nSampleRate, uint32 nChannels, uint32 nBits ) = 0;
	virtual IAudioOutputStream *unk018( SoundEventGuid_t guid, const char *pFieldName, uint32 nSampleRate, uint32 nChannels, uint32 nBits ) = 0;
	virtual void DestroyOutputStream( IAudioOutputStream *pOutputStream ) = 0;
	virtual void InitSoundSystem( PlatWindow_t hWindow, ISoundSystemServices *pSoundSystemServices ) = 0;
	virtual bool IsSoundEnabled() = 0;
	virtual void Update( const CAudioState *pAudioState, bool bAllowAsync ) = 0;
	virtual void StopAllSounds() = 0;
	virtual void unk024() = 0;
	// Starts a voice from sound parameters and returns its guid
	virtual SoundGuid_t StartSound( StartSoundParams_t &params ) = 0;
	virtual void StopSound( SoundGuid_t guid, bool bForceSync ) = 0;
	virtual uint32 unk027( double flTime, int nIndex ) = 0;
	virtual void unk028( int nIndex ) = 0;
	virtual float unk029( SoundGuid_t guid ) = 0;
	virtual bool IsSoundPlaying( SoundGuid_t guid ) = 0;
	virtual void unk032( SoundGuid_t guid, float flValue ) = 0;
	virtual bool unk033( SoundGuid_t guid ) = 0;
	virtual CSfxTable *PrecacheSound( const char *pSample ) = 0;
	virtual void PreloadSoundEvent( CSoundEvent *pSoundEvent, uint16 nUnk ) = 0;
	virtual void PreloadSoundEvent( const char *pSoundEventName, uint16 nUnk ) = 0;
	virtual void PreloadSound( CSfxTable *pSfx, uint16 nUnk ) = 0;
	virtual void PreloadSound( const char *pSoundName, uint16 nUnk ) = 0;
	virtual float unk039( int nUnk ) = 0;
	virtual void SetMixGroupOfCurrentMixer( const char *pszGroupName, const char *pszParam, float flValue ) = 0;
	// Returns -1 for an unknown mix layer
	virtual int GetMixLayerIndex( const char *pLayerName ) = 0;
	virtual void SetMixLayerLevel( int nLayer, float flLevel ) = 0;
	virtual void unk043( float flValue ) = 0;
	virtual float unk044() = 0;
	virtual void unk045( IAudioStreamRecorder *pRecorder, bool bAdd ) = 0;
	virtual IAudioWAVRecorder *unk046() = 0;
	virtual bool unk047() = 0;
	virtual IAudioWAVRecorder *unk048( int nUnk ) = 0;
	virtual void ResetMixControlInput( CUtlStringToken nName ) = 0;
	virtual void unk050( CUtlStringToken nName ) = 0;
	virtual void unk051( CUtlStringToken nName ) = 0;
	virtual void unk052( CUtlStringToken nName ) = 0;
	virtual void unk053( CUtlStringToken nName ) = 0;
	virtual void SetMixControlInput( CUtlStringToken nName, float flValue, bool bUnk ) = 0;
	virtual bool unk055( CUtlStringToken nName ) = 0;
	virtual void SetMixNameInput( CUtlStringToken nName, uint32 nValue ) = 0;
	virtual uint32 unk057( CUtlStringToken nName ) = 0;
	virtual float GetMixControlInputValue( CUtlStringToken nName ) = 0;
	virtual double unk059( uint32 nUnk ) = 0;
	virtual uint32 unk060( int nIndex ) = 0;
	virtual CUtlString unk061( int nIndex ) = 0;
	virtual int unk062() = 0;
	virtual void unk063( uint32 nUnk, void *pUnk, bool bUnk ) = 0;
	virtual void unk064( CUtlVector< CUtlString > &vecOut ) = 0;
	virtual SoundGuid_t GetGuidForLastSoundEmitted() = 0;
	virtual void unk066( void *pUnk ) = 0;
	virtual bool unk067( uint64 nXUID, uint32 nUnk, void *pUnk ) = 0;
	virtual void unk068( uint64 nXUID, uint32 nUnk1, uint32 nUnk2, const void *pData, int nSize, void *pUnk ) = 0;
	virtual void unk069( uint64 nXUID, int nUnk1, int nUnk2, void *pUnk1, int nUnk3, void *pUnk2, bool bUnk ) = 0;
	virtual bool unk070( uint64 nXUID ) = 0;
	virtual void unk071( uint64 nXUID, float flUnk ) = 0;
	virtual float unk072( uint64 nXUID ) = 0;
	virtual void unk073() = 0;
	virtual bool unk074( void *pUnk ) = 0;
	virtual void unk075() = 0;
	virtual void unk076() = 0;
	virtual void unk077() = 0;
	virtual void unk078() = 0;
	virtual void unk079() = 0;
	virtual void unk080() = 0;
	virtual bool unk081() = 0;
	virtual void unk082( int *pUnk1, int *pUnk2, bool bUnk ) = 0;
	virtual bool unk083( SoundGuid_t guid, float flUnk, bool bUnk ) = 0;
	virtual void FadeOutAllSounds( float flFadeTime, sound_voice_layer_t nVoiceLayer, sound_fade_type_t nFadeType ) = 0;
	virtual bool unk085( int nUnk, float flUnk, bool bUnk ) = 0;
	virtual CAudioState *unk086() = 0;
	virtual bool IsSoundPlaying( SoundEventGuid_t guid ) = 0;
	virtual void unk087( bool bUnk ) = 0;
	virtual bool unk088() = 0;
	virtual bool unk089() = 0;
	// Plays a wave file through the operating system
	virtual void PlaySystemSound( const char *pWavFile ) = 0;
	virtual void unk091( bool bUnk1, bool bUnk2 ) = 0;
	virtual CSfxTable *unk092( SoundGuid_t guid ) = 0;
	virtual void unk093( void *pUnk, const char *pszUnk ) = 0;
	virtual void unk094( void *pUnk ) = 0;
	virtual bool DisableAudioMuting( float flDisableDurationSeconds ) = 0;
	virtual void ResetMainGraphInputs() = 0;
	virtual void *unk097( void *pUnk1, void *pUnk2 ) = 0;
	virtual void unk098( void *pUnk, const char *pszUnk ) = 0;
	virtual CUtlString unk099( SoundGuid_t guid ) = 0;
	virtual void unk100( void *pUnk ) = 0;
	virtual void unk101() = 0;
	virtual void unk102() = 0;
	virtual void unk103( void *pUnk ) = 0;
	virtual SoundGuid_t unk104( int nUnk1, int nUnk2, void *pUnk ) = 0;
	virtual void unk105( SoundGuid_t guid, void *pUnk ) = 0;
	virtual void unk106( uint32 nUnk, void *pUnk ) = 0;
	virtual void unk107( uint32 nUnk, void *pUnk ) = 0;
	virtual void unk108( SoundGuid_t guid, uint16 nUnk ) = 0;
	virtual void unk109( void *pUnk1, void *pUnk2 ) = 0;
	virtual void unk110( void *pUnk ) = 0;
	virtual void *unk111( void *pUnk ) = 0;
	virtual void unk112() = 0;
	// Writes the state of the active voices and sound events as text
	virtual void OutputDebugInfo( CUtlBuffer &buf ) = 0;
	virtual float unk114( uint32 nUnk ) = 0;
	virtual void unk115( bool bUnk ) = 0;
	virtual void unk116() = 0;
	virtual void unk117() = 0;
	virtual void unk118() = 0;
	virtual void unk119() = 0;
	virtual std::unique_ptr< CAudioMixer > unk120( CAudioSource *pSource ) = 0;
	virtual int unk121( void *pUnk ) = 0;
	virtual void unk122( int nUnk ) = 0;
	virtual std::unique_ptr< CAudioSource > unk123( CSfxTable *pSfx, VSound_t *pSound ) = 0;
	virtual std::unique_ptr< IAudioCacheHandle > unk124( CSfxTable *pSfx ) = 0;
	virtual IAudioDecoder *unk125( void *pUnk, float flUnk ) = 0;
	virtual uint32 unk126( CSfxTable *pSfx ) = 0;
	virtual const char *unk127( CSfxTable *pSfx, CBufferString *pBuf ) = 0;
	virtual CVSndParameterTable *unk128() = 0;
	virtual std::shared_ptr< CUtlOrderedMap< CUtlStringToken, VoiceContainerParamMem_t > > unk129() = 0;
	virtual void unk130( CSfxTable *pSfx ) = 0;
	virtual void unk130( CSoundEvent *pSoundEvent ) = 0;
	virtual HSound unk132( const char *pName, void *pUnk ) = 0;
	virtual void unk133( std::function< void() > func ) = 0;
};



#endif // ISOUNDSYSTEM_H
