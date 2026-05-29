#include "common/assert.h"
#include "common/macros.h"

#include <tier0/keyvalues3.h>
#include <tier1/keyvalues3.h>

REGISTER_NAMED_TEST( "Smoke.KeyValues3Headers.Include", Smoke_KeyValues3Headers_Include )
{
	// Including both KeyValues3 headers should expose the shared runtime symbols.
	KeyValues3 kv;

	TEST_TRUE( kv.IsNull() );
	TEST_EQ( g_KV3Format_Generic.m_name[0], 'g' );
}
