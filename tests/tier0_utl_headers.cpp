#include "common/assert.h"
#include "common/macros.h"

#include <tier0/utlbuffer.h>
#include <tier0/utlscratchmemory.h>
#include <tier0/utlsignalslot.h>
#include <tier0/utlstring.h>
#include <tier0/utlstringtoken.h>
#include <tier0/utlsymbol.h>
#include <tier0/utlvectormemory.h>

REGISTER_NAMED_TEST( "Smoke.Tier0UtlHeaders.Include", Smoke_Tier0UtlHeaders_Include )
{
	// Tier0 utility headers should compile together and expose basic runtime types.
	CUtlString sString( "tier0" );
	CUtlStringToken token( "tier0" );

	TEST_EQ( sString.Length(), 5 );
	TEST_NE( token.GetHashCode(), 0u );
}
