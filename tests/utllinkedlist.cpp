#include "common/assert.h"
#include "common/macros.h"

#include <tier1/utllinkedlist.h>

class CTestLinkedListMemory : public CUtlLeanVector< UtlLinkedListElem_t< int, int >, int >
{
public:
	using BaseClass = CUtlLeanVector< UtlLinkedListElem_t< int, int >, int >;
	using BaseClass::BaseClass;

	static inline const int INVALID_INDEX = -1;
};

REGISTER_NAMED_TEST( "CUtlLinkedList.AddRemoveIterate", CUtlLinkedList_AddRemoveIterate )
{
	// Linked lists should preserve order across head, tail and middle insertions.
	CUtlLinkedList< int, int, false, int, CTestLinkedListMemory > list;

	auto iHead = list.AddToHead( 1 );
	auto iTail = list.AddToTail( 3 );
	auto iMiddle = list.InsertAfter( iHead, 2 );

	TEST_TRUE( list.IsValidIndex( iHead ) );
	TEST_TRUE( list.IsValidIndex( iMiddle ) );
	TEST_TRUE( list.IsValidIndex( iTail ) );
	TEST_EQ( list.Count(), 3 );

	int nExpected = 1;

	FOR_EACH_LL( list, i )
	{
		TEST_EQ( list[i], nExpected );
		++nExpected;
	}
	TEST_EQ( nExpected, 4 );

	list.Remove( iMiddle );
	TEST_EQ( list[list.Head()], 1 );
	TEST_EQ( list[list.Tail()], 3 );

	list.RemoveAll();
	TEST_EQ( list.Head(), list.InvalidIndex() );
	TEST_EQ( list.Tail(), list.InvalidIndex() );
}

REGISTER_NAMED_TEST( "CUtlPtrLinkedList.InsertAfter", CUtlPtrLinkedList_InsertAfter )
{
	// Pointer linked lists should insert after a node, including after the tail.
	CUtlPtrLinkedList< int > list;

	auto iHead = list.AddToTail( 1 );
	auto iTail = list.AddToTail( 3 );

	list.InsertAfter( iHead, 2 );
	list.InsertAfter( iTail, 4 );

	TEST_EQ( list.Count(), 4 );

	int nExpected = 1;

	for ( auto i = list.Head(); i != list.InvalidIndex(); i = list.Next( i ) )
	{
		TEST_EQ( list[ i ], nExpected );
		++nExpected;
	}
	TEST_EQ( nExpected, 5 );

	list.RemoveAll();
	TEST_EQ( list.Count(), 0 );
}

REGISTER_NAMED_TEST( "CUtlLinkedList.ElementCount", CUtlLinkedList_ElementCount )
{
	// Count() should be the number of linked elements, not the number of allocated nodes.
	CUtlLinkedList< int, int, false, int, CTestLinkedListMemory > list;

	TEST_EQ( list.Count(), 0 );

	auto iFirst = list.AddToTail( 1 );
	auto iRemoved = list.AddToTail( 2 );
	list.AddToHead( 0 );
	TEST_EQ( list.Count(), 3 );

	list.Remove( iRemoved );
	TEST_EQ( list.Count(), 2 );
	TEST_FALSE( list.IsValidIndex( iRemoved ) );

	// A freed node is reused and counted again once linked.
	TEST_EQ( list.AddToTail( 3 ), iRemoved );
	TEST_EQ( list.Count(), 3 );

	// Allocated but not linked nodes aren't counted.
	auto iUnlinked = list.Alloc();
	TEST_EQ( list.Count(), 3 );
	list.LinkAfter( iFirst, iUnlinked );
	TEST_EQ( list.Count(), 4 );
	list.Unlink( iUnlinked );
	TEST_EQ( list.Count(), 3 );
	list.Free( iUnlinked );
	TEST_EQ( list.Count(), 3 );

	list.RemoveAll();
	TEST_EQ( list.Count(), 0 );

	list.AddToTail( 4 );
	TEST_EQ( list.Count(), 1 );
	list.Purge();
	TEST_EQ( list.Count(), 0 );
}
