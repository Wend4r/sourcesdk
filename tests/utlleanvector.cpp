#include "common/assert.h"
#include "common/macros.h"

#include <tier1/utlleanvector.h>

REGISTER_NAMED_TEST( "CUtlLeanVector.InsertRemove", CUtlLeanVector_InsertRemove )
{
	// Lean vectors should append, remove and clear values without leaking indices.
	CUtlLeanVector< int > vec;

	TEST_EQ( vec.Count(), 0 );
	TEST_FALSE( vec.IsValidIndex( 0 ) );

	TEST_EQ( vec.AddToTail( 1 ), 0 );
	TEST_EQ( vec.AddToTail( 2 ), 1 );
	TEST_EQ( vec.AddToTail( 3 ), 2 );
	TEST_EQ( vec.Count(), 3 );
	TEST_EQ( vec[0], 1 );
	TEST_EQ( vec[2], 3 );

	vec.Remove( 1 );
	TEST_EQ( vec.Count(), 2 );
	TEST_EQ( vec[0], 1 );
	TEST_EQ( vec[1], 3 );

	TEST_TRUE( vec.FindAndFastRemove( 1 ) );
	TEST_EQ( vec.Count(), 1 );
	TEST_EQ( vec[0], 3 );

	vec.RemoveAll();
	TEST_EQ( vec.Count(), 0 );
}

REGISTER_NAMED_TEST( "CUtlLeanVector.PointersCountAndSwap", CUtlLeanVector_PointersCountAndSwap )
{
	// Pointer-returning insertion helpers should remain valid through count changes and swap.
	CUtlLeanVector< int > vec;

	int *pTail = vec.AddToTailGetPtr();

	*pTail = 10;

	int *pInserted = vec.InsertBeforeGetPtr( 0 );

	*pInserted = 5;

	TEST_EQ( vec.Count(), 2 );
	TEST_EQ( vec[0], 5 );
	TEST_EQ( vec[1], 10 );
	TEST_EQ( vec.Find( 10 ), 1 );

	vec.SetCount( 4 );
	TEST_EQ( vec.Count(), 4 );
	TEST_TRUE( vec.IsValidIndex( 3 ) );

	CUtlLeanVector< int > other;

	other.AddToTail( 21 );
	vec.Swap( other );

	TEST_EQ( vec.Count(), 1 );
	TEST_EQ( vec[0], 21 );
}

REGISTER_NAMED_TEST( "CUtlLeanVector.FastRemoveAndPurge", CUtlLeanVector_FastRemoveAndPurge )
{
	// Fast removal and purge should leave lean vectors empty and with no backing storage.
	CUtlLeanVector< int > vec;

	vec.AddToTail( 1 );
	vec.AddToTail( 2 );
	vec.AddToTail( 3 );

	vec.FastRemove( 0 );
	TEST_EQ( vec.Count(), 2 );
	TEST_TRUE( vec.Find( 2 ) != vec.InvalidIndex() || vec.Find( 3 ) != vec.InvalidIndex() );

	vec.EnsureCount( 5 );
	TEST_EQ( vec.Count(), 5 );

	TEST_TRUE( vec.FindAndRemove( 2 ) || vec.FindAndRemove( 3 ) );
	vec.Purge();
	TEST_EQ( vec.Count(), 0 );
	TEST_NULL( vec.Base() );
}

REGISTER_NAMED_TEST( "CUtlLeanVector.CopyAndMove", CUtlLeanVector_CopyAndMove )
{
	// Copy and move operations should preserve visible values in order.
	CUtlLeanVector< int > vec;

	vec.AddToTail( 12 );
	vec.AddToTail( 24 );

	CUtlLeanVector< int > copy( vec );

	TEST_EQ( copy.Count(), 2 );
	TEST_EQ( copy[0], 12 );
	TEST_EQ( copy[1], 24 );

	CUtlLeanVector< int > moved( Move( vec ) );

	TEST_EQ( moved.Count(), 2 );
	TEST_EQ( moved[0], 12 );

	CUtlLeanVector< int > assigned;

	assigned = Move( moved );
	TEST_EQ( assigned.Count(), 2 );
	TEST_EQ( assigned[1], 24 );
}

