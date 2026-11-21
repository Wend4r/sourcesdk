//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The sound envelope controller (looping sounds with volume and pitch envelopes) and the
// game system that implements it.
//
//=============================================================================//

#ifndef SOUNDENVELOPE_H
#define SOUNDENVELOPE_H
#ifdef _WIN32
#pragma once
#endif

#include "igamesystem.h"
#include "playerslot.h"

#include <tier0/utlvector.h>
#include <tier0/utlpriorityqueue.h>
#include <tier0/utlsymbollarge.h>

class CSoundPatch;
class CEntityIndex;
class IRecipientFilter;
class Vector;

enum soundcommands_t
{
	SOUNDCTRL_CHANGE_VOLUME,
	SOUNDCTRL_CHANGE_PITCH,
	SOUNDCTRL_STOP,
	SOUNDCTRL_DESTROY,
};

struct envelopePoint_t
{
	float amplitudeMin, amplitudeMax;
	float durationMin, durationMax;
};

struct envelopeDescription_t
{
	envelopePoint_t *pPoints;
	int nNumPoints;
};

//-----------------------------------------------------------------------------
// No virtual destructor. There are no SoundCreate overloads that take a channel and an
// attenuation or sound level; the order is the Linux one (MSVC groups the overloads at the
// first one, in reverse order).
//-----------------------------------------------------------------------------
abstract_class CSoundEnvelopeController
{
public:
	virtual void SystemReset() = 0;
	virtual void SystemUpdate() = 0;
	virtual void Play( CSoundPatch *pSound, float volume, float pitch, float flStartTime = 0 ) = 0;
	virtual void CommandAdd( CSoundPatch *pSound, float executeDeltaTime, soundcommands_t command, float commandTime, float value ) = 0;
	virtual void CommandClear( CSoundPatch *pSound ) = 0;
	virtual void ShutdownSoundPatch( CSoundPatch *pSound ) = 0;
	virtual CSoundPatch *SoundCreate( IRecipientFilter &filter, CEntityIndex nEntIndex, const char *pSoundName ) = 0;
	// The body is the same as the overload above; the parameter type of the name is not recoverable
	virtual CSoundPatch *SoundCreate( IRecipientFilter &filter, CEntityIndex nEntIndex, CUtlSymbolLarge iszSoundName ) = 0;
	virtual CSoundPatch *SoundCreate( IRecipientFilter &filter, CEntityIndex nEntIndex, const char *pSoundName, const Vector *pSoundOrigin ) = 0;
	virtual void SoundDestroy( CSoundPatch *pSound ) = 0;
	virtual void SoundChangePitch( CSoundPatch *pSound, float pitchTarget, float deltaTime ) = 0;
	virtual void SoundChangeVolume( CSoundPatch *pSound, float volumeTarget, float deltaTime ) = 0;
	virtual void SoundFadeOut( CSoundPatch *pSound, float deltaTime, bool destroyOnFadeout = false ) = 0;
	virtual float SoundGetPitch( CSoundPatch *pSound ) = 0;
	virtual float SoundGetVolume( CSoundPatch *pSound ) = 0;
	virtual float SoundPlayEnvelope( CSoundPatch *pSound, soundcommands_t soundCommand, envelopePoint_t *points, int numPoints ) = 0;
	virtual float SoundPlayEnvelope( CSoundPatch *pSound, soundcommands_t soundCommand, envelopeDescription_t *envelope ) = 0;
	virtual void CheckLoopingSoundsForPlayer( CPlayerSlot nPlayerSlot ) = 0;
	virtual CUtlSymbolLarge SoundGetName( CSoundPatch *pSound ) = 0;
	virtual CUtlSymbolLarge SoundGetScriptName( CSoundPatch *pSound ) = 0;
	virtual void SoundSetCloseCaptionDuration( CSoundPatch *pSound, float flDuration ) = 0;
	virtual void SoundSetOrigin( CSoundPatch *pSound, const Vector *pOrigin ) = 0;
};

