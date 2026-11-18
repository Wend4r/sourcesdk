#include "common/assert.h"
#include "common/macros.h"

#include <tier0/platform.h>
#include <tier0/dbg.h>
#include <tier0/utlbinaryblock.h>

REGISTER_NAMED_TEST( "CUtlBinaryBlock.SetAndCompare", CUtlBinaryBlock_SetAndCompare )
{
	// The tier0 exports should agree with the inline accessors on where the length lives.
	CUtlBinaryBlock block;

	TEST_TRUE( block.IsEmpty() );

	block.Set( "abcdef", 6 );
	TEST_EQ( block.Length(), 6 );
	TEST_EQ( block[ 0 ], ( uchar )'a' );
	TEST_EQ( block[ 5 ], ( uchar )'f' );

	char buf[ 4 ] = {};
	block.Get( buf, 3 );
	TEST_EQ( buf[ 0 ], 'a' );
	TEST_EQ( buf[ 2 ], 'c' );

	block.SetLength( 2 );
	TEST_EQ( block.Length(), 2 );

	CUtlBinaryBlock copy;
	copy = block;
	TEST_EQ( copy.Length(), 2 );
	TEST_TRUE( copy == block );

	copy.Purge();
	TEST_TRUE( copy.IsEmpty() );
}

REGISTER_NAMED_TEST( "CUtlBinaryBlock.ExternalMemory", CUtlBinaryBlock_ExternalMemory )
{
	// An external buffer should keep its initial contents and length.
	uchar buf[ 8 ] = { 1, 2, 3, 4 };
	CUtlBinaryBlock block( buf, sizeof( buf ), 4 );

	TEST_EQ( block.Length(), 4 );
	TEST_EQ( block.Get(), ( void * )buf );
	TEST_EQ( block[ 3 ], ( uchar )4 );

	CUtlBinaryBlock moved( Move( block ) );
	TEST_EQ( moved.Length(), 4 );
	TEST_EQ( block.Length(), 0 );
}
