#include "common/assert.h"
#include "common/macros.h"

#include <tier1/utlstack.h>
#include <tier1/utlvectormemory.h>

REGISTER_NAMED_TEST( "CUtlStack.PushPopCopyPurge", CUtlStack_PushPopCopyPurge )
{
	// Stacks should push, pop, copy and purge while preserving top-of-stack order.
	CUtlStack< int, CUtlVectorMemory_Growable< int, int, 0 > > stack;

	stack.EnsureCapacity( 4 );
	TEST_EQ( stack.Push( 10 ), 0 );
	TEST_EQ( stack.Push( 20 ), 1 );
	TEST_EQ( stack.Count(), 2 );
	TEST_EQ( stack.Top(), 20 );
	TEST_TRUE( stack.IsIdxValid( 1 ) );
	TEST_EQ( stack[0], 10 );

	int nPopped = 0;

	stack.Pop( nPopped );
	TEST_EQ( nPopped, 20 );
	TEST_EQ( stack.Count(), 1 );

	stack.Push();
	TEST_EQ( stack.Count(), 2 );

	CUtlStack< int, CUtlVectorMemory_Growable< int, int, 0 > > copy;

	copy.CopyFrom( stack );
	TEST_EQ( copy.Count(), stack.Count() );
	TEST_EQ( copy[0], stack[0] );

	stack.PopMultiple( 2 );
	TEST_EQ( stack.Count(), 0 );

	copy.Clear();
	TEST_EQ( copy.Count(), 0 );
	copy.Purge();
	TEST_EQ( copy.Count(), 0 );
}

REGISTER_NAMED_TEST( "CUtlStack.MoveFromCopiedState", CUtlStack_MoveFromCopiedState )
{
	// Moving a copied stack should preserve the copied payload and top element.
	CUtlStack< int, CUtlVectorMemory_Growable< int, int, 0 > > stack;

	stack.Push( 3 );
	stack.Push( 6 );

	CUtlStack< int, CUtlVectorMemory_Growable< int, int, 0 > > copied;

	copied.CopyFrom( stack );

	CUtlStack< int, CUtlVectorMemory_Growable< int, int, 0 > > moved( Move( copied ) );

	TEST_EQ( moved.Count(), 2 );
	TEST_EQ( moved.Top(), 6 );
}
