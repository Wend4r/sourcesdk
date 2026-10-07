#include "common/assert.h"
#include "common/macros.h"

#include <kv3lib/kv3formats.h>
#include <tier0/strtools.h>
#include <tier0/utlbuffer.h>
#include <tier0/utlstring.h>
#include <kv3lib/keyvalues3.h>

#include <cmath>
#include <fstream>
#include <iterator>
#include <string>

static CUtlString ReadKeyValues3TestFile( const char *pFilename )
{
	std::ifstream file( pFilename, std::ios::in | std::ios::binary );

	TEST_TRUE( file.is_open() );

	std::string sText( ( std::istreambuf_iterator< char >( file ) ), std::istreambuf_iterator< char >() );

	return CUtlString( sText.c_str() );
}

// Const subscripts return a null value for missing members, so null checks must confirm the member exists.
template < int N >
static const KeyValues3 &FindRequiredMember( const KeyValues3 &kv, const char (&pName)[N] )
{
	TEST_NOT_NULL( kv.FindMember( pName ) );
	return kv[ pName ];
}

static void TestFloatClose( float flValue, float flExpected )
{
	TEST_TRUE( std::fabs( flValue - flExpected ) < 0.001f );
}

static void TestDoubleClose( double flValue, double flExpected )
{
	TEST_TRUE( std::fabs( flValue - flExpected ) < 0.001 );
}

static void ValidateStringSubType( const KeyValues3 &kv, KV3SubType_t eSubType, const char *pValue )
{
	TEST_TRUE( kv.IsString() );
	TEST_EQ( kv.GetSubType(), eSubType );
	TEST_EQ( V_strcmp( kv.GetString(), pValue ), 0 );
}

static void ValidateKV3TypeExMembers( const KeyValues3 &kv, bool bExpectBinaryBlob )
{
	const KeyValues3 &null = FindRequiredMember( kv, "null_member" );

	TEST_EQ( null.GetTypeEx(), KV3_TYPEEX_NULL );
	TEST_TRUE( null.IsNull() );

	TEST_EQ( kv[ "bool_member" ].GetTypeEx(), KV3_TYPEEX_BOOL );
	TEST_TRUE( kv[ "bool_member" ].GetBool() );

	TEST_EQ( kv[ "int_member" ].GetTypeEx(), KV3_TYPEEX_INT );
	TEST_EQ( kv[ "int_member" ].GetInt(), -42 );

	TEST_EQ( kv[ "uint_member" ].GetTypeEx(), KV3_TYPEEX_UINT );
	TEST_EQ( kv[ "uint_member" ].GetUInt64(), 18446744073709551615ull );

	TEST_EQ( kv[ "double_member" ].GetTypeEx(), KV3_TYPEEX_DOUBLE );
	TestDoubleClose( kv[ "double_member" ].GetDouble(), 12.5 );

	TEST_EQ( kv[ "string_short_member" ].GetTypeEx(), KV3_TYPEEX_STRING_SHORT );
	TEST_EQ( V_strcmp( kv[ "string_short_member" ].GetString(), "short" ), 0 );

	TEST_EQ( kv[ "string_member" ].GetTypeEx(), KV3_TYPEEX_STRING );
	TEST_EQ( V_strcmp( kv[ "string_member" ].GetString(), "this string is longer than seven bytes" ), 0 );

	if ( bExpectBinaryBlob )
	{
		const KeyValues3 &blob = kv[ "binary_blob_member" ];

		TEST_EQ( blob.GetTypeEx(), KV3_TYPEEX_BINARY_BLOB );
		TEST_EQ( blob.GetBinaryBlobSize(), 4 );
		TEST_EQ( blob.GetBinaryBlobByte( 0 ), 0x00 );
		TEST_EQ( blob.GetBinaryBlobByte( 1 ), 0x7F );
		TEST_EQ( blob.GetBinaryBlobByte( 2 ), 0xA5 );
		TEST_EQ( blob.GetBinaryBlobByte( 3 ), 0xFF );
	}

	const KeyValues3 &array = kv[ "array_member" ];

	TEST_EQ( array.GetTypeEx(), KV3_TYPEEX_ARRAY );
	TEST_EQ( array.GetArrayElementCount(), 7 );
	TEST_NOT_NULL( array.GetArrayElement( 0 ) );
	TEST_TRUE( array[ 0 ].IsNull() );
	TEST_FALSE( array[ 1 ].GetBool() );
	TEST_EQ( array[ 2 ].GetInt(), -1 );
	TestDoubleClose( array[ 4 ].GetDouble(), 3.5 );
	TEST_EQ( V_strcmp( array[ 5 ].GetString(), "array" ), 0 );
	TEST_TRUE( array[ 6 ][ "nested" ].GetBool() );

	const KeyValues3 &floatArray = kv[ "array_float_member" ];

	TEST_EQ( floatArray.GetTypeEx(), KV3_TYPEEX_ARRAY );
	TEST_EQ( floatArray.GetArrayElementCount(), 3 );
	TestDoubleClose( floatArray[ 0 ].GetDouble(), 1.25 );

	const KeyValues3 &intArray = kv[ "array_int_member" ];

	TEST_EQ( intArray.GetTypeEx(), KV3_TYPEEX_ARRAY );
	TEST_EQ( intArray.GetArrayElementCount(), 3 );
	TEST_EQ( intArray[ 0 ].GetInt(), 100000 );

	const KeyValues3 &table = kv[ "table_member" ];

	TEST_EQ( table.GetTypeEx(), KV3_TYPEEX_TABLE );
	TEST_EQ( table[ "child_int" ].GetInt(), 7 );
	TEST_EQ( V_strcmp( table[ "child_object" ][ "name" ].GetString(), "nested" ), 0 );
	TEST_EQ( table[ "child_array" ].GetArrayElementCount(), 2 );
}

