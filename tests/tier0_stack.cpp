#include "common/assert.h"
#include "common/macros.h"

#include <tier0/platform.h>
#include <tier0/bufferstring.h>
#include <tier0/stackcontext.h>
#include <tier0/stackstats_base.h>
#include <tier0/stacktools.h>

#include <cstddef>
#include <cstdlib>
#include <cstring>

// A stack for the tests of the alternate stacks
class CTestAlternateStack
{
public:
	CTestAlternateStack()
	{
		m_pMemory = static_cast< uint8 * >( malloc( k_nBytes ) );
		m_Range.pLow = m_pMemory;
		m_Range.pHigh = m_pMemory + k_nBytes;
	}

	~CTestAlternateStack() { free( m_pMemory ); }

	static const size_t k_nBytes = 256 * 1024;

	uint8 *m_pMemory;
	MemoryRange_t m_Range;
};

static const CTestAlternateStack *s_pRunningStack;
static bool s_bRanOnAlternateStack;

static intp TestRunPlusOne( void *pArg )
{
	int nLocal = 0;
	s_bRanOnAlternateStack = s_pRunningStack && s_pRunningStack->m_Range.Contains( &nLocal );
	return reinterpret_cast< intp >( pArg ) + 1;
}

REGISTER_NAMED_TEST( "RunWithAlternateStack.RunsOnTheStack", RunWithAlternateStack_RunsOnTheStack )
{
	CTestAlternateStack stack;
	TEST_EQ( stack.m_Range.GetSize(), ( size_t )CTestAlternateStack::k_nBytes );
	TEST_TRUE( stack.m_Range.Contains( stack.m_pMemory ) );
	TEST_FALSE( stack.m_Range.Contains( stack.m_pMemory + CTestAlternateStack::k_nBytes ) );

	s_pRunningStack = &stack;
	s_bRanOnAlternateStack = false;

	TEST_EQ( RunWithAlternateStack( stack.m_Range, TestRunPlusOne, reinterpret_cast< void * >( 41 ), nullptr ), ( intp )42 );
	TEST_TRUE( s_bRanOnAlternateStack );
	s_pRunningStack = nullptr;
}

REGISTER_NAMED_TEST( "RunWithAlternateStack.Template", RunWithAlternateStack_Template )
{
	CTestAlternateStack stack;

	// The typed overload casts the function and the argument.
	intp ( *pfnRun )( void * ) = TestRunPlusOne;
	const intp nResult = RunWithAlternateStack< intp, void * >( stack.m_Range, pfnRun, reinterpret_cast< void * >( 9 ), nullptr );
	TEST_EQ( nResult, ( intp )10 );
}

static JmpBuf_AlternateStack_t *s_pJmpBackToCaller;
static JmpBuf_AlternateStack_t *s_pJmpBackToInner;
static intp s_nInnerResumedWith;

static intp TestRunAndJumpOut( void *pArg )
{
	// Leaves the alternate stack in the middle and continues when the caller jumps back.
	s_nInnerResumedWith = JmpToAlternateStack( *s_pJmpBackToCaller, &s_pJmpBackToInner, 7 );
	return 42;
}

REGISTER_NAMED_TEST( "JmpToAlternateStack.OutAndBack", JmpToAlternateStack_OutAndBack )
{
	CTestAlternateStack stack;
	s_pJmpBackToCaller = nullptr;
	s_pJmpBackToInner = nullptr;
	s_nInnerResumedWith = 0;

	// The jump out makes RunWithAlternateStack return the value of the jump.
	const intp nFirst = RunWithAlternateStack( stack.m_Range, TestRunAndJumpOut, nullptr, &s_pJmpBackToCaller );
	TEST_EQ( nFirst, ( intp )7 );
	TEST_NOT_NULL( s_pJmpBackToCaller );
	TEST_NOT_NULL( s_pJmpBackToInner );
	TEST_TRUE( stack.m_Range.Contains( s_pJmpBackToInner ) );

	// Jumping back continues the function, whose return ends up here.
	JmpBuf_AlternateStack_t *pJmpBack = nullptr;
	const intp nSecond = JmpToAlternateStack( *s_pJmpBackToInner, &pJmpBack, 9 );
	TEST_EQ( s_nInnerResumedWith, ( intp )9 );
	TEST_EQ( nSecond, ( intp )42 );
}

