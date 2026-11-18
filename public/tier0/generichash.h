//======= Copyright © 2005, , Valve Corporation, All rights reserved. =========
//
// Purpose: Variant Pearson Hash general purpose hashing algorithm described
//			by Cargill in C++ Report 1994. Generates a 16-bit result.
//
//=============================================================================

#ifndef GENERICHASH_H
#define GENERICHASH_H

#if defined(_WIN32)
#pragma once
#endif

#include "platform.h"

#include <string.h>
#include <type_traits>

#define TOLOWERU( c ) ( ( uint32 ) ( ( ( c >= 'A' ) && ( c <= 'Z' ) ) ? ( c | ( 1 << 5 ) ) : c ) )

//-----------------------------------------------------------------------------

namespace GenericHash
{
	namespace detail
	{
		// Branchless ASCII lowercase conversion usable in constant expressions.
		constexpr char to_lower( char c )
		{
			return c + ( ( ( c >= 'A' ) & ( c <= 'Z' ) ) << 5 );
		}

		// Table of randomly shuffled values from 0-255, see tier1/generichash.cpp.
		inline constexpr uint32 kRandVals[ 256 ] =
		{
			238,	164,	191,	168,	115,	 16,	142,	 11,	213,	214,	 57,	151,	248,	252,	 26,	198,
			 13,	105,	102,	 25,	 43,	 42,	227,	107,	210,	251,	 86,	 66,	 83,	193,	126,	108,
			131,	  3,	 64,	186,	192,	 81,	 37,	158,	 39,	244,	 14,	254,	 75,	 30,	  2,	 88,
			172,	176,	255,	 69,	  0,	 45,	116,	139,	 23,	 65,	183,	148,	 33,	 46,	203,	 20,
			143,	205,	 60,	197,	118,	  9,	171,	 51,	233,	135,	220,	 49,	 71,	184,	 82,	109,
			 36,	161,	169,	150,	 63,	 96,	173,	125,	113,	 67,	224,	 78,	232,	215,	 35,	219,
			 79,	181,	 41,	229,	149,	153,	111,	217,	 21,	 72,	120,	163,	133,	 40,	122,	140,
			208,	231,	211,	200,	160,	182,	104,	110,	178,	237,	 15,	101,	 27,	 50,	 24,	189,
			177,	130,	187,	 92,	253,	136,	100,	212,	 19,	174,	 70,	 22,	170,	206,	162,	 74,
			247,	  5,	 47,	 32,	179,	117,	132,	195,	124,	123,	245,	128,	236,	223,	 12,	 84,
			 54,	218,	146,	228,	157,	 94,	106,	 31,	 17,	 29,	194,	 34,	 56,	134,	239,	246,
			241,	216,	127,	 98,	  7,	204,	154,	152,	209,	188,	 48,	 61,	 87,	 97,	225,	 85,
			 90,	167,	155,	112,	145,	114,	141,	 93,	250,	  4,	201,	156,	 38,	 89,	226,	196,
			  1,	235,	 44,	180,	159,	121,	119,	166,	190,	144,	 10,	 91,	 76,	230,	221,	 80,
			207,	 55,	 58,	 53,	175,	  8,	  6,	 52,	 68,	242,	 18,	222,	103,	249,	147,	129,
			138,	243,	 28,	185,	 62,	 59,	240,	202,	234,	 99,	 77,	 73,	199,	137,	 95,	165,
		};
	}
}

//-----------------------------------------------------------------------------

unsigned FASTCALL HashString( const char *pszKey );
unsigned FASTCALL HashStringCaseless( const char *pszKey );
unsigned FASTCALL HashStringFastCaselessConventional( const char *pszKey );
unsigned FASTCALL Hash4( const void *pKey );
unsigned FASTCALL Hash8( const void *pKey );
unsigned FASTCALL Hash12( const void *pKey );
unsigned FASTCALL Hash16( const void *pKey );
unsigned FASTCALL HashBlock( const void *pKey, unsigned size );