// A queued command of the controller (32 bytes, allocated by CommandAdd)
struct SoundCommand_t
{
	CSoundPatch *m_pPatch;
	float m_time;
	float m_deltaTime;
	soundcommands_t m_command;
	float m_value;
	SoundCommand_t *m_pNext;
};

//-----------------------------------------------------------------------------
// Registered as "SoundControllerImp" (static factory). 88 bytes on both platforms.
// The declaration order of the IGameSystem overrides and of the destructor is the order of the
// entries libserver.so adds to the CSoundEnvelopeController table.
//-----------------------------------------------------------------------------
class CSoundControllerImp : public CSoundEnvelopeController, public CAutoGameSystem
{
public:
	DECLARE_GAME_SYSTEM();

	// Forward to the game system factory
	virtual bool DoesGameSystemReallocate() override;
	virtual void SetGameSystemGlobalPtrs( void *pValue ) override;

	GS_EVENT( RestoreGame );
	// Calls SystemUpdate
	GS_EVENT( ServerPreClientUpdate );
	// Calls SystemReset
	GS_EVENT( GameDeactivate );

	virtual ~CSoundControllerImp();

	// CSoundEnvelopeController
	virtual void SystemReset() override;
	virtual void SystemUpdate() override;
	virtual void Play( CSoundPatch *pSound, float volume, float pitch, float flStartTime = 0 ) override;
	virtual void CommandAdd( CSoundPatch *pSound, float executeDeltaTime, soundcommands_t command, float commandTime, float value ) override;
	virtual void CommandClear( CSoundPatch *pSound ) override;
	virtual void ShutdownSoundPatch( CSoundPatch *pSound ) override;
	virtual CSoundPatch *SoundCreate( IRecipientFilter &filter, CEntityIndex nEntIndex, const char *pSoundName ) override;
	virtual CSoundPatch *SoundCreate( IRecipientFilter &filter, CEntityIndex nEntIndex, CUtlSymbolLarge iszSoundName ) override;
	virtual CSoundPatch *SoundCreate( IRecipientFilter &filter, CEntityIndex nEntIndex, const char *pSoundName, const Vector *pSoundOrigin ) override;
	virtual void SoundDestroy( CSoundPatch *pSound ) override;
	virtual void SoundChangePitch( CSoundPatch *pSound, float pitchTarget, float deltaTime ) override;
	virtual void SoundChangeVolume( CSoundPatch *pSound, float volumeTarget, float deltaTime ) override;
	virtual void SoundFadeOut( CSoundPatch *pSound, float deltaTime, bool destroyOnFadeout = false ) override;
	virtual float SoundGetPitch( CSoundPatch *pSound ) override;
	virtual float SoundGetVolume( CSoundPatch *pSound ) override;
	virtual float SoundPlayEnvelope( CSoundPatch *pSound, soundcommands_t soundCommand, envelopePoint_t *points, int numPoints ) override;
	virtual float SoundPlayEnvelope( CSoundPatch *pSound, soundcommands_t soundCommand, envelopeDescription_t *envelope ) override;
	virtual void CheckLoopingSoundsForPlayer( CPlayerSlot nPlayerSlot ) override;
	virtual CUtlSymbolLarge SoundGetName( CSoundPatch *pSound ) override;
	virtual CUtlSymbolLarge SoundGetScriptName( CSoundPatch *pSound ) override;
	virtual void SoundSetCloseCaptionDuration( CSoundPatch *pSound, float flDuration ) override;
	virtual void SoundSetOrigin( CSoundPatch *pSound, const Vector *pOrigin ) override;

public:
	CUtlVector< CSoundPatch * > m_soundList;
	CUtlPriorityQueue< SoundCommand_t * > m_commandList;
	float m_flLastTime;
};

#endif // SOUNDENVELOPE_H
