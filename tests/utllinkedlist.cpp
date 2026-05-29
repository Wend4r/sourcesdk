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
