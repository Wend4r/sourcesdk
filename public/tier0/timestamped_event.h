#ifndef TIER0_TIMESTAMPED_EVENT_H
#define TIER0_TIMESTAMPED_EVENT_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"

#include <stdarg.h>

//-----------------------------------------------------------------------------
// Timed events of the startup of the process: tier0 buffers them until a listener registers
//-----------------------------------------------------------------------------
enum TimestampedEvent_t
{
	TIMESTAMPED_EVENT_DLL_LOAD = 0,
	TIMESTAMPED_EVENT_APPSYS_CONNECT,
	TIMESTAMPED_EVENT_APPSYS_INIT,

	TIMESTAMPED_EVENT_COUNT,
};

enum TimestampedEventPart_t
{
	TIMESTAMPED_EVENT_PART_INSTANT = 0,
	TIMESTAMPED_EVENT_PART_BEGIN,
	TIMESTAMPED_EVENT_PART_END,
};

// Receives the time of the event, the event, its ID, the part and the message
typedef void ( *TimestampedEventListener_t )( uint64 nTime, TimestampedEvent_t nEvent, int64 nID, TimestampedEventPart_t nPart, const char *pMessage );

PLATFORM_INTERFACE int64 TimestampedEvent( TimestampedEvent_t nEvent, int64 nID, TimestampedEventPart_t nPart );
PLATFORM_INTERFACE int64 TimestampedEventMsg( TimestampedEvent_t nEvent, int64 nID, TimestampedEventPart_t nPart, PRINTF_FORMAT_STRING const char *pFormat, ... ) FMTFUNCTION( 4, 5 );
PLATFORM_INTERFACE int64 TimestampedEventMsgV( TimestampedEvent_t nEvent, int64 nID, TimestampedEventPart_t nPart, const char *pFormat, va_list marker );

// Sets the only listener and passes it the events buffered so far
PLATFORM_INTERFACE void TimestampedEvent_RegisterListenerAndFlushBuffer( TimestampedEventListener_t pListener );

#endif // TIER0_TIMESTAMPED_EVENT_H
