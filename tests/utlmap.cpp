#include "common/assert.h"
#include "common/macros.h"

#include <tier1/utlmap.h>

#include <type_traits>
#include <utility>

REGISTER_NAMED_TEST( "CUtlMap.InsertFindRemove", CUtlMap_InsertFindRemove )
{
	// Maps should insert, find and remove key/value pairs by ordered key lookup.
	CUtlMap< int, int > map;

	TEST_EQ( map.Count(), 0u );

	const auto iTwo = map.Insert( 2, 20 );
	const auto iOne = map.Insert( 1, 10 );

	TEST_TRUE( map.IsValidIndex( iTwo ) );
	TEST_TRUE( map.IsValidIndex( iOne ) );
	TEST_EQ( map.Count(), 2u );
	TEST_EQ( map[map.Find( 1 )], 10 );
	TEST_EQ( map[map.Find( 2 )], 20 );
	TEST_EQ( map.Find( 3 ), map.InvalidIndex() );

	TEST_TRUE( map.Remove( 1 ) );
	TEST_EQ( map.Count(), 1u );
	TEST_EQ( map.Find( 1 ), map.InvalidIndex() );
}

REGISTER_NAMED_TEST( "CUtlMap.InsertOrReplace", CUtlMap_InsertOrReplace )
{
	// Replacing an existing key should update the element without duplicating entries.
	CUtlMap< int, int > map;

	map.InsertOrReplace( 5, 50 );
	map.InsertOrReplace( 5, 55 );

	TEST_EQ( map.Count(), 1u );
	TEST_EQ( map[map.Find( 5 )], 55 );
}

REGISTER_NAMED_TEST( "CUtlMap.InorderIteration", CUtlMap_InorderIteration )
{
	// In-order iteration should visit keys in sorted order.
	CUtlMap< int, int > map;
	map.Insert( 3, 30 );
	map.Insert( 1, 10 );
	map.Insert( 2, 20 );

	int nExpectedKey = 1;

	FOR_EACH_MAP( map, i )
	{
		TEST_EQ( map.Key( i ), nExpectedKey );
		TEST_EQ( map.Element( i ), nExpectedKey * 10 );
		++nExpectedKey;
	}

	TEST_EQ( nExpectedKey, 4 );
}

REGISTER_NAMED_TEST( "CUtlMap.FindClosestAndReinsert", CUtlMap_FindClosestAndReinsert )
{
	// Closest-key lookup and reinsertion should preserve tree ordering.
	CUtlMap< int, int > map;

	map.EnsureCapacity( 4 );
	map.Insert( 10, 100 );
	map.Insert( 20, 200 );
	map.Insert( 30, 300 );

	const int iGE = map.FindClosest( 15, k_EGreaterThanOrEqualTo );
	const int iLE = map.FindClosest( 25, k_ELessThanOrEqualTo );

	TEST_EQ( map.Key( iGE ), 20 );
	TEST_EQ( map.Key( iLE ), 20 );

	const int iThirty = map.Find( 30 );

	map.Reinsert( 5, iThirty );
	TEST_EQ( map.Key( map.FirstInorder() ), 5 );
}

REGISTER_NAMED_TEST( "CUtlMap.FindElementSameKeyAndRemoveAll", CUtlMap_FindElementSameKeyAndRemoveAll )
{
	// Duplicate-key traversal should expose all matching elements before reset.
	CUtlMap< int, int > map;

	map.Insert( 1, 10 );
	map.Insert( 2, 20 );
	map.InsertWithDupes( 2, 22 );

	const int nDefaultValue = -1;

	TEST_TRUE( map.Find( 1 ) != map.InvalidIndex() );
	TEST_EQ( map.FindElement( 2, nDefaultValue ), 20 );

	const int iFirst = map.FindFirst( 2 );
	TEST_TRUE( map.IsValidIndex( iFirst ) );
	TEST_EQ( map.Key( iFirst ), 2 );
	TEST_EQ( map.Element( iFirst ), 20 );

	const int iNext = map.NextInorderSameKey( iFirst );
	TEST_TRUE( map.IsValidIndex( iNext ) );
	TEST_EQ( map.Key( iNext ), 2 );
	TEST_EQ( map.Element( iNext ), 22 );

	map.RemoveAll();
	TEST_EQ( map.Count(), 0u );
}

REGISTER_NAMED_TEST( "CUtlMap.CopyAndMove", CUtlMap_CopyAndMove )
{
	// Copy and move operations should preserve the ordered key/value payload.
	CUtlMap< int, int > map;

	map.Insert( 1, 10 );
	map.Insert( 3, 30 );
	map.Insert( 2, 20 );

	CUtlMap< int, int > copy( map );

	TEST_EQ( copy.Count(), 3u );
	TEST_EQ( copy.Key( copy.FirstInorder() ), 1 );
	TEST_EQ( copy.Element( copy.Find( 1 ) ), 10 );
	TEST_EQ( copy.Element( copy.Find( 2 ) ), 20 );
	TEST_EQ( copy.Element( copy.Find( 3 ) ), 30 );

	CUtlMap< int, int > assigned;

	assigned = map;
	TEST_EQ( assigned.Count(), 3u );
	TEST_EQ( assigned.Element( assigned.Find( 1 ) ), 10 );
	TEST_EQ( assigned.Element( assigned.Find( 2 ) ), 20 );
	TEST_EQ( assigned.Element( assigned.Find( 3 ) ), 30 );

	CUtlMap< int, int > moved( Move( map ) );

	TEST_EQ( moved.Count(), 3u );
	TEST_EQ( moved.Element( moved.Find( 1 ) ), 10 );
	TEST_EQ( moved.Element( moved.Find( 2 ) ), 20 );
	TEST_EQ( moved.Element( moved.Find( 3 ) ), 30 );

	CUtlMap< int, int > moveAssigned;

	moveAssigned = Move( moved );
	TEST_EQ( moveAssigned.Count(), 3u );
	TEST_EQ( moveAssigned.Element( moveAssigned.Find( 1 ) ), 10 );
	TEST_EQ( moveAssigned.Element( moveAssigned.Find( 2 ) ), 20 );
	TEST_EQ( moveAssigned.Element( moveAssigned.Find( 3 ) ), 30 );
}
