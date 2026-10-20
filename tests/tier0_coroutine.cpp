#include "common/assert.h"
#include "common/macros.h"

#include <tier0/platform.h>
#include <tier0/coroutine.h>

#include <csetjmp>

static int s_nCoroutineSteps;
static bool s_bCoroutineSawActive;

static void TestCoroutine( void *pParam )
{
	s_bCoroutineSawActive = Coroutine_IsActive();
	s_nCoroutineSteps++;
	Coroutine_YieldToMain();
	s_nCoroutineSteps++;
}

REGISTER_NAMED_TEST( "Coroutine_Create.RunsOnContinue", Coroutine_Create_RunsOnContinue )
{
	s_nCoroutineSteps = 0;

	// Nothing runs before the first Coroutine_Continue.
	HCoroutine hCoroutine = Coroutine_Create( TestCoroutine, nullptr );
	TEST_TRUE( hCoroutine > 0 );
	TEST_EQ( s_nCoroutineSteps, 0 );

	const bool bRunning = Coroutine_Continue( hCoroutine );
	TEST_TRUE( bRunning );
	TEST_EQ( s_nCoroutineSteps, 1 );

	// The second part finishes it.
	const bool bStillRunning = Coroutine_Continue( hCoroutine );
	TEST_FALSE( bStillRunning );
	TEST_EQ( s_nCoroutineSteps, 2 );
}

REGISTER_NAMED_TEST( "Coroutine_Continue.Name", Coroutine_Continue_Name )
{
	s_nCoroutineSteps = 0;
	HCoroutine hCoroutine = Coroutine_Create( TestCoroutine, nullptr );

	// The name only labels the profiling of the call.
	const bool bRunning = Coroutine_Continue( hCoroutine, "sourcesdk_coroutine" );
	TEST_TRUE( bRunning );

	const bool bStillRunning = Coroutine_Continue( hCoroutine, "sourcesdk_coroutine" );
	TEST_FALSE( bStillRunning );
	TEST_EQ( s_nCoroutineSteps, 2 );
}

REGISTER_NAMED_TEST( "Coroutine_Cancel.StopsCoroutine", Coroutine_Cancel_StopsCoroutine )
{
	s_nCoroutineSteps = 0;
	HCoroutine hCoroutine = Coroutine_Create( TestCoroutine, nullptr );
	Coroutine_Continue( hCoroutine );
	TEST_EQ( s_nCoroutineSteps, 1 );

	// The rest of the coroutine never runs.
	Coroutine_Cancel( hCoroutine );
	TEST_EQ( s_nCoroutineSteps, 1 );
}

REGISTER_NAMED_TEST( "Coroutine_YieldToMain.ReturnsToCaller", Coroutine_YieldToMain_ReturnsToCaller )
{
	s_nCoroutineSteps = 0;
	HCoroutine hCoroutine = Coroutine_Create( TestCoroutine, nullptr );

	// Continue returns at the yield, in the middle of the coroutine.
	TEST_TRUE( Coroutine_Continue( hCoroutine ) );
	TEST_EQ( s_nCoroutineSteps, 1 );
	TEST_FALSE( Coroutine_Continue( hCoroutine ) );
}

REGISTER_NAMED_TEST( "Coroutine_IsActive.InsideOnly", Coroutine_IsActive_InsideOnly )
{
	TEST_FALSE( Coroutine_IsActive() );

	s_bCoroutineSawActive = false;
	HCoroutine hCoroutine = Coroutine_Create( TestCoroutine, nullptr );
	Coroutine_Continue( hCoroutine );
	TEST_TRUE( s_bCoroutineSawActive );
	TEST_FALSE( Coroutine_IsActive() );
	Coroutine_Continue( hCoroutine );
}

#ifdef PLATFORM_LINUX
static jmp_buf s_LaunchExit;
static int s_nLaunchParam;

static void TestLaunchExec( void *pParam )
{
	s_nLaunchParam = static_cast< int >( reinterpret_cast< intp >( pParam ) );
}

static void TestLaunchFinish()
{
	// pfnFinish must not return.
	longjmp( s_LaunchExit, 1 );
}

REGISTER_NAMED_TEST( "Coroutine_Launch_ASM.ExecThenFinish", Coroutine_Launch_ASM_ExecThenFinish )
{
	byte *pStackHigh = nullptr;
	byte *pStackLow = nullptr;
	s_nLaunchParam = 0;

	if ( setjmp( s_LaunchExit ) == 0 )
		Coroutine_Launch_ASM( &pStackHigh, &pStackLow, TestLaunchExec, reinterpret_cast< void * >( 3 ), TestLaunchFinish );

	TEST_EQ( s_nLaunchParam, 3 );

	// The bounds of the saved registers on the current stack.
	TEST_NOT_NULL( pStackHigh );
	TEST_NOT_NULL( pStackLow );
	TEST_TRUE( pStackHigh > pStackLow );
	TEST_TRUE( pStackHigh - pStackLow <= 128 );
}

REGISTER_NAMED_TEST( "SaveNonVolatileRegs.FramePointer", SaveNonVolatileRegs_FramePointer )
{
	uint64 nRegs[ 6 ] = {};
	SaveNonVolatileRegs( nRegs );

	// rbp of this frame, which is on the stack.
	const uint64 nFrame = reinterpret_cast< uint64 >( __builtin_frame_address( 0 ) );
	const uint64 nLocal = reinterpret_cast< uint64 >( &nRegs );
	TEST_TRUE( nRegs[ 1 ] == nFrame || ( nRegs[ 1 ] > nLocal && nRegs[ 1 ] - nLocal < 0x100000 ) );
}
#endif // PLATFORM_LINUX
