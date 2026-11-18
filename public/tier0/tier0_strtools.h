#ifndef TIER0_TIER0_STRTOOLS_H
#define TIER0_TIER0_STRTOOLS_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"

struct characterset_t
{
	char set[256];
};

// Replaces every nOrigChar of pStr with nNewChar
PLATFORM_INTERFACE void StrReplaceChar( char *pStr, char nOrigChar, char nNewChar );

// Length of the leading directories that both paths share, case and slash insensitive
PLATFORM_INTERFACE int LengthOfMatchingPaths( const char *pFilenamePath, const char *pMatchPath );

// Copies the text of str up to the next sep to token, returns the text after the separator
// or null when str is empty
PLATFORM_INTERFACE const char *nexttoken( char *token, const char *str, char sep );

#endif // TIER0_TIER0_STRTOOLS_H