REGISTER_NAMED_TEST( "JmpBuf_AlternateStack_t.Layout", JmpBuf_AlternateStack_t_Layout )
{
	JmpBuf_AlternateStack_t jmpBuf;
	TEST_TRUE( jmpBuf.m_ExitLink.GetEnclosingJmpBuf() == &jmpBuf );
	TEST_EQ( offsetof( JmpBuf_AlternateStack_t, m_ExitLink ), ( size_t )0 );

#ifdef PLATFORM_WINDOWS
	TEST_EQ( sizeof( NonVolatileSIMDRegisters_t ), ( size_t )160 );
	TEST_EQ( sizeof( NonVolatileIntegerRegisters_t ), ( size_t )64 );
#else
	// The assembly stores r12 at +0x28 and rbp at +0x50.
	TEST_EQ( sizeof( JmpBuf_AlternateStack_t ), ( size_t )88 );
	TEST_EQ( offsetof( JmpBuf_AlternateStack_t, m_Registers.m_R12 ), ( size_t )0x28 );
	TEST_EQ( offsetof( JmpBuf_AlternateStack_t, m_Registers.m_RBP ), ( size_t )0x50 );
	TEST_EQ( sizeof( NonVolatileIntegerRegisters_t ), ( size_t )48 );
#endif
	TEST_EQ( sizeof( _SIMDRegisterAlignmentPadding_t ), sizeof( void * ) );
}

REGISTER_NAMED_TEST( "StackContext_LongJumpLink_t.Flags", StackContext_LongJumpLink_t_Flags )
{
	TEST_EQ( ( int )StackContext_LongJumpLink_t::UNBREAKABLE, 1 );
	TEST_EQ( ( int )StackContext_StartData_t::THREAD_TOP, 1 );
}

#ifdef PLATFORM_POSIX
REGISTER_NAMED_TEST( "StackContext_SetContextReturn_To_JmpToAlternateStack.ReturnFrame", StackContext_SetContextReturn_To_JmpToAlternateStack_ReturnFrame )
{
	JmpBuf_AlternateStack_t jmpBuf;
	StackContext_StartData_t context;
	memset( &context, 0, sizeof( context ) );

	// The frame pointer is the saved rbp of the buffer, the return address a part of JmpToAlternateStack.
	StackContext_SetContextReturn_To_JmpToAlternateStack( &context, &jmpBuf );
	TEST_TRUE( context.m_ParentFramePointer == reinterpret_cast< IntegerRegisterValue_t >( &jmpBuf.m_Registers.m_RBP ) );
	TEST_TRUE( context.m_ReturnAddress != 0 );
}

REGISTER_NAMED_TEST( "TestStackPointerAlignment_ASM.Aligned", TestStackPointerAlignment_ASM_Aligned )
{
	// The compiler keeps the stack aligned to 16 bytes at calls.
	TEST_TRUE( TestStackPointerAlignment_ASM() );
}

static intp TestVolatilityRun( void *pArg )
{
	return reinterpret_cast< intp >( pArg ) * 2;
}

REGISTER_NAMED_TEST( "TestStackVolatility_ASM.PreservesRegisters", TestStackVolatility_ASM_PreservesRegisters )
{
	JmpBuf_AlternateStack_t before, after;
	memset( &before, 0, sizeof( before ) );
	memset( &after, 0, sizeof( after ) );

	// The registers are saved before and after the call, which must not change them.
	TEST_EQ( TestStackVolatility_ASM( before, TestVolatilityRun, reinterpret_cast< void * >( 21 ), after ), ( intp )42 );
	TEST_TRUE( before.m_Registers.m_RBP != 0 );
	TEST_TRUE( before.m_Registers.m_R12 == after.m_Registers.m_R12 );
	TEST_TRUE( before.m_Registers.m_R15 == after.m_Registers.m_R15 );
	TEST_TRUE( before.m_Registers.m_RBX == after.m_Registers.m_RBX );
	TEST_TRUE( before.m_Registers.m_RBP == after.m_Registers.m_RBP );
}
#endif // PLATFORM_POSIX

