#include "common/assert.h"
#include "common/macros.h"

#include <tier1/utlblockmemory.h>

REGISTER_NAMED_TEST( "CUtlBlockMemory.GrowEnsurePurge", CUtlBlockMemory_GrowEnsurePurge )
{
	// Block memory should grow, expose indexed storage and purge back to empty.
	CUtlBlockMemory< int, int > memory( 4, 0 );

	TEST_EQ( memory.NumAllocated(), 0 );
	TEST_FALSE( memory.IsIdxValid( 0 ) );

	memory.Grow( 5 );
	TEST_TRUE( memory.NumAllocated() >= 5 );
	TEST_TRUE( memory.IsIdxValid( 4 ) );

	memory[0] = 10;
	memory[4] = 40;
	TEST_EQ( memory[0], 10 );
	TEST_EQ( memory.Element( 4 ), 40 );

	memory.EnsureCapacity( 12 );
	TEST_TRUE( memory.NumAllocated() >= 12 );

	memory.Purge( 4 );
	TEST_TRUE( memory.NumAllocated() >= 4 );
	TEST_FALSE( memory.IsIdxValid( 4 ) );

	memory.Purge();
	TEST_EQ( memory.NumAllocated(), 0 );
}
