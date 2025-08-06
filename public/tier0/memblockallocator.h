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

#define MEMBLOCK_DEFAULT_PAGESIZE (0x800)
#define MEMBLOCK_PAGESIZE_SECTION1 (0x10000)
#define MEMBLOCK_PAGESIZE_SECTION2 (0x80000)
#define MEMBLOCK_MAX_TOTAL_PAGESIZE (0x200000)

#define MEMBLOCK_PAGEINDEX_BIT (11)
#define MEMBLOCK_PAGEINDEX_MASK ((1 << MEMBLOCK_PAGEINDEX_BIT) - 1)

#define MEMBLOCK_PAGEOFFSET_BIT (21)
#define MEMBLOCK_PAGEOFFSET_MASK ((1 << MEMBLOCK_PAGEOFFSET_BIT) - 1)

template <class T, class A = CMemAllocAllocator>
class CUtlMemoryBlockAllocator
{
public:
	using MemoryAllocator_t = A;

	CUtlMemoryBlockAllocator( int nInitPages = 0, unsigned int nPageSize = MEMBLOCK_DEFAULT_PAGESIZE, unsigned int nPageMask = MEMBLOCK_PAGEOFFSET_MASK, uint32 nMaxPageSize = MEMBLOCK_MAX_TOTAL_PAGESIZE, bool bStaticPageSize = false ) :
		m_nPageOffsetBits( MEMBLOCK_PAGEOFFSET_BIT ),
		m_nPageOffsetMask( MEMBLOCK_PAGEOFFSET_MASK ),
		m_MemPages( 0, nInitPages ),
		m_nPageSize( nPageSize ),
		m_bStaticPageSize( bStaticPageSize ),
		m_unk002( false )
	{
		SetPageSize( nPageSize, nMaxPageSize, bStaticPageSize );
	}

	~CUtlMemoryBlockAllocator( void )
	{
		Purge();
	}

	uint32 MaxPageSize() const { return m_bStaticPageSize ? m_nPageSize : (m_nPageOffsetMask + 1); }
	uint32 MaxPossiblePageSize() const { return (1 << (32 - m_nPageOffsetBits)); }

	// Clears all memory buffers preserving only nSize bytes, which would be treated like a fresh memory
	void RemoveAll( size_t nSize = 0 );
	void Purge( void );

	MemBlockHandle_t Alloc( unsigned int nSize );

	size_t MemUsage( void ) const;

	void SetPageSize( uint32 nPageSize = MEMBLOCK_DEFAULT_PAGESIZE, uint32 nMaxPageSize = MEMBLOCK_MAX_TOTAL_PAGESIZE, bool unk01 = false );

	void* GetBlock( MemBlockHandle_t handle ) const;

private:
	int AddPage( unsigned int nCount );

	MemBlockHandle_t CreateHandle( int page_idx ) const
	{
		Assert( page_idx >= 0 && page_idx < m_MemPages.Count() );
		return m_MemPages[page_idx].m_nUsedSize | (page_idx << m_nPageOffsetBits);
	}

	uint32 GetPageIdxFromHandle( MemBlockHandle_t handle ) const { return handle >> m_nPageOffsetBits; }
	uint32 GetPageOffsetFromHandle( MemBlockHandle_t handle ) const { return handle & m_nPageOffsetMask; }

	uint32 CalcPageSize( int page_idx, int requested_size ) const;
	int FindPageWithSpace( unsigned int nSize ) const;

	struct MemPage_t
	{
		unsigned int MemoryLeft() const { return m_nTotalSize - m_nUsedSize; }

		unsigned int	m_nTotalSize;
		unsigned int	m_nUsedSize;
		byte			*m_pMemory;
	};

	typedef CUtlLeanVector<MemPage_t, int, A> MemPagesVec_t;

	unsigned int			m_nPageOffsetBits;
	unsigned int			m_nPageOffsetMask;
	MemPagesVec_t			m_MemPages;
	unsigned int			m_nPageSize;
	bool					m_bStaticPageSize;
	bool					m_unk002;
};

