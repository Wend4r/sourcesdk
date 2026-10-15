#include "common/assert.h"
#include "common/macros.h"

#include <tier0/platform.h>
#include <tier0/threadtools.h>

#include <cstring>

REGISTER_NAMED_TEST( "Thread.InitOnce", Thread_InitOnce )
{
	// The first caller runs the initialization, later callers see it finished.
	ThreadInitOnce_t marker = 0;

	TEST_TRUE( ThreadInitOnceCall_ShouldRun( &marker ) );
	ThreadInitOnceCall_Finish( &marker );
	TEST_FALSE( ThreadInitOnceCall_ShouldRun( &marker ) );
}

REGISTER_NAMED_TEST( "Thread.SecondaryMainThread", Thread_SecondaryMainThread )
{
	DeclareCurrentThreadIsSecondaryMainThread();
	TEST_TRUE( ThreadInSecondaryMainThread() );

	ClearSecondaryMainThread();
	TEST_FALSE( ThreadInSecondaryMainThread() );

	DeclareCurrentThreadIsMainThread();
	TEST_TRUE( ThreadInMainThread() );
	TEST_EQ( ThreadGetMainThreadId(), static_cast< ThreadId_t >( ThreadGetCurrentId() ) );
}

REGISTER_NAMED_TEST( "Thread.Helpers", Thread_Helpers )
{
	ThreadSpin( 100 );
	ThreadYield();

	// The sorted processor indices are valid logical processor numbers.
	uint16 nIndices[ 256 ];
	const uint16 nCount = GetPerfSortedLogicalProcessors( nIndices, 256 );

	for ( uint16 i = 0; i < nCount; i++ )
	{
		TEST_TRUE( nIndices[ i ] < 256 );
	}
}

REGISTER_NAMED_TEST( "Thread.CurrentProcess", Thread_CurrentProcess )
{
	const uint32 nProcessId = ThreadGetCurrentProcessId();

	TEST_TRUE( nProcessId != 0 );
	TEST_EQ( ThreadGetProcessIdFromHandle( ThreadGetCurrentProcessHandle() ), nProcessId );
	TEST_TRUE( ThreadIsProcessIdActive( nProcessId ) );

	const ThreadProcessPriority_t ePriority = ThreadGetProcessPriority( ThreadGetCurrentProcessHandle() );

	TEST_TRUE( ePriority >= THREAD_PROCESS_PRIORITY_IDLE );
	TEST_TRUE( ePriority <= THREAD_PROCESS_PRIORITY_REALTIME );
}

REGISTER_NAMED_TEST( "Thread.MultiWait", Thread_MultiWait )
{
	CThreadMultiWaitEvent autoReset( false );
	CThreadMultiWaitEvent manualReset( true );
	CThreadMultiWaitEvent *pEvents[] = { &autoReset, &manualReset };

	ThreadMultiWaitObjectSet_t *pSet = ThreadMultiWaitObjectSetCreate( pEvents, 2 );

	TEST_NOT_NULL( pSet );
	TEST_EQ( ThreadMultiWaitObjectSetGetCount( pSet ), 2 );
	TEST_EQ( pSet->m_nItems, 2 );
	TEST_EQ( pSet->m_pExtra[ 0 ].nFlags, 2u );
	TEST_EQ( pSet->m_pExtra[ 1 ].nFlags, 4u );

	uint64 nSignalled = 0;

	// Nothing is signalled, so a zero timeout returns no objects.
	TEST_EQ( ThreadMultiWait( pSet, nullptr, nullptr, 0, false, &nSignalled, 0 ), 0 );
	TEST_EQ( nSignalled, static_cast< uint64 >( 0 ) );

	// pSignalled receives a bit per signalled object.
	manualReset.Set();
	TEST_EQ( ThreadMultiWait( pSet, nullptr, nullptr, 0, false, &nSignalled, 0 ), 1 );
	TEST_EQ( nSignalled, static_cast< uint64 >( 2 ) );

	manualReset.Reset();
	autoReset.Set();
	TEST_EQ( ThreadMultiWait( pSet, nullptr, nullptr, 0, false, &nSignalled, 0 ), 1 );
	TEST_EQ( nSignalled, static_cast< uint64 >( 1 ) );

	// The auto-reset event was consumed by the wait.
	TEST_EQ( ThreadMultiWait( pSet, nullptr, nullptr, 0, false, &nSignalled, 0 ), 0 );

	ThreadMultiWaitObjectSetDestroy( pSet );
}
