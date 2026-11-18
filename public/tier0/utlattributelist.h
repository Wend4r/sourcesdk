#ifndef UTLATTRIBUTELIST_H
#define UTLATTRIBUTELIST_H

#ifdef _WIN32
#pragma once
#endif

#include "mathlib/fltx4.h"
#include "tier0/utlintrusivelist.h"

// A block of attributes: the 32-bit keys packed four to a SIMD word so that a lookup compares
// four keys at once, followed by the values in the same order.
template < typename AttributeValue_t, int NUMENTRIES, typename STORAGETYPE = AttributeValue_t >
class CUtlAttributeListChunk
{
public:
	i32x4 m_nFixedKeys[ ( NUMENTRIES + 3 ) / 4 ];
	STORAGETYPE m_FixedValues[ NUMENTRIES ];
};

// Attributes keyed by 32-bit values, stored in a fixed chunk and then in a list of overflow chunks.
// Lookups that miss fall back to the parent list.
template < typename AttributeValue_t, int NUMENTRIES = 16, typename STORAGETYPE = AttributeValue_t, int NUMOVERFLOWENTRIES = NUMENTRIES >
class CUtlAttributeList
{
public:
	typedef CUtlAttributeList< AttributeValue_t, NUMENTRIES, STORAGETYPE, NUMOVERFLOWENTRIES > ThisType_t;
	typedef CUtlAttributeListChunk< AttributeValue_t, NUMENTRIES, STORAGETYPE > FixedChunkType_t;
	typedef CUtlAttributeListChunk< AttributeValue_t, NUMOVERFLOWENTRIES, STORAGETYPE > OverflowChunkType_t;

	struct OverflowChunk_t
	{
		OverflowChunk_t *m_pNext;
		OverflowChunkType_t m_storage;
	};

	FixedChunkType_t m_fixedStorage;
	const ThisType_t *m_pParent;
	CUtlIntrusiveList< OverflowChunk_t > m_pOverflow;
};

#endif // UTLATTRIBUTELIST_H
