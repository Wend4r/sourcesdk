//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The game system that empties the global pawn list on shutdown
//			(CPawnListGameSystem, factory "CPawnListGameSystem"). No Valve path
//			names this header.
//
//=============================================================================//

#ifndef PAWNLIST_GAMESYSTEM_H
#define PAWNLIST_GAMESYSTEM_H
#ifdef _WIN32
#pragma once
#endif

#include "igamesystem.h"

//-----------------------------------------------------------------------------
// Static instance, 24 bytes on both platforms. The pawn list itself is a
// separate global (a CUtlLinkedList with unsigned short indices).
//-----------------------------------------------------------------------------
class CPawnListGameSystem : public CAutoGameSystem
{
public:
	DECLARE_GAME_SYSTEM();

	// Removes every element of the global pawn list and frees its memory
	GS_EVENT( GameShutdown );

	// Forward to the game system factory
	virtual void SetGameSystemGlobalPtrs( void *pValue ) override;
	virtual bool DoesGameSystemReallocate() override;
};

#endif // PAWNLIST_GAMESYSTEM_H
