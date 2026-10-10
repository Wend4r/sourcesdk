#include "common/assert.h"
#include "common/macros.h"

#include <tier1/utlhashmaplarge.h>

REGISTER_NAMED_TEST( "CUtlHashMapLarge.FindDuringRehash", CUtlHashMapLarge_FindDuringRehash )
{
	// Looking up a missing key while the buckets are still being migrated should stop at the smallest bucket count.
	CUtlHashMapLarge< int, int > map;

	for ( int i = 0; i < 16; i++ )
	{
		map.Insert( i, i * 10 );
	}

	map.EnsureCapacity( 256 );

	TEST_EQ( map.Find( 1000 ), map.InvalidIndex() );

	for ( int i = 0; i < 16; i++ )
	{
		auto iElem = map.Find( i );

		TEST_TRUE( map.IsValidIndex( iElem ) );
		TEST_EQ( map.Element( iElem ), i * 10 );
	}

	TEST_TRUE( map.Remove( 7 ) );
	TEST_FALSE( map.Remove( 7 ) );
	TEST_EQ( map.Count(), 15 );
	TEST_EQ( map.Find( 7 ), map.InvalidIndex() );
}

REGISTER_NAMED_TEST( "CUtlHashMapLarge.StringKeys", CUtlHashMapLarge_StringKeys )
{
	// String keys should match by their contents, not by their first character or their address.
	CUtlHashMapLarge< const char *, int > map;

	static char s_szKeys[ 64 ][ 8 ];

	for ( int i = 0; i < 64; i++ )
	{
		V_snprintf( s_szKeys[ i ], sizeof( s_szKeys[ i ] ), "a%d", i );
		map.Insert( s_szKeys[ i ], i );
	}

	// Keys sharing the first character and a bucket stay separate.
	TEST_EQ( map.Count(), 64 );

	for ( int i = 0; i < 64; i++ )
	{
		char szLookup[ 8 ];
		V_snprintf( szLookup, sizeof( szLookup ), "a%d", i );

		auto iElem = map.Find( szLookup );

		TEST_TRUE( map.IsValidIndex( iElem ) );
		TEST_EQ( map.Element( iElem ), i );
	}

	TEST_EQ( map.Find( "a64" ), map.InvalidIndex() );
	TEST_EQ( map.Find( "b0" ), map.InvalidIndex() );
}
