#ifndef SOUNDSERVICE_H
#define SOUNDSERVICE_H

#ifdef _WIN32
#pragma once
#endif

#include "engine2/isoundservice.h"
#include "engine2/iengineservicemgr.h"
#include "tier0/circularbuffer.h"
#include "tier0/threadtools.h"
#include "tier0/utlstring.h"
#include "tier1/convar.h"
#include "tier1/utlvector.h"

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

#endif // SOUNDSERVICE_H