template <class T, class A>
inline void CUtlMemoryBlockAllocator<T, A>::RemoveAll( size_t nSize )
{
	size_t accumulated_total = 0;
	int removed_at = -1;

	FOR_EACH_LEANVEC( m_MemPages, i )
	{
		accumulated_total += m_MemPages[i].m_nTotalSize;

		if(removed_at != -1 || nSize && accumulated_total > nSize)
		{
			CRawAllocator::Free( m_MemPages[i].m_pMemory );

			if(removed_at == -1)
				removed_at = i.index;
		}
		else
		{
			m_MemPages[i].m_nUsedSize = 0;
		}
	}

	if(removed_at != -1)
	{
		m_MemPages.RemoveMultiple(removed_at, m_MemPages.Count() - removed_at);
	}
}

template <class T, class A>
inline void CUtlMemoryBlockAllocator<T, A>::Purge( void )
{
	FOR_EACH_LEANVEC( m_MemPages, i )
	{
		CRawAllocator::Free( m_MemPages[i].m_pMemory );
	}

	m_MemPages.Purge();
}

template <class T, class A>
inline MemBlockHandle_t CUtlMemoryBlockAllocator<T, A>::Alloc( unsigned int nSize )
{
	int page_idx = FindPageWithSpace( nSize );

	// Allocate new page since we can't fit in existing ones
	if(page_idx == -1)
	{
		page_idx = AddPage(nSize);
		if (page_idx == -1)
			return MEMBLOCKHANDLE_INVALID;
	}

	MemBlockHandle_t handle = CreateHandle( page_idx );
	m_MemPages[page_idx].m_nUsedSize += nSize;

	return handle;
}

template<class T, class A>
inline int CUtlMemoryBlockAllocator<T, A>::AddPage( unsigned int nCount )
{
	if(nCount >= MaxPossiblePageSize())
	{
		Plat_FatalError( "%s: no space for allocation of %u\n", __FUNCTION__, nCount );
		DebuggerBreak();
	}

	int page_idx = m_MemPages.AddToTail();
	auto &page = m_MemPages[page_idx];

	uint32 alloced_page_size = 0;
	page.m_pMemory = MemoryAllocator_t::template Alloc< byte >( CalcPageSize( page_idx, nCount ), alloced_page_size );
	page.m_nUsedSize = 0;

	if(m_bStaticPageSize)
	{
		page.m_nTotalSize = m_nPageSize;
	}
	else
	{
		uint32 nMaxPageSize = MaxPageSize();

		page.m_nTotalSize = MIN( alloced_page_size, nMaxPageSize );
	}

	return page_idx;
}

template <class T, class A>
inline size_t CUtlMemoryBlockAllocator<T, A>::MemUsage( void ) const
{
	size_t mem_usage = 0;

	FOR_EACH_LEANVEC( m_MemPages, i )
	{
		mem_usage += m_MemPages[i].m_nTotalSize;
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

	if(m_nPageOffsetMask >= 2)
	{
		uint32 largest_bit = 1;
		uint32 temp_val = (1 << largest_bit);

		while(temp_val <= m_nPageOffsetMask)
		{
			temp_val = (1 << ++largest_bit);
		}

		m_nPageOffsetBits = largest_bit;
	}

	m_MemPages.EnsureCapacity( MaxPossiblePageSize() );
}

template <class T, class A>
inline uint32 CUtlMemoryBlockAllocator<T, A>::CalcPageSize( int page_idx, int requested_size ) const
{
	uint32 page_size = MEMBLOCK_DEFAULT_PAGESIZE;
	if(page_idx >= 8)
	{
		if(page_idx >= 16)
			page_size = MEMBLOCK_PAGESIZE_SECTION2;
		else
			page_size = MEMBLOCK_PAGESIZE_SECTION1;
	}

	page_size = MAX( page_size, m_nPageSize );
	return MAX( page_size, requested_size );
}

template <class T, class A>
inline int CUtlMemoryBlockAllocator<T, A>::FindPageWithSpace( unsigned int nSize ) const
{
	if(m_MemPages.Count() > 0)
	{
		if(nSize <= m_MemPages.Tail().MemoryLeft())
		{
			return m_MemPages.Count() - 1;
		}
		else
		{
			FOR_EACH_LEANVEC( m_MemPages, i )
			{
				if(nSize <= m_MemPages[i].MemoryLeft())
					return i.index;
			}
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
	{
		if (m_MemPages.NumAllocated() > 0)
		{
			return (void*)&m_MemPages[ nPageIndex ].m_pMemory[ nPageOffset ];
		}
	}

	return NULL;
}

#endif // MEMBLOCKALLOCATOR_H
