#include "common/assert.h"
#include "common/macros.h"

#include <tier0/platform.h>
#include <tier0/dbg.h>
#include <tier0/stringpool.h>

#include <cstring>

REGISTER_NAMED_TEST( "CStringPool_CI.AllocateFind", CStringPool_CI_AllocateFind )
{
	// The pool is case-insensitive, so equal strings in any case share one entry.
	CStringPool_CI pool;

	TEST_EQ( pool.Count(), 0u );
	TEST_NULL( pool.Find( "tier0" ) );

	const char *pString = pool.Allocate( "tier0" );
	TEST_NOT_NULL( pString );
	TEST_EQ( std::strcmp( pString, "tier0" ), 0 );
	TEST_EQ( pool.Allocate( "TIER0" ), pString );
	TEST_EQ( pool.Find( "Tier0" ), pString );

	pool.Allocate( "tier1" );
	TEST_EQ( pool.Count(), 2u );

	pool.Purge();
	TEST_EQ( pool.Count(), 0u );
}
