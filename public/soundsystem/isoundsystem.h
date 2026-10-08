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


//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class IAudioDevice;
class CAudioSource;
class CAudioMixer;


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
//-----------------------------------------------------------------------------
#define SOUNDSYSTEM_INTERFACE_VERSION "SoundSystem001"

abstract_class ISoundSystem : public IAppSystem
{
public:
	virtual void unk011() = 0;
	virtual void unk012() = 0;
	virtual void unk013() = 0;
	virtual void unk014() = 0;
	virtual void unk015() = 0;
	virtual void unk016() = 0;
	virtual void unk017() = 0;
	virtual void unk018() = 0;
	virtual void unk019() = 0;
	virtual void Unk_SoundInit( void *p ) = 0;
	virtual bool IsSoundEnabled() = 0;
	virtual void Unk_Update( void *p ) = 0;
	virtual void StopAllSounds() = 0;
	virtual void unk024() = 0;
	// Starts a voice from sound parameters and returns its guid
	virtual void Unk_StartSound( void *p ) = 0;
	virtual void unk026() = 0;
	virtual void unk027() = 0;
	virtual void unk028() = 0;
	virtual void unk029() = 0;
	virtual bool IsSoundPlaying( uint32 nSoundGuid ) = 0;
	virtual void unk032() = 0;
	virtual void unk033() = 0;
	virtual void unk034() = 0;
	virtual void unk035() = 0;
	virtual void unk036() = 0;
	virtual void unk037() = 0;
	virtual void unk038() = 0;
	virtual void unk039() = 0;
	virtual void unk040() = 0;
	// Returns -1 for an unknown mix layer
	virtual int GetMixLayerIndex( const char *pLayerName ) = 0;
	virtual void SetMixLayerLevel( int nLayer, float flLevel ) = 0;
	virtual void unk043( float flValue ) = 0;
	virtual float unk044() = 0;
	virtual void unk045() = 0;
	virtual void unk046() = 0;
	virtual void unk047() = 0;
	virtual void unk048() = 0;
	virtual void unk049() = 0;
	virtual void unk050() = 0;
	virtual void unk051() = 0;
	virtual void unk052() = 0;
	virtual void unk053() = 0;
	virtual void unk054() = 0;
	virtual void unk055() = 0;
	virtual void unk056() = 0;
	virtual void unk057() = 0;
	virtual void unk058() = 0;
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
	virtual void unk084() = 0;
	virtual void unk085() = 0;
	virtual void unk086() = 0;
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
	virtual void unk095() = 0;
	virtual void unk096() = 0;
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
	virtual void Unk_PrintSoundState( void *p ) = 0;
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
	virtual void unk128() = 0;
	virtual void unk129() = 0;
	virtual void unk130( void *p ) = 0;
	virtual void unk130( const void *p ) = 0;
	virtual void unk132() = 0;
	virtual void unk133() = 0;
};



#endif // ISOUNDSYSTEM_H
