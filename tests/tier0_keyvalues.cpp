#include "common/assert.h"
#include "common/macros.h"

#include <tier0/dbg.h>
#include <tier0/keyvalues.h>
#include <tier0/keyvaluesjson.h>

#include <cstring>

REGISTER_NAMED_TEST( "KeyValues.RemoveOptionalSubKey", KeyValues_RemoveOptionalSubKey )
{
	KeyValues *pRoot = new KeyValues( "root" );
	KeyValues *pFirst = pRoot->CreateNewKey();
	KeyValues *pSecond = pRoot->CreateNewKey();
	KeyValues *pThird = pRoot->CreateNewKey();

	TEST_TRUE( pRoot->GetFirstSubKey() == pFirst );

	// A key in the middle is unlinked from its peers and not deleted.
	pRoot->RemoveOptionalSubKey( pSecond );
	TEST_TRUE( pFirst->GetNextKey() == pThird );
	TEST_TRUE( pSecond->GetNextKey() == NULL );
	TEST_FALSE( pRoot->ContainsSubKey( pSecond ) );

	// The first key hands the list over to the next one.
	pRoot->RemoveOptionalSubKey( pFirst );
	TEST_TRUE( pRoot->GetFirstSubKey() == pThird );

	// The last key leaves the parent without subkeys.
	pRoot->RemoveOptionalSubKey( pThird );
	TEST_TRUE( pRoot->GetFirstSubKey() == NULL );

	// A null key is ignored.
	pRoot->RemoveOptionalSubKey( NULL );

	pFirst->deleteThis();
	pSecond->deleteThis();
	pThird->deleteThis();
	pRoot->deleteThis();
}

REGISTER_NAMED_TEST( "KeyValues.DisposableStringTable", KeyValues_DisposableStringTable )
{
	// The names of the key and its subkeys live in a string table of its own.
	KeyValues *pRoot = new KeyValues( "disposable_root", KeyValues::KV_DISPOSABLE_STRING_TABLE );
	TEST_EQ( strcmp( pRoot->GetName(), "disposable_root" ), 0 );

	pRoot->SetInt( "disposable_value", 7 );
	TEST_EQ( pRoot->GetInt( "disposable_value" ), 7 );

	pRoot->deleteThis();
}

REGISTER_NAMED_TEST( "TestKeyValuesJSONParser.Passes", TestKeyValuesJSONParser_Passes )
{
	// The self test of tier0 asserts on a wrong result.
	SetAssertionPrompt( ASSERTION_TYPE_ALL_TYPES, AP_NOTHING );
	TestKeyValuesJSONParser();
}
