#include "common/assert.h"
#include "common/macros.h"

#include <tier1/utlleanvector.h>
#include <tier0/memblockallocator.h>

#include <cstring>

REGISTER_NAMED_TEST( "CUtlMemoryBlockAllocator.LargeAllocation", CUtlMemoryBlockAllocator_LargeAllocation )
{
	// Allocations of 2048 bytes and more get a page of their own instead of a fatal error.
	CUtlMemoryBlockAllocator< char > allocator;

	MemBlockHandle_t hSmall = allocator.Alloc( 16 );
	MemBlockHandle_t hLarge = allocator.Alloc( 4096 );

	TEST_NE( hSmall, MEMBLOCKHANDLE_INVALID );
	TEST_NE( hLarge, MEMBLOCKHANDLE_INVALID );

	char *pLarge = ( char * )allocator.GetBlock( hLarge );

	TEST_NOT_NULL( pLarge );
	std::memset( pLarge, 'x', 4096 );
	TEST_EQ( pLarge[ 4095 ], 'x' );
	TEST_TRUE( allocator.MemUsage() >= 4096 + 16 );
}

REGISTER_NAMED_TEST( "CUtlMemoryBlockAllocator.PageSizeInElements", CUtlMemoryBlockAllocator_PageSizeInElements )
{
	// Page sizes are counted in elements, the default one holds 2048 bytes of them.
	CUtlMemoryBlockAllocator< uint64 > allocator;

	allocator.Alloc( 1 );
	TEST_EQ( allocator.m_MemPages.Count(), 1 );
	TEST_EQ( allocator.m_MemPages[ 0 ].m_nTotalSize, 2048u / sizeof( uint64 ) );
}

REGISTER_NAMED_TEST( "CUtlMemoryBlockAllocator.GrowAndSwap", CUtlMemoryBlockAllocator_GrowAndSwap )
{
	// Grow keeps the handles of a static page size consecutive, Swap exchanges the pages.
	CUtlMemoryBlockAllocator< int > allocator( 0, 8, 8, true );

	TEST_EQ( allocator.Grow( 10, false ), 10u );
	TEST_EQ( allocator.m_MemPages.Count(), 2 );
	TEST_EQ( allocator.m_MemPages[ 1 ].m_nUsedSize, 2u );

	for ( int i = 0; i < 10; i++ )
	{
		*( int * )allocator.GetBlock( ( MemBlockHandle_t )i ) = i;
	}

	TEST_EQ( *( int * )allocator.GetBlock( 9 ), 9 );

	TEST_EQ( allocator.Grow( 1, true ), 6u );
	TEST_EQ( allocator.m_MemPages[ 1 ].MemoryLeft(), 0u );

	CUtlMemoryBlockAllocator< int > other;

	other.Swap( allocator );
	TEST_EQ( allocator.m_MemPages.Count(), 0 );
	TEST_EQ( other.m_MemPages.Count(), 2 );
	TEST_EQ( *( int * )other.GetBlock( 9 ), 9 );
	TEST_EQ( other.MaxPageSize(), 8u );
}

REGISTER_NAMED_TEST( "CUtlMemoryBlockAllocator.AllocFromLastPageOnly", CUtlMemoryBlockAllocator_AllocFromLastPageOnly )
{
	// Earlier pages with room are reused unless only the last page may be allocated from.
	CUtlMemoryBlockAllocator< char > allocator( 0, 16, 16, true );

	allocator.Alloc( 8 );
	allocator.Alloc( 12 );
	TEST_EQ( allocator.m_MemPages.Count(), 2 );

	allocator.Alloc( 4 );
	TEST_EQ( allocator.m_MemPages.Count(), 2 );
	TEST_EQ( allocator.m_MemPages[ 0 ].m_nUsedSize, 12u );

	allocator.m_bAllocFromLastPageOnly = true;
	allocator.Alloc( 4 );
	TEST_EQ( allocator.m_MemPages.Count(), 3 );
}
