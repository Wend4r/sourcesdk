//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: Generic CRC functions
//
// $NoKeywords: $
//=============================================================================//
#ifndef CHECKSUM_CRC_H
#define CHECKSUM_CRC_H
#include "platform.h"
#ifdef _WIN32
#pragma once
#endif

#define CRC32_INIT_VALUE 0xFFFFFFFFUL
#define CRC32_XOR_VALUE  0xFFFFFFFFUL

typedef uint32 CRC32_t;

inline void CRC32_Init(CRC32_t *pulCRC)
{
	*pulCRC = CRC32_INIT_VALUE;
}

PLATFORM_INTERFACE void CRC32_ProcessBuffer( CRC32_t *pulCRC, const void *p, int len );
PLATFORM_INTERFACE CRC32_t CRC32_GetTableEntry( unsigned int slot );

inline void CRC32_Final(CRC32_t *pulCRC)
{
	*pulCRC ^= CRC32_XOR_VALUE;
}

inline CRC32_t CRC32_ProcessSingleBuffer( const void *p, int len )
{
	CRC32_t crc;

	CRC32_Init( &crc );
	CRC32_ProcessBuffer( &crc, p, len );
	CRC32_Final( &crc );

	return crc;
}

inline unsigned long CRC32_ConvertToUnsignedLong( CRC32_t *pulCRC  )
{
	return (unsigned long)(*pulCRC);
}


inline CRC32_t CRC32_ConvertFromUnsignedLong( unsigned long ulCRC  )
{
	return (CRC32_t)(ulCRC);
}

//-----------------------------------------------------------------------------
// CRC64
//-----------------------------------------------------------------------------
#define CRC64_INIT_VALUE 0xFFFFFFFFFFFFFFFFULL
#define CRC64_XOR_VALUE  0xFFFFFFFFFFFFFFFFULL

typedef uint64 CRC64_t;

inline void CRC64_Init( CRC64_t *pCRC )
{
	*pCRC = CRC64_INIT_VALUE;
}

PLATFORM_INTERFACE void CRC64_ProcessBuffer( CRC64_t *pCRC, const void *p, int len );

inline void CRC64_Final( CRC64_t *pCRC )
{
	*pCRC ^= CRC64_XOR_VALUE;
}

inline CRC64_t CRC64_ProcessSingleBuffer( const void *p, int len )
{
	CRC64_t crc;

	CRC64_Init( &crc );
	CRC64_ProcessBuffer( &crc, p, len );
	CRC64_Final( &crc );

	return crc;
}

#endif // CHECKSUM_CRC_H
