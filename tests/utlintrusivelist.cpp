#include "common/assert.h"
#include "common/macros.h"

#include <tier1/utlintrusivelist.h>

struct IntrusiveListTestNode_t
{
	int m_nValue;
	IntrusiveListTestNode_t *m_pNext;
	IntrusiveListTestNode_t *m_pPrev;
};

REGISTER_NAMED_TEST( "CUtlIntrusiveList.NthNode", CUtlIntrusiveList_NthNode )
{
	// NthNode should walk the list from the head and return the node pointer.
	IntrusiveListTestNode_t nodes[ 3 ] = { { 1, nullptr, nullptr }, { 2, nullptr, nullptr }, { 3, nullptr, nullptr } };

	CUtlIntrusiveList< IntrusiveListTestNode_t > list;

	list.AddToTail( &nodes[ 0 ] );
	list.AddToTail( &nodes[ 1 ] );
	list.AddToTail( &nodes[ 2 ] );

	TEST_EQ( list.Count(), 3 );
	TEST_EQ( list.NthNode( 0 ), &nodes[ 0 ] );
	TEST_EQ( list.NthNode( 2 ), &nodes[ 2 ] );
	TEST_NULL( list.NthNode( 3 ) );
	TEST_EQ( list.PrevNode( &nodes[ 1 ] ), &nodes[ 0 ] );
}

REGISTER_NAMED_TEST( "CUtlIntrusiveDList.PrevNode", CUtlIntrusiveDList_PrevNode )
{
	// Doubly linked lists should resolve the previous node through m_pPrev.
	IntrusiveListTestNode_t nodes[ 3 ] = { { 1, nullptr, nullptr }, { 2, nullptr, nullptr }, { 3, nullptr, nullptr } };

	CUtlIntrusiveDList< IntrusiveListTestNode_t > list;

	list.AddToTail( &nodes[ 0 ] );
	list.AddToTail( &nodes[ 1 ] );
	list.AddToTail( &nodes[ 2 ] );

	TEST_EQ( list.NthNode( 1 ), &nodes[ 1 ] );
	TEST_EQ( list.PrevNode( &nodes[ 2 ] ), &nodes[ 1 ] );
	TEST_NULL( list.PrevNode( &nodes[ 0 ] ) );
	TEST_NULL( list.PrevNode( nullptr ) );

	CUtlIntrusiveDListWithTailPtr< IntrusiveListTestNode_t > tailList;

	nodes[ 0 ].m_pNext = nodes[ 0 ].m_pPrev = nullptr;
	nodes[ 1 ].m_pNext = nodes[ 1 ].m_pPrev = nullptr;
	nodes[ 2 ].m_pNext = nodes[ 2 ].m_pPrev = nullptr;

	tailList.AddToTail( &nodes[ 0 ] );
	tailList.AddToTail( &nodes[ 1 ] );
	tailList.AddToTail( &nodes[ 2 ] );

	TEST_EQ( tailList.m_pTailPtr, &nodes[ 2 ] );
	TEST_EQ( tailList.PrevNode( &nodes[ 2 ] ), &nodes[ 1 ] );
	TEST_EQ( tailList.NthNode( 2 ), &nodes[ 2 ] );
}
