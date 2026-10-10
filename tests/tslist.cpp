#include "common/assert.h"
#include "common/macros.h"

#include <tier0/tslist.h>

REGISTER_NAMED_TEST( "CTSList.PushPop", CTSList_PushPop )
{
	// Thread-safe lists should push and pop items in LIFO order.
	CTSList< int > list;

	list.PushItem( 1 );
	list.PushItem( 2 );

	int nValue = 0;

	TEST_TRUE( list.PopItem( &nValue ) );
	TEST_EQ( nValue, 2 );
	TEST_TRUE( list.PopItem( &nValue ) );
	TEST_EQ( nValue, 1 );
	TEST_FALSE( list.PopItem( &nValue ) );
}

REGISTER_NAMED_TEST( "CTSListWithFreeList.PushPop", CTSListWithFreeList_PushPop )
{
	// Thread-safe lists with a free list should reuse popped nodes.
	CTSListWithFreeList< int > list;

	list.PushItem( 1 );
	list.PushItem( 2 );

	int nValue = 0;

	TEST_TRUE( list.PopItem( &nValue ) );
	TEST_EQ( nValue, 2 );

	list.PushItem( 3 );

	TEST_TRUE( list.PopItem( &nValue ) );
	TEST_EQ( nValue, 3 );
	TEST_TRUE( list.PopItem( &nValue ) );
	TEST_EQ( nValue, 1 );
	TEST_FALSE( list.PopItem( &nValue ) );
}
