#include "common/assert.h"
#include "common/macros.h"

#include <tier0/platform.h>
#include <tier0/lzss.h>
#include <tier0/blake3hash.h>
#include <tier0/blockcompress.h>
#include <tier0/checksum_sha1.h>

#include <cstring>

// Writes the bytes as lowercase hex, for comparing digests with the reference values.
static void BytesToHex( const uint8 *pBytes, size_t nCount, char *pszOut )
{
	static const char s_szDigits[] = "0123456789abcdef";

	for ( size_t i = 0; i < nCount; i++ )
	{
		pszOut[ i * 2 ] = s_szDigits[ pBytes[ i ] >> 4 ];
		pszOut[ i * 2 + 1 ] = s_szDigits[ pBytes[ i ] & 15 ];
	}

	pszOut[ nCount * 2 ] = '\0';
}

REGISTER_NAMED_TEST( "Blake3.EmptyInputGivesTheReferenceDigest", Blake3_EmptyInputGivesTheReferenceDigest )
{
	// The digest of no data from the BLAKE3 specification.
	uint8 digest[ 32 ];
	CBlake3HashContext::ProcessSingleBuffer( "", 0, digest, sizeof( digest ) );

	char szHex[ 65 ];
	BytesToHex( digest, sizeof( digest ), szHex );
	TEST_EQ( strcmp( szHex, "af1349b9f5f9a1a6a0404dea36dcc9499bcb25c9adc112b7cc9a93cae41f3262" ), 0 );
}

REGISTER_NAMED_TEST( "Blake3.HashingInPiecesGivesTheSameDigest", Blake3_HashingInPiecesGivesTheSameDigest )
{
	const char szText[] = "The quick brown fox jumps over the lazy dog";
	const size_t nLength = sizeof( szText ) - 1;

	uint8 wholeDigest[ 32 ];
	CBlake3HashContext::ProcessSingleBuffer( szText, nLength, wholeDigest, sizeof( wholeDigest ) );

	// Feed the same text in three uneven pieces.
	CBlake3HashContext context;
	context.Update( szText, 4 );
	context.Update( szText + 4, 15 );
	context.Update( szText + 19, nLength - 19 );

	uint8 piecesDigest[ 32 ];
	context.Finalize( piecesDigest, sizeof( piecesDigest ) );

	TEST_EQ( memcmp( wholeDigest, piecesDigest, sizeof( wholeDigest ) ), 0 );
}

REGISTER_NAMED_TEST( "Blake3.SignedHashOnlyAuthenticatesWithItsKey", Blake3_SignedHashOnlyAuthenticatesWithItsKey )
{
	const char szData[] = "save game payload";

	Blake3SignedHash_t hash = Blake3SignedHash_t::CreateUnsignedHash( szData, sizeof( szData ) );

	// The unsigned hash is the plain digest of the data.
	uint8 digest[ 32 ];
	CBlake3HashContext::ProcessSingleBuffer( szData, sizeof( szData ), digest, sizeof( digest ) );
	TEST_EQ( memcmp( hash.m_Hash, digest, sizeof( digest ) ), 0 );

	hash.SignHash( "secret" );

	TEST_TRUE( hash.Authenticate( "secret" ) );
	TEST_FALSE( hash.Authenticate( "another secret" ) );

	// Changing the hash breaks the signature.
	hash.m_Hash[ 0 ] ^= 1;
	TEST_FALSE( hash.Authenticate( "secret" ) );
}

REGISTER_NAMED_TEST( "SHA1.DigestIsWrittenAsLowercaseHex", SHA1_DigestIsWrittenAsLowercaseHex )
{
	// The SHA-1 of "abc" from FIPS 180-1.
	const SHADigest_t digest =
	{
		0xa9, 0x99, 0x3e, 0x36, 0x47, 0x06, 0x81, 0x6a, 0xba, 0x3e,
		0x25, 0x71, 0x78, 0x50, 0xc2, 0x6c, 0x9c, 0xd0, 0xd8, 0x9d,
	};

	CSHAStr hashString( digest );
	TEST_EQ( strcmp( hashString.Get(), "a9993e364706816aba3e25717850c26c9cd0d89d" ), 0 );
}

