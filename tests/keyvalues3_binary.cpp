#include "common/assert.h"
#include "common/macros.h"

#include <kv3lib/kv3formats.h>
#include <tier0/bufferstring.h>
#include <tier0/strtools.h>
#include <tier0/utlbuffer.h>
#include <tier0/utlstring.h>
#include <kv3lib/keyvalues3.h>

// Kept out of keyvalues3_tests: after these binary loads, a later tier0 LoadKV3 in the same process
// crashes inside tier0 (see the arena symbol note in KeyValues3.Arena).

static void ValidateBinaryTypedMembers( KeyValues3 &kv, KV3TypeEx_t eStringType )
{
	CBufferStringN< 128 > sBuffer;

	KeyValues3 *pU32 = kv.FindMember( "uint32" );

	TEST_NOT_NULL( pU32 );
	TEST_EQ( pU32->GetTypeEx(), KV3_TYPEEX_ARRAY_UINT32 );
	TEST_EQ( pU32->GetArrayElementCount(), 3 );
	TEST_EQ( V_strcmp( pU32->ToString( sBuffer ), "10 20 4000000000" ), 0 );

	KeyValues3 *pU64 = kv.FindMember( "uint64" );

	TEST_NOT_NULL( pU64 );
	TEST_EQ( pU64->GetTypeEx(), KV3_TYPEEX_ARRAY_UINT64 );
	TEST_EQ( pU64->GetArrayElementCount(), 2 );
	TEST_EQ( V_strcmp( pU64->ToString( sBuffer ), "1 9223372036854775808" ), 0 );

	KeyValues3 *pColor = kv.FindMember( "color" );

	TEST_NOT_NULL( pColor );
	TEST_EQ( pColor->GetTypeEx(), KV3_TYPEEX_ARRAY_UINT32 );
	TEST_TRUE( kv.GetMemberColor( "color" ) == Color( 1, 2, 3, 4 ) );

	KeyValues3 *pString = kv.FindMember( "string" );

	TEST_NOT_NULL( pString );
	TEST_EQ( pString->GetTypeEx(), eStringType );
	TEST_EQ( V_strcmp( kv.GetMemberString( "string" ), "a string long enough to be allocated on the heap" ), 0 );
	TEST_EQ( V_strcmp( kv.GetMemberString( "short" ), "abc" ), 0 );
}

