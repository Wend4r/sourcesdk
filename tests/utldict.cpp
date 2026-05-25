#include "common/assert.h"
#include "common/macros.h"

#include <tier1/utldict.h>

#include <string.h>

REGISTER_NAMED_TEST( "CUtlDict.InsertFindRemove", CUtlDict_InsertFindRemove )
{
	// Dictionaries should insert, rename, find and remove string-keyed entries.
	CUtlDict< int > dict;

	const int nAlpha = dict.Insert( "alpha", 10 );
	const int nBeta = dict.Insert( "beta", 20 );

	TEST_EQ( dict.Count(), 2u );
	TEST_TRUE( dict.IsValidIndex( nAlpha ) );
	TEST_TRUE( dict.IsValidIndex( nBeta ) );
	TEST_EQ( dict.Find( "alpha" ), nAlpha );
	TEST_EQ( dict[nBeta], 20 );
	TEST_EQ( strcmp( dict.GetElementName( nAlpha ), "alpha" ), 0 );

	dict.SetElementName( nAlpha, "gamma" );
	TEST_EQ( dict.Find( "alpha" ), dict.InvalidIndex() );
	TEST_EQ( dict.Find( "gamma" ), nAlpha );

	dict.Remove( "beta" );
	TEST_EQ( dict.Count(), 1u );
	TEST_EQ( dict.Find( "beta" ), dict.InvalidIndex() );

	dict.RemoveAll();
	TEST_EQ( dict.Count(), 0u );
}
