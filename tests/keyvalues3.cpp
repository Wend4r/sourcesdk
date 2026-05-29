#include "common/assert.h"
#include "common/macros.h"

#include <tier0/keyvalues3.h>
#include <tier0/strtools.h>
#include <tier0/utlbuffer.h>
#include <tier0/utlstring.h>
#include <tier1/keyvalues3.h>

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

template < int N >
static KeyValues3 *FindRequiredMember( KeyValues3 &kv, const char (&pName)[N] )
{
	KeyValues3 *pMember = kv.FindMember( pName );
	TEST_NOT_NULL( pMember );
	return pMember;
}

static void TestFloatClose( float flValue, float flExpected )
{
	TEST_TRUE( std::fabs( flValue - flExpected ) < 0.001f );
}

static void TestDoubleClose( double flValue, double flExpected )
{
	TEST_TRUE( std::fabs( flValue - flExpected ) < 0.001 );
}

template < int N >
static void ValidateStringSubTypeMember( KeyValues3 &kv, const char ( &pName )[ N ], KV3SubType_t eSubType, const char *pValue )
{
	KeyValues3 *pMember = kv.FindMember( pName );
	TEST_NOT_NULL( pMember );
	TEST_TRUE( pMember->IsString() );
	TEST_EQ( pMember->GetSubType(), eSubType );
	TEST_EQ( V_strcmp( pMember->GetString(), pValue ), 0 );
}

static void ValidateKV3TypeExMembers( KeyValues3 &kv, bool bExpectBinaryBlob )
{
	KeyValues3 *pNull = FindRequiredMember( kv, "null_member" );

	TEST_EQ( pNull->GetTypeEx(), KV3_TYPEEX_NULL );
	TEST_TRUE( pNull->IsNull() );

	KeyValues3 *pBool = FindRequiredMember( kv, "bool_member" );

	TEST_EQ( pBool->GetTypeEx(), KV3_TYPEEX_BOOL );
	TEST_TRUE( pBool->GetBool() );

	KeyValues3 *pInt = FindRequiredMember( kv, "int_member" );

	TEST_EQ( pInt->GetTypeEx(), KV3_TYPEEX_INT );
	TEST_EQ( pInt->GetInt(), -42 );

	KeyValues3 *pUInt = FindRequiredMember( kv, "uint_member" );

	TEST_EQ( pUInt->GetTypeEx(), KV3_TYPEEX_UINT );
	TEST_EQ( pUInt->GetUInt64(), 18446744073709551615ull );

	KeyValues3 *pDouble = FindRequiredMember( kv, "double_member" );

	TEST_EQ( pDouble->GetTypeEx(), KV3_TYPEEX_DOUBLE );
	TestDoubleClose( pDouble->GetDouble(), 12.5 );

	KeyValues3 *pStringShort = FindRequiredMember( kv, "string_short_member" );

	TEST_EQ( pStringShort->GetTypeEx(), KV3_TYPEEX_STRING_SHORT );
	TEST_EQ( V_strcmp( pStringShort->GetString(), "short" ), 0 );

	KeyValues3 *pString = FindRequiredMember( kv, "string_member" );

	TEST_EQ( pString->GetTypeEx(), KV3_TYPEEX_STRING );
	TEST_EQ( V_strcmp( pString->GetString(), "this string is longer than seven bytes" ), 0 );

	if ( bExpectBinaryBlob )
	{
		KeyValues3 *pBlob = FindRequiredMember( kv, "binary_blob_member" );

		TEST_EQ( pBlob->GetTypeEx(), KV3_TYPEEX_BINARY_BLOB );
		TEST_EQ( pBlob->GetBinaryBlobSize(), 4 );
		TEST_EQ( pBlob->GetBinaryBlob()[0], 0x00 );
		TEST_EQ( pBlob->GetBinaryBlob()[1], 0x7F );
		TEST_EQ( pBlob->GetBinaryBlob()[2], 0xA5 );
		TEST_EQ( pBlob->GetBinaryBlob()[3], 0xFF );
	}

	KeyValues3 *pArray = FindRequiredMember( kv, "array_member" );

	TEST_EQ( pArray->GetTypeEx(), KV3_TYPEEX_ARRAY );
	TEST_EQ( pArray->GetArrayElementCount(), 7 );
	TEST_TRUE( pArray->GetArrayElement( 0 )->IsNull() );
	TEST_TRUE( pArray->GetArrayElement( 1 )->GetBool() == false );
	TEST_EQ( pArray->GetArrayElement( 2 )->GetInt(), -1 );
	TestDoubleClose( pArray->GetArrayElement( 4 )->GetDouble(), 3.5 );
	TEST_EQ( V_strcmp( pArray->GetArrayElement( 5 )->GetString(), "array" ), 0 );
	TEST_TRUE( pArray->GetArrayElement( 6 )->GetMemberBool( "nested" ) );

	KeyValues3 *pFloatArray = FindRequiredMember( kv, "array_float_member" );

	TEST_EQ( pFloatArray->GetTypeEx(), KV3_TYPEEX_ARRAY );
	TEST_EQ( pFloatArray->GetArrayElementCount(), 3 );
	TestDoubleClose( pFloatArray->GetArrayElement( 0 )->GetDouble(), 1.25 );

	KeyValues3 *pIntArray = FindRequiredMember( kv, "array_int_member" );

	TEST_EQ( pIntArray->GetTypeEx(), KV3_TYPEEX_ARRAY );
	TEST_EQ( pIntArray->GetArrayElementCount(), 3 );
	TEST_EQ( pIntArray->GetArrayElement( 0 )->GetInt(), 100000 );

	KeyValues3 *pTable = FindRequiredMember( kv, "table_member" );

	TEST_EQ( pTable->GetTypeEx(), KV3_TYPEEX_TABLE );
	TEST_EQ( pTable->GetMemberInt( "child_int" ), 7 );
	TEST_EQ( V_strcmp( pTable->FindMember( "child_object" )->GetMemberString( "name" ), "nested" ), 0 );
	TEST_EQ( pTable->FindMember( "child_array" )->GetArrayElementCount(), 2 );
}

