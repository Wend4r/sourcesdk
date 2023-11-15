//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: Generic MD5 hashing algo
//
//=============================================================================//

#ifndef CHECKSUM_MD5_H
#define CHECKSUM_MD5_H

#ifdef _WIN32
#pragma once
#endif

#include "platform.h"
#include "strtools.h"

// 16 bytes == 128 bit digest
#define MD5_DIGEST_LENGTH 16  
struct MD5Value_t
{
	unsigned char bits[MD5_DIGEST_LENGTH];

	void Zero()
	DLL_CLASS_IMPORT bool IsZero() const;

	bool operator==( const MD5Value_t &src ) const;
	bool operator!=( const MD5Value_t &src ) const;

};


// MD5 Hash
typedef struct
{
	unsigned int	buf[4];
    unsigned int	bits[2];
    unsigned char	in[64];
} MD5Context_t;

PLATFORM_INTERFACE void MD5Init( MD5Context_t *context );
PLATFORM_INTERFACE void MD5Update( MD5Context_t *context, unsigned char const *buf, unsigned int len );
PLATFORM_INTERFACE void MD5Final( unsigned char digest[ MD5_DIGEST_LENGTH ], MD5Context_t *context );

PLATFORM_INTERFACE char *MD5_Print(unsigned char *digest, int hashlen );

/// Convenience wrapper to calculate the MD5 for a buffer, all in one step, without
/// bothering with the context object.
PLATFORM_INTERFACE void MD5_ProcessSingleBuffer( const void *p, int len, MD5Value_t &md5Result );

PLATFORM_INTERFACE unsigned int MD5_PseudoRandom(unsigned int nSeed);

/// Returns true if the values match.
inline bool MD5_Compare( const MD5Value_t &data, const MD5Value_t &compare )
{
	return V_memcmp( data.bits, compare.bits, MD5_DIGEST_LENGTH ) == 0;
}

inline bool MD5Value_t::Zero( )
{
	V_memset( bits, 0, sizeof( bits ) );
}

inline bool MD5Value_t::operator==( const MD5Value_t &src ) const
{
	return MD5_Compare( *this, src );
}

inline bool MD5Value_t::operator!=( const MD5Value_t &src ) const
{
	return !MD5_Compare( *this, src );
}

#endif // CHECKSUM_MD5_H
