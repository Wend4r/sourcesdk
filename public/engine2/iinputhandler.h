//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: A handler of input events, installed with IInputService::InstallInputHandler
//
//=============================================================================//

#ifndef IINPUTHANDLER_H
#define IINPUTHANDLER_H
#ifdef _WIN32
#pragma once
#endif

#include <tier0/platform.h>
#include <splitscreenslot.h>

struct InputEvent_t;

//-----------------------------------------------------------------------------
// One method, no destructor in the table. Named by what it does (no source names it): gets every
// input event with the split screen slot it belongs to; returning true consumes the event
// (CDebugOverlayGameSystem).
//-----------------------------------------------------------------------------
abstract_class IInputHandler
{
public:
	virtual bool HandleInputEvent( const InputEvent_t &event, CSplitScreenSlot nSlot ) = 0;
};

#endif // IINPUTHANDLER_H