REGISTER_NAMED_TEST( "KeyValues3.LoadBinary.TypedArraysAndSymbols", KeyValues3_LoadBinary_TypedArraysAndSymbols )
{
	// Load flag 2 stores binary strings as arena symbols; there is no named KV3LoadTextFlags_t value for it.
	const uint nLoadStringsAsSymbols = 1 << 1;

	CKV3Arena arena;
	KeyValues3 *pRoot = arena.Root();

	pRoot->SetToEmptyTable();

	KeyValues3 *pU32 = pRoot->FindOrCreateMember( "uint32" );

	pU32->SetToEmptyKV3Array();
	pU32->ArrayAddElementToTail()->SetUInt( 10 );
	pU32->ArrayAddElementToTail()->SetUInt( 20 );
	pU32->ArrayAddElementToTail()->SetUInt( 4000000000u );

	KeyValues3 *pU64 = pRoot->FindOrCreateMember( "uint64" );

	pU64->SetToEmptyKV3Array();
	pU64->ArrayAddElementToTail()->SetUInt64( 1 );
	pU64->ArrayAddElementToTail()->SetUInt64( 0x8000000000000000ull );

	KeyValues3 *pColor = pRoot->FindOrCreateMember( "color" );

	pColor->SetToEmptyKV3Array();

	for ( int i = 1; i <= 4; ++i )
		pColor->ArrayAddElementToTail()->SetUInt( i );

	pRoot->SetMemberString( "string", "a string long enough to be allocated on the heap" );
	pRoot->SetMemberString( "short", "abc" );

	// tier0's binary loader stores small arrays of unsigned integers as typed arrays.
	CUtlBuffer buffer( 0, 0, CUtlBuffer::NONE );
	CUtlString sError;

	TEST_TRUE( SaveKV3( g_KV3Encoding_Binary, g_KV3Format_Generic, pRoot, &sError, &buffer ) );

	CKV3Arena loaded;

	TEST_TRUE( LoadKV3( loaded.Root(), &sError, &buffer, g_KV3Format_Generic, "typed-arrays.kv3" ) );
	ValidateBinaryTypedMembers( *loaded.Root(), KV3_TYPEEX_STRING_EXTERN );

	buffer.SeekGet( CUtlBuffer::SEEK_HEAD, 0 );

	CKV3Arena symbols;

	TEST_TRUE( LoadKV3( symbols.Root(), &sError, &buffer, g_KV3Format_Generic, "typed-arrays.kv3", nLoadStringsAsSymbols ) );
	ValidateBinaryTypedMembers( *symbols.Root(), KV3_TYPEEX_STRING_SYMBOL );
	TEST_EQ( symbols.Root()->FindMember( "short" )->GetTypeEx(), KV3_TYPEEX_STRING_SYMBOL );

	// Copies keep the typed arrays and own their strings.
	CKV3Arena other;

	*other.Root() = *symbols.Root();
	ValidateBinaryTypedMembers( *other.Root(), KV3_TYPEEX_STRING );

	KeyValues3 standalone;

	standalone = *symbols.Root();
	ValidateBinaryTypedMembers( standalone, KV3_TYPEEX_STRING );

	// Setting a loaded value frees its typed array.
	symbols.Root()->FindMember( "uint32" )->SetInt( 5 );
	symbols.Root()->FindMember( "uint64" )->SetToEmptyKV3Array();
	symbols.Root()->SetMemberString( "string", "changed" );
	TEST_EQ( symbols.Root()->GetMemberInt( "uint32" ), 5 );
	TEST_EQ( symbols.Root()->FindMember( "uint64" )->GetArrayElementCount(), 0 );
	TEST_EQ( V_strcmp( symbols.Root()->GetMemberString( "string" ), "changed" ), 0 );
	ValidateBinaryTypedMembers( *other.Root(), KV3_TYPEEX_STRING );

	// Accessing the elements turns a typed array into a regular one.
	KeyValues3 *pLoadedU32 = loaded.Root()->FindMember( "uint32" );

	TEST_NOT_NULL( pLoadedU32->GetArrayElement( 2 ) );
	TEST_EQ( pLoadedU32->GetArrayElement( 2 )->GetUInt(), 4000000000u );
	TEST_EQ( pLoadedU32->GetTypeEx(), KV3_TYPEEX_ARRAY );
	TEST_EQ( pLoadedU32->GetArrayElementCount(), 3 );
	TEST_EQ( pLoadedU32->GetArrayElement( 0 )->GetSubType(), KV3_SUBTYPE_UINT32 );
	TEST_EQ( pLoadedU32->GetArrayElement( 0 )->GetUInt(), 10u );

	KeyValues3 *pLoadedU64 = loaded.Root()->FindMember( "uint64" );

	pLoadedU64->ArrayAddElementToTail()->SetUInt64( 7 );
	TEST_EQ( pLoadedU64->GetTypeEx(), KV3_TYPEEX_ARRAY );
	TEST_EQ( pLoadedU64->GetArrayElementCount(), 3 );
	TEST_NOT_NULL( pLoadedU64->GetArrayElement( 1 ) );
	TEST_EQ( pLoadedU64->GetArrayElement( 1 )->GetUInt64(), 0x8000000000000000ull );
	TEST_NOT_NULL( pLoadedU64->GetArrayElement( 2 ) );
	TEST_EQ( pLoadedU64->GetArrayElement( 2 )->GetUInt64(), 7ull );
}

