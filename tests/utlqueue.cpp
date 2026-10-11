#include "common/assert.h"
#include "common/macros.h"

#include <tier1/utlvectormemory.h>
#include <tier1/utlqueue.h>
#include <tier0/utlstring.h>

#include <type_traits>

REGISTER_NAMED_TEST( "CUtlQueue.InsertRemoveIterate", CUtlQueue_InsertRemoveIterate )
{
	// Queues should preserve FIFO order across insertion, iteration and removal.
	CUtlQueue< int, CUtlVectorMemory_Growable< CUtlQueueNode_t< int >, int, 0 > > queue;

	TEST_TRUE( queue.IsEmpty() );
	TEST_EQ( queue.Count(), 0 );

	queue.Insert( 1 );
	queue.Insert( 2 );
	queue.Insert( 3 );

	TEST_FALSE( queue.IsEmpty() );
	TEST_EQ( queue.Count(), 3 );
	TEST_EQ( queue.Head(), 1 );
	TEST_EQ( queue.Tail(), 3 );
	TEST_TRUE( queue.Check( 2 ) );
	TEST_FALSE( queue.Check( 4 ) );

	int nSum = 0;

	for ( QueueIter_t it = queue.First(); it != QUEUE_ITERATOR_INVALID; it = queue.Next( it ) )
	{
		nSum += queue.Element( it );
	}
	TEST_EQ( nSum, 6 );

	int nRemoved = 0;

	TEST_TRUE( queue.RemoveAtHead( nRemoved ) );
	TEST_EQ( nRemoved, 1 );
	TEST_TRUE( queue.RemoveAtTail( nRemoved ) );
	TEST_EQ( nRemoved, 3 );
	TEST_EQ( queue.Count(), 1 );
	TEST_EQ( queue.Head(), 2 );

	queue.RemoveAll();
	TEST_TRUE( queue.IsEmpty() );
}

REGISTER_NAMED_TEST( "CUtlQueue.BackwardIterationAndPurge", CUtlQueue_BackwardIterationAndPurge )
{
	// Reverse iteration and purge should keep the queue internally consistent.
	CUtlQueue< int, CUtlVectorMemory_Growable< CUtlQueueNode_t< int >, int, 0 > > queue;

	queue.Insert( 10 );
	queue.Insert( 20 );
	queue.Insert( 30 );

	int nSum = 0;

	for ( QueueIter_t it = queue.Last(); it != QUEUE_ITERATOR_INVALID; it = queue.Previous( it ) )
	{
		nSum += queue.Element( it );
	}

	TEST_EQ( nSum, 60 );

	queue.Purge();
	TEST_TRUE( queue.IsEmpty() );
	TEST_EQ( queue.Count(), 0 );
}

REGISTER_NAMED_TEST( "CUtlQueue.CopyAndMove", CUtlQueue_CopyAndMove )
{
	// Copy and move operations should preserve the visible FIFO contents.
	using Queue_t = CUtlQueue< int, CUtlVectorMemory_Growable< CUtlQueueNode_t< int >, int, 0 > >;

	Queue_t queue;

	queue.Insert( 4 );
	queue.Insert( 5 );
	queue.Insert( 6 );

	Queue_t copy( queue );

	TEST_EQ( copy.Count(), 3 );
	TEST_EQ( copy.Head(), 4 );
	TEST_EQ( copy.Tail(), 6 );

	Queue_t assigned;

	assigned = queue;
	TEST_EQ( assigned.Count(), 3 );
	TEST_EQ( assigned.Head(), 4 );
	TEST_EQ( assigned.Tail(), 6 );

	Queue_t moved( Move( queue ) );

	TEST_EQ( moved.Count(), 3 );
	TEST_EQ( moved.Head(), 4 );
	TEST_EQ( moved.Tail(), 6 );

	Queue_t moveAssigned;

	moveAssigned = Move( moved );
	TEST_EQ( moveAssigned.Count(), 3 );
	TEST_EQ( moveAssigned.Head(), 4 );
	TEST_EQ( moveAssigned.Tail(), 6 );
}

REGISTER_NAMED_TEST( "CUtlQueue.RingGrowth", CUtlQueue_RingGrowth )
{
	// The default lean vector storage should wrap around and grow while keeping FIFO order.
	CUtlQueue< CUtlString > queue;

	for ( int i = 0; i < 4; ++i )
	{
		queue.Insert( CUtlString( "head" ) );
	}

	CUtlString sRemoved;

	TEST_TRUE( queue.RemoveAtHead( sRemoved ) );
	TEST_TRUE( queue.RemoveAtHead( sRemoved ) );

	char szValue[ 16 ];

	for ( int i = 0; i < 16; ++i )
	{
		V_snprintf( szValue, sizeof( szValue ), "%d", i );
		queue.Insert( CUtlString( szValue ) );
	}

	TEST_EQ( queue.Count(), 18 );
	TEST_EQ( V_strcmp( queue.Head().Get(), "head" ), 0 );
	TEST_EQ( V_strcmp( queue.Tail().Get(), "15" ), 0 );

	TEST_TRUE( queue.RemoveAtHead( sRemoved ) );
	TEST_TRUE( queue.RemoveAtHead( sRemoved ) );

	for ( int i = 0; i < 16; ++i )
	{
		V_snprintf( szValue, sizeof( szValue ), "%d", i );
		TEST_TRUE( queue.RemoveAtHead( sRemoved ) );
		TEST_EQ( V_strcmp( sRemoved.Get(), szValue ), 0 );
	}

	TEST_TRUE( queue.IsEmpty() );

	queue.Insert( CUtlString( "left" ) );

	CUtlQueue< CUtlString > copy( queue );

	TEST_EQ( copy.Count(), 1 );
	TEST_EQ( V_strcmp( copy.Head().Get(), "left" ), 0 );

	CUtlQueue< CUtlString > moved( Move( copy ) );

	TEST_TRUE( copy.IsEmpty() );
	TEST_EQ( V_strcmp( moved.Tail().Get(), "left" ), 0 );
}

REGISTER_NAMED_TEST( "CUtlQueueFixed.InsertRemove", CUtlQueueFixed_InsertRemove )
{
	// Fixed queues should keep FIFO order within their inline storage.
	CUtlQueueFixed< int, 4 > queue;

	queue.Insert( 1 );
	queue.Insert( 2 );
	queue.Insert( 3 );

	TEST_EQ( queue.Count(), 3 );
	TEST_EQ( queue.RemoveAtHead(), 1 );

	queue.Insert( 4 );
	queue.Insert( 5 );

	TEST_EQ( queue.Count(), 4 );
	TEST_EQ( queue.Head(), 2 );
	TEST_EQ( queue.Tail(), 5 );
}
