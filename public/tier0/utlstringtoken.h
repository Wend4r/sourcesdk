//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: 
//
// $NoKeywords: $
//
//===========================================================================//

#ifndef UTLSTRINGTOKEN_H
#define UTLSTRINGTOKEN_H

#ifdef _WIN32
#pragma once
#endif

#include <limits.h>
#include "tier1/generichash.h"

#define DEBUG_STRINGTOKENS 0
#define STRINGTOKEN_MURMURHASH_SEED 0x31415926

class IFormatOutputStream;
class CFormatStringElement;

// See VStringTokenSystem001
// Interact with stringtokendatabase.txt
PLATFORM_INTERFACE bool g_bUpdateStringTokenDatabase;
PLATFORM_INTERFACE void RegisterStringToken( uint32 nHashCode, const char *pStart, const char *pEnd = NULL, bool bExtraAddToDatabase = true );

#define TOLOWERU( c ) ( ( uint32 ) ( ( ( c >= 'A' ) && ( c <= 'Z' ) )? c + 32 : c ) )
class CUtlStringToken
{
public:
	FORCEINLINE CUtlStringToken( uint32 nHashCode = 0 ) : m_nHashCode( nHashCode ) {}
	template < uintp N > FORCEINLINE CUtlStringToken( const char (&str)[N] )
	{
		constexpr uint m = 0x5bd1e995;
		constexpr int r = 24;

		uint nLength = static_cast< uint >( N - 1 );

		uint32 h = STRINGTOKEN_MURMURHASH_SEED ^ nLength;

		const char* pstr = reinterpret_cast< const char* >(str);

		while ( nLength >= 4 )
		{
			uint32 k = TOLOWERU( pstr[0] ) | ( TOLOWERU( pstr[1] ) << 8 ) | ( TOLOWERU( pstr[2] ) << 16 ) | ( TOLOWERU( pstr[3] ) << 24 );

			k *= m;
			k ^= k >> r;
			k *= m;

			h *= m;
			h ^= k;

			pstr += 4;
			nLength -= 4;
		}

		switch ( nLength )
		{
			case 3: h ^= TOLOWERU( pstr[2] ) << 16; [[fallthrough]];
			case 2: h ^= TOLOWERU( pstr[1] ) << 8;  [[fallthrough]];
			case 1: h ^= TOLOWERU( pstr[0] );
			        h *= m;
		}

		h ^= h >> 13;
		h *= m;
		h ^= h >> 15;

		m_nHashCode = h;
	}

	FORCEINLINE bool operator==( CUtlStringToken const &other ) const
	{
		return ( other.m_nHashCode == m_nHashCode );
	}

	FORCEINLINE bool operator!=( CUtlStringToken const &other ) const
	{
		return !operator==( other );
	}
	
	FORCEINLINE bool operator<( CUtlStringToken const &other ) const
	{
		return ( m_nHashCode < other.m_nHashCode );
	}

	/// access to the hash code for people who need to store thse as 32-bits, regardless of the
	FORCEINLINE uint32 GetHashCode() const { return m_nHashCode; }

	DLL_CLASS_IMPORT void FormatTo( IFormatOutputStream* pOutputStream, CFormatStringElement pElement ) const;
	DLL_CLASS_IMPORT static void TrackTokenCreation( const char *s1, const char *s2 );

// private:
	uint32 m_nHashCode;
};

FORCEINLINE CUtlStringToken MakeStringToken( char const *pString, int nLen )
{
	uint32 nHashCode = MurmurHash2LowerCase( pString, nLen, STRINGTOKEN_MURMURHASH_SEED );

	if ( g_bUpdateStringTokenDatabase )
	{
		RegisterStringToken( nHashCode, pString );
	}

	return nHashCode;
}

FORCEINLINE CUtlStringToken MakeStringToken( char const *pString )
{
	return MakeStringToken( pString, ( int )strlen(pString) );
}

#endif // UTLSTRINGTOKEN_H
