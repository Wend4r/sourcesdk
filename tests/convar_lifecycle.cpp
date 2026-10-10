#include "common/assert.h"
#include "common/macros.h"

#include <tier0/interface.h>
#include <tier0/strtools.h>
#include <tier1/convar.h>
#include <interfaces/interfaces.h>
#include <icvar.h>

//-----------------------------------------------------------------------------
// The cvar system of tier0, connected and initialized once
//-----------------------------------------------------------------------------
static ICvar *GetTier0Cvar()
{
	static ICvar *s_pCvar = nullptr;
	static bool s_bInitialized = false;

	if ( !s_bInitialized )
	{
		s_bInitialized = true;

		HMODULE hTier0 = Plat_LoadModule( "tier0" );
		CreateInterfaceFn fnFactory = hTier0 ? Plat_GetModuleInterfaceFactory( hTier0 ) : nullptr;
		ICvar *pCvar = fnFactory ? static_cast< ICvar * >( fnFactory( CVAR_INTERFACE_VERSION, nullptr ) ) : nullptr;

		if ( pCvar && pCvar->Connect( fnFactory ) && pCvar->Init() == INIT_OK )
		{
			s_pCvar = pCvar;
		}
	}

	return s_pCvar;
}

static int s_nCommandCalls = 0;
static int s_nRegisterCallbackCalls = 0;

static void TestCommand_Callback( const CCommandContext &context, const CCommand &args )
{
	s_nCommandCalls++;
}

static void TestCommand_RegisterCallback( ConCommandRef *ref )
{
	s_nRegisterCallbackCalls++;
}

// Whether the tier0 cvar system knows the command and still has its callback
static bool HasCommandCallback( const char *pszName )
{
	ConCommandRef cmd = g_pCVar->FindConCommand( pszName );

	return cmd.IsValidRef() && cmd.HasCallback();
}

// Dispatches the command through the tier0 cvar system and returns how many times the callback ran
static int DispatchCommand( const char *pszName )
{
	ConCommandRef cmd = g_pCVar->FindConCommand( pszName );

	if ( !cmd.IsValidRef() )
	{
		return 0;
	}

	CCommand args;
	args.Tokenize( pszName );

	int nCallsBefore = s_nCommandCalls;
	g_pCVar->DispatchConCommand( cmd, CCommandContext( CT_NO_TARGET, CPlayerSlot( 0 ) ), args );

	return s_nCommandCalls - nCallsBefore;
}

//-----------------------------------------------------------------------------
// Installs the tier0 cvar system for the duration of a test and leaves the registration state clean
//-----------------------------------------------------------------------------
class CTier0CvarScope
{
public:
	CTier0CvarScope()
	{
		s_nRegisterCallbackCalls = 0;
		g_pCVar = GetTier0Cvar();
	}

	~CTier0CvarScope()
	{
		ConVar_Unregister();
		g_pCVar = nullptr;
	}
};

REGISTER_NAMED_TEST( "ConCommand.Tier0Cvar", ConCommand_Tier0Cvar )
{
	TEST_NOT_NULL( GetTier0Cvar() );
}

REGISTER_NAMED_TEST( "ConCommand.RegisterWithoutCvar", ConCommand_RegisterWithoutCvar )
{
	g_pCVar = nullptr;

	TEST_FALSE( ConVar_Register() );
	TEST_FALSE( ConVar_Unregister() );
}

