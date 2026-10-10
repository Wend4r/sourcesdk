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

REGISTER_NAMED_TEST( "CUtlMap.FindConditionAndFindOrInsert", CUtlMap_FindConditionAndFindOrInsert )
{
	// Map wrappers should expose the tree's find conditions and insert behaviors.
	CUtlMap< int, int > map;

	TEST_TRUE( map.IsEmpty() );

	map.Insert( 10, 100 );
	map.Insert( 20, 200 );
	map.Insert( 30, 300 );

	TEST_FALSE( map.IsEmpty() );
	TEST_EQ( map.Find( 25, EXACT_MATCH ), map.InvalidIndex() );
	TEST_EQ( map.Key( map.Find( 25, MATCH_OR_LESS ) ), 20 );
	TEST_EQ( map.Key( map.Find( 25, MATCH_OR_GREATER ) ), 30 );
	TEST_EQ( map.FindElement( 25, -1 ), -1 );
	TEST_EQ( map.FindElement( 25, -1, MATCH_OR_LESS ), 200 );
	TEST_NULL( map.FindGetPtr( 25 ) );
	TEST_EQ( *map.FindGetPtr( 25, MATCH_OR_GREATER ), 300 );
	TEST_EQ( map.FindFirstElement( 10, -1 ), 100 );
	TEST_EQ( map.FindFirstElement( 15, -1 ), -1 );
	TEST_EQ( map.FindClosestElement( 15, -1, k_EGreaterThan ), 200 );
	TEST_EQ( map.FindClosestElement( 35, -1, k_EGreaterThan ), -1 );

	bool bInserted = true;

	TEST_EQ( map.FindOrInsert( 10, 111, &bInserted ), map.Find( 10 ) );
	TEST_FALSE( bInserted );
	TEST_EQ( map.Element( map.Find( 10 ) ), 100 );

	int *pForty = map.FindOrInsertGetPtr( 40, 400, &bInserted );
	TEST_TRUE( bInserted );
	TEST_EQ( *pForty, 400 );

	int *pFifty = map.InsertGetPtr( 50, k_eInsertUpdateDupes );
	*pFifty = 500;
	TEST_EQ( map.InsertGetPtr( 50, k_eInsertUpdateDupes ), pFifty );
	TEST_EQ( map.Count(), 5u );

	map.InsertOrReplace( 20, 222 );
	TEST_EQ( map.Count(), 5u );
	TEST_EQ( map.Element( map.Find( 20 ) ), 222 );
	TEST_TRUE( map.HasElement( 40 ) );
}

REGISTER_NAMED_TEST( "CUtlMap.PurgeAndDeleteElements", CUtlMap_PurgeAndDeleteElements )
{
	// PurgeAndDeleteElements should delete pointer values and only purge non-pointer ones.
	CUtlMap< int, int * > mapPointers;

	mapPointers.Insert( 1, new int( 10 ) );
	mapPointers.Insert( 2, new int( 20 ) );
	mapPointers.PurgeAndDeleteElements();
	TEST_EQ( mapPointers.Count(), 0u );

	CUtlMap< int, int > mapValues;

	mapValues.Insert( 1, 10 );
	mapValues.PurgeAndDeleteElements();
	TEST_EQ( mapValues.Count(), 0u );
}