constexpr uint32 HashStringConventional( const char *pszKey )
{
	uint32 hash = 0xAAAAAAAA; // Alternating 1's and 0's to maximize the effect of the later multiply and add

	for ( ; *pszKey; pszKey++ )
	{
		hash = ( ( hash << 5 ) + hash ) + ( uint8 )*pszKey;
	}

	return hash;
}

constexpr uint32 HashInt( const int n )
{
	uint32 un = ( uint32 )n;
	uint32 even = 0, odd = 0;

	even = GenericHash::detail::kRandVals[ un & 0xff ];
	odd = GenericHash::detail::kRandVals[ ( ( un >> 8 ) & 0xff ) ];

	even = GenericHash::detail::kRandVals[ odd ^ ( un >> 24 ) ];
	odd = GenericHash::detail::kRandVals[ even ^ ( ( un >> 16 ) & 0xff ) ];
	even = GenericHash::detail::kRandVals[ odd ^ ( ( un >> 8 ) & 0xff ) ];
	odd = GenericHash::detail::kRandVals[ even ^ ( un & 0xff ) ];

	return ( even << 8 ) | odd;
}

// hash a uint32 into a uint32
constexpr uint32 HashIntAlternate( uint32 n)
{
	n = ( n + 0x7ed55d16 ) + ( n << 12 );
	n = ( n ^ 0xc761c23c ) ^ ( n >> 19 );
	n = ( n + 0x165667b1 ) + ( n << 5 );
	n = ( n + 0xd3a2646c ) ^ ( n << 9 );
	n = ( n + 0xfd7046c5 ) + ( n << 3 );
	n = ( n ^ 0xb55a4f09 ) ^ ( n >> 16 );
	return n;
}


inline uint64 HashUint64( uint64 s )
{
	// Thomas Wang hash, http://www.concentric.net/~ttwang/tech/inthash.htm
	s = ( ~s ) + ( s << 21 ); // s = (s << 21) - s - 1;
	s = s ^ ( s >> 24 );
	s = ( s + ( s << 3 ) ) + ( s << 8 ); // s * 265
	s = s ^ ( s >> 14 );
	s = ( s + ( s << 2 ) ) + ( s << 4 ); // s * 21
	s = s ^ ( s >> 28 );
	s = s + ( s << 31 );
	return s;
}


inline intp HashIntp( intp s )
{
#ifdef PLATFORM_64BITS
	COMPILE_TIME_ASSERT( sizeof( s ) == sizeof( uint64 ) );
	return ( intp )HashUint64( ( uint64 )s );
#else
	return HashInt( s );
#endif
}

constexpr uint32 HashIntConventional( const int n ) // faster but less effective
{
	// first byte
	uint32 hash = 0xAAAAAAAA + (n & 0xFF);
	// second byte
	hash = ( hash << 5 ) + hash + ( (n >> 8) & 0xFF );
	// third byte
	hash = ( hash << 5 ) + hash + ( (n >> 16) & 0xFF );
	// fourth byte
	hash = ( hash << 5 ) + hash + ( (n >> 24) & 0xFF );

	return hash;

	/* this is the old version, which would cause a load-hit-store on every
	   line on a PowerPC, and therefore took hundreds of clocks to execute!
	  
	byte *p = (byte *)&n;
	unsigned hash = 0xAAAAAAAA + *p++;
	hash = ( ( hash << 5 ) + hash ) + *p++;
	hash = ( ( hash << 5 ) + hash ) + *p++;
	return ( ( hash << 5 ) + hash ) + *p;
	*/
}

//-----------------------------------------------------------------------------

