//============ Copyright (c) Valve Corporation, All rights reserved. ============
//
// Logging system declarations.
//
// The logging system is a channel-based output mechanism which allows
// subsystems to route their text/diagnostic output to various listeners
//
//===============================================================================

#ifndef LOGGINGSYSTEM_H
#define LOGGINGSYSTEM_H

#pragma once

#include "logging.h"
#include "icommandline.h"

//-----------------------------------------------------------------------------
// Interface for policy classes which determine how to behave when a 
// message is logged.
//
// Can return:
//   LR_CONTINUE (continue execution)
//   LR_DEBUGGER (break into debugger if one is present, otherwise continue)
//   LR_ABORT (terminate process immediately with a failure code of 1)
//-----------------------------------------------------------------------------
class ILoggingResponsePolicy
{
public:
	virtual LoggingResponse_t OnLog( const LoggingContext_t *pContext ) = 0;
};

//////////////////////////////////////////////////////////////////////////
// Common Logging Listeners & Logging Response Policies
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
// A basic logging listener which prints to stdout and the debug channel.
//-----------------------------------------------------------------------------
class CSimpleLoggingListener : public ILoggingListener
{
public:
	CSimpleLoggingListener( bool bQuietPrintf = false, bool bQuietDebugger = false ) : 
	  m_bQuietPrintf( bQuietPrintf ), 
		  m_bQuietDebugger( bQuietDebugger ) 
	  { 
	  }

	  virtual void Log( const LoggingContext_t *pContext, const tchar *pMessage )
	  {
#ifdef _X360
		  if ( !m_bQuietDebugger && XBX_IsConsoleConnected() )
		  {
			  // send to console
			  XBX_DebugString( XMAKECOLOR( 0,0,0 ), pMessage );
		  }
		  else
#endif
		  {
#ifndef _CERT
			  if ( !m_bQuietPrintf )
			  {
				  Plat_Printf( _T("%s"), pMessage );
			  }
#endif

#ifdef _WIN32
			  if ( !m_bQuietDebugger && Plat_IsInDebugSession() )
			  {
				  Plat_DebugString( pMessage );
			  }
#endif
		  }
	  }

	  // If set to true, does not print anything to stdout.
	  bool m_bQuietPrintf;
	  // If set to true, does not print anything to debugger.
	  bool m_bQuietDebugger;
};

//-----------------------------------------------------------------------------
// A basic logging listener for GUI applications
//-----------------------------------------------------------------------------
class CSimpleWindowsLoggingListener : public ILoggingListener
{
public:
	virtual void Log( const LoggingContext_t *pContext, const tchar *pMessage )
	{
		if ( Plat_IsInDebugSession() )
		{
			Plat_DebugString( pMessage );
		}
		if ( pContext->m_Severity == LS_ERROR )
		{
			if ( Plat_IsInDebugSession() )
				DebuggerBreak();
			
			if ( !Plat_IsInTestMode() )
				Plat_MessageBox( "Error", pMessage );
		}
	}
};


//-----------------------------------------------------------------------------
// ** NOTE FOR INTEGRATION **
// This was copied over from source 2 rather than integrated because 
// source 2 has more significantly refactored tier0 logging.
//
// A logging listener with Win32 console API color support which which prints 
// to stdout and the debug channel.
//-----------------------------------------------------------------------------
#if !defined _X360
class CColorizedLoggingListener : public CSimpleLoggingListener
{
public:
	CColorizedLoggingListener( bool bQuietPrintf = false, bool bQuietDebugger = false ) : CSimpleLoggingListener( bQuietPrintf, bQuietDebugger )
	{
		InitWin32ConsoleColorContext( &m_ColorContext );
	}

	virtual void Log( const LoggingContext_t *pContext, const tchar *pMessage )
	{
		if ( !m_bQuietPrintf )
		{
			int nPrevColor = -1;

			if ( pContext->m_Color != UNSPECIFIED_LOGGING_COLOR )
			{
				nPrevColor = SetWin32ConsoleColor( &m_ColorContext,
					pContext->m_Color.r(), pContext->m_Color.g(), pContext->m_Color.b(), 
					MAX( MAX( pContext->m_Color.r(), pContext->m_Color.g() ), pContext->m_Color.b() ) > 128 );
			}

			Plat_Printf( _T("%s"), pMessage );

			if ( nPrevColor >= 0 )
			{
				RestoreWin32ConsoleColor( &m_ColorContext, nPrevColor );
			}
		}

#ifdef _WIN32
		if ( !m_bQuietDebugger && Plat_IsInDebugSession() )
		{
			Plat_DebugString( pMessage );
		}
#endif
	}

