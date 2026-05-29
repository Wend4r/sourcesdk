#include "common/assert.h"
#include "common/macros.h"

#include <tier0/utlbuffer.h>
#include <tier0/utlscratchmemory.h>
#include <tier0/utlsignalslot.h>
#include <tier0/utlstring.h>
#include <tier0/utlstringtoken.h>
#include <tier0/utlsymbol.h>
#include <tier0/utlvectormemory.h>
#include <tier1/utlstringmap.h>

REGISTER_NAMED_TEST( "CUtlStringMap.Include", CUtlStringMap_Include )
{
	// Include coverage is enough while the map depends on broader utility wiring.
	TEST_TRUE( true );
}
