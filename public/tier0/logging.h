//============ Copyright (c) Valve Corporation, All rights reserved. ============
//
// Logging system declarations.
//
// The logging system is a channel-based output mechanism which allows
// subsystems to route their text/diagnostic output to various listeners
//
//===============================================================================

#ifndef LOGGING_H
#define LOGGING_H

#if defined( COMPILER_MSVC )
#pragma once
#endif

#include "color.h"
#include <stdio.h>

// For XBX_** functions
#if defined( _X360 )
#include "xbox/xbox_console.h"
#endif

// Used by CColorizedLoggingListener
#if defined( _WIN32 ) || (defined(POSIX) && !defined(_GAMECONSOLE))
#include "tier0/win32consoleio.h"
#endif

/*
	---- Logging System ----

	The logging system is a channel-based mechanism for all code (engine,
	mod, tool) across all platforms to output information, warnings, 
	errors, etc.

	This system supersedes the existing Msg(), Warning(), Error(), DevMsg(), ConMsg() etc. functions.
	There are channels defined in the new system through which all old messages are routed;
	see LOG_GENERAL, LOG_CONSOLE, LOG_DEVELOPER, etc.

	To use the system, simply call one of the predefined macros:

		Log_Msg( ChannelID, [Color], Message, ... )
		Log_Warning( ChannelID, [Color], Message, ... )
		Log_Error( ChannelID, [Color], Message, ... )

	A ChannelID is typically created by defining a logging channel with the
	log channel macros:

		DEFINE_LOGGING_CHANNEL_NO_TAGS( LOG_ChannelName, "ChannelName", [Flags], [Verbosity], [Color] );
	
		or

		BEGIN_DEFINE_LOGGING_CHANNEL( LOG_ChannelName, "ChannelName", [Flags], [Verbosity], [Color] );
		ADD_LOGGING_CHANNEL_TAG( "Tag1" );
		ADD_LOGGING_CHANNEL_TAG( "Tag2" );
		END_DEFINE_LOGGING_CHANNEL();

	These macros create a global channel ID variable with the name specified 
	by the first parameter (in this example, LOG_ChannelName).  This channel ID 
	can be used by various LoggingSystem_** functions to manipulate the channel settings.

	The optional [Flags] parameter is an OR'd together set of LoggingChannelFlags_t 
	values (default: 0).

	The optional [Verbosity] parameter is the highest threshold
	below which messages will be processed (inclusive).  The default is LV_DEFAULT,
	which results in all messages, warnings, and errors being logged. 
	Variadic parameters to the Log_** functions will be ignored if a channel
	is not enabled for a given verbosity (for performance reasons).

	Logging channels can have their verbosity modified by name, ID, or tag.

	Logging channels are not hierarchical since there are situations in which
	a channel needs to belong to multiple hierarchies.  Use tags to create
	categories or shallow hierarchies.

	@TODO (Feature wishlist):
	1) Callstack logging support
	2) Registering dynamic channels and unregistering channels at runtime
	3) Sentient robot to clean up the thousands of places using the old/legacy logging system.
*/

//////////////////////////////////////////////////////////////////////////
// Constants, Types, Forward Declares
//////////////////////////////////////////////////////////////////////////

class CLoggingSystem;
class CThreadSpinMutex;

//-----------------------------------------------------------------------------
// Maximum length of a sprintf'ed logging message.
//-----------------------------------------------------------------------------
const int MAX_LOGGING_MESSAGE_LENGTH = 2048;

//-----------------------------------------------------------------------------
// Maximum length of a channel or tag name.
//-----------------------------------------------------------------------------
const int MAX_LOGGING_IDENTIFIER_LENGTH = 32;

//-----------------------------------------------------------------------------
// Maximum number of logging channels.  Increase if needed.
//-----------------------------------------------------------------------------
const int MAX_LOGGING_CHANNEL_COUNT = 256;

//-----------------------------------------------------------------------------
// Maximum number of logging tags across all channels.  Increase if needed.
//-----------------------------------------------------------------------------
const int MAX_LOGGING_TAG_COUNT = 1024;

//-----------------------------------------------------------------------------
// Maximum number of characters across all logging tags. Increase if needed.
//-----------------------------------------------------------------------------
const int MAX_LOGGING_TAG_CHARACTER_COUNT = 8192;

//-----------------------------------------------------------------------------
// Maximum number of concurrent logging listeners in a given logging state.
//-----------------------------------------------------------------------------
const int MAX_LOGGING_LISTENER_COUNT = 16;

