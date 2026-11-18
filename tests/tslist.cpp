#include "common/assert.h"
#include "common/macros.h"

#include <tier0/tslist.h>
#include <tier0/tspointerlist.h>

#include <cstddef>

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

REGISTER_NAMED_TEST( "CTSList.IsEmpty", CTSList_IsEmpty )
{
	// A list is empty when its head has no node.
	CTSList< int > list;
	TEST_TRUE( list.IsEmpty() );

	list.PushItem( 1 );
	TEST_FALSE( list.IsEmpty() );

	int nValue = 0;
	TEST_TRUE( list.PopItem( &nValue ) );
	TEST_TRUE( list.IsEmpty() );
}

REGISTER_NAMED_TEST( "TSPointerListNode_t.Pool", TSPointerListNode_t_Pool )
{
	// The node carries the pointer after the link. MSVC places it right after the pointer of the
	// 16-aligned link, so the Windows tier0 pools nodes of 16 bytes.
#ifdef _MSC_VER
	TEST_EQ( offsetof( TSPointerListNode_t, pData ), sizeof( TSLNodeBase_t * ) );
	TEST_EQ( sizeof( TSPointerListNode_t ), static_cast< size_t >( 16 ) );
#else
	TEST_EQ( offsetof( TSPointerListNode_t, pData ), sizeof( TSLNodeBase_t ) );
#endif

	TSPointerListNode_t *pFirst = g_TSPointerListNodes.Alloc();
	TSPointerListNode_t *pSecond = g_TSPointerListNodes.Alloc();
	TEST_NOT_NULL( pFirst );
	TEST_NOT_NULL( pSecond );
	TEST_TRUE( pFirst != pSecond );

	int nValue = 0;
	pFirst->pData = &nValue;
	TEST_TRUE( pFirst->pData == &nValue );

	g_TSPointerListNodes.Free( pSecond );
	g_TSPointerListNodes.Free( pFirst );
}
