#include "common/assert.h"
#include "common/macros.h"

#include <tier1/utlpair.h>

REGISTER_NAMED_TEST( "CUtlPair.CompareAndMake", CUtlPair_CompareAndMake )
{
	// Pair helpers should compare and construct tuples with the expected ordering.
	CUtlPair< int, int > first( 1, 2 );
	CUtlPair< int, int > same = MakeUtlPair( 1, 2 );
	CUtlPair< int, int > larger = MakeUtlPair( 2, 1 );

	TEST_TRUE( first == same );
	TEST_FALSE( first == larger );
	TEST_TRUE( first < larger );
	TEST_EQ( first.first, 1 );
	TEST_EQ( first.second, 2 );
}
