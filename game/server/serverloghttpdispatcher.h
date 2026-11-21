//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Sends the server log to the HTTP listeners of logaddress_add_http(_delayed).
//
//=============================================================================//

#ifndef SERVERLOGHTTPDISPATCHER_H
#define SERVERLOGHTTPDISPATCHER_H
#ifdef _WIN32
#pragma once
#endif

#include "igamesystem.h"

#include <tier0/utlstring.h>
#include <tier0/utlvector.h>
#include <tier0/utllinkedlist.h>

// A listener added by logaddress_add_http(_delayed): 112 bytes (its URI, the Steam HTTP request in
// flight with its CCallResult, the server tokens sent as X-Server-Unique-Token and
// X-Server-Instance-Token). Named by the message "Server log http listener registered".
class CServerLogHTTPListener;

//-----------------------------------------------------------------------------
// Registered as "ServerLogHttpDispatcher" (static factory). 120 bytes on both platforms. No source
// has the class; the names are named by what the code does.
//-----------------------------------------------------------------------------
class CServerLogHTTPDispatcher : public CAutoGameSystem
{
public:
	DECLARE_GAME_SYSTEM();

	// One per tick that added log text: the length of both log buffers at that tick, from which
	// the delayed listeners find the bytes they may send
	struct LogTickRecord_t
	{
		int m_nTick;
		size_t m_nDelayedLogLength;
		size_t m_nLogLength;
	};

	virtual ~CServerLogHTTPDispatcher();

	// Stops the requests in flight and deletes every listener
	virtual void Shutdown() override;
	// Store the time of activation (m_nActivateTime, m_flActivateFloatTime)
	GS_EVENT( GameActivate );
	// Empties the log buffers and the tick records
	GS_EVENT( GameDeactivate );
	// Records the tick and sends the due log text to the listeners
	GS_EVENT( GameFrameBoundary );

	// Forward to the game system factory
	virtual void SetGameSystemGlobalPtrs( void *pValue ) override;
	virtual bool DoesGameSystemReallocate() override;

public:
	CUtlVector< CServerLogHTTPListener * > m_Listeners;
	CUtlLinkedList< LogTickRecord_t, int > m_TickRecords;
	CUtlString m_LogText;
	CUtlString m_DelayedLogText;
	// Plat_GetTime and Plat_FloatTime at GameActivate; the X-Timestamp header is based on them
	int64 m_nActivateTime;
	double m_flActivateFloatTime;
	// Freed by the destructor; nothing else reads or writes it
	CUtlString m_UnusedString;
};

#endif // SERVERLOGHTTPDISPATCHER_H
