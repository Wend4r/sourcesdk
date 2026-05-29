#include "common/assert.h"
#include "common/macros.h"

#include <tier1/utlhashtable.h>

REGISTER_NAMED_TEST( "CUtlHashtable.InsertFindRemove", CUtlHashtable_InsertFindRemove )
{
	// Hashtable inserts should expose keys, values and duplicate detection.
	CUtlHashtable< int, int > table;

	bool bDidInsert = false;
	const UtlHashHandle_t hOne = table.Insert( 1, 10, &bDidInsert );

	TEST_TRUE( bDidInsert );
	TEST_TRUE( table.IsValidHandle( hOne ) );
	TEST_EQ( table.Count(), 1 );
	TEST_EQ( table.Key( hOne ), 1 );
	TEST_EQ( table.Element( hOne ), 10 );

	const UtlHashHandle_t hOneAgain = table.Insert( 1, 20, &bDidInsert );

	TEST_FALSE( bDidInsert );
	TEST_EQ( hOneAgain, hOne );
	TEST_EQ( table.Element( hOneAgain ), 10 );

	const UtlHashHandle_t hTwo = table.Insert( 2, 20, &bDidInsert );

	TEST_TRUE( bDidInsert );
	TEST_EQ( table.Count(), 2 );
	TEST_EQ( table.Find( 2 ), hTwo );
	TEST_TRUE( table.HasElement( 1 ) );

	TEST_TRUE( table.Remove( 1 ) );
	TEST_FALSE( table.HasElement( 1 ) );
	TEST_EQ( table.Count(), 1 );

	table.RemoveAll();
	TEST_EQ( table.Count(), 0 );
}

REGISTER_NAMED_TEST( "CUtlHashtable.GetSwapCompactPurge", CUtlHashtable_GetSwapCompactPurge )
{
	// Lookup helpers, swaps and purge should leave the table in a coherent state.
	CUtlHashtable< int, int > table;

	table.Insert( 10, 100 );
	table.Insert( 20, 200 );

	TEST_EQ( table.Get( 10, -1 ), 100 );
	TEST_EQ( table.Get( 30, -1 ), -1 );
	TEST_NOT_NULL( table.GetPtr( 20 ) );
	TEST_NULL( table.GetPtr( 30 ) );

	UtlHashHandle_t hNext = table.RemoveAndAdvance( table.FirstHandle() );

	TEST_TRUE( hNext == table.InvalidHandle() || table.IsValidHandle( hNext ) );

	CUtlHashtable< int, int > other;

	other.Insert( 99, 999 );
	table.Swap( other );
	TEST_TRUE( table.HasElement( 99 ) );
	TEST_FALSE( other.HasElement( 99 ) );

	table.Compact( true );
	table.Purge();
	TEST_EQ( table.Count(), 0 );
}
