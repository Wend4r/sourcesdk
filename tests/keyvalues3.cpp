#include "common/assert.h"
#include "common/macros.h"

#include <tier0/strtools.h>
#include <tier1/keyvalues3.h>

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
