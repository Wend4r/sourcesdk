#include "common/assert.h"
#include "common/macros.h"

#include <tier1/utlfixedmemory.h>

REGISTER_NAMED_TEST( "CUtlFixedMemory.GrowEnsurePurge", CUtlFixedMemory_GrowEnsurePurge )
{
	// Fixed memory should grow, expose iterators and purge back to zero allocation.
	CUtlFixedMemory< int > memory( 2, 0 );

	TEST_EQ( memory.NumAllocated(), 0 );
	TEST_FALSE( memory.IsIdxValid( memory.InvalidIndex() ) );

	memory.Grow( 3 );
	TEST_TRUE( memory.NumAllocated() >= 3 );

	CUtlFixedMemory< int >::Iterator_t it = memory.First();

	TEST_TRUE( memory.IsValidIterator( it ) );

	const intp nFirst = memory.GetIndex( it );

	memory[nFirst] = 31;
	TEST_EQ( memory[nFirst], 31 );

	memory.EnsureCapacity( 8 );
	TEST_TRUE( memory.NumAllocated() >= 8 );

	memory.Purge();
	TEST_EQ( memory.NumAllocated(), 0 );
	TEST_FALSE( memory.IsValidIterator( memory.First() ) );
}
