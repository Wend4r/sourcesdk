//===== Copyright © Valve Corporation, All rights reserved. ======//
//
// Purpose: A list of values keyed by CUtlStringToken hashes, searched with SIMD
//          compares: a fixed chunk inside the list, then a chain of heap
//          overflow chunks, then the parent list
//
//===========================================================================//

#ifndef UTLATTRIBUTELIST_H
#define UTLATTRIBUTELIST_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "tier0/utlintrusivelist.h"
#include "mathlib/fltx4.h"

// The keys are packed four per i32x4, a zero key marks a free entry
template < class T, int COUNT, class STORAGE_T = T >
struct CUtlAttributeListChunk
{
	i32x4 m_nFixedKeys[ ( COUNT + 3 ) / 4 ];
	STORAGE_T m_FixedValues[ COUNT ];
};

template < class T, int FIXED_COUNT, class STORAGE_T = T, int OVERFLOW_COUNT = FIXED_COUNT >
class CUtlAttributeList
{
public:
	typedef CUtlAttributeList< T, FIXED_COUNT, STORAGE_T, OVERFLOW_COUNT > ThisType_t;
	typedef CUtlAttributeListChunk< T, FIXED_COUNT, STORAGE_T > FixedChunkType_t;
	typedef CUtlAttributeListChunk< T, OVERFLOW_COUNT, STORAGE_T > OverflowChunkType_t;

	// Allocated when the fixed chunk is full
	struct OverflowChunk_t
	{
		OverflowChunk_t *m_pNext;
		OverflowChunkType_t m_storage;
	};

	FixedChunkType_t m_fixedStorage;

	// Searched when the key is in none of the chunks
	const ThisType_t *m_pParent;
	CUtlIntrusiveList< OverflowChunk_t > m_pOverflow;
};

#endif // UTLATTRIBUTELIST_H
