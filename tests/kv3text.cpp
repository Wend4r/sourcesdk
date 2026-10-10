#include "common/assert.h"
#include "common/macros.h"

#include <kv3lib/kv3formats.h>
#include <kv3lib/kv3text.h>
#include <tier0/keyvalues3.h>
#include <tier0/strtools.h>
#include <tier0/utlbuffer.h>
#include <tier0/utlstring.h>
#include <kv3lib/keyvalues3.h>

#include <fstream>
#include <iterator>
#include <string>

static std::string ReadCodecTestFile( const char *pFilename )
{
	std::ifstream file( pFilename, std::ios::in | std::ios::binary );

	TEST_TRUE( file.is_open() );

	return std::string( ( std::istreambuf_iterator< char >( file ) ), std::istreambuf_iterator< char >() );
}

static std::string SaveText( const KeyValues3 &kv )
{
	CUtlBuffer buffer;

	TEST_TRUE( KV3Codec_SaveText( &kv, buffer ) );

	return std::string( static_cast< const char * >( buffer.Base() ), buffer.TellPut() );
}

static void LoadText( KeyValues3 &kv, const std::string &sText, const char *pszName )
{
	CUtlString sError;

	if ( !KV3Codec_LoadText( &kv, &sError, sText.c_str(), static_cast< int >( sText.size() ), pszName ) )
	{
		TEST_EQ( std::string( sError.Get() ), std::string() );
	}
}

static void ClearMultilineFlags( KeyValues3 *pKV )
{
	pKV->SetFlag( KEYVALUES3_FLAG_MULTILINE_STRING, false );

	if ( pKV->IsKV3Array() )
	{
		for ( int i = 0; i < pKV->GetArrayElementCount(); ++i )
			ClearMultilineFlags( pKV->GetArrayElement( i ) );
	}
	else if ( pKV->IsTable() )
	{
		for ( KV3MemberId_t id = 0; id < pKV->GetMemberCount(); ++id )
			ClearMultilineFlags( pKV->GetMember( id ) );
	}
}

static void ValidateValueMembers( const KeyValues3 &root )
{
	TEST_TRUE( root.IsTable() );
	TEST_EQ( root[ "answer" ].GetInt(), 42 );
	TEST_EQ( V_strcmp( root[ "name" ].GetString(), "kv3" ), 0 );
	TEST_TRUE( root[ "null_member" ].IsNull() );
	TEST_NOT_NULL( root.FindMember( "null_member" ) );
	TEST_TRUE( root[ "bool_member" ].GetBool() );
	TEST_EQ( root[ "int_member" ].GetTypeEx(), KV3_TYPEEX_INT );
	TEST_EQ( root[ "int_member" ].GetInt(), -42 );
	TEST_EQ( root[ "uint_member" ].GetTypeEx(), KV3_TYPEEX_UINT );
	TEST_EQ( root[ "uint_member" ].GetUInt64(), 18446744073709551615ull );
	TEST_EQ( root[ "double_member" ].GetDouble(), 12.5 );
	TEST_EQ( V_strcmp( root[ "string_member" ].GetString(), "this string is longer than seven bytes" ), 0 );

	const KeyValues3 &blob = root[ "binary_blob_member" ];

	TEST_EQ( blob.GetBinaryBlobSize(), 4 );
	TEST_EQ( blob.GetBinaryBlobByte( 1 ), 0x7F );
	TEST_EQ( blob.GetBinaryBlobByte( 3 ), 0xFF );

	const KeyValues3 &array = root[ "array_member" ];

	TEST_EQ( array.GetArrayElementCount(), 7 );
	TEST_TRUE( array[ 0 ].IsNull() );
	TEST_EQ( array[ 2 ].GetInt(), -1 );
	TEST_EQ( V_strcmp( array[ 5 ].GetString(), "array" ), 0 );
	TEST_TRUE( array[ 6 ][ "nested" ].GetBool() );

	TEST_EQ( root[ "table_member" ][ "child_int" ].GetInt(), 7 );
	TEST_EQ( V_strcmp( root[ "table_member" ][ "child_object" ][ "name" ].GetString(), "nested" ), 0 );
	TEST_EQ( root[ "table_member" ][ "child_array" ][ 1 ].GetInt(), 9 );
}

REGISTER_NAMED_TEST( "KV3Codec.LoadText.Heap", KV3Codec_LoadText_Heap )
{
	KeyValues3 kv;

	LoadText( kv, ReadCodecTestFile( SOURCESDK_KEYVALUES3_DATA_DIR "/value.kv3" ), "value.kv3" );
	ValidateValueMembers( kv );
}

REGISTER_NAMED_TEST( "KV3Codec.LoadText.Arena", KV3Codec_LoadText_Arena )
{
	CKV3Arena arena;

	LoadText( *arena.Root(), ReadCodecTestFile( SOURCESDK_KEYVALUES3_DATA_DIR "/value.kv3" ), "value.kv3" );
	ValidateValueMembers( *arena.Root() );

	// Long strings point into the arena symbol table
	TEST_EQ( arena.Root()->FindMember( "string_member" )->GetTypeEx(), KV3_TYPEEX_STRING_EXTERN );
}