//-----------------------------------------------------------------------------
// An invalid color set on a channel to imply that it should use
// a device-dependent default color where applicable.
//-----------------------------------------------------------------------------
const Color UNSPECIFIED_LOGGING_COLOR( 0, 0, 0, 0 );

//-----------------------------------------------------------------------------
// An ID returned by the logging system to refer to a logging channel.
//-----------------------------------------------------------------------------
typedef int LoggingChannelID_t;

//-----------------------------------------------------------------------------
// A sentinel value indicating an invalid logging channel ID.
//-----------------------------------------------------------------------------
const LoggingChannelID_t INVALID_LOGGING_CHANNEL_ID = -1;

//-----------------------------------------------------------------------------
// An handle returned by the logging system to refer to a tag.
//-----------------------------------------------------------------------------
typedef int LoggingTagHandle_t;

//-----------------------------------------------------------------------------
// The severity of a logging operation.
//-----------------------------------------------------------------------------
enum LoggingSeverity_t
{
	//-----------------------------------------------------------------------------
	// A debug message.
	//-----------------------------------------------------------------------------
	LS_DETAILED = 1,

	//-----------------------------------------------------------------------------
	// An informative logging message.
	//-----------------------------------------------------------------------------
	LS_MESSAGE = 2,

	//-----------------------------------------------------------------------------
	// A warning, typically non-fatal
	//-----------------------------------------------------------------------------
	LS_WARNING = 3,

	//-----------------------------------------------------------------------------
	// A message caused by an Assert**() operation.
	//-----------------------------------------------------------------------------
	LS_ASSERT = 4,

	//-----------------------------------------------------------------------------
	// An error, typically fatal/unrecoverable.
	//-----------------------------------------------------------------------------
	LS_ERROR = 5,

	//-----------------------------------------------------------------------------
	// A placeholder level, higher than any legal value.
	// Not a real severity value!
	//-----------------------------------------------------------------------------
	LS_HIGHEST_SEVERITY = 6,
};

enum LoggingVerbosity_t
{
	//-----------------------------------------------------------------------------
	// Turns off all spew.
	//-----------------------------------------------------------------------------
	LV_OFF = 0,

	//-----------------------------------------------------------------------------
	// Turns on vital logs.
	//-----------------------------------------------------------------------------
	LV_ESSENTIAL = 1,

	//-----------------------------------------------------------------------------
	// Turns on most messages.
	//-----------------------------------------------------------------------------
	LV_DEFAULT = 2,

	//-----------------------------------------------------------------------------
	// Allows for walls of text that are usually useful.
	//-----------------------------------------------------------------------------
	LV_DETAILED = 3,

	//-----------------------------------------------------------------------------
	// Allows everything.
	//-----------------------------------------------------------------------------
	LV_MAX = 4,
};

//-----------------------------------------------------------------------------
// Action which should be taken by logging system as a result of 
// a given logged message.
//
// The logging system invokes ILoggingResponsePolicy::OnLog() on
// the specified policy object, which returns a LoggingResponse_t.
//-----------------------------------------------------------------------------
enum LoggingResponse_t
{
	LR_CONTINUE,
	LR_DEBUGGER,
	LR_ABORT,
};

//-----------------------------------------------------------------------------
// Logging channel behavior flags, set on channel creation.
//-----------------------------------------------------------------------------
enum LoggingChannelFlags_t
{
	//-----------------------------------------------------------------------------
	// Indicates that the spew is only relevant to interactive consoles.
	//-----------------------------------------------------------------------------
	LCF_CONSOLE_ONLY = 0x00000001,

	//-----------------------------------------------------------------------------
	// Indicates that spew should not be echoed to any output devices.
	// A suitable logging listener must be registered which respects this flag 
	// (e.g. a file logger).
	//-----------------------------------------------------------------------------
	LCF_DO_NOT_ECHO = 0x00000002,
};

//-----------------------------------------------------------------------------
// A callback function used to register tags on a logging channel 
// during initialization.
//-----------------------------------------------------------------------------
typedef void ( *RegisterTagsFunc )( LoggingChannelID_t channelID );

struct LoggingRareOptions_t
{
	const char *m_File;
	int m_Line;
	const char *m_Function;
};

struct LoggingMetaData_t
{
	int m_Unknown;
	int m_Unknown2;
	int m_Unknown3;
	int m_Unknown4;
	int m_Unknown5;
	int m_Unknown6;
	int m_Unknown7;
};