static void ValidateKV1TranslatedMembers( const KeyValues3 &kv )
{
	TEST_TRUE( kv[ "bool_member" ].GetBool() );
	TEST_EQ( kv[ "int_member" ].GetInt(), -42 );
	TEST_EQ( kv[ "uint_member" ].GetUInt64(), 18446744073709551615ull );
	TestDoubleClose( kv[ "double_member" ].GetDouble(), 12.5 );
	TEST_EQ( V_strcmp( kv[ "string_short_member" ].GetString(), "short" ), 0 );
	TEST_EQ( V_strcmp( kv[ "string_member" ].GetString(), "this string is longer than seven bytes" ), 0 );

	const KeyValues3 &table = kv[ "table_member" ];

	TEST_TRUE( table.IsTable() );
	TEST_EQ( table[ "child_int" ].GetInt(), 7 );
	TEST_EQ( V_strcmp( table[ "child_object" ][ "name" ].GetString(), "nested" ), 0 );
}

REGISTER_NAMED_TEST( "KeyValues3.Empty", KeyValues3_Empty )
{
	// A default-initialized KeyValues3 object should behave like a null value.
	KeyValues3 kv;

	TEST_TRUE( kv.IsNull() );
	TEST_EQ( kv.GetType(), KV3_TYPE_NULL );
	TEST_EQ( kv.GetInt( 42 ), 42 );
}

REGISTER_NAMED_TEST( "KeyValues3.Primitives", KeyValues3_Primitives )
{
	// Primitive setters and getters should update the stored type and visible value.
	KeyValues3 kv;

	kv.SetString( "value" );
	TEST_TRUE( kv.IsString() );
	TEST_EQ( V_strcmp( kv.GetString(), "value" ), 0 );

	kv.SetInt( 123 );
	TEST_EQ( kv.GetInt(), 123 );

	kv.SetFloat( 1.5f );
	TEST_EQ( kv.GetFloat(), 1.5f );

	kv.SetBool( true );
	TEST_TRUE( kv.GetBool() );
}

REGISTER_NAMED_TEST( "KeyValues3.Array", KeyValues3_Array )
{
	// Array storage should allocate elements, expose them and support removal.
	KeyValues3 kv;

	kv.SetToEmptyKV3Array();
	kv.SetArrayElementCount( 2 );

	TEST_TRUE( kv.IsArray() );
	TEST_EQ( kv.GetArrayElementCount(), 2 );

	KeyValues3 *pFirst = kv.GetArrayElement( 0 );
	KeyValues3 *pSecond = kv.GetArrayElement( 1 );

	TEST_NOT_NULL( pFirst );
	TEST_NOT_NULL( pSecond );

	pFirst->SetInt( 10 );
	pSecond->SetString( "tail" );

	TEST_EQ( kv.GetArrayElement( 0 )->GetInt(), 10 );
	TEST_EQ( V_strcmp( kv.GetArrayElement( 1 )->GetString(), "tail" ), 0 );

	kv.ArrayRemoveElement( 0 );
	TEST_EQ( kv.GetArrayElementCount(), 1 );
}

