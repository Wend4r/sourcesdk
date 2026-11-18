#include "common/assert.h"
#include "common/macros.h"

#include <tier1/utlntree.h>

// Instantiate the whole tree so every member, including RemoveSubTree, is compiled.
template class CUtlNTree< int, int >;

REGISTER_NAMED_TEST( "CUtlNTree.RemoveSubTree", CUtlNTree_RemoveSubTree )
{
	// Removing a subtree should free its root node.
	CUtlNTree< int, int > tree;

	int iRoot = tree.Alloc();

	tree.SetRoot( iRoot );
	tree[ iRoot ] = 1;

	TEST_EQ( tree.Count(), 1 );
	TEST_TRUE( tree.IsInTree( iRoot ) );

	tree.RemoveSubTree( iRoot );

	TEST_EQ( tree.Count(), 0 );
	TEST_FALSE( tree.IsInTree( iRoot ) );
}

REGISTER_NAMED_TEST( "CUtlNTree.ExternalMemory", CUtlNTree_ExternalMemory )
{
	// The external memory constructor should size the buffer in nodes, not in elements.
	alignas( 16 ) unsigned char pMemory[ 256 ];

	CUtlNTree< int, int > tree( pMemory, sizeof( pMemory ) );

	TEST_EQ( tree.Count(), 0 );
	TEST_EQ( tree.Root(), tree.InvalidIndex() );
}