//-----------------------------------------------------------------------------
// A context structure passed to logging listeners and response policy classes.
//-----------------------------------------------------------------------------
struct LoggingContext_t
{
	// ID of the channel being logged to.
	LoggingChannelID_t m_ChannelID;
	// Flags associated with the channel.
	LoggingChannelFlags_t m_Flags;
	// Severity of the logging event.
	LoggingSeverity_t m_Severity;
	// Color of logging message if one was specified to Log_****() macro.
	// If not specified, falls back to channel color.
	// If channel color is not specified, this value is UNSPECIFIED_LOGGING_COLOR
	// and indicates that a suitable default should be chosen.
	Color m_Color;
	const LoggingMetaData_t *m_MetaData;
};

//-----------------------------------------------------------------------------
// Interface for classes to handle logging output.
//
// The Log() function of this class is called synchronously and serially
// by the logging system on all registered instances of ILoggingListener
// in the current "logging state".
//
// Derived classes may do whatever they want with the message (write to disk,
// write to console, send over the network, drop on the floor, etc.).
//
// In general, derived classes should do one, simple thing with the output
// to allow callers to register multiple, orthogonal logging listener classes.
//-----------------------------------------------------------------------------
class ILoggingListener
{
public:
	virtual void Log( const LoggingContext_t *pContext, const tchar *pMessage ) = 0;
	virtual void OnChannelRegistered( LoggingChannelID_t channelID ) { };
	virtual void OnChannelVerbosityChanged( LoggingChannelID_t channelID ) { };
	virtual void OnChannelFlagsChanged( LoggingChannelID_t channelID ) { };
};

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

struct LoggingChannel_t
{
	DLL_CLASS_IMPORT bool HasTag( const char *pName );
};

//////////////////////////////////////////////////////////////////////////
// Logging Macros
//////////////////////////////////////////////////////////////////////////

// This macro will resolve to the most appropriate overload of LoggingSystem_Log() depending on the number of parameters passed in.
#define InternalMsg( Channel, Severity, /* [Color], Message, */ ... ) do { if ( LoggingSystem_IsChannelEnabled( Channel, Severity ) ) LoggingSystem_Log( Channel, Severity, /* [Color], Message, */ ##__VA_ARGS__ ); } while( 0 )

//-----------------------------------------------------------------------------
// New macros, use these!
//
// The macros take an optional Color parameter followed by the message 
// and the message formatting.
// We rely on the variadic macro (__VA_ARGS__) operator to paste in the 
// extra parameters and resolve to the appropriate overload.
//-----------------------------------------------------------------------------
#define Log_Msg( Channel, /* [Color], Message, */ ... ) InternalMsg( Channel, LS_MESSAGE, /* [Color], Message, */ ##__VA_ARGS__ )
#define Log_Warning( Channel, /* [Color], Message, */ ... ) InternalMsg( Channel, LS_WARNING, /* [Color], Message, */ ##__VA_ARGS__ )
#define Log_Error( Channel, /* [Color], Message, */ ... ) InternalMsg( Channel, LS_ERROR, /* [Color], Message, */ ##__VA_ARGS__ )
#define Log_Assert( Message, ... ) LoggingSystem_LogAssert( Message, ##__VA_ARGS__ )


#define DECLARE_LOGGING_CHANNEL( Channel ) extern LoggingChannelID_t Channel