REGISTER_NAMED_TEST( "KV3Codec.LoadText.Example", KV3Codec_LoadText_Example )
{
	CKV3Arena arena;
	const KeyValues3 &root = *arena.Root();

	LoadText( *arena.Root(), ReadCodecTestFile( SOURCESDK_KEYVALUES3_DATA_DIR "/example.kv3" ), "example.kv3" );

	TEST_EQ( root[ "stringThatIsAResourceReference" ].GetSubType(), KV3_SUBTYPE_RESOURCE );
	TEST_EQ( root[ "localizeValue" ].GetSubType(), KV3_SUBTYPE_LOCALIZE );
	TEST_EQ( V_strcmp( root[ "subclassValue" ][ "name" ].GetString(), "derived" ), 0 );
	TEST_EQ( V_strcmp( root[ "multiLineStringValue" ].GetString(), "First line of a multi-line string literal.\nSecond line of a multi-line string literal." ), 0 );
	TEST_TRUE( root[ "multiLineStringValue" ].HasFlag( KEYVALUES3_FLAG_MULTILINE_STRING ) );
	TEST_EQ( root[ "arrayValue" ].GetArrayElementCount(), 2 );
	TEST_EQ( root[ "doubleValue" ].GetDouble(), 64.0 );
}

// Save, load and save again gives the same text
REGISTER_NAMED_TEST( "KV3Codec.Text.RoundTrip", KV3Codec_Text_RoundTrip )
{
	for ( const char *pszFile : { "/value.kv3", "/example.kv3", "/typeex.kv3" } )
	{
		CKV3Arena first, second;

		LoadText( *first.Root(), ReadCodecTestFile( ( std::string( SOURCESDK_KEYVALUES3_DATA_DIR ) + pszFile ).c_str() ), pszFile );

		const std::string sFirst = SaveText( *first.Root() );

		LoadText( *second.Root(), sFirst, pszFile );

		TEST_EQ( SaveText( *second.Root() ), sFirst );
	}
}

REGISTER_NAMED_TEST( "KV3Codec.Binary.RoundTrip", KV3Codec_Binary_RoundTrip )
{
	CKV3Arena text, binary;
	KeyValues3 heap;

	LoadText( *text.Root(), ReadCodecTestFile( SOURCESDK_KEYVALUES3_DATA_DIR "/example.kv3" ), "example.kv3" );

	CUtlBuffer buffer;
	CUtlString sError;

	TEST_TRUE( KV3Codec_SaveBinary( text.Root(), buffer ) );
	TEST_TRUE( KV3Codec_Load( binary.Root(), &sError, buffer.Base(), buffer.TellPut(), "example.bin" ) );
	TEST_TRUE( KV3Codec_LoadBinary( &heap, &sError, buffer.Base(), buffer.TellPut(), "example.bin" ) );

	const std::string sExpected = SaveText( *text.Root() );

	TEST_EQ( SaveText( *binary.Root() ), sExpected );
	TEST_EQ( SaveText( heap ), sExpected );
}

REGISTER_NAMED_TEST( "KV3Codec.Text.Errors", KV3Codec_Text_Errors )
{
	struct ErrorCase_t
	{
		const char *m_pszInput;
		const char *m_pszError;
	};

	const ErrorCase_t cases[] =
	{
		{ "{\n\tname = \n}", "case.kv3:3:1: unexpected '}'" },
		{ "{\n\tname = \"open\n}", "case.kv3:2:9: newline in a string, use \"\"\" for multi-line strings" },
		{ "{ a = [ 1 2 ] }", "case.kv3:1:11: expected ',' or ']', got '2'" },
		{ "{ a = 1 } x", "case.kv3:1:11: unexpected 'x' after the root value" },
		{ "{ a = maybe }", "case.kv3:1:7: unknown value 'maybe'" },
		{ "{ a = 99999999999999999999 }", "case.kv3:1:7: integer '99999999999999999999' is out of range" },
	};

	for ( const ErrorCase_t &errorCase : cases )
	{
		KeyValues3 kv;
		CUtlString sError;

		TEST_FALSE( KV3Codec_LoadText( &kv, &sError, errorCase.m_pszInput, V_strlen( errorCase.m_pszInput ), "case.kv3" ) );
		TEST_EQ( std::string( sError.Get() ), std::string( errorCase.m_pszError ) );
	}
}

// The text written by kv3lib reads back through the tier0 LoadKV3 and the other way round
// Not on macOS: CS2 has no macOS build, lib/osx64 holds the Dota 2 tier0, whose text KV3 crashes on the CS2 layout
#if !defined( PLATFORM_OSX )
REGISTER_NAMED_TEST( "KV3Codec.Text.Tier0", KV3Codec_Text_Tier0 )
{
	for ( const char *pszFile : { "/value.kv3", "/example.kv3" } )
	{
		const std::string sSource = ReadCodecTestFile( ( std::string( SOURCESDK_KEYVALUES3_DATA_DIR ) + pszFile ).c_str() );

		CKV3Arena ours;

		LoadText( *ours.Root(), sSource, pszFile );

		const std::string sOurs = SaveText( *ours.Root() );

		KeyValues3 tier0;
		CUtlString sError;

		if ( !LoadKV3( &tier0, &sError, sOurs.c_str(), g_KV3Format_Generic, pszFile ) )
		{
			TEST_EQ( std::string( sError.Get() ), std::string() );
		}

		CUtlBuffer tier0Text( 0, 0, CUtlBuffer::TEXT_BUFFER );

		TEST_TRUE( SaveKV3( g_KV3Encoding_Text, g_KV3Format_Generic, &tier0, &sError, &tier0Text ) );

		CKV3Arena reloaded;

		LoadText( *reloaded.Root(), std::string( static_cast< const char * >( tier0Text.Base() ), tier0Text.TellPut() ), pszFile );

		// tier0 writes multi-line strings escaped on one line
		ClearMultilineFlags( ours.Root() );

		TEST_EQ( SaveText( *reloaded.Root() ), SaveText( *ours.Root() ) );
	}
}
#endif // !PLATFORM_OSX
