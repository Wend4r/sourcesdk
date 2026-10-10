#include "common/assert.h"
#include "common/macros.h"

#include <tier0/strtools.h>
#include <tier1/convar.h>

REGISTER_NAMED_TEST( "CCommand.ArgListConstructor", CCommand_ArgListConstructor )
{
	// Arguments with spaces are quoted and ArgS() starts right after the 0th arg.
	const char *ppArgV[] = { "say", "hello world", "x" };
	CCommand args( ARRAYSIZE( ppArgV ), ppArgV );

	TEST_EQ( args.ArgC(), 3 );
	TEST_EQ( V_strcmp( args.Arg( 0 ), "say" ), 0 );
	TEST_EQ( V_strcmp( args.Arg( 1 ), "hello world" ), 0 );
	TEST_EQ( V_strcmp( args.Arg( 2 ), "x" ), 0 );
	TEST_EQ( V_strcmp( args.ArgS(), "\"hello world\" x" ), 0 );
	TEST_EQ( V_strcmp( args.GetCommandString(), "say \"hello world\" x" ), 0 );
}

REGISTER_NAMED_TEST( "CCommand.ArgListConstructorSingleArg", CCommand_ArgListConstructorSingleArg )
{
	// A lone command has no ArgS() and a terminated command string.
	const char *ppArgV[] = { "status" };
	CCommand args( ARRAYSIZE( ppArgV ), ppArgV );

	TEST_EQ( args.ArgC(), 1 );
	TEST_EQ( V_strcmp( args.ArgS(), "" ), 0 );
	TEST_EQ( V_strcmp( args.GetCommandString(), "status" ), 0 );
}

REGISTER_NAMED_TEST( "CCommand.ArgListConstructorOverflow", CCommand_ArgListConstructorOverflow )
{
	// Arguments which don't fit the tokenizer buffers reset the command.
	char szLong[ 1024 ];
	V_memset( szLong, 'a', sizeof( szLong ) - 1 );
	szLong[ sizeof( szLong ) - 1 ] = '\0';

	const char *ppArgV[] = { "echo", szLong };
	CCommand args( ARRAYSIZE( ppArgV ), ppArgV );

	TEST_EQ( args.ArgC(), 0 );
	TEST_EQ( V_strcmp( args.ArgS(), "" ), 0 );
	TEST_EQ( V_strcmp( args.GetCommandString(), "" ), 0 );
}