// Binary KV3 of { a = [ -3, -2, -1, 1000, 2000, 30000 ], b = [ 10, -20, 300 ] } with int16 typed arrays,
// which tier0's binary writer only produces from int16 typed arrays.
static const unsigned char s_Int16ArraysKV3[] =
{
	0x05, 0x33, 0x56, 0x4b, 0x7c, 0x16, 0x12, 0x74, 0xe9, 0x06, 0x98, 0x46, 0xaf, 0xf2, 0xe6, 0x3e,
	0xb5, 0x90, 0x37, 0xe7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00,
	0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x01, 0x00, 0x02, 0x00,
	0x35, 0x00, 0x00, 0x00, 0x35, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x19, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x61, 0x00, 0x62, 0x00, 0xfd, 0xff, 0xfe, 0xff,
	0xff, 0xff, 0xe8, 0x03, 0xd0, 0x07, 0x30, 0x75, 0x0a, 0x00, 0xec, 0xff, 0x2c, 0x01, 0x00, 0x00,
	0x02, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x06, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x01, 0x00, 0x00, 0x00, 0x09, 0x19, 0x14, 0x19, 0x14, 0x00, 0xdd, 0xee, 0xff,
};

REGISTER_NAMED_TEST( "KeyValues3.LoadBinary.Int16Arrays", KeyValues3_LoadBinary_Int16Arrays )
{
	CUtlBuffer buffer( s_Int16ArraysKV3, sizeof( s_Int16ArraysKV3 ), CUtlBuffer::READ_ONLY );
	CUtlString sError;
	CKV3Arena arena;

	TEST_TRUE( LoadKV3( arena.Root(), &sError, &buffer, g_KV3Format_Generic, "int16-arrays.kv3" ) );

	KeyValues3 *pRoot = arena.Root();
	KeyValues3 *pA = pRoot->FindMember( "a" );
	KeyValues3 *pB = pRoot->FindMember( "b" );

	TEST_NOT_NULL( pA );
	TEST_NOT_NULL( pB );
	TEST_EQ( pA->GetTypeEx(), KV3_TYPEEX_ARRAY_INT16 );
	TEST_EQ( pA->GetArrayElementCount(), 6 );
	TEST_EQ( pB->GetTypeEx(), KV3_TYPEEX_ARRAY_INT16 );
	TEST_EQ( pB->GetArrayElementCount(), 3 );

	CBufferStringN< 128 > sBuffer;

	TEST_EQ( V_strcmp( pB->ToString( sBuffer ), "10 -20 300" ), 0 );

	// Color components truncate to uint8: -20 -> 236, 300 -> 44.
	TEST_TRUE( pB->GetColor() == Color( 10, 236, 44, 255 ) );

	// A copy of up to four elements stores them in the value itself.
	pRoot->SetMemberToCopyOfValue( "copy", pB );

	KeyValues3 *pCopy = pRoot->FindMember( "copy" );

	TEST_EQ( pCopy->GetTypeEx(), KV3_TYPEEX_ARRAY_INT16_SHORT );
	TEST_EQ( V_strcmp( pCopy->ToString( sBuffer ), "10 -20 300" ), 0 );
	TEST_TRUE( pCopy->GetColor() == Color( 10, 236, 44, 255 ) );

	// Accessing the elements turns a typed array into a regular one.
	const int nValues[] = { -3, -2, -1, 1000, 2000, 30000 };

	for ( int i = 0; i < 6; ++i )
	{
		TEST_NOT_NULL( pA->GetArrayElement( i ) );
		TEST_EQ( pA->GetArrayElement( i )->GetInt(), nValues[ i ] );
	}

	TEST_EQ( pA->GetTypeEx(), KV3_TYPEEX_ARRAY );
	TEST_EQ( pA->GetArrayElementCount(), 6 );
	TEST_EQ( pA->GetArrayElement( 0 )->GetSubType(), KV3_SUBTYPE_INT16 );

	pCopy->ArrayAddElementToTail()->SetInt( 40 );
	TEST_EQ( pCopy->GetTypeEx(), KV3_TYPEEX_ARRAY );
	TEST_EQ( pCopy->GetArrayElementCount(), 4 );
	TEST_EQ( V_strcmp( pCopy->ToString( sBuffer ), "10 -20 300 40" ), 0 );

	pB->SetArrayElementCount( 2 );
	TEST_EQ( pB->GetTypeEx(), KV3_TYPEEX_ARRAY );
	TEST_EQ( pB->GetArrayElementCount(), 2 );
	TEST_NOT_NULL( pB->GetArrayElement( 1 ) );
	TEST_EQ( pB->GetArrayElement( 1 )->GetInt(), -20 );
}
