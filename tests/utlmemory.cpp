#include "common/assert.h"
#include "common/macros.h"

#include <tier1/utlvectormemory.h>

#include <type_traits>

REGISTER_NAMED_TEST( "CUtlVectorMemory.GrowEnsureCapacity", CUtlVectorMemory_GrowEnsureCapacity )
{
	// Vector memory should allocate storage, expose indexed writes and purge cleanly.
	CUtlVectorMemory< int > memory;

	TEST_EQ( memory.Count(), 0 );
	TEST_EQ( memory.NumAllocated(), 0 );
	TEST_NULL( memory.Base() );

	memory.Grow( 2 );
	TEST_TRUE( memory.NumAllocated() >= 2 );
	TEST_NOT_NULL( memory.Base() );

	memory[0] = 10;
	memory[1] = 20;
	TEST_EQ( memory[0], 10 );
	TEST_EQ( memory[1], 20 );

	memory.EnsureCapacity( 8 );
	TEST_TRUE( memory.NumAllocated() >= 8 );

	memory.Purge();
	TEST_EQ( memory.NumAllocated(), 0 );
	TEST_NULL( memory.Base() );
}

REGISTER_NAMED_TEST( "CUtlVectorMemory.CopyAndMove", CUtlVectorMemory_CopyAndMove )
{
	// Copy and move operations should preserve both allocation size and stored values.
	CUtlVectorMemory< int > memory;

	memory.Grow( 3 );
	memory[0] = 11;
	memory[1] = 22;
	memory[2] = 33;

	CUtlVectorMemory< int > copy( memory );

	TEST_TRUE( copy.NumAllocated() >= 3 );
	TEST_EQ( copy[0], 11 );
	TEST_EQ( copy[1], 22 );
	TEST_EQ( copy[2], 33 );

	CUtlVectorMemory< int > assigned;

	assigned = memory;
	TEST_TRUE( assigned.NumAllocated() >= 3 );
	TEST_EQ( assigned[0], 11 );
	TEST_EQ( assigned[1], 22 );
	TEST_EQ( assigned[2], 33 );

	CUtlVectorMemory< int > moved( Move( memory ) );

	TEST_TRUE( moved.NumAllocated() >= 3 );
	TEST_EQ( moved[0], 11 );
	TEST_EQ( moved[1], 22 );
	TEST_EQ( moved[2], 33 );

	CUtlVectorMemory< int > moveAssigned;

	moveAssigned = Move( moved );
	TEST_TRUE( moveAssigned.NumAllocated() >= 3 );
	TEST_EQ( moveAssigned[0], 11 );
	TEST_EQ( moveAssigned[1], 22 );
	TEST_EQ( moveAssigned[2], 33 );
}
