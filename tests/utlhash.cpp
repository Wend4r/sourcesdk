#include "common/assert.h"
#include "common/macros.h"

#include <tier1/utlhash.h>

static bool CompareIntForHash( const int &lhs, const int &rhs )
{
	return lhs == rhs;
}

static unsigned int HashIntForHash( const int &value )
{
	return ( unsigned int )value;
}

REGISTER_NAMED_TEST( "CUtlHash.InsertFindRemove", CUtlHash_InsertFindRemove )
{
	// Hash handles should stay stable across duplicate inserts and removals.
	using IntHash_t = CUtlHash< int, bool ( * )( const int &, const int & ), unsigned int ( * )( const int & ) >;

	IntHash_t hash( 8, 0, 0, &CompareIntForHash, &HashIntForHash );

	TEST_EQ( hash.Count(), 0 );

	bool bDidInsert = false;
	const UtlHashHandle_t hTen = hash.Insert( 10, &bDidInsert );

	TEST_TRUE( bDidInsert );
	TEST_TRUE( hash.IsValidHandle( hTen ) );
	TEST_EQ( hash[hTen], 10 );
	TEST_EQ( hash.Count(), 1 );

	const UtlHashHandle_t hTenAgain = hash.Insert( 10, &bDidInsert );

	TEST_FALSE( bDidInsert );
	TEST_EQ( hTenAgain, hTen );
	TEST_EQ( hash.Count(), 1 );

	TEST_EQ( hash.Find( 10 ), hTen );
	TEST_EQ( hash.Find( 11 ), IntHash_t::InvalidHandle() );

	hash.Remove( hTen );
	TEST_EQ( hash.Count(), 0 );
}