	Win32ConsoleColorContext_t m_ColorContext;
};
#endif // !_X360


//-----------------------------------------------------------------------------
// Default logging response policy used when one is not specified.
//-----------------------------------------------------------------------------
class CDefaultLoggingResponsePolicy : public ILoggingResponsePolicy
{
public:
	virtual LoggingResponse_t OnLog( const LoggingContext_t *pContext )
	{
		if ( pContext->m_Severity == LS_ASSERT && !CommandLine()->FindParm( "-noassert" ) ) 
		{
			return LR_DEBUGGER;
		}
		else if ( pContext->m_Severity == LS_ERROR )
		{
			return LR_ABORT;
		}
		else
		{
			return LR_CONTINUE;
		}
	}
};

//-----------------------------------------------------------------------------
// A logging response policy which never terminates the process, even on error.
//-----------------------------------------------------------------------------
class CNonFatalLoggingResponsePolicy : public ILoggingResponsePolicy
{
public:
	virtual LoggingResponse_t OnLog( const LoggingContext_t *pContext )
	{
		if ( ( pContext->m_Severity == LS_ASSERT && !CommandLine()->FindParm( "-noassert" ) ) || pContext->m_Severity == LS_ERROR )
		{
			return LR_DEBUGGER;
		}
		else
		{
			return LR_CONTINUE;
		}
	}
};

//////////////////////////////////////////////////////////////////////////
// Central Logging System
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
// The central logging system.
//
// Multiple instances can exist, though all exported tier0 functionality
// specifically works with a single global instance 
// (via GetGlobalLoggingSystem()).
//-----------------------------------------------------------------------------
class CLoggingSystem
{
public:
	CLoggingSystem();
	~CLoggingSystem();

	//-----------------------------------------------------------------------------
	// Register a logging channel with the logging system.
	// The same channel can be registered multiple times, but the parameters
	// in each call to RegisterLoggingChannel must either match across all calls
	// or be set to defaults on any given call
	//
	// This function is not thread-safe and should generally only be called 
	// by a single thread. Using the logging channel definition macros ensures 
	// that this is called on the static initialization thread.
	//-----------------------------------------------------------------------------
	LoggingChannelID_t RegisterLoggingChannel( const char *pChannelName, RegisterTagsFunc registerTagsFunc, int flags = 0, LoggingVerbosity_t verbosity = LV_DEFAULT, Color spewColor = UNSPECIFIED_LOGGING_COLOR );

	//-----------------------------------------------------------------------------
	// Gets a channel ID from a string name.
	// Performs a simple linear search; cache the value whenever possible
	// or re-register the logging channel to get a global ID.
	//-----------------------------------------------------------------------------
	LoggingChannelID_t FindChannel( const char *pChannelName ) const;

	int GetChannelCount() const { return m_nChannelCount; }

	//-----------------------------------------------------------------------------
	// Gets a pointer to the logging channel description.
	//-----------------------------------------------------------------------------
	LoggingChannel_t *GetChannel( LoggingChannelID_t channelID );
	const LoggingChannel_t *GetChannel( LoggingChannelID_t channelID ) const;

	//-----------------------------------------------------------------------------
	// Returns true if the given channel has the specified tag.
	//-----------------------------------------------------------------------------
	bool HasTag( LoggingChannelID_t channelID, const char *pTag ) const { return GetChannel( channelID )->HasTag( pTag ); }

	//-----------------------------------------------------------------------------
	// Returns true if the given channel will spew at the given verbosity level.
	//-----------------------------------------------------------------------------
	bool IsChannelEnabled( LoggingChannelID_t channelID, LoggingVerbosity_t verbosity ) const { return GetChannel( channelID )->IsEnabled( verbosity ); }

	//-----------------------------------------------------------------------------
	// Functions to set the verbosity level of a channel either directly by ID or 
	// string name, or for all channels with a given tag.
	//
	// These functions are not technically thread-safe but calling them across 
	// multiple threads should cause no significant problems 
	// (the underlying data types being changed are 32-bit/atomic).
	//-----------------------------------------------------------------------------
	void SetChannelVerbosity( LoggingChannelID_t channelID, LoggingVerbosity_t verbosity );
	void SetChannelVerbosityByName( const char *pName, LoggingVerbosity_t verbosity );
	void SetChannelVerbosityByTag( const char *pTag, LoggingVerbosity_t verbosity );

