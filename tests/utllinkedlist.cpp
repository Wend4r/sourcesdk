#include "common/assert.h"
#include "common/macros.h"

#include <tier0/utllinkedlist.h>

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

REGISTER_NAMED_TEST( "CUtlLinkedList.RangeFor", CUtlLinkedList_RangeFor )
{
	// Range-for should walk the list in link order and the iterators should step both ways.
	CUtlLinkedList< int, int, false, int, CTestLinkedListMemory > list;

	list.AddToTail( 2 );
	list.AddToHead( 1 );
	list.AddToTail( 3 );

	int nExpected = 1;

	for ( int &nValue : list )
	{
		TEST_EQ( nValue, nExpected );
		nValue *= 10;
		++nExpected;
	}

	TEST_EQ( nExpected, 4 );

	const auto &constList = list;
	int nSum = 0;

	for ( const int &nValue : constList )
	{
		nSum += nValue;
	}

	TEST_EQ( nSum, 60 );

	auto it = list.end();

	--it;
	TEST_EQ( *it, 30 );
	--it;
	TEST_EQ( *it, 20 );
	++it;
	TEST_EQ( *it, 30 );

	decltype( list )::const_iterator itConst = list.begin();
	TEST_EQ( *itConst, 10 );
	TEST_TRUE( itConst == list.begin() );
	TEST_TRUE( itConst != list.end() );
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

template class CUtlBlockLinkedList< int >;

REGISTER_NAMED_TEST( "CUtlBlockLinkedList.AddRemoveIterate", CUtlBlockLinkedList_AddRemoveIterate )
{
	// Block linked lists keep their nodes in a block vector, so nodes never move.
	CUtlBlockLinkedList< int > list;

	auto iFirst = list.AddToTail( 1 );
	int *pFirst = &list[ iFirst ];

	for ( int i = 2; i <= 300; i++ )
	{
		list.AddToTail( i );
	}

	TEST_EQ( &list[ iFirst ], pFirst );

	int nExpected = 1;

	for ( auto i = list.Head(); i != list.InvalidIndex(); i = list.Next( i ) )
	{
		TEST_EQ( list[ i ], nExpected );
		++nExpected;
	}
	TEST_EQ( nExpected, 301 );

	list.Remove( iFirst );
	TEST_EQ( list[ list.Head() ], 2 );
	TEST_EQ( list.AddToHead( 0 ), iFirst );

	list.RemoveAll();
	TEST_EQ( list.Head(), list.InvalidIndex() );
	list.Purge();
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

REGISTER_NAMED_TEST( "CUtlLinkedList.MultilistElementCount", CUtlLinkedList_MultilistElementCount )
{
	// Multilist nodes count from allocation, since their invalid links already mark them as linked.
	CUtlLinkedList< int, int, true, int, CTestLinkedListMemory > list;

	auto iFirst = list.Alloc( true );
	TEST_EQ( list.Count(), 1 );

	auto iSecond = list.Alloc( true );
	list.LinkBefore( iFirst, iSecond );
	TEST_EQ( list.Count(), 2 );

	list.Free( iSecond );
	TEST_EQ( list.Count(), 1 );

	list.Free( iFirst );
	TEST_EQ( list.Count(), 0 );

	list.Alloc( true );
	list.Alloc( true );
	list.RemoveAll();
	TEST_EQ( list.Count(), 0 );
}

struct LinkedListLifetime_t
{
	static inline int s_nAlive = 0;
	static inline int s_nConstructed = 0;

	LinkedListLifetime_t() : m_nValue( 0 ) { ++s_nAlive; ++s_nConstructed; }
	LinkedListLifetime_t( const LinkedListLifetime_t &other ) : m_nValue( other.m_nValue ) { ++s_nAlive; ++s_nConstructed; }
	~LinkedListLifetime_t() { --s_nAlive; }

	int m_nValue;
};

class CLifetimeLinkedListMemory : public CUtlLeanVector< UtlLinkedListElem_t< LinkedListLifetime_t, int >, int >
{
public:
	using BaseClass = CUtlLeanVector< UtlLinkedListElem_t< LinkedListLifetime_t, int >, int >;
	using BaseClass::BaseClass;

	static inline const int INVALID_INDEX = -1;
};

template < class TList >
static void CheckLinkedListPayloadLifetime( TList &list )
{
	LinkedListLifetime_t::s_nAlive = 0;
	LinkedListLifetime_t::s_nConstructed = 0;

	auto iFirst = list.Alloc( true );
	list.Alloc( true );
	TEST_EQ( LinkedListLifetime_t::s_nConstructed, 2 );
	TEST_EQ( LinkedListLifetime_t::s_nAlive, 2 );

	list.Free( iFirst );
	TEST_EQ( LinkedListLifetime_t::s_nAlive, 1 );

	// The freed node is reused without constructing its payload twice
	list.Alloc( true );
	TEST_EQ( LinkedListLifetime_t::s_nConstructed, 3 );
	TEST_EQ( LinkedListLifetime_t::s_nAlive, 2 );

	list.Purge();
	TEST_EQ( LinkedListLifetime_t::s_nAlive, 0 );
}

REGISTER_NAMED_TEST( "CUtlLinkedList.PayloadLifetime", CUtlLinkedList_PayloadLifetime )
{
	// Payloads are constructed once per allocation and destructed once, whatever the node storage.
	CUtlLinkedList< LinkedListLifetime_t, int, true, int, CLifetimeLinkedListMemory > list;
	CheckLinkedListPayloadLifetime( list );

	CUtlBlockLinkedList< LinkedListLifetime_t > blockList;
	CheckLinkedListPayloadLifetime( blockList );
}