#ifdef PLATFORM_OSX
REGISTER_NAMED_TEST( "g_pThreadSwapTarget.MainThreads", g_pThreadSwapTarget_MainThreads )
{
	// Both run on the main thread, which the tests are on.
	TEST_NOT_NULL( g_pThreadSwapTarget_Apple_Main_Thread );
	TEST_NOT_NULL( g_pThreadSwapTarget_SDL_Main_Thread );
}
#endif

REGISTER_NAMED_TEST( "GetAddressTranslationData.EmptiesOutputs", GetAddressTranslationData_EmptiesOutputs )
{
	CBufferString module( "module" ), symbol( "symbol" ), file( "file" );
	uint32 nLine = 3;
	uint64 nSymbolDisplacement = 4, nLineDisplacement = 5;

	const bool bFound = GetAddressTranslationData( reinterpret_cast< void * >( &Plat_FloatTime ), &module, &symbol, &file, &nLine, &nSymbolDisplacement, &nLineDisplacement );
	TEST_FALSE( bFound );
	TEST_TRUE( module.IsEmpty() );
	TEST_TRUE( symbol.IsEmpty() );
	TEST_TRUE( file.IsEmpty() );
	TEST_EQ( nLine, 0u );
	TEST_EQ( nSymbolDisplacement, ( uint64 )0 );
	TEST_EQ( nLineDisplacement, ( uint64 )0 );

	// Every output is optional.
	TEST_FALSE( GetAddressTranslationData( nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr ) );
}

REGISTER_NAMED_TEST( "GetAddressInlineCallData.EmptiesOutputs", GetAddressInlineCallData_EmptiesOutputs )
{
	CBufferString module( "module" ), symbol( "symbol" ), file( "file" );
	uint32 nLine = 3;
	uint64 nSymbolDisplacement = 4, nLineDisplacement = 5;

	const bool bFound = GetAddressInlineCallData( reinterpret_cast< void * >( &Plat_FloatTime ), 0, &module, &symbol, &file, &nLine, &nSymbolDisplacement, &nLineDisplacement );
	TEST_FALSE( bFound );
	TEST_TRUE( module.IsEmpty() );
	TEST_TRUE( symbol.IsEmpty() );
	TEST_TRUE( file.IsEmpty() );
	TEST_EQ( nLine, 0u );
	TEST_EQ( nSymbolDisplacement, ( uint64 )0 );
	TEST_EQ( nLineDisplacement, ( uint64 )0 );
}

REGISTER_NAMED_TEST( "StackMarkupState_t.Clear", StackMarkupState_t_Clear )
{
	// A new state is clear.
	StackMarkupState_t state;
	TEST_NULL( state.pStackTop );
	TEST_NULL( state.pStackVMSegmentMarker );

	memset( static_cast< void * >( &state ), 0xff, sizeof( state ) );
	state.Clear();
	TEST_NULL( state.pStackTop );
	TEST_NULL( state.pDeferredMarkers );
	TEST_NULL( state.pStackAnnotation );
	TEST_NULL( state.pStackVMSegmentMarker );
}

REGISTER_NAMED_TEST( "StackMarkup_SaveState.NoMarkers", StackMarkup_SaveState_NoMarkers )
{
	// CS2 keeps no markers, the state stays as it was.
	StackMarkupState_t state;
	StackMarkup_SaveState( state );
	TEST_NULL( state.pStackTop );
	TEST_NULL( state.pDeferredMarkers );
}

