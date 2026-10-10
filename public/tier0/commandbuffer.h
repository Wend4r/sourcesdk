//===== Copyright © 1996-2006, Valve Corporation, All rights reserved. ======//
//
// Purpose: 
//
// $Workfile:     $
// $Date:         $
// $NoKeywords: $
//===========================================================================//


#ifndef COMMANDBUFFER_H
#define COMMANDBUFFER_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "tier1/convar.h"
#include "tier0/utlstring.h"
#include "tier1/utlvector.h"

#include <cmath>


//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class CUtlBuffer;


//-----------------------------------------------------------------------------
// Invalid command handle
//-----------------------------------------------------------------------------
typedef intp CommandHandle_t;
enum
{
	COMMAND_BUFFER_INVALID_COMMAND_HANDLE = 0
};


//-----------------------------------------------------------------------------
// A command buffer class- a queue of argc/argv based commands associated
// with a particular time
//-----------------------------------------------------------------------------
class CCommandBuffer
{
public:
	// Constructor, destructor
	DLL_CLASS_IMPORT CCommandBuffer();
	DLL_CLASS_IMPORT ~CCommandBuffer();

	// Inserts text into the command buffer
	// AMNOTE: The last three are stored in the CCommand of each command added
	DLL_CLASS_IMPORT bool AddText( const char *pText, int nTickDelay = 0, int nMaxCommands = 0, bool unk3 = false, double flInputTime = NAN, uint64 unk5 = 0 );

	// Used to iterate over all commands appropriate for the current time
	DLL_CLASS_IMPORT void BeginProcessingCommands( int nDeltaTicks );
	DLL_CLASS_IMPORT bool DequeueNextCommand();
	DLL_CLASS_IMPORT int DequeueNextCommand( const char **&ppArgv );
	DLL_CLASS_IMPORT void EndProcessingCommands();

	// Are we in the middle of processing commands?
	DLL_CLASS_IMPORT bool IsProcessingCommands();

	// Delays all queued commands to execute at a later time
	DLL_CLASS_IMPORT void DelayAllQueuedCommands( int nTickDelay );

	// Indicates how long to delay when encountering a 'wait' command
	DLL_CLASS_IMPORT void SetWaitDelayTime( int nTickDelay );

	// Splits pText into individual commands, appending each to pOut.
	DLL_CLASS_IMPORT static void SplitCommands( const char *pText, int nMaxCommands, CUtlVector< CUtlString > *pOut );

	// Returns a handle to the next command to process
	// (useful when inserting commands into the buffer during processing
	// of commands to force immediate execution of those commands,
	// most relevantly, to implement a feature where you stream a file
	// worth of commands into the buffer, where the file size is too large
	// to entirely contain in the buffer).
	DLL_CLASS_IMPORT CommandHandle_t GetNextCommandHandle();

	// Specifies a max limit of the args buffer. For unittesting. Size == 0 means use default
	DLL_CLASS_IMPORT void LimitArgumentBufferSize( int nSize );

	// Sets the FCVAR flags that commands added from now on require, returns the previous flags.
	// AMNOTE: The engine refuses to run a queued concommand that lacks any of them
	// ("missing required FCVAR flag"), DequeueNextCommand itself doesn't filter on them
	DLL_CLASS_IMPORT uint64 SetRequiredFlags( uint64 nRequiredFlags );

	// Locks/unlocks the command buffer.
	DLL_CLASS_IMPORT void LockCommandBuffer( bool bLock );

	void SetWaitEnabled( bool bEnable )		{ m_bWaitEnabled = bEnable; }
	bool IsWaitEnabled() const				{ return m_bWaitEnabled; }

private:
	enum
	{
		ARGS_BUFFER_LENGTH = 32768,
	};

	char			m_ArgSBuffer[ ARGS_BUFFER_LENGTH ];	// 0x0000
	uint8			m_unk001[ 56 ];			// 0x8000
	uint64			m_nRequiredFlags;			// 0x8038
	CommandHandle_t	m_hNextCommand;				// 0x8040
	uint8			m_unk101[ 4 ];			// 0x8048
	int				m_nArgSBufferSize;			// 0x804C
	int				m_nCurrentTick;				// 0x8050
	int				m_nLastTickToProcess;		// 0x8054
	int			    m_nWaitDelayTicks;			// 0x8058
	int			    m_nMaxArgSBufferLength;	    // 0x805C
	bool			m_bIsProcessingCommands;	// 0x8060
	bool			m_bWaitEnabled;				// 0x8061
	bool			m_bIsLocked;				// 0x8062
	CCommand		m_CurrentCommand;			// 0x8068
	uint64			m_nCurrentCommandRequiredFlags;	// 0x86D0
};

#endif // COMMANDBUFFER_H