REGISTER_NAMED_TEST( "KeyValues3.Table", KeyValues3_Table )
{
	// Table members should support insertion, lookup, replacement and removal.
	KeyValues3 kv;

	kv.SetToEmptyTable();

	TEST_TRUE( kv.IsTable() );
	TEST_EQ( kv.GetMemberCount(), 0 );

	kv.SetMemberInt( "answer", 42 );
	kv.SetMemberString( "name", "source" );

	TEST_EQ( kv.GetMemberCount(), 2 );
	TEST_EQ( kv.GetMemberInt( "answer" ), 42 );
	TEST_EQ( V_strcmp( kv.GetMemberString( "name" ), "source" ), 0 );
	TEST_NULL( kv.FindMember( "missing" ) );

	kv.SetMemberInt( "answer", 43 );
	TEST_EQ( kv.GetMemberInt( "answer" ), 43 );

	TEST_TRUE( kv.RemoveMember( "answer" ) );
	TEST_EQ( kv.GetMemberInt( "answer", -1 ), -1 );
}

REGISTER_NAMED_TEST( "KeyValues3.Convenience", KeyValues3_Convenience )
{
	// Value, member, array and blob helpers should forward to the typed accessors.
	KeyValues3 kv;

	kv.SetToEmptyTable();
	kv.SetMemberUint64( "big", 0x100000000ull );
	kv.SetMemberResourceString( "model", "models/a.vmdl" );
	kv.SetMemberVector( "origin", Vector( 1.0f, 2.0f, 3.0f ) );

	TEST_EQ( kv.GetMemberUint64( "big" ), 0x100000000ull );
	TEST_EQ( kv.FindMember( "model" )->GetSubType(), KV3_SUBTYPE_RESOURCE );
	TEST_EQ( kv.FindMember( "big" )->GetValueAsNumeric< int64 >(), 0x100000000ll );

	Vector vecOrigin;
	TEST_TRUE( kv.GetMemberVector( "origin", &vecOrigin ) );
	TestFloatClose( vecOrigin.y, 2.0f );

	Vector2D vecMissing( 5.0f, 5.0f );
	TEST_FALSE( kv.GetMemberVector2D( "missing", &vecMissing ) );
	TestFloatClose( vecMissing.x, 5.0f );

	char szBig[ 32 ];
	kv.GetMemberAsString( "big", szBig, sizeof( szBig ) );
	TEST_EQ( V_strcmp( szBig, "4294967296" ), 0 );

	CUtlString sMissing;
	kv.GetMemberAsString( "missing", &sMissing, "fallback" );
	TEST_EQ( V_strcmp( sMissing.Get(), "fallback" ), 0 );

	KeyValues3 array;
	array.ArrayAddToTail()->SetInt( 1 );
	array.ArrayInsertMultipleBefore( 0, 2 );
	TEST_EQ( array.GetArrayLength(), 3 );
	TEST_EQ( array.GetArrayElement( 2 )->GetValueInt(), 1 );

	array.EnsureIsAnyArray( 3 );
	TEST_EQ( array.GetArrayElement( 2 )->GetValueInt(), 1 );

	array.ArrayRemoveMultiple( 0, 2 );
	TEST_EQ( array.GetArrayLength(), 1 );

	KeyValues3 blob;
	blob.SetToZeroedBinaryBlob( 4 );
	TEST_EQ( blob.GetBinaryBlobSize(), 4 );
	TEST_EQ( blob.GetBinaryBlobByte( 3 ), 0 );

	KeyValues3 copy;
	copy.CopyFrom( &kv );
	TEST_EQ( copy.GetMemberUint64( "big" ), 0x100000000ull );
	TEST_FALSE( copy.HasMetadata() );
}

