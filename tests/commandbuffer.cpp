#include "common/assert.h"
#include "common/macros.h"

#include <tier0/platform.h>
#include <tier0/interface.h>
#include <tier0/commandbuffer.h>
#include <icvar.h>

#include <cstring>

// The factory of the linked tier0, the one CCommandBuffer runs in
DLL_IMPORT void *CreateInterface( const char *pName, int *pReturnCode );

//-----------------------------------------------------------------------------
// AddText needs the cvar system of tier0, connected and initialized once
//-----------------------------------------------------------------------------
static bool InitTier0Cvar()
{
	static bool s_bInitialized = false;

	if ( !s_bInitialized )
	{
		ICvar *pCvar = static_cast< ICvar * >( CreateInterface( CVAR_INTERFACE_VERSION, nullptr ) );

		s_bInitialized = pCvar && pCvar->Connect( CreateInterface ) && pCvar->Init() == INIT_OK;
	}

	return s_bInitialized;
}

// Runs one processing pass and returns the commands it dequeued, joined by '|'
static CUtlString ProcessCommands( CCommandBuffer &buffer, int nDeltaTicks = 1 )
{
	CUtlString sResult;

	buffer.BeginProcessingCommands( nDeltaTicks );

	const char **ppArgv = nullptr;
	int nArgc;

	while ( ( nArgc = buffer.DequeueNextCommand( ppArgv ) ) != 0 )
	{
		if ( !sResult.IsEmpty() )
		{
			sResult += "|";
		}

		for ( int i = 0; i < nArgc; i++ )
		{
			if ( i != 0 )
			{
				sResult += " ";
			}

			sResult += ppArgv[ i ];
		}
	}

	buffer.EndProcessingCommands();

	return sResult;
}

REGISTER_NAMED_TEST( "CCommandBuffer.AddTextSplitsCommands", CCommandBuffer_AddTextSplitsCommands )
{
	TEST_TRUE( InitTier0Cvar() );

	// Commands are split on ';' and new lines, quoted arguments stay whole.
	CCommandBuffer *pBuffer = new CCommandBuffer;

	TEST_FALSE( pBuffer->IsProcessingCommands() );
	TEST_TRUE( pBuffer->AddText( "echo a 1; echo \"b;c\"\nsay hi" ) );
	TEST_EQ( std::strcmp( ProcessCommands( *pBuffer ).Get(), "echo a 1|echo b;c|say hi" ), 0 );
	TEST_FALSE( pBuffer->IsProcessingCommands() );
	TEST_TRUE( ProcessCommands( *pBuffer ).IsEmpty() );

	delete pBuffer;
}

REGISTER_NAMED_TEST( "CCommandBuffer.TickDelay", CCommandBuffer_TickDelay )
{
	TEST_TRUE( InitTier0Cvar() );

	// A command added with a tick delay runs once that many ticks have passed.
	CCommandBuffer *pBuffer = new CCommandBuffer;

	TEST_TRUE( pBuffer->AddText( "later", 2 ) );
	TEST_TRUE( ProcessCommands( *pBuffer ).IsEmpty() );
	TEST_TRUE( ProcessCommands( *pBuffer ).IsEmpty() );
	TEST_EQ( std::strcmp( ProcessCommands( *pBuffer ).Get(), "later" ), 0 );

	// DelayAllQueuedCommands pushes back what is already queued.
	TEST_TRUE( pBuffer->AddText( "delayed", 1 ) );
	pBuffer->DelayAllQueuedCommands( 2 );

	for ( int i = 0; i < 3; i++ )
	{
		TEST_TRUE( ProcessCommands( *pBuffer ).IsEmpty() );
	}

	TEST_EQ( std::strcmp( ProcessCommands( *pBuffer ).Get(), "delayed" ), 0 );

	delete pBuffer;
}

REGISTER_NAMED_TEST( "CCommandBuffer.MaxCommands", CCommandBuffer_MaxCommands )
{
	TEST_TRUE( InitTier0Cvar() );

	// nMaxCommands limits how many commands of the text are queued.
	CCommandBuffer *pBuffer = new CCommandBuffer;

	TEST_TRUE( pBuffer->AddText( "m1; m2; m3", 0, 2 ) );
	TEST_EQ( std::strcmp( ProcessCommands( *pBuffer ).Get(), "m1|m2" ), 0 );

	delete pBuffer;
}

REGISTER_NAMED_TEST( "CCommandBuffer.LockAndLimits", CCommandBuffer_LockAndLimits )
{
	TEST_TRUE( InitTier0Cvar() );

	CCommandBuffer *pBuffer = new CCommandBuffer;

	// A locked buffer refuses new text.
	pBuffer->LockCommandBuffer( true );
	TEST_FALSE( pBuffer->AddText( "locked" ) );
	pBuffer->LockCommandBuffer( false );
	TEST_TRUE( ProcessCommands( *pBuffer ).IsEmpty() );

	// Text that doesn't fit the limited argument buffer is refused.
	pBuffer->LimitArgumentBufferSize( 16 );
	TEST_FALSE( pBuffer->AddText( "a_command_longer_than_the_limit" ) );
	pBuffer->LimitArgumentBufferSize( 0 );
	TEST_TRUE( pBuffer->AddText( "a_command_longer_than_the_limit" ) );
	TEST_EQ( std::strcmp( ProcessCommands( *pBuffer ).Get(), "a_command_longer_than_the_limit" ), 0 );

	// SetRequiredFlags returns the previous flags.
	TEST_EQ( pBuffer->SetRequiredFlags( FCVAR_CHEAT ), 0ull );
	TEST_EQ( pBuffer->SetRequiredFlags( FCVAR_NONE ), static_cast< uint64 >( FCVAR_CHEAT ) );

	delete pBuffer;
}

REGISTER_NAMED_TEST( "CCommandBuffer.SplitCommands", CCommandBuffer_SplitCommands )
{
	// SplitCommands appends each command of the text, nMaxCommands of 0 means no limit.
	CUtlVector< CUtlString > vecAll;

	CCommandBuffer::SplitCommands( "a 1; b \"2;3\"\nc", 0, &vecAll );
	TEST_EQ( vecAll.Count(), 3 );
	TEST_EQ( std::strcmp( vecAll[ 0 ].Get(), "a 1" ), 0 );
	TEST_EQ( std::strcmp( vecAll[ 1 ].Get(), " b \"2;3\"" ), 0 );
	TEST_EQ( std::strcmp( vecAll[ 2 ].Get(), "c" ), 0 );

	CUtlVector< CUtlString > vecLimited;

	CCommandBuffer::SplitCommands( "a 1; b \"2;3\"\nc", 2, &vecLimited );
	TEST_EQ( vecLimited.Count(), 2 );
	TEST_EQ( std::strcmp( vecLimited[ 1 ].Get(), " b \"2;3\"" ), 0 );
}
