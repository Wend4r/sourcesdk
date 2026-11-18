#ifndef BLAKE3HASH_H
#define BLAKE3HASH_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"

//-----------------------------------------------------------------------------
// Incremental BLAKE3 hash, wraps a blake3_hasher
//-----------------------------------------------------------------------------
class CBlake3HashContext
{
public:
	DLL_CLASS_IMPORT CBlake3HashContext();

	DLL_CLASS_IMPORT void Update( const void *pData, size_t nSize );
	DLL_CLASS_IMPORT void Finalize( uint8 *pOutput, size_t nOutputSize );

	DLL_CLASS_IMPORT static void ProcessSingleBuffer( const void *pData, size_t nSize, uint8 *pOutput, size_t nOutputSize );

private:
	ALIGN8 uint8 m_Hasher[ 1912 ] ALIGN8_POST;
};

//-----------------------------------------------------------------------------
// BLAKE3 hash with its signature, a keyed hash of the hash by the secret key
//-----------------------------------------------------------------------------
struct Blake3SignedHash_t
{
	DLL_CLASS_IMPORT static Blake3SignedHash_t CreateUnsignedHash( const void *pData, size_t nSize );

	DLL_CLASS_IMPORT void SignHash( const char *pszSecretKey );
	DLL_CLASS_IMPORT bool Authenticate( const char *pszSecretKey ) const;

	uint8 m_Hash[ 32 ];
	uint8 m_Signature[ 32 ];
};

#endif // BLAKE3HASH_H