REGISTER_NAMED_TEST( "KeyValues3.Operators", KeyValues3_Operators )
{
	// Subscripts should create members and grow arrays, assignments should pick the matching setter.
	KeyValues3 kv;

	kv[ "--old-connection-literal--" ][ "value" ] = "OnTrigger";
	kv[ "flag" ] = true;
	kv[ "count" ] = 3;
	kv[ "big" ] = 0x100000000ull;
	kv[ "scale" ] = 0.5f;
	kv[ "origin" ] = Vector( 1.0f, 2.0f, 3.0f );
	kv[ "list" ][ 2 ] = 7;
	kv[ "nothing" ] = nullptr;

	TEST_TRUE( kv.IsTable() );
	TEST_EQ( V_strcmp( kv.FindMember( "--old-connection-literal--" )->GetMemberString( "value" ), "OnTrigger" ), 0 );
	TEST_EQ( kv[ "flag" ].GetSubType(), KV3_SUBTYPE_BOOL8 );
	TEST_EQ( kv[ "count" ].GetSubType(), KV3_SUBTYPE_INT32 );
	TEST_EQ( kv[ "big" ].GetSubType(), KV3_SUBTYPE_UINT64 );
	TEST_EQ( kv[ "scale" ].GetSubType(), KV3_SUBTYPE_FLOAT32 );
	TestFloatClose( kv[ "origin" ].GetVector().z, 3.0f );
	TEST_EQ( kv[ "list" ].GetArrayElementCount(), 3 );
	TEST_TRUE( kv[ "list" ][ 0 ].IsNull() );
	TEST_EQ( kv[ "list" ][ 2 ].GetInt(), 7 );
	TEST_TRUE( kv[ "nothing" ].IsNull() );

	// Const access must not create anything.
	const KeyValues3 &constKV = kv;
	const int nMemberCount = kv.GetMemberCount();

	TEST_TRUE( constKV[ "missing" ][ "deeper" ][ 5 ].IsNull() );
	TEST_EQ( constKV[ "list" ][ 2 ].GetInt(), 7 );
	TEST_EQ( kv.GetMemberCount(), nMemberCount );

	// Packed arrays are expanded before element access.
	KeyValues3 packed;
	packed.SetVector( Vector( 4.0f, 5.0f, 6.0f ) );
	packed[ 1 ] = 9.0f;
	TestFloatClose( packed.GetVector().x, 4.0f );
	TestFloatClose( packed.GetVector().y, 9.0f );
}

REGISTER_NAMED_TEST( "KeyValues3.Arena", KeyValues3_Arena )
{
	// Arena-owned values should allocate members, arrays and tables from the arena clusters.
	// Keep the member count low: after the binary save/load test, tier0's string token registration
	// crashes on the 7th arena symbol regardless of how the arena is laid out.
	CKV3Arena arena;

	TEST_TRUE( arena.IsRootAvailabe() );

	KeyValues3 *pRoot = arena.Root();

	TEST_NOT_NULL( pRoot );
	TEST_TRUE( pRoot->GetContext() == &arena );

	pRoot->SetToEmptyTable();
	pRoot->SetMemberInt( "answer", 42 );
	pRoot->SetMemberVector( "vector", Vector( 1.0f, 2.0f, 3.0f ) );
	pRoot->SetMemberString( "name", "arena" );

	TEST_EQ( pRoot->GetMemberCount(), 3 );
	TEST_EQ( pRoot->GetMemberInt( "answer" ), 42 );
	TEST_EQ( V_strcmp( pRoot->GetMemberString( "name" ), "arena" ), 0 );
	TestFloatClose( pRoot->GetMemberVector( "vector" ).z, 3.0f );

	KeyValues3 *pAnswer = pRoot->FindMember( "answer" );

	TEST_NOT_NULL( pAnswer );
	TEST_TRUE( pAnswer->GetContext() == &arena );

	KeyValues3 *pLoose = arena.AllocKV( KV3_TYPEEX_INT, KV3_SUBTYPE_INT32 );

	TEST_NOT_NULL( pLoose );
	TEST_TRUE( pLoose->GetContext() == &arena );
	arena.FreeKV( pLoose );

	arena.Clear();
	TEST_TRUE( arena.Root()->IsNull() );
}

REGISTER_NAMED_TEST( "KeyValues3.LoadText.Arena", KeyValues3_LoadText_Arena )
{
	// tier0 fills the arena through its own layout, so the root must be readable from kv3lib afterwards.
	CKV3Arena arena;
	CUtlString sError;
	const CUtlString sText = ReadKeyValues3TestFile( SOURCESDK_KEYVALUES3_DATA_DIR "/value.kv3" );
	CUtlBuffer buffer( sText.Get(), sText.Length(), ( CUtlBuffer::BufferFlags_t )( CUtlBuffer::TEXT_BUFFER | CUtlBuffer::READ_ONLY ) );

	TEST_TRUE( LoadKV3( &arena, &sError, &buffer, g_KV3Format_Generic, "value.kv3" ) );

	const CKV3Arena &constArena = arena;
	const KeyValues3 &root = *constArena.Root();

	TEST_TRUE( root.IsTable() );
	TEST_EQ( root[ "answer" ].GetInt(), 42 );
	TEST_EQ( V_strcmp( root[ "name" ].GetString(), "kv3" ), 0 );
	ValidateKV3TypeExMembers( root, true );
}

