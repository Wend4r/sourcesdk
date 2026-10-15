#include "common/assert.h"
#include "common/macros.h"

#include <tier0/platform.h>
#include <tier0/modulelistsnapshot.h>
#include <tier0/stacktools.h>
#include <tier0/threadtools.h>
#include <tier0/vmem_utils.h>

#include <cstring>

REGISTER_NAMED_TEST( "ModuleListSnapshot.ListsTheLoadedModules", ModuleListSnapshot_ListsTheLoadedModules )
{
	CModuleListSnapshot snapshot;

	TEST_TRUE( snapshot.Capture( ThreadGetCurrentProcessId() ) );
	TEST_TRUE( snapshot.GetModuleCount() > 1 );

	// Iteration visits every module once, tier0 among them.
	const void *pIterator = nullptr;
	const char *pszName = nullptr;
	void *pBase = nullptr;
	size_t nSize = 0;
	int nVisited = 0;
	bool bFoundTier0 = false;

	while ( snapshot.GetNextModule( &pIterator, &pszName, &pBase, &nSize ) )
	{
		TEST_NOT_NULL( pszName );
		TEST_NOT_NULL( pBase );
		TEST_TRUE( nSize > 0 );

		if ( strstr( pszName, "tier0" ) )
			bFoundTier0 = true;

		nVisited++;
	}

	TEST_EQ( nVisited, snapshot.GetModuleCount() );
	TEST_TRUE( bFoundTier0 );

	snapshot.Purge();
	TEST_EQ( snapshot.GetModuleCount(), 0 );
}

REGISTER_NAMED_TEST( "VirtualMemorySnapshot.DescribesTheMappedRegions", VirtualMemorySnapshot_DescribesTheMappedRegions )
{
	CVirtualMemorySnapshot snapshot;

	TEST_TRUE( snapshot.Capture( ThreadGetCurrentProcessId() ) );
	TEST_FALSE( snapshot.GetCaptureHasErrors() );
	TEST_FALSE( snapshot.IsDiff() );
	TEST_TRUE( snapshot.GetRegionsCount() > 1 );
	TEST_TRUE( snapshot.GetTotalVirtualBytes() > 0 );

	// The regions are indexed by ascending start and by descending size.
	for ( int i = 1; i < snapshot.GetRegionsCount(); i++ )
	{
		TEST_TRUE( snapshot.GetRegionByStart( i - 1 )->m_nStart < snapshot.GetRegionByStart( i )->m_nStart );
		TEST_TRUE( snapshot.GetRegionByBytes( i - 1 )->m_nBytes >= snapshot.GetRegionByBytes( i )->m_nBytes );
	}

	// A local variable lives in one of the mapped regions.
	int nLocal = 0;
	const uint64 nAddress = reinterpret_cast< uintp >( &nLocal );
	bool bFound = false;

	for ( int i = 0; i < snapshot.GetRegionsCount(); i++ )
	{
		const CVirtualMemorySnapshot::Region_t *pRegion = snapshot.GetRegion( i );

		if ( nAddress >= pRegion->m_nStart && nAddress < pRegion->m_nStart + pRegion->m_nBytes )
			bFound = true;
	}

	TEST_TRUE( bFound );
}

// A stack function that records how CCallStackCapture calls it and returns three made up frames.
static int s_nRequestedFrames;
static int s_nRequestedSkip;

static int FakeGetCallStack( ThreadId_t nThreadId, void **pReturnAddressesOut, int nArrayCount, int nSkipCount )
{
	s_nRequestedFrames = nArrayCount;
	s_nRequestedSkip = nSkipCount;

	for ( int i = 0; i < 3; i++ )
	{
		pReturnAddressesOut[ i ] = reinterpret_cast< void * >( static_cast< uintp >( 0x1000 * ( i + 1 ) ) );
	}

	return 3;
}

REGISTER_NAMED_TEST( "CallStackCapture.StoresTheFramesOfTheStackFunction", CallStackCapture_StoresTheFramesOfTheStackFunction )
{
	CCallStackCapture capture( &FakeGetCallStack, 2, 16 );

	TEST_EQ( capture.nValidEntries, 3u );
	TEST_EQ( capture.pStack[ 0 ], reinterpret_cast< void * >( 0x1000 ) );
	TEST_EQ( capture.pStack[ 2 ], reinterpret_cast< void * >( 0x3000 ) );

	// The capture asks for nMaxCapture frames and skips its own frame on top of nSkipCalls.
	TEST_EQ( s_nRequestedFrames, 16 );
	TEST_EQ( s_nRequestedSkip, 3 );

	// Without a stack function nothing is captured.
	capture.Capture( nullptr, 0, 16 );
	TEST_EQ( capture.nValidEntries, 0u );
}
