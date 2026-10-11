#include "common/assert.h"
#include "common/macros.h"

#include <tier1/utlrbtree.h>

REGISTER_NAMED_TEST( "CUtlRBTree.InsertFindIterateRemove", CUtlRBTree_InsertFindIterateRemove )
{
	// RB trees should insert, iterate in order and remove values without breaking validity.
	CUtlRBTree< int, CDefLess< int >, int > tree;

	const int nTwo = tree.Insert( 2 );

	tree.Insert( 1 );
	tree.Insert( 3 );

	TEST_EQ( tree.Count(), 3u );
	TEST_TRUE( tree.IsValid() );
	TEST_EQ( tree.Find( 2 ), nTwo );
	TEST_EQ( tree.Find( 4 ), tree.InvalidIndex() );

	int values[3] = {};
	int nCount = 0;

	for ( int i = tree.FirstInorder(); i != tree.InvalidIndex(); i = tree.NextInorder( i ) )
	{
		values[nCount++] = tree[i];
	}

	TEST_EQ( nCount, 3 );
	TEST_EQ( values[0], 1 );
	TEST_EQ( values[1], 2 );
	TEST_EQ( values[2], 3 );

	TEST_TRUE( tree.Remove( 2 ) );
	TEST_EQ( tree.Count(), 2u );
	TEST_EQ( tree.Find( 2 ), tree.InvalidIndex() );

	tree.RemoveAll();
	TEST_EQ( tree.Count(), 0u );
}

REGISTER_NAMED_TEST( "CUtlRBTree.DuplicatesDepthAndTraversal", CUtlRBTree_DuplicatesDepthAndTraversal )
{
	// Duplicate keys and traversal helpers should stay coherent after removal.
	CUtlRBTree< int, CDefLess< int >, int > tree;

	tree.EnsureCapacity( 6 );

	tree.Insert( 2 );
	const int iFirstDup = tree.Insert( 2 );
	tree.Insert( 1 );
	tree.Insert( 3 );

	TEST_TRUE( tree.Depth() >= 1 );
	const int iFoundFirst = tree.FindFirst( 2 );

	TEST_TRUE( tree.IsValidIndex( iFoundFirst ) );
	TEST_EQ( tree.Element( iFoundFirst ), 2 );
	TEST_TRUE( iFoundFirst == iFirstDup || tree.NextInorder( iFoundFirst ) == iFirstDup || tree.PrevInorder( iFoundFirst ) == iFirstDup );
	TEST_EQ( tree.Element( tree.FirstInorder() ), 1 );
	TEST_EQ( tree.Element( tree.LastInorder() ), 3 );
	TEST_EQ( tree.Element( tree.PrevInorder( tree.LastInorder() ) ), 2 );

	tree.RemoveAt( iFoundFirst );
	TEST_EQ( tree.Count(), 3u );
	TEST_TRUE( tree.Find( 2 ) != tree.InvalidIndex() );
}

REGISTER_NAMED_TEST( "CUtlRBTree.InsertBehaviorAndFindCondition", CUtlRBTree_InsertBehaviorAndFindCondition )
{
	// Insert behaviors should control duplicates and Find conditions should pick neighbours.
	CUtlRBTree< int, CDefLess< int >, int > tree;

	TEST_TRUE( tree.IsEmpty() );

	const int values[] = { 10, 20, 30 };

	tree.Insert( values, 3 );
	TEST_EQ( tree.Count(), 3 );
	TEST_FALSE( tree.IsEmpty() );

	const int iTwenty = tree.Find( 20 );
	TEST_EQ( tree.Insert( 20, k_eInsertUpdateDupes ), iTwenty );
	TEST_EQ( tree.Count(), 3 );

	const int iDupe = tree.Insert( 20, k_eInsertAllowDupes );
	TEST_TRUE( iDupe != iTwenty );
	TEST_EQ( tree.Count(), 4 );
	tree.RemoveAt( iDupe );

	TEST_TRUE( tree.HasElement( 30 ) );
	TEST_FALSE( tree.HasElement( 25 ) );

	TEST_EQ( tree.Find( 20, EXACT_MATCH ), iTwenty );
	TEST_EQ( tree.Find( 25, EXACT_MATCH ), tree.InvalidIndex() );
	TEST_EQ( tree[ tree.Find( 25, MATCH_OR_LESS ) ], 20 );
	TEST_EQ( tree[ tree.Find( 25, MATCH_OR_GREATER ) ], 30 );
	TEST_EQ( tree[ tree.Find( 15, MATCH_OR_LESS ) ], 10 );
	TEST_EQ( tree[ tree.Find( 15, MATCH_OR_GREATER ) ], 20 );
	TEST_EQ( tree.Find( 5, MATCH_OR_LESS ), tree.InvalidIndex() );
	TEST_EQ( tree[ tree.Find( 5, MATCH_OR_GREATER ) ], 10 );
	TEST_EQ( tree[ tree.Find( 35, MATCH_OR_LESS ) ], 30 );
	TEST_EQ( tree.Find( 35, MATCH_OR_GREATER ), tree.InvalidIndex() );

	bool bInserted = true;

	TEST_EQ( tree.FindOrInsert( 10, &bInserted ), tree.Find( 10 ) );
	TEST_FALSE( bInserted );

	const int iForty = tree.FindOrInsert( 40, &bInserted );
	TEST_TRUE( bInserted );
	TEST_EQ( tree[ iForty ], 40 );
	TEST_EQ( tree.Count(), 4 );
}

