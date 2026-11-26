//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The voice rules of the game.
//
//=============================================================================//

#ifndef VOICE_GAMEMGR_H
#define VOICE_GAMEMGR_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"

class CBasePlayerController;

//-----------------------------------------------------------------------------
// The game rules of voice: who hears whom (CVoiceGameMgrHelper).
//-----------------------------------------------------------------------------
abstract_class IVoiceGameMgrHelper
{
public:
	virtual ~IVoiceGameMgrHelper() {}
	virtual bool CanPlayerHearPlayer( CBasePlayerController *pListener, CBasePlayerController *pTalker, bool &bProximity ) = 0;

	// Empty, no override in the server.
	virtual void Stub_3() = 0;

	// Empty, no override in the server.
	virtual void Stub_4() = 0;

	// Empty, no override in the server.
	virtual void Stub_5() = 0;
};

//-----------------------------------------------------------------------------
// The voice manager of the game (one global instance, 32 bytes): tells each client whom it hears (ban and
// listen masks) and handles the voice client commands ("vban"). The destructor is its only virtual
// method.
//-----------------------------------------------------------------------------
abstract_class CVoiceGameMgr
{
public:
	virtual ~CVoiceGameMgr() = 0;

public:
	// Set by Init from the game rules constructor
	IVoiceGameMgrHelper *m_pHelper;
	// Seconds accumulated since the last update of the masks; the update runs and resets it at 0.3
	double m_UpdateInterval;
	// -1 from the static initialisation; nothing else reads or writes it
	int m_iProximityDistance;
};

#endif // VOICE_GAMEMGR_H
