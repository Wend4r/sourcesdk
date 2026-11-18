#ifndef TIER0_BITSTRING_H
#define TIER0_BITSTRING_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"

class CUtlBuffer;

// Masks of the unused bits in the last int of a bit string: g_BitStringEndMasks[ n ] has all
// but the lowest n bits set, and is 0 for n = 0
PLATFORM_INTERFACE uint32 g_BitStringEndMasks[ 32 ];

// Prints the bits of the ints
PLATFORM_INTERFACE void DebugPrintBitStringBits( const int *pInts, int nInts );

// Writes and reads the ints of a bit string
PLATFORM_INTERFACE void SaveBitString( const int *pInts, int nInts, CUtlBuffer &buf );
PLATFORM_INTERFACE void LoadBitString( int *pInts, int nInts, CUtlBuffer &buf );

#endif // TIER0_BITSTRING_H
