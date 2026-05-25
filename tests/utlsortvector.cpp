#include "common/assert.h"
#include "common/macros.h"

#include <tier1/UtlSortVector.h>

#include <type_traits>

REGISTER_NAMED_TEST( "CUtlSortVector.InsertFindRemove", CUtlSortVector_InsertFindRemove )
{
	// Sorted vectors should insert in order and support lookup and removal.
	CUtlSortVector< int > vec;

	TEST_EQ( vec.Insert( 3 ), 0 );
	TEST_EQ( vec.Insert( 1 ), 0 );
	TEST_EQ( vec.Insert( 2 ), 1 );

	TEST_EQ( vec.Count(), 3 );
	TEST_EQ( vec[0], 1 );
	TEST_EQ( vec[1], 2 );
	TEST_EQ( vec[2], 3 );
	TEST_EQ( vec.Find( 2 ), 1 );
	TEST_EQ( vec.FindLessOrEqual( 2 ), 1 );
	TEST_EQ( vec.FindLess( 2 ), 0 );

	TEST_EQ( vec.InsertIfNotFound( 2 ), 1 );
	TEST_EQ( vec.Count(), 3 );

	static_cast< CUtlVector< int > & >( vec ).Remove( vec.Find( 2 ) );
	TEST_EQ( vec.Count(), 2 );
	TEST_EQ( vec.Find( 2 ), -1 );
}

REGISTER_NAMED_TEST( "CUtlSortVector.RedoSort", CUtlSortVector_RedoSort )
{
	// Deferred sorting should reorder previously unsorted inserts on demand.
	CUtlSortVector< int > vec;

	vec.InsertNoSort( 4 );
	vec.InsertNoSort( 1 );
	vec.InsertNoSort( 3 );

	TEST_EQ( vec.FindUnsorted( 1 ), 1 );
	vec.RedoSort();

	TEST_EQ( vec[0], 1 );
	TEST_EQ( vec[1], 3 );
	TEST_EQ( vec[2], 4 );
}

REGISTER_NAMED_TEST( "CUtlSortVector.CopyMoveTraits", CUtlSortVector_CopyMoveTraits )
{
	// Trait checks capture the current public copy and move constraints.
	TEST_FALSE( ( std::is_copy_constructible_v< CUtlSortVector< int > > ) );
	TEST_FALSE( ( std::is_move_constructible_v< CUtlSortVector< int > > ) );
}