REGISTER_NAMED_TEST( "ConCommand.QueuedUntilRegister", ConCommand_QueuedUntilRegister )
{
	CTier0CvarScope scope;
	TEST_NOT_NULL( g_pCVar );

	// Created before ConVar_Register, the command waits in the registration list.
	ConCommand *pCommand = new ConCommand( "test_lifecycle_queued", TestCommand_Callback, "", FCVAR_NONE );

	TEST_FALSE( pCommand->IsValidRef() );
	TEST_FALSE( g_pCVar->FindConCommand( "test_lifecycle_queued" ).IsValidRef() );

	TEST_TRUE( ConVar_Register( FCVAR_GAMEDLL, nullptr, TestCommand_RegisterCallback ) );
	TEST_TRUE( pCommand->IsValidRef() );
	TEST_EQ( s_nRegisterCallbackCalls, 1 );
	TEST_EQ( ConVar_GetDefaultFlags(), static_cast< uint64 >( FCVAR_GAMEDLL ) );

	ConCommandRef found = g_pCVar->FindConCommand( "test_lifecycle_queued" );
	TEST_TRUE( found.IsValidRef() );
	TEST_EQ( found.GetAccessIndex(), pCommand->GetAccessIndex() );
	TEST_TRUE( found.IsFlagSet( FCVAR_GAMEDLL ) );
	TEST_TRUE( found.HasCallback() );
	TEST_EQ( DispatchCommand( "test_lifecycle_queued" ), 1 );

	// The command stays known to tier0, only its callback is removed.
	TEST_TRUE( ConVar_Unregister() );
	TEST_FALSE( pCommand->IsValidRef() );
	TEST_TRUE( g_pCVar->FindConCommand( "test_lifecycle_queued" ).IsValidRef() );
	TEST_FALSE( HasCommandCallback( "test_lifecycle_queued" ) );
	TEST_EQ( DispatchCommand( "test_lifecycle_queued" ), 0 );
	TEST_EQ( ConVar_GetDefaultFlags(), static_cast< uint64 >( FCVAR_NONE ) );

	// The ref is already invalid, so the destructor doesn't touch tier0.
	delete pCommand;
}

REGISTER_NAMED_TEST( "ConCommand.RegisterTwice", ConCommand_RegisterTwice )
{
	CTier0CvarScope scope;
	TEST_NOT_NULL( g_pCVar );

	ConCommand *pCommand = new ConCommand( "test_lifecycle_twice", TestCommand_Callback, "", FCVAR_NONE );

	TEST_TRUE( ConVar_Register( FCVAR_NONE, nullptr, TestCommand_RegisterCallback ) );
	TEST_FALSE( ConVar_Register( FCVAR_NONE, nullptr, TestCommand_RegisterCallback ) );
	TEST_EQ( s_nRegisterCallbackCalls, 1 );

	TEST_TRUE( ConVar_Unregister() );
	TEST_FALSE( ConVar_Unregister() );
	TEST_FALSE( HasCommandCallback( "test_lifecycle_twice" ) );

	delete pCommand;
}

REGISTER_NAMED_TEST( "ConCommand.CreatedWhileRegistered", ConCommand_CreatedWhileRegistered )
{
	CTier0CvarScope scope;
	TEST_NOT_NULL( g_pCVar );

	TEST_TRUE( ConVar_Register( FCVAR_NONE, nullptr, TestCommand_RegisterCallback ) );

	{
		// Created after ConVar_Register, the command is registered right away.
		ConCommand command( "test_lifecycle_late", TestCommand_Callback, "", FCVAR_NONE );

		TEST_TRUE( command.IsValidRef() );
		TEST_EQ( s_nRegisterCallbackCalls, 1 );
		TEST_TRUE( HasCommandCallback( "test_lifecycle_late" ) );
		TEST_EQ( DispatchCommand( "test_lifecycle_late" ), 1 );
	}

	// The destructor removes its callback.
	TEST_FALSE( HasCommandCallback( "test_lifecycle_late" ) );
	TEST_EQ( DispatchCommand( "test_lifecycle_late" ), 0 );

	TEST_TRUE( ConVar_Unregister() );
}

