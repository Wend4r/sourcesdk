//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The item generation game system (CItemGeneration, factory
//			"CItemGeneration"). No Valve path names this header.
//
//=============================================================================//

#ifndef ITEM_GENERATION_H
#define ITEM_GENERATION_H
#ifdef _WIN32
#pragma once
#endif

#include "igamesystem.h"

//-----------------------------------------------------------------------------
// The server's class handles no game system event; it only registers itself.
// Static instance, 24 bytes on both platforms.
//-----------------------------------------------------------------------------
class CItemGeneration : public CAutoGameSystem
{
public:
	DECLARE_GAME_SYSTEM();

	// Forward to the game system factory
	virtual void SetGameSystemGlobalPtrs( void *pValue ) override;
	virtual bool DoesGameSystemReallocate() override;
};

#endif // ITEM_GENERATION_H