	//-----------------------------------------------------------------------------
	// Gets or sets the color of a logging channel.
	// (The functions are not thread-safe, but the consequences are not 
	// significant.)
	//-----------------------------------------------------------------------------
	Color GetChannelColor( LoggingChannelID_t channelID ) const { return GetChannel( channelID )->m_SpewColor; }
	void SetChannelColor( LoggingChannelID_t channelID, Color color ) { GetChannel( channelID )->m_SpewColor = color; }

	//-----------------------------------------------------------------------------
	// Gets or sets the flags on a logging channel.
	// (The functions are not thread-safe, but the consequences are not 
	// significant.)
	//-----------------------------------------------------------------------------
	LoggingChannelFlags_t GetChannelFlags( LoggingChannelID_t channelID ) const { return GetChannel( channelID )->m_Flags; }
	void SetChannelFlags( LoggingChannelID_t channelID, LoggingChannelFlags_t flags ) { GetChannel( channelID )->m_Flags = flags; }

	//-----------------------------------------------------------------------------
	// Adds a string tag to a channel.
	// This is not thread-safe and should only be called by a RegisterTagsFunc
	// callback passed in to RegisterLoggingChannel (via the 
	// channel definition macros).
	//-----------------------------------------------------------------------------
	void AddTagToChannel( LoggingChannelID_t channelID, const char *pTagName );

	//-----------------------------------------------------------------------------
	// Functions to save/restore the current logging state.  
	// Set bThreadLocal to true on a matching Push/Pop call if the intent
	// is to override the logging listeners on the current thread only.
	//
	// Pushing the current logging state onto the state stack results
	// in the current state being cleared by default (no listeners, default logging response policy).
	// Set bClearState to false to copy the existing listener pointers to the new state.
	//
	// These functions which mutate logging state ARE thread-safe and are 
	// guarded by m_StateMutex.
	//-----------------------------------------------------------------------------
	void PushLoggingState( bool bThreadLocal = false, bool bClearState = true );
	void PopLoggingState( bool bThreadLocal = false );

	//-----------------------------------------------------------------------------
	// Registers a logging listener (a class which handles logged messages).
	//-----------------------------------------------------------------------------
	void RegisterLoggingListener( ILoggingListener *pListener );
	void RegisterBackdoorLoggingListener( ILoggingListener *pListener );

	//-----------------------------------------------------------------------------
	// Unregisters a logging listener.
	//-----------------------------------------------------------------------------
	void UnregisterLoggingListener( ILoggingListener *pListener );

	//-----------------------------------------------------------------------------
	// Enable or disable backdoor logging listeners.
	//-----------------------------------------------------------------------------	
	void EnableBackdoorLoggingListeners( bool bEnable );

	//-----------------------------------------------------------------------------
	// Returns whether the specified logging listener is registered.
	//-----------------------------------------------------------------------------
	bool IsListenerRegistered( ILoggingListener *pListener ) const;

	//-----------------------------------------------------------------------------
	// Clears out all of the current logging state (removes all listeners, 
	// sets the response policy to the default).
	//-----------------------------------------------------------------------------
	void ResetCurrentLoggingState();
	void ResetBackdoorLoggingState();

	//-----------------------------------------------------------------------------
	// Sets a policy class to decide what should happen when messages of a 
	// particular severity are logged 
	// (e.g. exit on error, break into debugger).
	// If pLoggingResponse is NULL, uses the default response policy class.
	//-----------------------------------------------------------------------------
	void SetLoggingResponsePolicy( ILoggingResponsePolicy *pLoggingResponse );

	//-----------------------------------------------------------------------------
	// Logs a message to the specified channel using a given severity and 
	// spew color.  Passing in UNSPECIFIED_LOGGING_COLOR for 'color' allows
	// the logging listeners to provide a default.
	//-----------------------------------------------------------------------------
	LoggingResponse_t LogDirect( LoggingChannelID_t channelID, LoggingSeverity_t severity, const LeafCodeInfo_t *codeInfo, const LoggingMetaData_t *metaData, Color color, const tchar *pMessage );

	// Internal data to represent a logging tag
	struct LoggingTag_t
	{
		const char *m_pTagName;
		LoggingTag_t *m_pNextTag;
	};

