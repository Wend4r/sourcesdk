#include "source2_main.h"

#include "common/runner.h"

#include <stdio.h>

#ifdef _WIN32
#include <process.h>
#include <windows.h>
#else
#include <stdlib.h>
#endif

#ifndef _WIN32
void *g_pUtlSortVectorQSortContext = nullptr;
#endif

int Source2Main( int argc, char **argv )
{
	( void )argc;
	( void )argv;

	return RunAllTests();
}

[[noreturn]] void Source2TestExit( int nExitCode )
{
	fflush( stdout );
	fflush( stderr );

#ifdef _WIN32
	TerminateProcess( GetCurrentProcess(), static_cast< UINT >( nExitCode ) );
	__assume( 0 );
#else
	_Exit( nExitCode );
#endif
}
