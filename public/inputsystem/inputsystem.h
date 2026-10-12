#ifndef INPUTSYSTEM_H
#define INPUTSYSTEM_H

#ifdef _WIN32
#pragma once
#endif

#include "inputsystem/iinputsystem.h"
#include "tier0/utlstring.h"
#include "tier2/tier2.h"

class CInputSystem : public CTier2AppSystem< IInputSystem >
{
public:
	virtual ~CInputSystem() {}

	uint8 m_pad028[ 0x8 ];

	// Windows attached through AttachToWindow, 0x10 bytes each
	uint8 m_Unk030[ 0x18 ];

	uint8 m_pad048[ 0x5 ];
	bool m_bUnk04D;
	bool m_bUnk04E;
	bool m_bUnk04F;
	uint8 m_pad050[ 0x4 ];
	bool m_bRelativeMouseMode;
	uint8 m_pad055[ 0x1B ];

	// Flags changed by unk075
	uint32 m_nUnk070;

	uint8 m_pad074[ 0x25D4 ];
	PlatWindow_t m_hCursorClipWindow;
	uint8 m_pad2650[ 0x228 ];

	// SDL_WINDOWS_CLOSE_ON_ALT_F4 hint value saved by unk075
	CUtlString m_Unk2878;
	bool m_bUnk2880;
	uint8 m_pad2881[ 0x2F ];

#ifdef _WIN32
	CUtlVector< InputWindowMessageHookFn_t > m_WindowMessageHooks;
#endif
	CUtlVector< InputSDLEventHookFn_t > m_SDLEventHooks;
};

#endif // INPUTSYSTEM_H