template <typename T>
inline unsigned HashItem( const T &item )
{
	// TODO: Confirm comiler optimizes out unused paths
	if ( sizeof(item) == 4 )
		return Hash4( &item );
	else if ( sizeof(item) == 8 )
		return Hash8( &item );
	else if ( sizeof(item) == 12 )
		return Hash12( &item );
	else if ( sizeof(item) == 16 )
		return Hash16( &item );
	else
		return HashBlock( &item, sizeof(item) );
}

template <> constexpr unsigned HashItem<int>(const int &key )
{
	return HashInt( key );
}

template <> constexpr unsigned HashItem<unsigned>(const unsigned &key )
{
	return HashInt( (int)key );
}

template<> inline unsigned HashItem<const char *>(const char * const &pszKey )
{
	return HashString( pszKey );
}

template<> inline unsigned HashItem<char *>(char * const &pszKey )
{
	return HashString( pszKey );
}

//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Murmur hash
//-----------------------------------------------------------------------------
uint32 MurmurHash2( const void *key, int len, uint32 seed );

// Constant-foldable variant for string literals, matches MurmurHash2( key, strlen( key ), seed ).
// Pointer casts and switch fallthrough are avoided so the hash can be computed at compile time.
// The length stops at the first null, so a partially filled char buffer is hashed like a string.
template < bool CASELESS = false, size_t N >
constexpr uint32 MurmurHash2( const char ( &key )[ N ], uint32 seed )
{
	// 'm' and 'r' are mixing constants generated offline.
	// They're not really 'magic', they just happen to work well.

	const uint32 m = 0x5bd1e995;
	const int r = 24;

	uint32 len = 0;

	while ( len < N - 1 && key[ len ] )
	{
		len++;
	}

	// Initialize the hash to a 'random' value

	const uint32 tail = len % 4;
	uint32 h = seed ^ len;

	// Mix 4 bytes at a time into the hash

	uint32 offset = 0;

	auto byte = []( char c ) constexpr -> uint32
	{
		if constexpr ( CASELESS )
		{
			c = GenericHash::detail::to_lower( c );
		}

		// Cast through uint8 so signed chars do not sign extend.
		return static_cast< uint32 >( static_cast< uint8 >( c ) );
	};

	while ( len >= 4 )
	{
		uint32 k = byte( key[ offset + 0 ] ) | ( byte( key[ offset + 1 ] ) << 8 ) | ( byte( key[ offset + 2 ] ) << 16 ) | ( byte( key[ offset + 3 ] ) << 24 );

		k *= m;
		k ^= k >> r;
		k *= m;

		h *= m;
		h ^= k;

		len -= 4;
		offset += 4;
	}

	// Handle the last few bytes of the input array

	if ( tail >= 3 )
	{
		h ^= byte( key[ offset + 2 ] ) << 16;
	}

	if ( tail >= 2 )
	{
		h ^= byte( key[ offset + 1 ] ) << 8;
	}

	if ( tail >= 1 )
	{
		h ^= byte( key[ offset + 0 ] );
		h *= m;
	}

	// Do a few final mixes of the hash to ensure the last few
	// bytes are well-incorporated.

	h ^= h >> 13;
	h *= m;
	h ^= h >> 15;

	return h;
}

// return murmurhash2 of a downcased string
uint32 MurmurHash2LowerCase( char const *pString, int nLength, uint32 nSeed );

template < size_t N >
constexpr uint32 MurmurHash2LowerCase( const char ( &pString )[ N ], uint32 nSeed )
{
	return MurmurHash2< true >( pString, nSeed );
}

// A template so that string literals pick the constant-foldable array overload above.
template < typename T, std::enable_if_t< std::is_convertible_v< T, const char * >, int > = 0 >
inline uint32 MurmurHash2LowerCase( T pString, uint32 nSeed )
{
	const char *pszString = pString;

	return MurmurHash2LowerCase( pszString, ( int )strlen( pszString ), nSeed );
}

// MurmurHash2, 64-bit version
uint64 MurmurHash64( const void *key, int len, uint32 seed );


#endif /* !GENERICHASH_H */