REGISTER_NAMED_TEST( "KeyValues3.LoadText.KV1", KeyValues3_LoadText_KV1 )
{
	KeyValues3 kv;
	CUtlString sError;
	const CUtlString sText = ReadKeyValues3TestFile( SOURCESDK_KEYVALUES3_DATA_DIR "/value.kv" );

	TEST_TRUE( LoadKV3FromKV1Text( &kv, &sError, sText.Get(), KV1TEXT_ESC_BEHAVIOR_UNK1, "value.kv", false ) );

	const KeyValues3 &root = kv;

	TEST_TRUE( root.IsTable() );
	TEST_EQ( root[ "answer" ].GetInt(), 41 );
	TEST_EQ( V_strcmp( root[ "name" ].GetString(), "kv" ), 0 );
	ValidateKV1TranslatedMembers( root );
}

REGISTER_NAMED_TEST( "KeyValues3.LoadText.KV3", KeyValues3_LoadText_KV3 )
{
	KeyValues3 kv;
	CUtlString sError;
	const CUtlString sText = ReadKeyValues3TestFile( SOURCESDK_KEYVALUES3_DATA_DIR "/value.kv3" );

	TEST_TRUE( LoadKV3( &kv, &sError, sText.Get(), g_KV3Format_Generic, "value.kv3" ) );

	const KeyValues3 &root = kv;

	TEST_TRUE( root.IsTable() );
	TEST_EQ( root[ "answer" ].GetInt(), 42 );
	TEST_EQ( V_strcmp( root[ "name" ].GetString(), "kv3" ), 0 );
	ValidateKV3TypeExMembers( root, true );
}

REGISTER_NAMED_TEST( "KeyValues3.LoadText.JSON", KeyValues3_LoadText_JSON )
{
	KeyValues3 kv;
	CUtlString sError;
	const CUtlString sText = ReadKeyValues3TestFile( SOURCESDK_KEYVALUES3_DATA_DIR "/value.json" );

	TEST_TRUE( LoadKV3FromJSON( &kv, &sError, sText.Get(), "value.json" ) );

	const KeyValues3 &root = kv;

	TEST_TRUE( root.IsTable() );
	TEST_EQ( root[ "answer" ].GetInt(), 43 );
	TEST_EQ( V_strcmp( root[ "name" ].GetString(), "json" ), 0 );
	ValidateKV3TypeExMembers( root, false );
}

REGISTER_NAMED_TEST( "KeyValues3.LoadText.TypeExMembers", KeyValues3_LoadText_TypeExMembers )
{
	KeyValues3 kv;
	CUtlString sError;
	const CUtlString sText = ReadKeyValues3TestFile( SOURCESDK_KEYVALUES3_DATA_DIR "/typeex.kv3" );

	TEST_TRUE( LoadKV3( &kv, &sError, sText.Get(), g_KV3Format_Generic, "typeex.kv3" ) );
	TEST_TRUE( kv.IsTable() );
	ValidateKV3TypeExMembers( kv, true );
}

