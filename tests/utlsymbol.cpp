#include "common/assert.h"
#include "common/macros.h"

#include <tier0/utlbuffer.h>
#include <tier0/utlscratchmemory.h>
#include <tier0/utlsignalslot.h>
#include <tier0/utlstring.h>
#include <tier0/utlstringtoken.h>
#include <tier0/utlsymbol.h>
#include <tier0/utlvectormemory.h>

REGISTER_NAMED_TEST( "CUtlSymbol.BasicHandle", CUtlSymbol_BasicHandle )
{
	// Symbols should expose valid and invalid handle semantics directly.
	CUtlSymbol invalid;
	CUtlSymbol valid( ( UtlSymId_t )3 );

	TEST_FALSE( invalid.IsValid() );
	TEST_TRUE( valid.IsValid() );
	TEST_EQ( valid.GetId(), ( UtlSymId_t )3 );
}
