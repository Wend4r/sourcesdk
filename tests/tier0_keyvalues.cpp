#include "common/assert.h"
#include "common/macros.h"

#include <tier0/keyvalues.h>

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
