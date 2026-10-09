#include "common/assert.h"
#include "common/macros.h"

#include <tier0/checksum_md5.h>
#include <tier0/murmurhash3.h>
#include <tier0/utlstring.h>
#include <tier1/utlhashmaplarge.h>

REGISTER_NAMED_TEST( "Smoke.Tier0Headers.ChecksumMD5", Smoke_Tier0Headers_ChecksumMD5 )
{
	// RFC 1321 test vector: MD5( "abc" ) = 900150983cd24fb0d6963f7d28e17f72.
	static const unsigned char s_Expected[ MD5_DIGEST_LENGTH ] = { 0x90, 0x01, 0x50, 0x98, 0x3c, 0xd2, 0x4f, 0xb0, 0xd6, 0x96, 0x3f, 0x7d, 0x28, 0xe1, 0x7f, 0x72 };

	MD5Value_t md5Expected;
	V_memcpy( md5Expected.bits, s_Expected, sizeof( md5Expected.bits ) );

	MD5Value_t md5Result;
	md5Result.Zero();
	TEST_TRUE( md5Result.IsZero() );

	MD5_ProcessSingleBuffer( "abc", 3, md5Result );
	TEST_FALSE( md5Result.IsZero() );
	TEST_TRUE( md5Result == md5Expected );
	TEST_FALSE( md5Result != md5Expected );
}

REGISTER_NAMED_TEST( "Smoke.Tier0Headers.MurmurHash3", Smoke_Tier0Headers_MurmurHash3 )
{
	// Reference MurmurHash3 x86_32 test vectors.
	TEST_EQ( MurmurHash3_32( "", 0, 0 ), 0u );
	TEST_EQ( MurmurHash3_32( "Hello, world!", 13, 1234 ), 0xfaf6cdb3u );
	TEST_EQ( MurmurHash3_32( "The quick brown fox jumps over the lazy dog", 43, 0 ), 0x2e4ff723u );

	TEST_EQ( MurmurHash3String( "source2" ), 0x61dc9e83u );
	TEST_EQ( MurmurHash3StringCaseless( "Source2" ), MurmurHash3StringCaseless( "SOURCE2" ) );
	TEST_EQ( MurmurHash3StringCaseless( "abcde" ), MurmurHash3String( "ABCDE" ) );
}

REGISTER_NAMED_TEST( "Smoke.Tier0Headers.UtlHashMapLargeDefaultHasher", Smoke_Tier0Headers_UtlHashMapLargeDefaultHasher )
{
	// The default hasher must link without a tier0 export.
	CUtlHashMapLarge< CUtlString, int > map;

	map.Insert( CUtlString( "alpha" ), 1 );
	map.Insert( CUtlString( "beta" ), 2 );

	TEST_EQ( map.Count(), 2 );

	int iBeta = map.Find( CUtlString( "beta" ) );

	TEST_NE( iBeta, map.InvalidIndex() );
	TEST_EQ( map[ iBeta ], 2 );
	TEST_EQ( map.Find( CUtlString( "gamma" ) ), map.InvalidIndex() );
}
