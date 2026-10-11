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

struct SortVectorKeyed_t
{
	int m_nKey;
	int m_nPayload;
};

class CSortVectorKeyedLess
{
public:
	bool Less( const SortVectorKeyed_t &lhs, const SortVectorKeyed_t &rhs, void * ) { return lhs.m_nKey < rhs.m_nKey; }
};

REGISTER_NAMED_TEST( "CUtlSortVector.FindGreaterAndInsertAfterEqual", CUtlSortVector_FindGreaterAndInsertAfterEqual )
{
	// Equal keys should be appended after their peers and removal should use the binary search.
	CUtlSortVector< int > vec;

	vec.Insert( 10 );
	vec.Insert( 20 );
	vec.Insert( 30 );

	TEST_EQ( vec.FindGreater( 5 ), 0 );
	TEST_EQ( vec.FindGreater( 10 ), 1 );
	TEST_EQ( vec.FindGreater( 25 ), 2 );
	TEST_EQ( vec.FindGreater( 30 ), vec.Count() );

	TEST_TRUE( vec.HasElement( 20 ) );
	TEST_FALSE( vec.HasElement( 25 ) );
	TEST_TRUE( vec.FindAndRemove( 20 ) );
	TEST_FALSE( vec.FindAndRemove( 20 ) );
	TEST_EQ( vec.Count(), 2 );
	TEST_FALSE( vec.HasElement( 20 ) );

	CUtlSortVector< SortVectorKeyed_t, CSortVectorKeyedLess > keyed;

	keyed.InsertAfterEqual( { 2, 0 } );
	keyed.InsertAfterEqual( { 1, 1 } );
	keyed.InsertAfterEqual( { 2, 2 } );
	TEST_EQ( keyed.InsertAfterEqual( { 2, 3 } ), 3 );
	keyed.InsertAfterEqual( { 3, 4 } );

	TEST_EQ( keyed.Count(), 5 );
	TEST_EQ( keyed[ 0 ].m_nPayload, 1 );
	TEST_EQ( keyed[ 1 ].m_nPayload, 0 );
	TEST_EQ( keyed[ 2 ].m_nPayload, 2 );
	TEST_EQ( keyed[ 3 ].m_nPayload, 3 );
	TEST_EQ( keyed[ 4 ].m_nPayload, 4 );
}
