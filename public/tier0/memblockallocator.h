//===== Copyright 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: 
//
// $Workfile:     $
// $Date:         $
//
//-----------------------------------------------------------------------------
// $Log: $
//
// $NoKeywords: $
//===========================================================================//

#ifndef MEMBLOCKALLOCATOR_H
#define MEMBLOCKALLOCATOR_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/memalloc.h"
#include "tier0/rawallocator.h"
#include "tier1/utlleanvector.h"

// AMNOTE: Handle that contains page/subpage indexes to allocated memory within internal storage
// Stored in the following format: 
// offset within page of size MEMBLOCK_PAGEOFFSET_BIT
// page index of size MEMBLOCK_PAGEINDEX_BIT
typedef unsigned int MemBlockHandle_t;

#define MEMBLOCKHANDLE_INVALID ((MemBlockHandle_t)~0)

// Sizes in bytes, the allocator divides them by sizeof( T )
#define MEMBLOCK_DEFAULT_PAGESIZE (0x800)
#define MEMBLOCK_PAGESIZE_SECTION1 (0x10000)
#define MEMBLOCK_PAGESIZE_SECTION2 (0x80000)
#define MEMBLOCK_MAX_TOTAL_PAGESIZE (0x200000)

#define MEMBLOCK_PAGEOFFSET_BIT (21)
#define MEMBLOCK_PAGEOFFSET_MASK ((1 << MEMBLOCK_PAGEOFFSET_BIT) - 1)

// Page sizes, offsets and allocation sizes are counted in elements of T
template <class T, class A = CMemAllocAllocator>
class CUtlMemoryBlockAllocator
{
public:
	using MemoryAllocator_t = A;

public:
	CUtlMemoryBlockAllocator( int nInitPages = 0, uint32 nPageSize = MEMBLOCK_DEFAULT_PAGESIZE / sizeof( T ), uint32 nMaxPageSize = MEMBLOCK_MAX_TOTAL_PAGESIZE / sizeof( T ), bool bStaticPageSize = false ) :
		m_nPageOffsetBits( MEMBLOCK_PAGEOFFSET_BIT ),
		m_nPageOffsetMask( MEMBLOCK_PAGEOFFSET_MASK ),
		m_MemPages( 0, nInitPages ),
		m_nPageSize( nPageSize ),
		m_bStaticPageSize( bStaticPageSize ),
		m_bAllocFromLastPageOnly( false )
	{
		SetPageSize( nPageSize, nMaxPageSize, bStaticPageSize );
	}

	~CUtlMemoryBlockAllocator( void )
	{
		Purge();
	}

	uint32 MaxPageSize() const { return m_bStaticPageSize ? m_nPageSize : (m_nPageOffsetMask + 1); }
	uint32 MaxPossiblePageSize() const { return (1 << (32 - m_nPageOffsetBits)); }

	// Clears all memory buffers preserving only nSize elements, which would be treated like a fresh memory
	void RemoveAll( size_t nSize = 0 );
	void Purge( void );

	void Swap( CUtlMemoryBlockAllocator< T, A > &other );

	MemBlockHandle_t Alloc( unsigned int nSize );

	// Allocates nCount elements on the last pages, so their handles are consecutive indexes,
	// and returns how many were allocated, which includes the rest of the last page if bFillLastPage
	unsigned int Grow( unsigned int nCount, bool bFillLastPage );

	size_t MemUsage( void ) const;

	void SetPageSize( uint32 nPageSize = MEMBLOCK_DEFAULT_PAGESIZE / sizeof( T ), uint32 nMaxPageSize = MEMBLOCK_MAX_TOTAL_PAGESIZE / sizeof( T ), bool bStaticPageSize = false );

	void* GetBlock( MemBlockHandle_t handle ) const;

public:
	int AddPage( unsigned int nCount );

	MemBlockHandle_t CreateHandle( int page_idx ) const
	{
		Assert( page_idx >= 0 && page_idx < m_MemPages.Count() );
		return m_MemPages[page_idx].m_nUsedSize | (page_idx << m_nPageOffsetBits);
	}

	int GetPageIdxFromHandle( MemBlockHandle_t handle ) const { return handle >> m_nPageOffsetBits; }
	int GetPageOffsetFromHandle( MemBlockHandle_t handle ) const { return handle & m_nPageOffsetMask; }

