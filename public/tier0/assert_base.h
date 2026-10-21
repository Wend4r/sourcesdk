#ifndef TIER0_ASSERT_BASE_H
#define TIER0_ASSERT_BASE_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"

//-----------------------------------------------------------------------------
// An ID returned by the logging system to refer to a logging channel.
//-----------------------------------------------------------------------------
typedef int LoggingChannelID_t;

// Kinds of assertions, content ones come from data and can be ignored separately
enum AssertionType_t
{
	ASSERTION_TYPE_CODE = 0,
	ASSERTION_TYPE_CONTENT = 1,
	ASSERTION_TYPE_ALL_TYPES = -1,
};

//-----------------------------------------------------------------------------
// The static record of a failed assertion that the assert macros pass to tier0
//-----------------------------------------------------------------------------
struct _AssertCompileTimeConstantStruct_t
{
	const char *m_pszFile;
	const char *m_pszFunction;
	const char *m_pszExpression;
	int m_nLine;
	uint32 m_nFlags;
};

// Reports the failed assertion, optionally with a formatted message; true when the caller
// should break into the debugger
PLATFORM_OVERLOAD bool Assert_ConditionFailed( const _AssertCompileTimeConstantStruct_t &assertion );
PLATFORM_OVERLOAD bool Assert_ConditionFailed( const _AssertCompileTimeConstantStruct_t &assertion, const char *pszFormat, ... ) FMTFUNCTION( 2, 3 );

// Reports the failed assertion and ends the process
PLATFORM_OVERLOAD void Assert_ConditionFailed_Fatal( const _AssertCompileTimeConstantStruct_t &assertion );
PLATFORM_OVERLOAD void Assert_ConditionFailed_Fatal( const _AssertCompileTimeConstantStruct_t &assertion, const char *pszFormat, ... ) FMTFUNCTION( 2, 3 );

// The number of failed assertions, and the mark of a new frame for the assertions counted per frame
PLATFORM_INTERFACE int AssertsGetCount();
PLATFORM_INTERFACE void AssertsMarkFrame();

#endif // TIER0_ASSERT_BASE_H
