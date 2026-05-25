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
