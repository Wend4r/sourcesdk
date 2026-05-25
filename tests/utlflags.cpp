#include "common/assert.h"
#include "common/macros.h"

#include <tier1/utlflags.h>

REGISTER_NAMED_TEST( "CUtlFlags.SetClear", CUtlFlags_SetClear )
{
	// Flag helpers should set, clear and query individual bits predictably.
	CUtlFlags< unsigned int > flags;

	TEST_FALSE( flags.IsAnyFlagSet() );

	flags.SetFlag( 0x01 );
	flags.SetFlag( 0x04 );
	TEST_TRUE( flags.IsAnyFlagSet() );
	TEST_TRUE( flags.IsFlagSet( 0x01 ) );
	TEST_TRUE( flags.IsFlagSet( 0x04 ) );
	TEST_FALSE( flags.IsFlagSet( 0x02 ) );

	flags.SetFlag( 0x01, false );
	TEST_FALSE( flags.IsFlagSet( 0x01 ) );
	TEST_TRUE( flags.IsFlagSet( 0x04 ) );

	flags.ClearAllFlags();
	TEST_FALSE( flags.IsAnyFlagSet() );
}