REGISTER_NAMED_TEST( "KeyValues3.LoadText.Example", KeyValues3_LoadText_Example )
{
	KeyValues3 kv;
	CUtlString sError;
	const CUtlString sText = ReadKeyValues3TestFile( SOURCESDK_KEYVALUES3_DATA_DIR "/example.kv3" );

	if ( !LoadKV3( &kv, &sError, sText.Get(), g_KV3Format_Generic, "example.kv3" ) )
	{
		TEST_EQ( sError.Get(), "" );
	}

	const KeyValues3 &root = kv;

	TEST_TRUE( root.IsTable() );
	TEST_FALSE( root[ "boolValue" ].GetBool( true ) );
	TEST_EQ( root[ "intValue" ].GetInt(), 128 );
	TestDoubleClose( root[ "doubleValue" ].GetDouble(), 64.0 );
	TEST_EQ( V_strcmp( root[ "stringValue" ].GetString(), "hello world" ), 0 );

	TEST_EQ( root[ "stringThatIsAResourceReference" ].GetTypeEx(), KV3_TYPEEX_STRING );
	ValidateStringSubType( root[ "stringThatIsAResourceReference" ], KV3_SUBTYPE_RESOURCE, "particles/items3_fx/star_emblem.vpcf" );
	ValidateStringSubType( root[ "resourceNameValue" ], KV3_SUBTYPE_RESOURCE_NAME, "materials/dev/measuregeneric01b.vmat" );
	ValidateStringSubType( root[ "panoramaValue" ], KV3_SUBTYPE_PANORAMA, "file://{resources}/layout/custom_game/example.xml" );
	ValidateStringSubType( root[ "soundEventValue" ], KV3_SUBTYPE_SOUNDEVENT, "sounds/ui/menu_accept.vsnd" );
	ValidateStringSubType( root[ "entityNameValue" ], KV3_SUBTYPE_ENTITY_NAME, "target_entity" );
	ValidateStringSubType( root[ "localizeValue" ], KV3_SUBTYPE_LOCALIZE, "#SFUI_MainMenu" );

	const KeyValues3 &subclass = root[ "subclassValue" ];

	TEST_TRUE( subclass.IsTable() );
	TEST_EQ( subclass.GetSubType(), KV3_SUBTYPE_SUBCLASS );
	TEST_EQ( V_strcmp( subclass[ "name" ].GetString(), "derived" ), 0 );

	TEST_TRUE( FindRequiredMember( root, "nullPrefixValue" ).IsNull() );

	const KeyValues3 &blob = root[ "binaryBlobValue" ];

	TEST_EQ( blob.GetTypeEx(), KV3_TYPEEX_BINARY_BLOB );
	TEST_EQ( blob.GetBinaryBlobSize(), 4 );
	TEST_EQ( blob.GetBinaryBlobByte( 0 ), 0xDE );
	TEST_EQ( blob.GetBinaryBlobByte( 1 ), 0xAD );
	TEST_EQ( blob.GetBinaryBlobByte( 2 ), 0xBE );
	TEST_EQ( blob.GetBinaryBlobByte( 3 ), 0xEF );

	const KeyValues3 &arrayPrefix = root[ "arrayPrefixValue" ];

	TEST_TRUE( arrayPrefix.IsArray() );
	TEST_EQ( arrayPrefix.GetArrayElementCount(), 2 );
	TEST_EQ( arrayPrefix[ 0 ].GetInt(), 3 );
	TEST_EQ( arrayPrefix[ 1 ].GetInt(), 4 );

	const KeyValues3 &tablePrefix = root[ "tablePrefixValue" ];

	TEST_TRUE( tablePrefix.IsTable() );
	TEST_EQ( V_strcmp( tablePrefix[ "key" ].GetString(), "value" ), 0 );

	TEST_FALSE( root[ "bool8Value" ].GetBool( true ) );
	TEST_EQ( root[ "char8Value" ].GetInt(), 65 );
	TEST_EQ( root[ "uchar32Value" ].GetUInt(), 66 );
	TEST_EQ( root[ "int8Value" ].GetInt(), -8 );
	TEST_EQ( root[ "uint8Value" ].GetUInt(), 8 );
	TEST_EQ( root[ "int16Value" ].GetInt(), -16 );
	TEST_EQ( root[ "uint16Value" ].GetUInt(), 16 );
	TEST_EQ( root[ "int32Value" ].GetInt(), -32 );
	TEST_EQ( root[ "uint32Value" ].GetUInt(), 32 );
	TEST_EQ( root[ "int64Value" ].GetInt64(), -64 );
	TEST_EQ( root[ "uint64Value" ].GetUInt64(), 64ull );
	TestDoubleClose( root[ "float32Value" ].GetDouble(), 32.5 );
	TestDoubleClose( root[ "float64Value" ].GetDouble(), 64.5 );
	TEST_EQ( V_strcmp( root[ "stringPrefixValue" ].GetString(), "prefixed string" ), 0 );

	const KeyValues3 &multiLine = root[ "multiLineStringValue" ];

	TEST_TRUE( multiLine.IsString() );
	TEST_NOT_NULL( V_strstr( multiLine.GetString(), "First line of a multi-line string literal." ) );
	TEST_NOT_NULL( V_strstr( multiLine.GetString(), "Second line of a multi-line string literal." ) );

	const KeyValues3 &array = root[ "arrayValue" ];

	TEST_TRUE( array.IsArray() );
	TEST_EQ( array.GetArrayElementCount(), 2 );
	TEST_EQ( array[ 0 ].GetInt(), 1 );
	TEST_EQ( array[ 1 ].GetInt(), 2 );

	const KeyValues3 &object = root[ "objectValue" ];

	TEST_TRUE( object.IsTable() );
	TEST_EQ( object[ "n" ].GetInt(), 5 );
	TEST_EQ( V_strcmp( object[ "s" ].GetString(), "foo" ), 0 );
}