#define DEFINE_LOGGING_CHANNEL_NO_TAGS( Channel, ChannelName, /* [Flags], [Verbosity], [Color] */ ... ) \
	LoggingChannelID_t Channel = LoggingSystem_RegisterLoggingChannel( ChannelName, NULL, ##__VA_ARGS__ )

#define BEGIN_DEFINE_LOGGING_CHANNEL( Channel, ChannelName, /* [Flags], [Verbosity], [Color] */ ... ) \
	static void Register_##Channel##_Tags( LoggingChannelID_t channelID ); \
	LoggingChannelID_t Channel = LoggingSystem_RegisterLoggingChannel( ChannelName, Register_##Channel##_Tags, ##__VA_ARGS__ ); \
	void Register_##Channel##_Tags( LoggingChannelID_t channelID ) \
	{

#define ADD_LOGGING_CHANNEL_TAG( Tag ) LoggingSystem_AddTagToChannel( channelID, Tag )

#define END_DEFINE_LOGGING_CHANNEL() \
	}
	
//////////////////////////////////////////////////////////////////////////
// DLL Exports
//////////////////////////////////////////////////////////////////////////

// For documentation on these functions, please look at the corresponding function
// in CLoggingSystem (unless otherwise specified).
PLATFORM_INTERFACE LoggingChannelID_t LoggingSystem_RegisterLoggingChannel( const char *pName, RegisterTagsFunc registerTagsFunc, int flags = 0, LoggingVerbosity_t verbosity = LV_DEFAULT, Color color = UNSPECIFIED_LOGGING_COLOR ); 

PLATFORM_INTERFACE void LoggingSystem_ResetCurrentLoggingState();
PLATFORM_INTERFACE void LoggingSystem_ResetBackdoorLoggingState();

PLATFORM_INTERFACE void LoggingSystem_RegisterLoggingListener( ILoggingListener *pListener );
PLATFORM_INTERFACE void LoggingSystem_RegisterBackdoorLoggingListener( ILoggingListener *pListener );
PLATFORM_INTERFACE void LoggingSystem_EnableBackdoorLoggingListeners( bool bEnable );

PLATFORM_INTERFACE void LoggingSystem_UnregisterLoggingListener( ILoggingListener *pListener );

PLATFORM_INTERFACE void LoggingSystem_SetLoggingResponsePolicy( ILoggingResponsePolicy *pResponsePolicy );

// NOTE: PushLoggingState() saves the current logging state on a stack and results in a new clear state
// (no listeners, default logging response policy).
PLATFORM_INTERFACE void LoggingSystem_PushLoggingState( bool bThreadLocal = false, bool bClearState = true );
PLATFORM_INTERFACE void LoggingSystem_PopLoggingState( bool bThreadLocal = false );
PLATFORM_INTERFACE int LoggingSystem_CurrentStateIndex( bool bThreadLocal = false );

PLATFORM_INTERFACE void LoggingSystem_AddTagToChannel( LoggingChannelID_t channelID, const char *pTagName );

// Returns INVALID_LOGGING_CHANNEL_ID if not found
PLATFORM_INTERFACE LoggingChannelID_t LoggingSystem_FindChannel( const char *pChannelName );
PLATFORM_INTERFACE int LoggingSystem_GetChannelCount();
PLATFORM_INTERFACE LoggingChannelID_t LoggingSystem_GetFirstChannelID();
// Returns INVALID_LOGGING_CHANNEL_ID when there are no channels remaining.
PLATFORM_INTERFACE LoggingChannelID_t LoggingSystem_GetNextChannelID( LoggingChannelID_t channelID );
PLATFORM_INTERFACE const LoggingChannel_t *LoggingSystem_GetChannel( LoggingChannelID_t channelID );

PLATFORM_INTERFACE bool LoggingSystem_HasTag( LoggingChannelID_t channelID, const char *pTag );

PLATFORM_INTERFACE bool LoggingSystem_IsChannelEnabled( LoggingChannelID_t channelID, LoggingSeverity_t severity );
PLATFORM_OVERLOAD bool LoggingSystem_IsChannelEnabled( LoggingChannelID_t channelID, LoggingVerbosity_t verbosity );
PLATFORM_INTERFACE LoggingVerbosity_t LoggingSystem_GetChannelVerbosity( LoggingChannelID_t channelID );
PLATFORM_INTERFACE void LoggingSystem_SetChannelVerbosity( LoggingChannelID_t channelID, LoggingVerbosity_t verbosity );
PLATFORM_INTERFACE void LoggingSystem_SetChannelVerbosityByName( const char *pName, LoggingVerbosity_t verbosity );
PLATFORM_INTERFACE void LoggingSystem_SetChannelVerbosityByTag( const char *pTag, LoggingVerbosity_t verbosity );

// Color is represented as an int32 due to C-linkage restrictions
PLATFORM_INTERFACE int32 LoggingSystem_GetChannelColor( LoggingChannelID_t channelID );
PLATFORM_INTERFACE void LoggingSystem_SetChannelColor( LoggingChannelID_t channelID, int color );

PLATFORM_INTERFACE LoggingChannelFlags_t LoggingSystem_GetChannelFlags( LoggingChannelID_t channelID );
PLATFORM_INTERFACE void LoggingSystem_SetChannelFlags( LoggingChannelID_t channelID, LoggingChannelFlags_t flags );

//-----------------------------------------------------------------------------
// Logs a variable-argument to a given channel with the specified severity.
// NOTE: if adding overloads to this function, remember that the Log_***
// macros simply pass their variadic parameters through to LoggingSystem_Log().
// Therefore, you need to ensure that the parameters are in the same general 
// order and that there are no ambiguities with the overload.
//-----------------------------------------------------------------------------
PLATFORM_INTERFACE LoggingResponse_t LoggingSystem_Log( LoggingChannelID_t channelID, LoggingSeverity_t severity, const char *pMessageFormat, ... ) FMTFUNCTION( 3, 4 );
PLATFORM_OVERLOAD LoggingResponse_t LoggingSystem_Log( LoggingChannelID_t channelID, LoggingSeverity_t severity, Color spewColor, const char *pMessageFormat, ... ) FMTFUNCTION( 4, 5 );
PLATFORM_OVERLOAD LoggingResponse_t LoggingSystem_Log( LoggingChannelID_t channelID, LoggingSeverity_t severity, const LoggingMetaData_t *metaData, const char *pMessageFormat, ... ) FMTFUNCTION( 4, 5 );
PLATFORM_OVERLOAD LoggingResponse_t LoggingSystem_Log( LoggingChannelID_t channelID, LoggingSeverity_t severity, const LoggingMetaData_t *metaData, Color spewColor, const char *pMessageFormat, ... ) FMTFUNCTION( 5, 6 );
PLATFORM_OVERLOAD LoggingResponse_t LoggingSystem_Log( LoggingChannelID_t channelID, LoggingSeverity_t severity, const LoggingRareOptions_t &codeInfo, const char *pMessageFormat, ... ) FMTFUNCTION( 4, 5 );
PLATFORM_OVERLOAD LoggingResponse_t LoggingSystem_Log( LoggingChannelID_t channelID, LoggingSeverity_t severity, const LoggingRareOptions_t &codeInfo, Color spewColor, const char *pMessageFormat, ... ) FMTFUNCTION( 5, 6 );
PLATFORM_OVERLOAD LoggingResponse_t LoggingSystem_Log( LoggingChannelID_t channelID, LoggingSeverity_t severity, const LoggingRareOptions_t &codeInfo, const LoggingMetaData_t *metaData, const char *pMessageFormat, ... ) FMTFUNCTION( 5, 6 );
PLATFORM_OVERLOAD LoggingResponse_t LoggingSystem_Log( LoggingChannelID_t channelID, LoggingSeverity_t severity, const LoggingRareOptions_t &codeInfo, const LoggingMetaData_t *metaData, Color spewColor, const char *pMessageFormat, ... ) FMTFUNCTION( 6, 7 );

PLATFORM_INTERFACE LoggingResponse_t LoggingSystem_LogDirect( LoggingChannelID_t channelID, LoggingSeverity_t severity, const char *pMessage );
PLATFORM_OVERLOAD LoggingResponse_t LoggingSystem_LogDirect( LoggingChannelID_t channelID, LoggingSeverity_t severity, Color spewColor, const char *pMessage );
PLATFORM_OVERLOAD LoggingResponse_t LoggingSystem_LogDirect( LoggingChannelID_t channelID, LoggingSeverity_t severity, const LoggingMetaData_t *metaData, const char *pMessage );
PLATFORM_OVERLOAD LoggingResponse_t LoggingSystem_LogDirect( LoggingChannelID_t channelID, LoggingSeverity_t severity, Color spewColor, const LoggingMetaData_t *metaData, const char *pMessage );
PLATFORM_OVERLOAD LoggingResponse_t LoggingSystem_LogDirect( LoggingChannelID_t channelID, LoggingSeverity_t severity, const LoggingRareOptions_t &codeInfo, const char *pMessage );
PLATFORM_OVERLOAD LoggingResponse_t LoggingSystem_LogDirect( LoggingChannelID_t channelID, LoggingSeverity_t severity, const LoggingRareOptions_t &codeInfo, Color spewColor, const char *pMessage );
PLATFORM_OVERLOAD LoggingResponse_t LoggingSystem_LogDirect( LoggingChannelID_t channelID, LoggingSeverity_t severity, const LoggingRareOptions_t &codeInfo, const LoggingMetaData_t *metaData, const char *pMessage );
PLATFORM_OVERLOAD LoggingResponse_t LoggingSystem_LogDirect( LoggingChannelID_t channelID, LoggingSeverity_t severity, const LoggingRareOptions_t &codeInfo, Color spewColor, const LoggingMetaData_t *metaData, const char *pMessage );

PLATFORM_INTERFACE LoggingResponse_t LoggingSystem_LogAssert( const char *pMessageFormat, ... ) FMTFUNCTION( 1, 2 );

PLATFORM_INTERFACE LoggingTagHandle_t LoggingSystem_FirstTag();
PLATFORM_INTERFACE LoggingTagHandle_t LoggingSystem_NextTag( LoggingTagHandle_t currentTag );
PLATFORM_INTERFACE LoggingTagHandle_t LoggingSystem_InvalidTag();
PLATFORM_INTERFACE const char *LoggingSystem_TagName( LoggingTagHandle_t tag );

#endif // LOGGING_H
