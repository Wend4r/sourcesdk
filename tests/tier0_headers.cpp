#include "common/assert.h"
#include "common/macros.h"

#include <tier0/checksum_md5.h>

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