struct RBTreeDeleteTracked_t
{
	static inline int s_nDeleted = 0;

	~RBTreeDeleteTracked_t() { ++s_nDeleted; }
};

REGISTER_NAMED_TEST( "CUtlRBTree.PurgeAndDeleteElements", CUtlRBTree_PurgeAndDeleteElements )
{
	// Pointer trees should delete their elements when asked to.
	using Tracked_t = RBTreeDeleteTracked_t;

	CUtlRBTree< Tracked_t *, CDefLess< Tracked_t * >, int > tree;

	tree.Insert( new Tracked_t() );
	tree.Insert( new Tracked_t() );
	tree.RemoveAllAndDeleteElements();

	TEST_EQ( Tracked_t::s_nDeleted, 2 );
	TEST_TRUE( tree.IsEmpty() );

	tree.Insert( new Tracked_t() );
	tree.PurgeAndDeleteElements();

	TEST_EQ( Tracked_t::s_nDeleted, 3 );
	TEST_TRUE( tree.IsEmpty() );
}

REGISTER_NAMED_TEST( "CUtlRBTree.RangeFor", CUtlRBTree_RangeFor )
{
	// Range-for should walk the tree in order and the iterators should step both ways.
	CUtlRBTree< int, CDefLess< int >, int > tree;

	TEST_TRUE( tree.begin() == tree.end() );

	tree.Insert( 3 );
	tree.Insert( 1 );
	tree.Insert( 2 );

	int nExpected = 1;

	for ( const int &nValue : tree )
	{
		TEST_EQ( nValue, nExpected );
		++nExpected;
	}

	TEST_EQ( nExpected, 4 );

	const auto &constTree = tree;
	int nSum = 0;

	for ( const int &nValue : constTree )
	{
		nSum += nValue;
	}

	TEST_EQ( nSum, 6 );

	auto it = tree.end();

	--it;
	TEST_EQ( *it, 3 );
	--it;
	TEST_EQ( *it, 2 );
	++it;
	TEST_EQ( *it, 3 );

	decltype( tree )::const_iterator itConst = tree.begin();
	TEST_EQ( *itConst, 1 );
	TEST_TRUE( itConst == tree.begin() );
	TEST_TRUE( itConst != tree.end() );
}

struct RBTreeTracked_t
{
	static inline int s_nAlive = 0;

	RBTreeTracked_t( int nValue = 0 ) : m_nValue( nValue ) { ++s_nAlive; }
	RBTreeTracked_t( const RBTreeTracked_t &other ) : m_nValue( other.m_nValue ) { ++s_nAlive; }
	~RBTreeTracked_t() { --s_nAlive; }

	RBTreeTracked_t &operator=( const RBTreeTracked_t &other ) { m_nValue = other.m_nValue; return *this; }
	bool operator<( const RBTreeTracked_t &other ) const { return m_nValue < other.m_nValue; }

	int m_nValue;
};

REGISTER_NAMED_TEST( "CUtlRBTree.FreeListLifetimes", CUtlRBTree_FreeListLifetimes )
{
	// Freed nodes must not be destructed again or copied from.
	RBTreeTracked_t::s_nAlive = 0;

	{
		CUtlRBTree< RBTreeTracked_t, CDefLess< RBTreeTracked_t >, int > tree;

		for ( int i = 0; i < 8; ++i )
		{
			tree.Insert( RBTreeTracked_t( i ) );
		}

		tree.Remove( RBTreeTracked_t( 2 ) );
		tree.Remove( RBTreeTracked_t( 5 ) );

		TEST_EQ( tree.Count(), 6u );
		TEST_EQ( RBTreeTracked_t::s_nAlive, 6 );

		CUtlRBTree< RBTreeTracked_t, CDefLess< RBTreeTracked_t >, int > copy( tree );

		TEST_EQ( copy.Count(), 6u );
		TEST_TRUE( copy.IsValid() );
		TEST_EQ( RBTreeTracked_t::s_nAlive, 12 );
		TEST_EQ( copy.Find( RBTreeTracked_t( 2 ) ), copy.InvalidIndex() );
		TEST_NE( copy.Find( RBTreeTracked_t( 7 ) ), copy.InvalidIndex() );

		copy.Insert( RBTreeTracked_t( 9 ) );
		TEST_EQ( RBTreeTracked_t::s_nAlive, 13 );

		copy = tree;
		TEST_EQ( copy.Count(), 6u );
		TEST_EQ( RBTreeTracked_t::s_nAlive, 12 );

		tree.RemoveAll();
		TEST_EQ( RBTreeTracked_t::s_nAlive, 6 );

		tree.Insert( RBTreeTracked_t( 1 ) );
		TEST_EQ( RBTreeTracked_t::s_nAlive, 7 );
	}

	TEST_EQ( RBTreeTracked_t::s_nAlive, 0 );
}
