#include "common/assert.h"
#include "common/macros.h"

#include <tier0/platform.h>
#include <tier0/tier0_stdlib.h>

#include <csetjmp>
#include <cstring>

REGISTER_NAMED_TEST( "jmp_buf_holder.HoldsJmpBuf", jmp_buf_holder_HoldsJmpBuf )
{
	TEST_EQ( sizeof( jmp_buf_holder ), sizeof( jmp_buf ) );
}

REGISTER_NAMED_TEST( "V_setjmp.SavesContext", V_setjmp_SavesContext )
{
	jmp_buf_holder env;
	memset( &env, 0, sizeof( env ) );

	// The first return is 0 and the context is saved. tier0 saves the frame of V_setjmp itself,
	// which is gone once it returns, so the test does not jump back to it.
	TEST_EQ( V_setjmp( &env ), 0 );

	const unsigned char *pBytes = reinterpret_cast< const unsigned char * >( &env );
	bool bSaved = false;

	for ( size_t i = 0; i < sizeof( env ); i++ )
		bSaved = bSaved || pBytes[ i ] != 0;

	TEST_TRUE( bSaved );
}

REGISTER_NAMED_TEST( "V_longjmp.JumpsToSetjmp", V_longjmp_JumpsToSetjmp )
{
	jmp_buf_holder env;
	volatile int nJumps = 0;

	// A context saved by the setjmp of this frame.
	const int nValue = setjmp( env.buf );

	if ( nValue == 0 )
	{
		nJumps = nJumps + 1;
		V_longjmp( &env, 7 );
	}

	TEST_EQ( nValue, 7 );
	TEST_EQ( static_cast< int >( nJumps ), 1 );
}
