//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: A game system that closes its log file at shutdown.
//
//=============================================================================//

#ifndef CHANGELOGGER_H
#define CHANGELOGGER_H
#ifdef _WIN32
#pragma once
#endif

#include "igamesystem.h"
#include "filesystem.h"

//-----------------------------------------------------------------------------
// Registered as "ChangeLogger" (static factory). 40 bytes on both platforms. Only its Shutdown
// touches the fields (nothing in the game writes the log);
// the names are named by what the code does with them.
//-----------------------------------------------------------------------------
class ChangeLogger : public CAutoGameSystem
{
public:
	DECLARE_GAME_SYSTEM();

	virtual ~ChangeLogger();

	// Closes m_hLogFile (IFileSystem::Close) when it is open and clears m_bLogging
	virtual void Shutdown() override;

	// Forward to the game system factory
	virtual void SetGameSystemGlobalPtrs( void *pValue ) override;
	virtual bool DoesGameSystemReallocate() override;

public:
	FileHandle_t m_hLogFile;
	// Initialised to -1 (the static instance), read nowhere
	int m_nLogIndex;
	// Cleared by Shutdown
	bool m_bLogging;
};

#endif // CHANGELOGGER_H
