#include "common/assert.h"
#include "common/macros.h"

#include <tier0/utlstringtoken.h>

REGISTER_NAMED_TEST( "CUtlStringToken.HashAndCompare", CUtlStringToken_HashAndCompare )
{
	// String tokens should hash case-insensitively and compare against text and peers.
	CUtlStringToken token( "Alpha" );
	CUtlStringToken sameCaseInsensitive( "alpha" );
	CUtlStringToken other( "beta" );

	TEST_EQ( token.GetHashCode(), sameCaseInsensitive.GetHashCode() );
	TEST_TRUE( token == "ALPHA" );
	TEST_TRUE( token == sameCaseInsensitive );
	TEST_TRUE( token != other );

	const uint32 nHash = MakeStringToken< true, false >( "Alpha" );

	TEST_EQ( nHash, token.GetHashCode() );
}