	// Internal data to represent a logging channel.
	struct LoggingChannel_t
	{
		bool HasTag( const char *pTag ) const
		{
			LoggingTag_t *pCurrentTag = m_pFirstTag;
			while( pCurrentTag != NULL )
			{
				if ( stricmp( pCurrentTag->m_pTagName, pTag ) == 0 )
				{
					return true;
				}
				pCurrentTag = pCurrentTag->m_pNextTag;
			}
			return false;
		}
		bool IsEnabled( LoggingVerbosity_t verbosity ) const { return verbosity <= m_Verbosity; }
		void SetVerbosity( LoggingVerbosity_t verbosity ) { m_Verbosity = verbosity; }

		LoggingChannelID_t m_ID;
		int m_Unknown;	// Appears to be the same as m_Flags?
		LoggingChannelFlags_t m_Flags; // an OR'd combination of LoggingChannelFlags_t
		int m_Unknown2;	// Appears to be the same as m_Verbosity?
		LoggingVerbosity_t m_Verbosity;	// The maximum verbosity level allowed to activate this channel.
		Color m_SpewColor;
		char m_Name[MAX_LOGGING_IDENTIFIER_LENGTH];
		LoggingTag_t *m_pFirstTag;
	};

private:
	// Represents the current state of the logger (registered listeners, response policy class, etc.) and can
	// vary from thread-to-thread.  It can also be pushed/popped to save/restore listener/response state.
	struct LoggingState_t
	{
		// Number of active listeners in this set.  Cannot exceed MAX_LOGGING_LISTENER_COUNT.
		// If set to -1, implies that this state structure is not in use.
		int m_nListenerCount;

		// Array of registered logging listener objects.
		ILoggingListener *m_RegisteredListeners[MAX_LOGGING_LISTENER_COUNT];

		// Index of the previous entry on the listener set stack.
		int m_nPreviousStackEntry;

		// Specific policy class to determine behavior of logging system under specific message types.
		ILoggingResponsePolicy *m_pLoggingResponse;
	};

	struct BackdoorLoggingState_t
	{
		// Number of active listeners in this set.  Cannot exceed MAX_LOGGING_LISTENER_COUNT.
		// If set to -1, implies that this state structure is not in use.
		int m_nListenerCount;

		// Array of registered logging listener objects.
		ILoggingListener *m_RegisteredListeners[MAX_LOGGING_LISTENER_COUNT];

		// Are the backdoor logging listeners enabled?
		bool m_bEnabled;
	};

	// These state functions to assume the caller has already grabbed the mutex.
	LoggingState_t *GetCurrentState();
	const LoggingState_t *GetCurrentState() const;

	int FindUnusedStateIndex();
	LoggingTag_t *AllocTag( const char *pTagName );

	int m_nChannelCount;
	LoggingChannel_t m_RegisteredChannels[MAX_LOGGING_CHANNEL_COUNT];

	int m_nChannelTagCount;
	LoggingTag_t m_ChannelTags[MAX_LOGGING_TAG_COUNT];

	// Index to first free character in name pool.
	int m_nTagNamePoolIndex;
	// Pool of character data used for tag names.
	char m_TagNamePool[MAX_LOGGING_TAG_CHARACTER_COUNT];

	// Protects all data in this class except the registered channels 
	// (which are supposed to be registered using the macros at static/global init time).
	// It is assumed that this mutex is reentrant safe on all platforms.
	CThreadSpinMutex *m_pStateMutex;

	// The index of the current "global" state of the logging system.  By default, all threads use this state
	// for logging unless a given thread has pushed the logging state with bThreadLocal == true.
	// If a thread-local state has been pushed, g_nThreadLocalStateIndex (a global thread-local integer) will be non-zero.
	// By default, g_nThreadLocalStateIndex is 0 for all threads.
	int m_nGlobalStateIndex;

	// A pool of logging states used to store a stack (potentially per-thread).
	static const int MAX_LOGGING_STATE_COUNT = 64;
	LoggingState_t m_LoggingStates[MAX_LOGGING_STATE_COUNT];

	// Backdoor logging state
	BackdoorLoggingState_t m_BackdoorLoggingState;

	// Default policy class which determines behavior.
	CDefaultLoggingResponsePolicy m_DefaultLoggingResponse;

	// Default spew function.
	CColorizedLoggingListener m_DefaultLoggingListener;
};

#endif // LOGGINGSYSTEM_H
