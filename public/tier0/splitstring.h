#ifndef SPLITSTRING_H
#define SPLITSTRING_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/bufferstring.h"
#include "tier1/utlvector.h"

// <Sergiy> placing it here a few days before Cert to minimize disruption to the rest of codebase
class CSplitString : public CUtlVector<char *, CUtlMemory<char *, int>>
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
		if (m_szBuffer)
			delete[] m_szBuffer;
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

#endif //#ifndef SPLITSTRING_H
