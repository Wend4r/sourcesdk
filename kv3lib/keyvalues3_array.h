#ifndef KEYVALUES3_ARRAY_H
#define KEYVALUES3_ARRAY_H

#ifdef _WIN32
#pragma once
#endif

#include "kv3lib/keyvalues3.h"
#include "keyvalues3_helpers.h"

#include "tier0/memdbgon.h"

class CKeyValues3Array
{
public:
	typedef KeyValues3 *Element_t;

	static const size_t DATA_SIZE = KV3_ARRAY_MAX_FIXED_MEMBERS;
	static const size_t DATA_ALIGNMENT = KV3Helpers::PackAlignOf<Element_t>();

	CKeyValues3Array( int cluster_elem = KV3_INVALID_CLUSTER_ELEMENT, int alloc_size = DATA_SIZE );
	~CKeyValues3Array() { Free(); }

	bool HasCluster() const { return m_nClusterElement != KV3_INVALID_CLUSTER_ELEMENT; }
	int GetClusterElement() const { return m_nClusterElement; }
	void SetClusterElement( int element ) { m_nClusterElement = element; }

	CKeyValues3ArrayCluster* GetCluster() const;
	CKV3Arena* GetContext() const;

	Element_t *Base() { return IsBaseStatic() ? &m_StaticElements[0] : m_pDynamicElements; };
	Element_t const *Base() const { return const_cast<CKeyValues3Array *>(this)->Base(); }

	Element_t Element( int i );
	Element_t Element( int i ) const { return const_cast<CKeyValues3Array*>(this)->Element( i ); }
	int Count() const { return m_nCount; }

	void EnsureElementCapacity( int count, bool force = false, bool dont_move = false );

	void SetCount( KeyValues3 *parent, int count, KV3TypeEx_t type = KV3_TYPEEX_NULL, KV3SubType_t subtype = KV3_SUBTYPE_UNSPECIFIED );
	Element_t* InsertMultipleBefore( KeyValues3 *parent, int from, int num );
	void CopyFrom( KeyValues3 *parent, const CKeyValues3Array* pSrc );
	void RemoveMultiple( KeyValues3 *parent, int from, int num );

	void Free( bool clearing_context = false ) { PurgeBuffers(); }
	void PurgeContent( KeyValues3 *parent, bool clearing_context = false );
	void PurgeBuffers();

	static constexpr size_t TotalSizeOf( int initial_size ) { return ALIGN_VALUE( TotalSizeWithoutStaticData() + TotalSizeOfData( MAX( initial_size, 0 ) ), 8 ); }
	static constexpr size_t TotalSizeOfData( int size ) { return MAX( (KV3Helpers::PackSizeOf<DATA_ALIGNMENT, Element_t>( size )), sizeof( m_pDynamicElements ) ); }
	static constexpr size_t TotalSizeWithoutStaticData() { return sizeof( CKeyValues3Array ) - sizeof( m_StaticElements ); }

private:
	int GetAllocatedChunks() const { return m_nAllocatedChunks; }
	bool IsBaseStatic() { return !m_bIsDynamicallySized; }

	size_t GetAllocatedBytesSize() const { return TotalSizeOfData( GetAllocatedChunks() ); }

private:
	int m_nClusterElement;
	int m_nAllocatedChunks;

	int m_nCount;
	uint8 m_nInitialSize;
	bool m_bIsDynamicallySized;

	bool m_unk001;
	bool m_unk002;

	union
	{
		Element_t m_StaticElements[DATA_SIZE];
		Element_t *m_pDynamicElements;
	};
};
COMPILE_TIME_ASSERT(sizeof(CKeyValues3Array) == 64);

#include "tier0/memdbgoff.h"

#endif // KEYVALUES3_ARRAY_H
