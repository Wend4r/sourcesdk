#ifndef SPLITSTRING_H
#define SPLITSTRING_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/bufferstring.h"
#include "tier1/utlvector.h"

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

// easy string list class with dynamically allocated strings. For use with V_SplitString, etc.
// Frees the dynamic strings in destructor.
class CUtlStringList : public CUtlVectorAutoPurge< char * >
{
public:
	CUtlStringList() {}

	CUtlStringList( char const *pString, const char **pSeparators, int nSeparators = -1 )
	{
		SplitString( pString, pSeparators, nSeparators );
	}

	~CUtlStringList()
	{
		PurgeAndDeleteElements();
	}

	void CopyAndAddToTail( char const *pString )			// clone the string and add to the end
	{
		char *pNewStr = new char[1 + strlen( pString )];
		V_strcpy( pNewStr, pString );
		AddToTail( pNewStr );
	}

	static int __cdecl SortFunc( char * const * sz1, char * const * sz2 )
	{
		return strcmp( *sz1, *sz2 );
	}

	PLATFORM_CLASS void SplitString( char const *pString, char const * const *pSeparator, int nSeparators = -1 );

private:
	CUtlStringList( const CUtlStringList &other ); // copying directly will cause double-release of the same strings; maybe we need to do a deep copy, but unless and until such need arises, this will guard against double-release
	inline void PurgeAndDeleteElements()
	{
		for( int i = 0; i < m_Size; i++ )
		{
			delete[] Element(i);
		}
		Purge();
	}
};


#include "tier0/memdbgoff.h"

#endif //#ifndef SPLITSTRING_H
