//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The game system that keeps the background nav space builder out of the game's
//          own work.
//
//=============================================================================//

#ifndef NAVSPACEBUILDLOCKER_H
#define NAVSPACEBUILDLOCKER_H
#ifdef _WIN32
#pragma once
#endif

#include "igamesystem.h"

//-----------------------------------------------------------------------------
// Registered as "NavSpaceBuilderUpdater" (static factory, one global object). 24 bytes on both
// platforms: only the DECLARE_GAME_SYSTEM byte. No source has the class; the handlers pause and
// resume a global CThreadBackgroundWorkGate of the nav space builder.
//-----------------------------------------------------------------------------
class CNavSpaceBuildLocker : public IGameSystem
{
public:
	DECLARE_GAME_SYSTEM();

	// Pauses the builder's work gate unless a pause is already requested
	GS_EVENT( GameInit );
	// Empty
	GS_EVENT( ServerPreEntityThink );
	// Resumes the builder's work while the asynchronous post-tick work runs (when a global
	// flag of the builder allows it)
	GS_EVENT( ServerGameBeginAsyncPostTickWork );
	// Same body as GameInit: pauses the builder again
	GS_EVENT( ServerPreEndAsyncPostTickWork );

	// Forward to the game system factory
	virtual void SetGameSystemGlobalPtrs( void *pValue ) override;
	virtual bool DoesGameSystemReallocate() override;

	virtual ~CNavSpaceBuildLocker();
};

#endif // NAVSPACEBUILDLOCKER_H
