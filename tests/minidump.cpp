#include "common/assert.h"
#include "common/macros.h"

#include <tier0/platform.h>
#include <tier0/dbg.h>
#include <tier0/minidump.h>

#include <cstring>

REGISTER_NAMED_TEST( "CMiniDumpComment.Append", CMiniDumpComment_Append )
{
	// The comment buffer should hold appended text within the requested size.
	CMiniDumpComment comment( 64 );

	TEST_TRUE( comment.GetAvailableBufferSize() > 0 );

	comment.AppendComment( "tier0" );
	TEST_NOT_NULL( std::strstr( comment.GetStartPointer(), "tier0" ) );
	TEST_TRUE( comment.GetCurrentPointer() > comment.GetStartPointer() );
	TEST_TRUE( comment.GetCurrentPointer() <= comment.GetEndPointer() );

	comment.Reset();
	TEST_EQ( comment.GetCurrentPointer(), comment.GetStartPointer() );
}