	unsigned int CalcPageSize( int page_idx, unsigned int requested_size ) const;
	int FindPageWithSpace( unsigned int nSize ) const;

	struct MemPage_t
	{
		unsigned int MemoryLeft() const { return m_nTotalSize - m_nUsedSize; }

		unsigned int	m_nTotalSize = 0;
		unsigned int	m_nUsedSize = 0;
		T				*m_pMemory = nullptr;
	};

	typedef CUtlLeanVector<MemPage_t, int, A> MemPagesVec_t;

	unsigned int			m_nPageOffsetBits;
	unsigned int			m_nPageOffsetMask;
	MemPagesVec_t			m_MemPages;
	unsigned int			m_nPageSize;
	bool					m_bStaticPageSize;
	bool					m_bAllocFromLastPageOnly;
};

template <class T, class A>
inline void CUtlMemoryBlockAllocator<T, A>::RemoveAll( size_t nSize )
{
	size_t accumulated_total = 0;
	int removed_at = -1;

	FOR_EACH_LEANVEC( m_MemPages, it )
	{
		accumulated_total += m_MemPages[it].m_nTotalSize;

		if ( removed_at != -1 || ( nSize && accumulated_total > nSize ) )
		{
			MemoryAllocator_t::Free( m_MemPages[it].m_pMemory );

			if ( removed_at == -1 )
				removed_at = it.index;
		}
		else
		{
			m_MemPages[it].m_nUsedSize = 0;
		}
	}

	if ( removed_at != -1 )
	{
		m_MemPages.RemoveMultipleFromTail( m_MemPages.Count() - removed_at );
	}
}

template <class T, class A>
inline void CUtlMemoryBlockAllocator<T, A>::Purge( void )
{
	FOR_EACH_LEANVEC( m_MemPages, it )
	{
		MemoryAllocator_t::Free( m_MemPages[it].m_pMemory );
	}

	m_MemPages.Purge();
}

template <class T, class A>
inline void CUtlMemoryBlockAllocator<T, A>::Swap( CUtlMemoryBlockAllocator<T, A> &other )
{
	V_swap( m_nPageOffsetBits, other.m_nPageOffsetBits );
	V_swap( m_nPageOffsetMask, other.m_nPageOffsetMask );
	m_MemPages.Swap( other.m_MemPages );
	V_swap( m_nPageSize, other.m_nPageSize );
	V_swap( m_bStaticPageSize, other.m_bStaticPageSize );
	V_swap( m_bAllocFromLastPageOnly, other.m_bAllocFromLastPageOnly );
}

template <class T, class A>
inline int CUtlMemoryBlockAllocator<T, A>::AddPage( unsigned int nSize )
{
	if ( ( uint32 )m_MemPages.Count() >= MaxPossiblePageSize() )
	{
		Plat_FatalError( "%s: no space for allocation of %u\n", __FUNCTION__, nSize );
		DebuggerBreak();
	}

	int page_idx = m_MemPages.AddToTail();
	auto &page = m_MemPages[page_idx];

	uint32 alloced_page_size = 0;
	page.m_pMemory = MemoryAllocator_t::template Alloc<T>( CalcPageSize( page_idx, nSize ), alloced_page_size );
	page.m_nUsedSize = 0;

	if ( m_bStaticPageSize )
		page.m_nTotalSize = m_nPageSize;
	else
		page.m_nTotalSize = MIN( alloced_page_size, MaxPageSize() );

	return page_idx;
}

template <class T, class A>
inline MemBlockHandle_t CUtlMemoryBlockAllocator<T, A>::Alloc( unsigned int nSize )
{
	if ( nSize > MaxPageSize() )
	{
		Plat_FatalError( "%s: allocation of %u too large, max %u\n", __FUNCTION__, nSize, MaxPageSize() );
		DebuggerBreak();
	}

	int page_idx = FindPageWithSpace( nSize );

	// Allocate new page since we can't fit in existing ones
	if ( page_idx == -1 )
		page_idx = AddPage( nSize );

	MemBlockHandle_t handle = CreateHandle( page_idx );
	m_MemPages[page_idx].m_nUsedSize += nSize;

	return handle;
}