REGISTER_NAMED_TEST( "CUtlLeanVectorFixedGrowable.InlineCapacity", CUtlLeanVectorFixedGrowable_InlineCapacity )
{
	// Inline storage should use all of the union's space before spilling to the heap.
	CUtlLeanVectorFixedGrowable< char, 1 > vec;

	TEST_EQ( sizeof( vec ), sizeof( int ) * 2 + sizeof( char * ) );
	TEST_EQ( vec.NumAllocated(), ( int )sizeof( char * ) );

	const char *pInline = reinterpret_cast< const char * >( &vec ) + sizeof( int ) * 2;

	for ( int i = 0; i < vec.NumAllocated(); ++i )
	{
		vec.AddToTail( ( char )( 'a' + i ) );
	}

	TEST_EQ( vec.Base(), pInline );
	TEST_EQ( vec[ 0 ], 'a' );

	int nInlineCount = vec.Count();

	vec.AddToTail( 'z' );

	TEST_NE( vec.Base(), pInline );
	TEST_EQ( vec.Count(), nInlineCount + 1 );
	TEST_EQ( vec[ 0 ], 'a' );
	TEST_EQ( vec[ nInlineCount ], 'z' );

	vec.Purge();
	TEST_EQ( vec.Count(), 0 );
	TEST_EQ( vec.NumAllocated(), ( int )sizeof( char * ) );
}

REGISTER_NAMED_TEST( "CUtlLeanVectorFixedGrowable.GrowAndPurge", CUtlLeanVectorFixedGrowable_GrowAndPurge )
{
	// Fixed growable vectors should keep their elements when they spill to the heap.
	CUtlLeanVectorFixedGrowable< int, 1 > vec;

	TEST_EQ( vec.NumAllocated(), 2 );

	for ( int i = 0; i < 10; ++i )
	{
		vec.AddToTail( i );
	}

	TEST_EQ( vec.Count(), 10 );

	for ( int i = 0; i < 10; ++i )
	{
		TEST_EQ( vec[ i ], i );
	}

	vec.Purge();
	TEST_EQ( vec.Count(), 0 );
	TEST_EQ( vec.NumAllocated(), 2 );
}

REGISTER_NAMED_TEST( "CUtlLeanVector.UnsignedIndex", CUtlLeanVector_UnsignedIndex )
{
	// An unsigned index type should insert like a signed one, the capacity is capped at its signed maximum.
	CUtlLeanVector< int, uint32 > vec;

	vec.AddToTail( 2 );
	*vec.InsertBeforeGetPtr( 0, 1 ) = 1;
	*vec.InsertBeforeGetPtr( 2, 1 ) = 3;
	TEST_EQ( vec.Count(), 3 );
	TEST_EQ( vec[ 0 ], 1 );
	TEST_EQ( vec[ 1 ], 2 );
	TEST_EQ( vec[ 2 ], 3 );
}

REGISTER_NAMED_TEST( "CUtlLeanVector.NarrowIndexCapacity", CUtlLeanVector_NarrowIndexCapacity )
{
	// The allocation the memory allocator rounds up must not overflow the capacity bits of a narrow index.
	CUtlLeanVector< uint8, uint8 > vec;

	for ( int i = 0; i < 127; i++ )
	{
		vec.AddToTail( ( uint8 )i );
	}

	TEST_EQ( vec.Count(), 127 );
	TEST_EQ( vec.NumAllocated(), 127 );

	for ( int i = 0; i < 127; i++ )
	{
		TEST_EQ( vec[ i ], ( uint8 )i );
	}
}