REGISTER_NAMED_TEST( "StackMarkup_RestoreState.NoMarkers", StackMarkup_RestoreState_NoMarkers )
{
	StackMarkupState_t state;
	StackMarkup_RestoreState( state );

	StackMarkupState_t saved;
	StackMarkup_SaveState( saved );
	TEST_NULL( saved.pStackTop );
}

static int s_nAnnotations;

static bool TestAnnotationCallback( const void *pAddress, int nIndex, const char *pszAnnotation, void *pContext )
{
	s_nAnnotations++;
	return true;
}

REGISTER_NAMED_TEST( "IterateCapturedStackAnnotations.NoAnnotations", IterateCapturedStackAnnotations_NoAnnotations )
{
	const void *pStack[] = { reinterpret_cast< void * >( &Plat_FloatTime ), reinterpret_cast< void * >( &Plat_GetTime ) };
	s_nAnnotations = 0;
	IterateCapturedStackAnnotations( pStack, 2, TestAnnotationCallback, nullptr );
	TEST_EQ( s_nAnnotations, 0 );
}

static int s_nGatherers;

static bool TestEnumGatherers( CCallStackStatsGatherer_Base *pGatherer, const char *szUniqueID, void *pContext )
{
	s_nGatherers++;
	return true;
}

REGISTER_NAMED_TEST( "StackStats_EnumerateRegisteredInstances.None", StackStats_EnumerateRegisteredInstances_None )
{
	// The gatherers are compiled out in CS2.
	s_nGatherers = 0;
	StackStats_EnumerateRegisteredInstances( TestEnumGatherers, nullptr );
	TEST_EQ( s_nGatherers, 0 );
}

REGISTER_NAMED_TEST( "StackStats_DumpNamedInstance.None", StackStats_DumpNamedInstance_None )
{
	TEST_FALSE( StackStats_DumpNamedInstance( "sourcesdk_gatherer", "sourcesdk_gatherer.txt", false, false ) );
}

REGISTER_NAMED_TEST( "StackStats_GathererRegistration.Ignored", StackStats_GathererRegistration_Ignored )
{
	// tier0 does not look at the gatherer.
	CCallStackStatsGatherer_Base *pGatherer = reinterpret_cast< CCallStackStatsGatherer_Base * >( &s_nGatherers );
	StackStats_GathererRegistration( *pGatherer, true );
	StackStats_GathererRegistration( *pGatherer, false );

	s_nGatherers = 0;
	StackStats_EnumerateRegisteredInstances( TestEnumGatherers, nullptr );
	TEST_EQ( s_nGatherers, 0 );
}

REGISTER_NAMED_TEST( "StackStats_GetSymbolTranslationDefault.Off", StackStats_GetSymbolTranslationDefault_Off )
{
	TEST_FALSE( StackStats_GetSymbolTranslationDefault() );
}

static int s_nGathererCreations;

static CCallStackStatsGatherer_Base *TestCreateGatherer( void *pContext )
{
	s_nGathererCreations++;
	return nullptr;
}

REGISTER_NAMED_TEST( "_CCallStackStatsGatherer_GetSharedGlobalInstance.None", _CCallStackStatsGatherer_GetSharedGlobalInstance_None )
{
	s_nGathererCreations = 0;
	TEST_NULL( _CCallStackStatsGatherer_GetSharedGlobalInstance( "sourcesdk_gatherer", TestCreateGatherer, nullptr ) );
	TEST_EQ( s_nGathererCreations, 0 );
}

static void TestDeleteGatherer( CCallStackStatsGatherer_Base *pGatherer )
{
	s_nGathererCreations--;
}

REGISTER_NAMED_TEST( "_CCallStackStatsGatherer_ReleaseSharedGlobalInstance.Ignored", _CCallStackStatsGatherer_ReleaseSharedGlobalInstance_Ignored )
{
	// Nothing is shared, so nothing is deleted.
	s_nGathererCreations = 0;
	_CCallStackStatsGatherer_ReleaseSharedGlobalInstance( "sourcesdk_gatherer", nullptr, nullptr, TestDeleteGatherer );
	TEST_EQ( s_nGathererCreations, 0 );
}