REGISTER_NAMED_TEST( "ConCommand.UnregisterSkipsInvalidRef", ConCommand_UnregisterSkipsInvalidRef )
{
	CTier0CvarScope scope;
	TEST_NOT_NULL( g_pCVar );

	ConCommand *pRegistered = new ConCommand( "test_lifecycle_valid", TestCommand_Callback, "", FCVAR_NONE );
	ConCommand *pInvalidated = new ConCommand( "test_lifecycle_invalidated", TestCommand_Callback, "", FCVAR_NONE );

	TEST_TRUE( ConVar_Register() );
	TEST_TRUE( HasCommandCallback( "test_lifecycle_valid" ) );
	TEST_TRUE( HasCommandCallback( "test_lifecycle_invalidated" ) );

	// A command whose ref is already invalid is skipped instead of being a fatal error.
	ConCommandRef invalidated = *pInvalidated;
	pInvalidated->InvalidateRef();

	TEST_TRUE( ConVar_Unregister() );
	TEST_FALSE( pRegistered->IsValidRef() );
	TEST_FALSE( HasCommandCallback( "test_lifecycle_valid" ) );
	TEST_TRUE( HasCommandCallback( "test_lifecycle_invalidated" ) );

	g_pCVar->UnregisterConCommandCallbacks( invalidated );
	TEST_FALSE( HasCommandCallback( "test_lifecycle_invalidated" ) );

	delete pRegistered;
	delete pInvalidated;
}

REGISTER_NAMED_TEST( "ConCommand.RegisterAfterUnregister", ConCommand_RegisterAfterUnregister )
{
	CTier0CvarScope scope;
	TEST_NOT_NULL( g_pCVar );

	ConCommand *pFirst = new ConCommand( "test_lifecycle_again", TestCommand_Callback, "", FCVAR_NONE );

	TEST_TRUE( ConVar_Register() );

	uint16 nAccessIndex = pFirst->GetAccessIndex();

	TEST_TRUE( ConVar_Unregister() );
	TEST_FALSE( HasCommandCallback( "test_lifecycle_again" ) );

	// The registration lists are freed by ConVar_Unregister, the next cycle only sees new commands.
	ConCommand *pSecond = new ConCommand( "test_lifecycle_again", TestCommand_Callback, "", FCVAR_NONE );

	TEST_FALSE( pSecond->IsValidRef() );
	TEST_TRUE( ConVar_Register() );
	TEST_FALSE( pFirst->IsValidRef() );
	TEST_TRUE( pSecond->IsValidRef() );

	// tier0 keeps the command, so it gets the same slot and its callback back.
	TEST_EQ( pSecond->GetAccessIndex(), nAccessIndex );
	TEST_TRUE( HasCommandCallback( "test_lifecycle_again" ) );
	TEST_EQ( DispatchCommand( "test_lifecycle_again" ), 1 );

	TEST_TRUE( ConVar_Unregister() );
	TEST_FALSE( pSecond->IsValidRef() );
	TEST_FALSE( HasCommandCallback( "test_lifecycle_again" ) );

	delete pFirst;
	delete pSecond;
}

REGISTER_NAMED_TEST( "ConCommand.ManyQueuedCommands", ConCommand_ManyQueuedCommands )
{
	CTier0CvarScope scope;
	TEST_NOT_NULL( g_pCVar );

	// More commands than one registration list holds, so the queue spans several lists.
	const int nCommands = 150;
	static char s_szNames[ nCommands ][ 32 ];
	ConCommand *pCommands[ nCommands ];

	for ( int i = 0; i < nCommands; i++ )
	{
		V_snprintf( s_szNames[ i ], sizeof( s_szNames[ i ] ), "test_lifecycle_many_%d", i );
		pCommands[ i ] = new ConCommand( s_szNames[ i ], TestCommand_Callback, "", FCVAR_NONE );
	}

	TEST_TRUE( ConVar_Register( FCVAR_NONE, nullptr, TestCommand_RegisterCallback ) );
	TEST_EQ( s_nRegisterCallbackCalls, nCommands );

	for ( int i = 0; i < nCommands; i++ )
	{
		TEST_TRUE( pCommands[ i ]->IsValidRef() );
		TEST_TRUE( HasCommandCallback( s_szNames[ i ] ) );
	}

	TEST_TRUE( ConVar_Unregister() );

	for ( int i = 0; i < nCommands; i++ )
	{
		TEST_FALSE( pCommands[ i ]->IsValidRef() );
		TEST_FALSE( HasCommandCallback( s_szNames[ i ] ) );

		delete pCommands[ i ];
	}
}
