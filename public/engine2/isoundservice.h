//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: Engine service for voice chat
//
//=============================================================================//

#ifndef ISOUNDSERVICE_H
#define ISOUNDSERVICE_H
#ifdef _WIN32
#pragma once
#endif

#include <engine/IEngineService.h>
#include "engine2/iengineservicemgr.h"
#include "tier0/circularbuffer.h"
#include "tier0/threadtools.h"
#include "tier0/utlstring.h"
#include "tier1/convar.h"
#include "tier1/utlvector.h"

class CAudioState;
class IAudioInputStream;
class IVoiceTarget;
struct VoiceRecordingLevel_t;

abstract_class ISoundService : public IEngineService
{
public:
	virtual bool Unk_PlayNetworkedVoice( void *p ) = 0;

	virtual bool GetNetworkedVoiceLevel( uint64 nXUID, VoiceRecordingLevel_t *pLevel ) = 0;

	virtual void AddVoiceListener( IVoiceTarget *pListener ) = 0;
	virtual void RemoveVoiceListener( IVoiceTarget *pListener ) = 0;

	virtual bool IsVoiceRecording() = 0;

	virtual void unk028( const CAudioState *pAudioState ) = 0;
	virtual void unk029() = 0;

	virtual void SetVoiceThreshold( float flThresholdDB ) = 0;
	virtual void SetVoiceThresholdDelay( float flTime ) = 0;
	virtual void SetVoiceThresholdAttack( float flTime ) = 0;
	virtual void SetVoiceThresholdHold( float flTime ) = 0;
	virtual float GetVoiceCurrentPeak() = 0;
	virtual bool IsVoiceActive() = 0;

	virtual void StopVoiceRecording() = 0;
};

class CSoundService : public CBaseEngineService< ISoundService >
{
public:
	CConCommandMemberAccessor< CSoundService > m_PlaySoundCommand;
	bool m_bUnk078;
	uint8 m_pad079[ 0x7 ];
	uint8 m_pad080[ 0x118 ];
	CThreadFastMutex m_UnkMutex198;
	CUtlVector< IVoiceTarget * > m_Unk1A8;
	uint8 m_pad1C0[ 0x8 ];
	int m_nUnk1C8;
	IAudioInputStream *m_pUnk1D0;
	CCircularBuffer m_Unk1D8;
	CCircularBuffer m_Unk1F0;
	uint8 m_pad208[ 0x68 ];
	float m_flUnk270;
	uint8 m_pad274[ 0xC ];
	CUtlString m_Unk280;
	uint8 m_pad288[ 0x58 ];
};

#endif // ISOUNDSERVICE_H
