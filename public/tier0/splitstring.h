#ifndef SPLITSTRING_H
#define SPLITSTRING_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/bufferstring.h"
#include "tier0/utlstringlist.h"
#include "tier0/utlvector.h"

#include "tier0/memdbgon.h"

// <Sergiy> placing it here a few days before Cert to minimize disruption to the rest of codebase
class CSplitString : public CUtlVector<char *>
{
public:
	// Splits the string based on separator provided
	// Example: string "test;string string2;here" with a separator ";"
	// results in [ "test", "string string2", "here" ] array entries.
	// Separator can be multicharacter, i.e. "-;" would split string like
	// "test-;string" into [ "test", "string" ]
	CSplitString( const char *pString, const char *pSeparator, bool include_empty = false ) : m_szBuffer( nullptr ), m_nBufferSize( 0 )
	{
		Split( pString, -1, &pSeparator, 1, include_empty );
	}

	// Splits the string based on array of separators provided
	// Example: string "test;string,string2;here" with a separators [ ";", "," ]
	// results in [ "test", "string", "string2", "here" ] array entries.
	// Separator can be multicharacter, i.e. "-;" would split string like
	// "test-;string" into [ "test", "string" ]
	CSplitString( const char *pString, const char **pSeparators, int nSeparators, bool include_empty = false ) : m_szBuffer( nullptr ), m_nBufferSize( 0 )
	{
		Split( pString, -1, pSeparators, nSeparators, include_empty );
	}

	~CSplitString()
	{
		if ( m_szBuffer )
			free( (void *)m_szBuffer );
	}

	// Works the same way as CSplitString::Split with the only exception that it allows you 
	// to provide single string of separators like ";,:" which will then get used to separate string
	// instead of providing an array of separators, but doesn't support multicharacter separators cuz of that!
	DLL_CLASS_IMPORT void SetPacked( const char *pString, const char *pSeparators, bool include_empty = false, int stringSize = -1 );

private:
	DLL_CLASS_IMPORT void Split(const char *pString, int stringSize, const char **pSeparators, int nSeparators, bool include_empty = false );

	void PurgeAndDeleteElements()
	{
		Purge();
	}

private:
	char *m_szBuffer; // a copy of original string, with '\0' instead of separators
	int m_nBufferSize;
};

// Parses the numbers of pString separated by chSeparator into the vector
PLATFORM_OVERLOAD int V_SplitNumbers( const char *pString, char chSeparator, CUtlVector< int > &values );
PLATFORM_OVERLOAD int V_SplitNumbers( const char *pString, char chSeparator, CUtlVector< uint > &values );
PLATFORM_OVERLOAD int V_SplitNumbers( const char *pString, char chSeparator, CUtlVector< int64 > &values );
PLATFORM_OVERLOAD int V_SplitNumbers( const char *pString, char chSeparator, CUtlVector< uint64 > &values );
PLATFORM_OVERLOAD int V_SplitNumbers( const char *pString, char chSeparator, CUtlVector< float > &values );
PLATFORM_OVERLOAD int V_SplitNumbers( const char *pString, char chSeparator, CUtlVector< double > &values );

// Writes the numbers separated by chSeparator to pOut and returns its text
PLATFORM_OVERLOAD const char *V_JoinNumbers( char chSeparator, const CUtlVector< int > &values, CUtlString *pOut );
PLATFORM_OVERLOAD const char *V_JoinNumbers( char chSeparator, const CUtlVector< uint > &values, CUtlString *pOut );
PLATFORM_OVERLOAD const char *V_JoinNumbers( char chSeparator, const CUtlVector< int64 > &values, CUtlString *pOut );
PLATFORM_OVERLOAD const char *V_JoinNumbers( char chSeparator, const CUtlVector< uint64 > &values, CUtlString *pOut );
PLATFORM_OVERLOAD const char *V_JoinNumbers( char chSeparator, const CUtlVector< float > &values, CUtlString *pOut );
PLATFORM_OVERLOAD const char *V_JoinNumbers( char chSeparator, const CUtlVector< double > &values, CUtlString *pOut );

// V_LogMultiline into one string per line
PLATFORM_INTERFACE void V_LogMultilineToArray( bool input, const char *label, const char *data, size_t len, CUtlVector< CUtlString > &output );

#include "tier0/memdbgoff.h"

#endif //#ifndef SPLITSTRING_H
