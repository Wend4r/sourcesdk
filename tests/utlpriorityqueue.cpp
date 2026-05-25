#include "common/assert.h"
#include "common/macros.h"

#include <tier1/utlleanvector.h>
#include <tier1/utlpriorityqueue.h>
#include <tier1/utlvectormemory.h>

static bool PriorityQueueLess( const int &lhs, const int &rhs )
{
	return lhs < rhs;
}

REGISTER_NAMED_TEST( "CUtlPriorityQueue.InsertRemove", CUtlPriorityQueue_InsertRemove )
{
	// Priority queues should heapify inserts, update priorities and remove the head.
	using IntPriorityQueue_t = CUtlPriorityQueue< int, int, CDefUtlPriorityQueueLessFunc< int >, CUtlVectorMemory_Growable< int, int, 0 >, CDefUtlPriorityQueueSetIndexFunc< int, int > >;

	IntPriorityQueue_t queue( 0, 0, &PriorityQueueLess );

	queue.Insert( 5 );
	queue.Insert( 2 );
	queue.Insert( 8 );
	queue.Insert( 1 );

	TEST_EQ( queue.Count(), 4 );
	TEST_TRUE( queue.IsHeapified() );
	TEST_EQ( queue.ElementAtHead(), 8 );

	queue.RemoveAtHead();
	TEST_EQ( queue.Count(), 3 );
	TEST_TRUE( queue.IsHeapified() );
	TEST_EQ( queue.ElementAtHead(), 5 );

	queue.Element( 0 ) = 0;
	queue.RevaluateElement( 0 );
	TEST_TRUE( queue.IsHeapified() );
	TEST_EQ( queue.ElementAtHead(), 2 );

	queue.RemoveAll();
	TEST_EQ( queue.Count(), 0 );
}
