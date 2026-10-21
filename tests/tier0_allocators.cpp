#include "common/assert.h"
#include "common/macros.h"

#include <tier0/platform.h>
#include <tier0/poolallocator.h>
#include <tier0/utl3dallocator.h>
#include <tier0/utlmemoryarena.h>
#include <tier0/vertexkdtree.h>

REGISTER_NAMED_TEST( "MemoryArena.AllocationsStayInsideTheArena", MemoryArena_AllocationsStayInsideTheArena )
{
	CUtlMemoryArena arena( 64 * 1024, false );

	const uint32 nInitialAvailable = arena.MaxAvailableAllocSize();
	TEST_TRUE( nInitialAvailable > 0 );
	TEST_TRUE( nInitialAvailable <= 64 * 1024 );

	byte *pFirst = arena.Alloc( 100 );
	byte *pSecond = arena.Alloc( 200 );

	TEST_NOT_NULL( pFirst );
	TEST_NOT_NULL( pSecond );
	TEST_TRUE( pSecond >= pFirst + 100 );
	TEST_TRUE( pFirst >= arena.MemoryBegin() );
	TEST_TRUE( pSecond + 200 <= arena.MemoryEnd() );

	// More than the arena holds cannot be allocated.
	TEST_NULL( arena.Alloc( 1024 * 1024 ) );

	// Free returns the size of the free block the memory ends up in: once everything
	// is freed the blocks merge back into one block of the whole arena.
	TEST_EQ( arena.Free( pFirst ), 100u );
	TEST_EQ( arena.Free( pSecond ), nInitialAvailable );
	TEST_EQ( arena.MaxAvailableAllocSize(), nInitialAvailable );

	// The freed memory is reused from the start.
	TEST_EQ( arena.Alloc( 50 ), pFirst );
}

REGISTER_NAMED_TEST( "PoolAllocator.AllocatesAlignedRanges", PoolAllocator_AllocatesAlignedRanges )
{
	CPoolAllocator pool( 1000 );

	CPoolAllocator::Handle_t first = pool.Allocate( 100, 1 );
	TEST_EQ( first.m_nOffset, 0u );
	TEST_EQ( first.m_nSize, 100u );

	// The next range starts at the requested alignment.
	CPoolAllocator::Handle_t aligned = pool.Allocate( 50, 64 );
	TEST_EQ( aligned.m_nOffset, 128u );
	TEST_EQ( aligned.m_nSize, 50u );

	TEST_EQ( pool.ActivePoolHighWatermark(), 178u );

	CUtlVector< CPoolAllocator::Handle_t > allocations;
	pool.Debug_GetAllocationsSnapshot( &allocations );
	TEST_EQ( allocations.Count(), 2 );

	// A range that does not fit fails with a zero size.
	CPoolAllocator::Handle_t tooBig = pool.Allocate( 2000, 1 );
	TEST_EQ( tooBig.m_nSize, 0u );

	// After the pool grows it fits, right after the first range once the aligned one is freed.
	TEST_TRUE( pool.Free( aligned ) );
	pool.GrowPool( 3000 );
	TEST_EQ( pool.GetPoolSize(), 3000u );

	CPoolAllocator::Handle_t grown = pool.Allocate( 2000, 1 );
	TEST_EQ( grown.m_nOffset, 100u );
	TEST_EQ( grown.m_nSize, 2000u );
}

REGISTER_NAMED_TEST( "3DAllocator.FillsTheVolumeWithBoxes", Allocator3D_FillsTheVolumeWithBoxes )
{
	CUtl3DAllocator2 allocator;
	allocator.Reset( 16, 16, 16 );

	// Eight 8x8x8 boxes fill a 16x16x16 volume without overlapping.
	bool bUsed[ 2 ][ 2 ][ 2 ] = {};

	for ( int i = 0; i < 8; i++ )
	{
		Rect3D_t box;
		TEST_TRUE( allocator.Allocate3D( 8, 8, 8, &box ) );
		TEST_EQ( box.width, 8 );
		TEST_EQ( box.height, 8 );
		TEST_EQ( box.depth, 8 );

		bool &bCell = bUsed[ box.x / 8 ][ box.y / 8 ][ box.z / 8 ];
		TEST_FALSE( bCell );
		bCell = true;
	}

	// The volume is full now.
	Rect3D_t box;
	TEST_FALSE( allocator.Allocate3D( 8, 8, 8, &box ) );
}

REGISTER_NAMED_TEST( "CUtl3DAllocator.UseClosestBlock", CUtl3DAllocator_UseClosestBlock )
{
	CUtl3DAllocator allocator;
	allocator.Reset( 8, 8, 8 );

	// No block is wide enough, the closest one is taken and the box is cut to it.
	Rect3D_t box = {};
	TEST_FALSE( allocator.Allocate3D( 16, 4, 4, false, &box ) );
	TEST_TRUE( allocator.Allocate3D( 16, 4, 4, true, &box ) );
	TEST_EQ( box.width, 8 );
	TEST_EQ( box.height, 4 );
	TEST_EQ( box.depth, 4 );
}

REGISTER_NAMED_TEST( "VertexKDTree.FindsNearbyVertices", VertexKDTree_FindsNearbyVertices )
{
	const float vertices[] =
	{
		0, 0, 0,
		10, 0, 0,
		0, 10, 0,
		0, 0, 10,
		5, 5, 5,
	};

	CVertexKDTree tree;
	tree.BuildMidpoint( vertices, 5, 3 );

	// The tree returns pointers into the vertex array.
	TEST_EQ( tree.Nearest( Vec3D< float >( 9, 1, 0 ) ), &vertices[ 3 ] );
	TEST_EQ( tree.Nearest( Vec3D< float >( 4, 6, 5 ) ), &vertices[ 12 ] );

	// Only the origin lies within 6 units of the origin.
	int nInSphere = 0;
	tree.ForEachInSphere( Vec3D< float >( 0, 0, 0 ), 6.0f, [ & ]( const float *pVertex )
	{
		TEST_EQ( pVertex, &vertices[ 0 ] );
		nInSphere++;
	}, 0 );
	TEST_EQ( nInSphere, 1 );

	// The box along the x axis holds the origin and the vertex at x = 10.
	int nInBox = 0;
	tree.ForEachInBox( Vec3D< float >( -1, -1, -1 ), Vec3D< float >( 11, 1, 1 ), [ & ]( const float * )
	{
		nInBox++;
	}, 0 );
	TEST_EQ( nInBox, 2 );
}
