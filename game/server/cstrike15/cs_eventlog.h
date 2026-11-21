//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The Counter-Strike event log. Factory "CCSEventLog".
//
//=============================================================================//

#ifndef CSTRIKE15_CS_EVENTLOG_H
#define CSTRIKE15_CS_EVENTLOG_H
#ifdef _WIN32
#pragma once
#endif

#include "../eventlog.h"

//-----------------------------------------------------------------------------
// Reallocating factory: 48 bytes on Windows, 40 on Linux, where the member
// DECLARE_GAME_SYSTEM() adds takes byte 33, in the tail padding of CEventLog.
//-----------------------------------------------------------------------------
class CCSEventLog : public CEventLog
{
public:
	DECLARE_GAME_SYSTEM();

	virtual ~CCSEventLog();

	// Forward to the game system factory; SetGameSystemGlobalPtrs also sets the global the
	// Counter-Strike code reads the event log from
	virtual bool DoesGameSystemReallocate() override;
	virtual void SetGameSystemGlobalPtrs( void *pValue ) override;

	// CEventLog::GameInit plus the Counter-Strike events (round, bomb, hostage, ...)
	GS_EVENT( GameInit );

protected:
	// Prints the Counter-Strike events it knows, else CEventLog::PrintEvent
	virtual bool PrintEvent( IGameEvent *event ) override;
};

#endif // CSTRIKE15_CS_EVENTLOG_H