REGISTER_NAMED_TEST( "SHA1.HashesTheData", SHA1_HashesTheData )
{
	// The SHA-1 of "abc" from FIPS 180-1.
	const SHADigest_t expected =
	{
		0xa9, 0x99, 0x3e, 0x36, 0x47, 0x06, 0x81, 0x6a, 0xba, 0x3e,
		0x25, 0x71, 0x78, 0x50, 0xc2, 0x6c, 0x9c, 0xd0, 0xd8, 0x9d,
	};

	CSHA1 sha1;
	sha1.Update( "abc", 3 );
	sha1.Final();

	SHADigest_t digest;
	sha1.GetHash( digest );
	TEST_EQ( memcmp( digest, expected, k_cubHash ), 0 );

	// Unlike CSHAStr, tier0 writes the hex digits in uppercase.
	char szHex[ k_cchHash ];
	sha1.GetHashHex( szHex, sizeof( szHex ) );
	TEST_EQ( strcmp( szHex, "A9993E364706816ABA3E25717850C26C9CD0D89D" ), 0 );

	// The report appends the bytes separated by spaces.
	char szReport[ 80 ] = "";
	sha1.ReportHash( szReport );
	TEST_EQ( strncmp( szReport, "A9 99 3E 36 47", 14 ), 0 );
	TEST_EQ( strlen( szReport ), static_cast< size_t >( 59 ) );

	char szDigits[ 80 ] = "";
	sha1.ReportHash( szDigits, CSHA1::REPORT_DIGIT );
	TEST_EQ( strncmp( szDigits, "169 153 62 54", 13 ), 0 );

	// A null report buffer is ignored
	sha1.ReportHash( nullptr );
}

REGISTER_NAMED_TEST( "SHA1.ResetStartsANewHash", SHA1_ResetStartsANewHash )
{
	SHADigest_t first, second;

	{
		CSHA1 sha1;
		sha1.Update( "abc", 3 );
		sha1.Final();
		sha1.GetHash( first );
	}

	// Data hashed before a reset does not change the digest.
	CSHA1 sha1;
	sha1.Update( "garbage", 7 );
	sha1.Reset();
	sha1.Update( "abc", 3 );
	sha1.Final();
	sha1.GetHash( second );

	TEST_EQ( memcmp( first, second, k_cubHash ), 0 );

	SHADigest_t generated;
	GenerateHash( generated, "abc", 3 );
	TEST_EQ( memcmp( first, generated, k_cubHash ), 0 );
}

REGISTER_NAMED_TEST( "BlockCompress.RoundTripRestoresTheData", BlockCompress_RoundTripRestoresTheData )
{
	// Repetitive data that compresses well.
	uint8 source[ 4096 ];

	for ( int i = 0; i < 4096; i++ )
	{
		source[ i ] = static_cast< uint8 >( "source sdk "[ i % 11 ] );
	}

	uint8 compressed[ 8192 ];
	const uint32 nCompressedSize = CBlockCompress::FastCompress( source, sizeof( source ), compressed, sizeof( compressed ) );

	TEST_TRUE( nCompressedSize > 0 );
	TEST_TRUE( nCompressedSize < sizeof( source ) );

	uint8 restored[ 4096 ];
	const uint32 nRestoredSize = CBlockCompress::FastDecompress( compressed, nCompressedSize, restored, sizeof( restored ) );

	TEST_EQ( nRestoredSize, static_cast< uint32 >( sizeof( source ) ) );
	TEST_EQ( memcmp( source, restored, sizeof( source ) ), 0 );
}

REGISTER_NAMED_TEST( "LZSS.CompressRoundTrip", LZSS_CompressRoundTrip )
{
	unsigned char input[ 4096 ];

	for ( int i = 0; i < ( int )sizeof( input ); i++ )
	{
		input[ i ] = ( unsigned char )( ( i * 7 ) % 61 );
	}

	// The fast mode compares fewer earlier matches, both decompress to the input.
	for ( int nFast = 0; nFast < 2; nFast++ )
	{
		CLZSS lzss;
		unsigned int nCompressedSize = 0;
		unsigned char *pCompressed = lzss.Compress( input, sizeof( input ), &nCompressedSize, nFast != 0 );

		TEST_TRUE( pCompressed != NULL );
		TEST_TRUE( lzss.IsCompressed( pCompressed ) );
		TEST_TRUE( nCompressedSize < sizeof( input ) );
		TEST_EQ( lzss.GetActualSize( pCompressed ), ( unsigned int )sizeof( input ) );

		unsigned char output[ sizeof( input ) ];
		TEST_EQ( lzss.SafeUncompress( pCompressed, output, sizeof( output ) ), ( unsigned int )sizeof( input ) );
		TEST_EQ( memcmp( input, output, sizeof( input ) ), 0 );

		MemAlloc_Free( pCompressed );
	}
}
