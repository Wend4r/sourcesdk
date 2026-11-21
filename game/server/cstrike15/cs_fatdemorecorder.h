//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Records a "fat demo" (.fatdem): length-prefixed protobuf messages
//          written to a file while the convar that enables it is set.
//
//=============================================================================//

#ifndef CS_FATDEMORECORDER_H
#define CS_FATDEMORECORDER_H
#ifdef _WIN32
#pragma once
#endif

#include "igamesystem.h"

#include <tier0/utlbuffer.h>
#include <tier0/utlstring.h>

// CGameEventListener
#include <vscript/ccscript.h>

typedef void *FileHandle_t;

//-----------------------------------------------------------------------------
// Registered as "CCSFatDemoRecorder" (static factory). No source names its members; they are named
// by what the code does with them. It adds no virtual methods to IGameSystem. On Linux the (empty)
// FireGameEvent override also takes slot 66 of the primary table.
//-----------------------------------------------------------------------------
class CCSFatDemoRecorder : public CAutoGameSystem, public CGameEventListener
{
public:
	DECLARE_GAME_SYSTEM();

	CCSFatDemoRecorder();
	virtual ~CCSFatDemoRecorder();

	// Empty
	virtual void PostInit() override;
	// When the recording convar is set: opens "<name>.fatdem", writes the "GOML" header and the first
	// message, sets m_bRecording and m_nResetOnGameInit = -1
	GS_EVENT( GameInit );
	// Closes the file, clears the buffer, deletes m_pDeletedOnGameShutdown; exits the process when it
	// was recording and demo_quitafterplayback is set
	GS_EVENT( GameShutdown );

	// Forward to the game system factory
	virtual void SetGameSystemGlobalPtrs( void *pValue ) override;
	virtual bool DoesGameSystemReallocate() override;

	// Empty
	virtual void FireGameEvent( IGameEvent *event ) override;

public:
	// Freed by the destructor only
	CUtlString m_sFreedByDestructor;
	// GameInit sets it to -1 when it starts recording; read nowhere in the server
	int m_nResetOnGameInit;
	// Set by GameInit when the file was opened, cleared by GameShutdown
	bool m_bRecording;
	// An object with a virtual destructor that GameShutdown deletes; set nowhere in the server
	void *m_pDeletedOnGameShutdown;
	// The open .fatdem file
	FileHandle_t m_hFile;
	// Each message is serialised here (4-byte size + the message) and then written to m_hFile
	CUtlBuffer m_MessageBuffer;
};

#endif // CS_FATDEMORECORDER_H