REGISTER_NAMED_TEST( "KeyValues3.LoadBinary.ArrayMembers", KeyValues3_LoadBinary_ArrayMembers )
{
	KeyValues3 source;

	source[ "vector_member" ] = Vector( 1.25f, 2.5f, 3.75f );
	source[ "color_member" ] = Color( 1, 2, 3, 4 );

	CUtlString sError;
	CUtlBuffer buffer( 0, 0, CUtlBuffer::NONE );

	TEST_TRUE( SaveKV3( g_KV3Encoding_Binary, g_KV3Format_Generic, &source, &sError, &buffer ) );
	buffer.SeekGet( CUtlBuffer::SEEK_HEAD, 0 );

	KeyValues3 loaded;

	TEST_TRUE( LoadKV3( &loaded, &sError, &buffer, g_KV3Format_Generic, "typeex-binary.kv3" ) );

	const KeyValues3 &root = loaded;

	TEST_TRUE( root.IsTable() );

	const KeyValues3 &vector = root[ "vector_member" ];

	TEST_TRUE( vector.IsArray() );
	TEST_EQ( vector.GetArrayElementCount(), 3 );
	Vector vec = vector.GetVector();
	TestFloatClose( vec.x, 1.25f );
	TestFloatClose( vec.y, 2.5f );
	TestFloatClose( vec.z, 3.75f );

	const KeyValues3 &colorMember = root[ "color_member" ];

	TEST_TRUE( colorMember.IsArray() );
	TEST_EQ( colorMember.GetArrayElementCount(), 4 );
	Color color = colorMember.GetColor();
	TEST_EQ( color.r(), 1 );
	TEST_EQ( color.g(), 2 );
	TEST_EQ( color.b(), 3 );
	TEST_EQ( color.a(), 4 );
}

REGISTER_NAMED_TEST( "KeyValues3.LoadBuffer.KV3OrKV1", KeyValues3_LoadBuffer_KV3OrKV1 )
{
	KeyValues3 kv;
	CUtlString sError;
	const CUtlString sText = ReadKeyValues3TestFile( SOURCESDK_KEYVALUES3_DATA_DIR "/buffer.kv3" );
	CUtlBuffer buffer( sText.Get(), sText.Length(), ( CUtlBuffer::BufferFlags_t )( CUtlBuffer::TEXT_BUFFER | CUtlBuffer::READ_ONLY ) );

	TEST_TRUE( LoadKV3FromKV3OrKV1( &kv, &sError, &buffer, g_KV3Format_Generic, "buffer.kv3" ) );

	const KeyValues3 &root = kv;

	TEST_TRUE( root.IsTable() );
	TEST_EQ( root[ "answer" ].GetInt(), 44 );
	TEST_EQ( V_strcmp( root[ "name" ].GetString(), "buffer" ), 0 );
	ValidateKV3TypeExMembers( root, true );
}

REGISTER_NAMED_TEST( "KeyValues3.LoadText.NoHeader", KeyValues3_LoadText_NoHeader )
{
	KeyValues3 kv;
	CUtlString sError;
	const CUtlString sText = ReadKeyValues3TestFile( SOURCESDK_KEYVALUES3_DATA_DIR "/no_header.kv3" );

	TEST_TRUE( LoadKV3Text_NoHeader( &kv, &sError, sText.Get(), g_KV3Format_Generic, "no-header.kv3" ) );

	const KeyValues3 &root = kv;

	TEST_TRUE( root.IsTable() );
	TEST_EQ( root[ "answer" ].GetInt(), 45 );
	TEST_EQ( V_strcmp( root[ "nested" ][ "value" ].GetString(), "no-header" ), 0 );
	ValidateKV3TypeExMembers( root, true );
}
