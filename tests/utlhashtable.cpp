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

enum HashtableTestEnum_t
{
	HASHTABLE_TEST_ALPHA = 1,
	HASHTABLE_TEST_BETA = 2,
};

enum class HashtableTestEnumClass_t : uint64
{
	Alpha = 1ull << 40,
	Beta = 2ull << 40,
};

REGISTER_NAMED_TEST( "CUtlHashtable.EnumAndPairKeys", CUtlHashtable_EnumAndPairKeys )
{
	// Enums and std::pair keys should get default hash functors.
	TEST_EQ( DefaultHashFunctor< HashtableTestEnum_t >()( HASHTABLE_TEST_BETA ), DefaultHashFunctor< int >()( 2 ) );
	TEST_EQ( DefaultHashFunctor< const HashtableTestEnum_t >()( HASHTABLE_TEST_BETA ), DefaultHashFunctor< int >()( 2 ) );
	TEST_TRUE( DefaultHashFunctor< HashtableTestEnumClass_t >()( HashtableTestEnumClass_t::Alpha ) != DefaultHashFunctor< HashtableTestEnumClass_t >()( HashtableTestEnumClass_t::Beta ) );
	using PairHash_t = DefaultHashFunctor< std::pair< int, int > >;

	TEST_TRUE( PairHash_t()( std::make_pair( 1, 2 ) ) != PairHash_t()( std::make_pair( 2, 1 ) ) );

	CUtlHashtable< HashtableTestEnum_t, int > enumTable;

	enumTable.Insert( HASHTABLE_TEST_ALPHA, 10 );
	enumTable.Insert( HASHTABLE_TEST_BETA, 20 );

	TEST_EQ( enumTable.Element( enumTable.Find( HASHTABLE_TEST_BETA ) ), 20 );

	CUtlHashtable< std::pair< int, int >, int > pairTable;

	pairTable.Insert( std::make_pair( 1, 2 ), 12 );
	pairTable.Insert( std::make_pair( 2, 1 ), 21 );

	TEST_EQ( pairTable.Count(), 2 );
	TEST_EQ( pairTable.Element( pairTable.Find( std::make_pair( 2, 1 ) ) ), 21 );
	TEST_FALSE( pairTable.HasElement( std::make_pair( 3, 3 ) ) );
}
