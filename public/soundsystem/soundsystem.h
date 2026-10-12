#ifndef SOUNDSYSTEM_H
#define SOUNDSYSTEM_H

#ifdef _WIN32
#pragma once
#endif

#include "soundsystem/isoundsystem.h"
#include "tier2/tier2.h"
#include "vconcomm/ivconcomm.h"

class CSoundSystem : public CTier2AppSystem< ISoundSystem >, public IVConCommDataReceived
{
public:
	uint32 m_nUnk030;
	uint32 m_nUnk034;
	uint32 m_nUnk038;
	uint32 m_nUnk03C;
	uint8 m_pad040[ 0x18 ];
	uint8 m_pad058[ 0x118 ];
	uint8 m_pad170[ 0xE0 ];
	uint8 m_pad250[ 0x40 ];
	double m_flUnk290;
	uint8 m_pad298[ 0x10 ];
	float m_flUnk2A8;
	int m_nUnk2AC;
	uint8 m_pad2B0[ 0x18 ];
	CThreadFastMutex m_UnkMutex2C8;
	uint8 m_pad2D4[ 0x5C ];
};

#endif // SOUNDSYSTEM_H
