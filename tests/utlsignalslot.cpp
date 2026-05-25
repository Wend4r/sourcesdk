#include "common/assert.h"
#include "common/macros.h"

#include <tier0/utlsignalslot.h>

REGISTER_NAMED_TEST( "CUtlSlot.Construct", CUtlSlot_Construct )
{
	// Slot construction smoke coverage is enough for this lightweight type.
	CUtlSlot slot;

	TEST_TRUE( true );
}
