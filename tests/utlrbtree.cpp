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