static void ValidateKV1TranslatedMembers( KeyValues3 &kv )
{
	TEST_TRUE( kv.GetMemberBool( "bool_member" ) );
	TEST_EQ( kv.GetMemberInt( "int_member" ), -42 );
	TEST_EQ( kv.GetMemberUInt64( "uint_member" ), 18446744073709551615ull );
	TestDoubleClose( kv.GetMemberDouble( "double_member" ), 12.5 );
	TEST_EQ( V_strcmp( kv.GetMemberString( "string_short_member" ), "short" ), 0 );
	TEST_EQ( V_strcmp( kv.GetMemberString( "string_member" ), "this string is longer than seven bytes" ), 0 );

	KeyValues3 *pTable = FindRequiredMember( kv, "table_member" );

	TEST_TRUE( pTable->IsTable() );
	TEST_EQ( pTable->GetMemberInt( "child_int" ), 7 );
	TEST_EQ( V_strcmp( pTable->FindMember( "child_object" )->GetMemberString( "name" ), "nested" ), 0 );
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

REGISTER_NAMED_TEST( "KeyValues3.LoadText.KV1", KeyValues3_LoadText_KV1 )
{
	KeyValues3 kv;
	CUtlString sError;
	const CUtlString sText = ReadKeyValues3TestFile( SOURCESDK_KEYVALUES3_DATA_DIR "/value.kv" );

	TEST_TRUE( LoadKV3FromKV1Text( &kv, &sError, sText.Get(), KV1TEXT_ESC_BEHAVIOR_UNK1, "value.kv", false ) );
	TEST_TRUE( kv.IsTable() );
	TEST_EQ( kv.GetMemberInt( "answer" ), 41 );
	TEST_EQ( V_strcmp( kv.GetMemberString( "name" ), "kv" ), 0 );
	ValidateKV1TranslatedMembers( kv );
}

REGISTER_NAMED_TEST( "KeyValues3.LoadText.KV3", KeyValues3_LoadText_KV3 )
{
	KeyValues3 kv;
	CUtlString sError;
	const CUtlString sText = ReadKeyValues3TestFile( SOURCESDK_KEYVALUES3_DATA_DIR "/value.kv3" );

	TEST_TRUE( LoadKV3( &kv, &sError, sText.Get(), g_KV3Format_Generic, "value.kv3" ) );
	TEST_TRUE( kv.IsTable() );
	TEST_EQ( kv.GetMemberInt( "answer" ), 42 );
	TEST_EQ( V_strcmp( kv.GetMemberString( "name" ), "kv3" ), 0 );
	ValidateKV3TypeExMembers( kv, true );
}

REGISTER_NAMED_TEST( "KeyValues3.LoadText.JSON", KeyValues3_LoadText_JSON )
{
	KeyValues3 kv;
	CUtlString sError;
	const CUtlString sText = ReadKeyValues3TestFile( SOURCESDK_KEYVALUES3_DATA_DIR "/value.json" );

	TEST_TRUE( LoadKV3FromJSON( &kv, &sError, sText.Get(), "value.json" ) );
	TEST_TRUE( kv.IsTable() );
	TEST_EQ( kv.GetMemberInt( "answer" ), 43 );
	TEST_EQ( V_strcmp( kv.GetMemberString( "name" ), "json" ), 0 );
	ValidateKV3TypeExMembers( kv, false );
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
	TEST_TRUE( kv.IsTable() );
	TEST_FALSE( kv.GetMemberBool( "boolValue", true ) );
	TEST_EQ( kv.GetMemberInt( "intValue" ), 128 );
	TestDoubleClose( kv.GetMemberDouble( "doubleValue" ), 64.0 );
	TEST_EQ( V_strcmp( kv.GetMemberString( "stringValue" ), "hello world" ), 0 );

	KeyValues3 *pResource = FindRequiredMember( kv, "stringThatIsAResourceReference" );

	TEST_EQ( pResource->GetTypeEx(), KV3_TYPEEX_STRING );
	TEST_EQ( pResource->GetSubType(), KV3_SUBTYPE_RESOURCE );
	TEST_EQ( V_strcmp( pResource->GetString(), "particles/items3_fx/star_emblem.vpcf" ), 0 );
	ValidateStringSubTypeMember( kv, "resourceNameValue", KV3_SUBTYPE_RESOURCE_NAME, "materials/dev/measuregeneric01b.vmat" );
	ValidateStringSubTypeMember( kv, "panoramaValue", KV3_SUBTYPE_PANORAMA, "file://{resources}/layout/custom_game/example.xml" );
	ValidateStringSubTypeMember( kv, "soundEventValue", KV3_SUBTYPE_SOUNDEVENT, "sounds/ui/menu_accept.vsnd" );
	ValidateStringSubTypeMember( kv, "entityNameValue", KV3_SUBTYPE_ENTITY_NAME, "target_entity" );
	ValidateStringSubTypeMember( kv, "localizeValue", KV3_SUBTYPE_LOCALIZE, "#SFUI_MainMenu" );

	KeyValues3 *pSubclass = FindRequiredMember( kv, "subclassValue" );

	TEST_TRUE( pSubclass->IsTable() );
	TEST_EQ( pSubclass->GetSubType(), KV3_SUBTYPE_SUBCLASS );
	TEST_EQ( V_strcmp( pSubclass->GetMemberString( "name" ), "derived" ), 0 );

	KeyValues3 *pNullPrefix = FindRequiredMember( kv, "nullPrefixValue" );

	TEST_TRUE( pNullPrefix->IsNull() );

	KeyValues3 *pBinaryBlob = FindRequiredMember( kv, "binaryBlobValue" );

	TEST_EQ( pBinaryBlob->GetTypeEx(), KV3_TYPEEX_BINARY_BLOB );
	TEST_EQ( pBinaryBlob->GetBinaryBlobSize(), 4 );
	TEST_EQ( pBinaryBlob->GetBinaryBlob()[0], 0xDE );
	TEST_EQ( pBinaryBlob->GetBinaryBlob()[1], 0xAD );
	TEST_EQ( pBinaryBlob->GetBinaryBlob()[2], 0xBE );
	TEST_EQ( pBinaryBlob->GetBinaryBlob()[3], 0xEF );

	KeyValues3 *pArrayPrefix = FindRequiredMember( kv, "arrayPrefixValue" );

	TEST_TRUE( pArrayPrefix->IsArray() );
	TEST_EQ( pArrayPrefix->GetArrayElementCount(), 2 );
	TEST_EQ( pArrayPrefix->GetArrayElement( 0 )->GetInt(), 3 );
	TEST_EQ( pArrayPrefix->GetArrayElement( 1 )->GetInt(), 4 );

	KeyValues3 *pTablePrefix = FindRequiredMember( kv, "tablePrefixValue" );

	TEST_TRUE( pTablePrefix->IsTable() );
	TEST_EQ( V_strcmp( pTablePrefix->GetMemberString( "key" ), "value" ), 0 );

	TEST_FALSE( kv.GetMemberBool( "bool8Value", true ) );
	TEST_EQ( kv.GetMemberInt( "char8Value" ), 65 );
	TEST_EQ( kv.GetMemberUInt( "uchar32Value" ), 66 );
	TEST_EQ( kv.GetMemberInt( "int8Value" ), -8 );
	TEST_EQ( kv.GetMemberUInt( "uint8Value" ), 8 );
	TEST_EQ( kv.GetMemberInt( "int16Value" ), -16 );
	TEST_EQ( kv.GetMemberUInt( "uint16Value" ), 16 );
	TEST_EQ( kv.GetMemberInt( "int32Value" ), -32 );
	TEST_EQ( kv.GetMemberUInt( "uint32Value" ), 32 );
	TEST_EQ( kv.GetMemberInt64( "int64Value" ), -64 );
	TEST_EQ( kv.GetMemberUInt64( "uint64Value" ), 64ull );
	TestDoubleClose( kv.GetMemberDouble( "float32Value" ), 32.5 );
	TestDoubleClose( kv.GetMemberDouble( "float64Value" ), 64.5 );
	TEST_EQ( V_strcmp( kv.GetMemberString( "stringPrefixValue" ), "prefixed string" ), 0 );

	KeyValues3 *pMultiLine = FindRequiredMember( kv, "multiLineStringValue" );

	TEST_TRUE( pMultiLine->IsString() );
	TEST_NOT_NULL( V_strstr( pMultiLine->GetString(), "First line of a multi-line string literal." ) );
	TEST_NOT_NULL( V_strstr( pMultiLine->GetString(), "Second line of a multi-line string literal." ) );

	KeyValues3 *pArray = FindRequiredMember( kv, "arrayValue" );

	TEST_TRUE( pArray->IsArray() );
	TEST_EQ( pArray->GetArrayElementCount(), 2 );
	TEST_EQ( pArray->GetArrayElement( 0 )->GetInt(), 1 );
	TEST_EQ( pArray->GetArrayElement( 1 )->GetInt(), 2 );

	KeyValues3 *pObject = FindRequiredMember( kv, "objectValue" );

	TEST_TRUE( pObject->IsTable() );
	TEST_EQ( pObject->GetMemberInt( "n" ), 5 );
	TEST_EQ( V_strcmp( pObject->GetMemberString( "s" ), "foo" ), 0 );
}

REGISTER_NAMED_TEST( "KeyValues3.LoadBinary.ArrayMembers", KeyValues3_LoadBinary_ArrayMembers )
{
	KeyValues3 source;
	source.SetToEmptyTable();
	source.SetMemberVector( "vector_member", Vector( 1.25f, 2.5f, 3.75f ) );
	source.SetMemberColor( "color_member", Color( 1, 2, 3, 4 ) );

	CUtlString sError;
	CUtlBuffer buffer( 0, 0, CUtlBuffer::NONE );

	TEST_TRUE( SaveKV3( g_KV3Encoding_Binary, g_KV3Format_Generic, &source, &sError, &buffer ) );
	buffer.SeekGet( CUtlBuffer::SEEK_HEAD, 0 );

	KeyValues3 loaded;

	TEST_TRUE( LoadKV3( &loaded, &sError, &buffer, g_KV3Format_Generic, "typeex-binary.kv3" ) );
	TEST_TRUE( loaded.IsTable() );

	KeyValues3 *pVector = FindRequiredMember( loaded, "vector_member" );

	TEST_TRUE( pVector->IsArray() );
	TEST_EQ( pVector->GetArrayElementCount(), 3 );
	Vector vec = pVector->GetVector();
	TestFloatClose( vec.x, 1.25f );
	TestFloatClose( vec.y, 2.5f );
	TestFloatClose( vec.z, 3.75f );

	KeyValues3 *pColor = FindRequiredMember( loaded, "color_member" );

	TEST_TRUE( pColor->IsArray() );
	TEST_EQ( pColor->GetArrayElementCount(), 4 );
	Color color = pColor->GetColor();
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
	TEST_TRUE( kv.IsTable() );
	TEST_EQ( kv.GetMemberInt( "answer" ), 44 );
	TEST_EQ( V_strcmp( kv.GetMemberString( "name" ), "buffer" ), 0 );
	ValidateKV3TypeExMembers( kv, true );
}

REGISTER_NAMED_TEST( "KeyValues3.LoadText.NoHeader", KeyValues3_LoadText_NoHeader )
{
	KeyValues3 kv;
	CUtlString sError;
	const CUtlString sText = ReadKeyValues3TestFile( SOURCESDK_KEYVALUES3_DATA_DIR "/no_header.kv3" );

	TEST_TRUE( LoadKV3Text_NoHeader( &kv, &sError, sText.Get(), g_KV3Format_Generic, "no-header.kv3" ) );
	TEST_TRUE( kv.IsTable() );
	TEST_EQ( kv.GetMemberInt( "answer" ), 45 );
	TEST_EQ( V_strcmp( kv.FindMember( "nested" )->GetMemberString( "value" ), "no-header" ), 0 );
	ValidateKV3TypeExMembers( kv, true );
}
