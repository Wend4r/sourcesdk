#include "common/assert.h"
#include "common/macros.h"

#include <tier1/refcount.h>

class CRefCountTestBase1 {};
class CRefCountTestBase2 {};
class CRefCountTestBase3 {};

class CRefCountTestObject : public CRefCounted3< CRefCountTestBase1, CRefCountTestBase2, CRefCountTestBase3, CRefCountServiceST >
{
};

REGISTER_NAMED_TEST( "CRefCounted3.AddRefRelease", CRefCounted3_AddRefRelease )
{
	// CRefCounted3 should expose AddRef and Release and delete itself on the last release.
	CRefCountTestObject *pObject = new CRefCountTestObject;

	TEST_EQ( pObject->AddRef(), 2 );
	TEST_EQ( pObject->Release(), 1 );
	TEST_EQ( pObject->Release(), 0 );
}