template <class T, class A>
inline unsigned int CUtlMemoryBlockAllocator<T, A>::Grow( unsigned int nCount, bool bFillLastPage )
{
	if ( nCount == 0 )
		return 0;

	unsigned int nAllocated = 0;
	MemPage_t *pPage = m_MemPages.Count() > 0 ? &m_MemPages.Tail() : nullptr;

	while ( nCount > 0 )
	{
		if ( !pPage || pPage->MemoryLeft() == 0 )
			pPage = &m_MemPages[ AddPage( MIN( nCount, MaxPageSize() ) ) ];

		unsigned int nUsed = MIN( nCount, pPage->MemoryLeft() );
		pPage->m_nUsedSize += nUsed;
		nAllocated += nUsed;
		nCount -= nUsed;
	}

	if ( bFillLastPage )
	{
		nAllocated += pPage->MemoryLeft();
		pPage->m_nUsedSize = pPage->m_nTotalSize;
	}

	return nAllocated;
}

template <class T, class A>
inline size_t CUtlMemoryBlockAllocator<T, A>::MemUsage( void ) const
{
	size_t mem_usage = 0;

	FOR_EACH_LEANVEC( m_MemPages, it )
	{
		mem_usage += m_MemPages[it].m_nTotalSize;
	}

	return mem_usage;
}

template <class T, class A>
inline void CUtlMemoryBlockAllocator<T, A>::SetPageSize( uint32 nPageSize, uint32 nMaxPageSize, bool bStaticPageSize )
{
	RemoveAll();

	m_nPageSize = nPageSize;
	m_bStaticPageSize = bStaticPageSize;

	m_nPageOffsetBits = 1;
	m_nPageOffsetMask = SmallestPowerOfTwoGreaterOrEqual( nMaxPageSize ) - 1;

	if ( m_nPageOffsetMask >= 2 )
	{
		uint32 largest_bit = 1;
		uint32 temp_val = (1 << largest_bit);

		while(temp_val <= m_nPageOffsetMask)
		{
			temp_val = (1 << ++largest_bit);
		}

		m_nPageOffsetBits = largest_bit;
	}

	int nMaxPages = ( int )MaxPossiblePageSize();
	if ( m_MemPages.Count() > nMaxPages )
	{
		for ( int i = nMaxPages; i < m_MemPages.Count(); i++ )
			MemoryAllocator_t::Free( m_MemPages[ i ].m_pMemory );

		m_MemPages.RemoveMultipleFromTail( m_MemPages.Count() - nMaxPages );
	}
}

template <class T, class A>
inline unsigned int CUtlMemoryBlockAllocator<T, A>::CalcPageSize( int page_idx, unsigned int requested_size ) const
{
	if ( m_bStaticPageSize )
		return m_nPageSize;

	uint32 page_size = MEMBLOCK_DEFAULT_PAGESIZE / sizeof( T );
	if ( page_idx >= 8 )
	{
		if ( page_idx >= 16 )
			page_size = MEMBLOCK_PAGESIZE_SECTION2 / sizeof( T );
		else
			page_size = MEMBLOCK_PAGESIZE_SECTION1 / sizeof( T );
	}

	page_size = MIN( page_size, m_nPageOffsetMask + 1 );
	page_size = MAX( page_size, m_nPageSize );
	return MAX( page_size, requested_size );
}

template <class T, class A>
inline int CUtlMemoryBlockAllocator<T, A>::FindPageWithSpace( unsigned int nSize ) const
{
	int last_idx = m_MemPages.Count() - 1;
	if ( last_idx < 0 )
		return -1;

	if ( nSize < m_MemPages[ last_idx ].MemoryLeft() )
		return last_idx;

	if ( !m_bAllocFromLastPageOnly )
	{
		for ( int i = 0; i < last_idx; i++ )
		{
			if ( nSize <= m_MemPages[ i ].MemoryLeft() )
				return i;
		}
	}

	return -1;
}

template <class T, class A>
inline void* CUtlMemoryBlockAllocator<T, A>::GetBlock( MemBlockHandle_t handle ) const
{
	int nPageIndex = GetPageIdxFromHandle( handle );
	int nPageOffset = GetPageOffsetFromHandle( handle );

	if ( nPageIndex >= 0 && nPageIndex < m_MemPages.Count() )
		return (void*)&m_MemPages[ nPageIndex ].m_pMemory[ nPageOffset ];

	return NULL;
}

#endif // MEMBLOCKALLOCATOR_H
