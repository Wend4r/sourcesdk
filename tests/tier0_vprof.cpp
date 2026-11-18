#include "common/assert.h"
#include "common/macros.h"

#include <tier0/platform.h>
#include <tier0/threadtools.h>
#include <tier0/vprof.h>

#include <cstddef>
#include <cstring>

REGISTER_NAMED_TEST( "VProfile.StartPauseAndFrames", VProfile_StartPauseAndFrames )
{
	CVProfile &profile = g_VProfCurrentProfile;

	// Start and Stop nest, the profile stays enabled until the last Stop.
	profile.Start();
	TEST_TRUE( profile.IsEnabled() );
	TEST_TRUE( profile.GetRoot() == &profile.m_Root );

	// Every frame of an enabled profile counts.
	const int nFrames = profile.NumFramesSampled();
	profile.MarkFrame();
	TEST_EQ( profile.NumFramesSampled(), nFrames + 1 );

	// A paused profile is disabled and gets its depth back on Resume.
	profile.Pause();
	TEST_FALSE( profile.IsEnabled() );
	profile.MarkFrame();
	TEST_EQ( profile.NumFramesSampled(), nFrames + 1 );
	profile.Resume();
	TEST_TRUE( profile.IsEnabled() );

	TEST_TRUE( profile.GetTotalWallClockSecondsSampled() >= 0.0 );
	TEST_TRUE( profile.GetTotalTimeSampled() >= 0.0 );

	profile.Stop();
}

REGISTER_NAMED_TEST( "VProfile.TargetThreadScopes", VProfile_TargetThreadScopes )
{
	CVProfile &profile = g_VProfCurrentProfile;

	const ThreadId_t nOldTargetThread = profile.GetTargetThreadId();
	profile.SetTargetThreadId( ThreadGetCurrentId() );
	TEST_TRUE( profile.InTargetThread() );

	profile.Start();
	TEST_TRUE( profile.GetCurrentNode() == profile.GetRoot() );

	static const char s_szScope[] = "VProfile.TargetThreadScopes";
	static const char s_szGroup[] = "VProfile tests";
	VProfBudgetGroupCallSite budgetGroup( s_szGroup, BUDGETFLAG_OTHER | BUDGETFLAG_SERVER );
	CUtlSourceLocation location( __FILE__, __LINE__, __FUNCTION__ );

	// Entering a scope by name moves to its child node, creating it the first time.
	const int nNodes = CVProfNode::GetUniqueNodeCount();
	profile.EnterScopeTargetThread( s_szScope, false, budgetGroup, location );

	CVProfNode *pNode = profile.GetCurrentNode();
	TEST_TRUE( pNode != profile.GetRoot() );
	TEST_EQ( strcmp( pNode->GetName(), s_szScope ), 0 );
	TEST_TRUE( pNode->GetVParent() == profile.GetRoot() );
	TEST_FALSE( profile.AtRoot() );
	TEST_TRUE( CVProfNode::GetUniqueNodeCount() > nNodes );

	// The budget group of the node has the flags of the call site.
	const int nBudgetGroupID = pNode->GetBudgetGroupID();
	TEST_EQ( strcmp( CVProfile::GetBudgetGroupName( nBudgetGroupID ), s_szGroup ), 0 );
	TEST_EQ( CVProfile::GetBudgetGroupFlags( nBudgetGroupID ) & ( BUDGETFLAG_OTHER | BUDGETFLAG_SERVER ), BUDGETFLAG_OTHER | BUDGETFLAG_SERVER );
	TEST_EQ( CVProfile::BudgetGroupNameToBudgetGroupIDNoCreate( s_szGroup ), nBudgetGroupID );

	// A group can be hidden by its name.
	profile.HideBudgetGroup( s_szGroup, true );
	TEST_TRUE( ( CVProfile::GetBudgetGroupFlags( nBudgetGroupID ) & BUDGETFLAG_HIDDEN ) != 0 );
	profile.HideBudgetGroup( s_szGroup, false );
	TEST_TRUE( ( CVProfile::GetBudgetGroupFlags( nBudgetGroupID ) & BUDGETFLAG_HIDDEN ) == 0 );

	// Leaving it goes back to the root.
	profile.ExitScopeTargetThreadChecked();
	TEST_TRUE( profile.GetCurrentNode() == profile.GetRoot() );
	TEST_TRUE( profile.AtRoot() );
	TEST_EQ( pNode->GetCurCalls(), 1 );

	// Entering the same name again finds the node.
	profile.EnterScopeTargetThread( s_szScope, false, budgetGroup, location );
	TEST_TRUE( profile.GetCurrentNode() == pNode );
	profile.ExitScopeTargetThreadChecked();

	// Other threads are not profiled into the tree.
	profile.SetTargetThreadId( ThreadGetCurrentId() + 1 );
	TEST_FALSE( profile.InTargetThread() );
	profile.EnterScopeTargetThread( s_szScope, false, budgetGroup, location );
	TEST_TRUE( profile.GetCurrentNode() == profile.GetRoot() );

	profile.Stop();
	profile.SetTargetThreadId( nOldTargetThread );
}

// Lets about 2 ms of a frame pass
static void SpendFrameTime()
{
	const double flStart = Plat_FloatTime();
	while ( Plat_FloatTime() - flStart < 0.002 )
		;
}

// Runs a few frames of VProfLite and stops it, its history stays until VProfLite_Clear
class CTestVProfLiteFrames
{
public:
	CTestVProfLiteFrames()
	{
		VProfLite_SetEnabled( true );
		VProfLite_Clear();

		for ( int i = 0; i < 4; i++ )
		{
			VProfLite_NextFrame( 0 );
			SpendFrameTime();
		}
	}

	~CTestVProfLiteFrames()
	{
		VProfLite_Clear();
		VProfLite_SetEnabled( false );
	}
};

REGISTER_NAMED_TEST( "VProfLite_NextFrame.FrameTypes", VProfLite_NextFrame_FrameTypes )
{
	CTestVProfLiteFrames frames;

	VProfLiteHistoricalData_t data;
	VProfLite_GetHistoricalData( &data );
	const int nDiscarded = data.m_nDiscardedFrames;

	// 1 counts the frame as discarded, 2 does not.
	VProfLite_NextFrame( 1 );
	VProfLite_GetHistoricalData( &data );
	TEST_EQ( data.m_nDiscardedFrames, nDiscarded + 1 );

	VProfLite_NextFrame( 2 );
	VProfLite_GetHistoricalData( &data );
	TEST_EQ( data.m_nDiscardedFrames, nDiscarded + 1 );
}

REGISTER_NAMED_TEST( "VProfLite_GetBudgetGroupCurrentFrameStats.FrameTotal", VProfLite_GetBudgetGroupCurrentFrameStats_FrameTotal )
{
	// Zeros while VProfLite is off.
	VProfLiteFrameStats_t stats = VProfLite_GetBudgetGroupCurrentFrameStats( 1, Plat_RelativeTicks() );
	TEST_EQ( stats.m_flInclusive, 0.0f );
	TEST_EQ( stats.m_flExclusive, 0.0f );

	CTestVProfLiteFrames frames;

	// Group 1 is the frame total: the frame so far is in it.
	VProfLite_NextFrame( 0 );
	SpendFrameTime();
	stats = VProfLite_GetBudgetGroupCurrentFrameStats( 1, Plat_RelativeTicks() );
	TEST_TRUE( stats.m_flInclusive >= 0.001f );
	TEST_TRUE( stats.m_flInclusive < 1.0f );
	TEST_TRUE( stats.m_flExclusive <= stats.m_flInclusive );

	// An invalid group has no stats.
	stats = VProfLite_GetBudgetGroupCurrentFrameStats( -1, Plat_RelativeTicks() );
	TEST_EQ( stats.m_flInclusive, 0.0f );
	stats = VProfLite_GetBudgetGroupCurrentFrameStats( CVProfile::GetNumBudgetGroups(), Plat_RelativeTicks() );
	TEST_EQ( stats.m_flInclusive, 0.0f );
}

REGISTER_NAMED_TEST( "VProfLite_DedicatedServerSleepEnd.TakesSleepOut", VProfLite_DedicatedServerSleepEnd_TakesSleepOut )
{
	CTestVProfLiteFrames frames;
	VProfLite_NextFrame( 0 );

	// The sleep leaves the frame time only on the thread that VProfLite profiles, so the test
	// only checks that the frame stays consistent.
	VProfLiteHistoricalData_t data;
	VProfLite_GetHistoricalData( &data );
	const int nDiscarded = data.m_nDiscardedFrames;

	VProfLite_DedicatedServerSleepBegin();
	ThreadSleep( 5 );
	VProfLite_DedicatedServerSleepEnd( 0.005 );

	VProfLiteFrameStats_t stats = VProfLite_GetBudgetGroupCurrentFrameStats( 1, Plat_RelativeTicks() );
	TEST_TRUE( stats.m_flInclusive >= 0.0f );
	TEST_TRUE( stats.m_flExclusive <= stats.m_flInclusive );

	VProfLite_GetHistoricalData( &data );
	TEST_EQ( data.m_nDiscardedFrames, nDiscarded );
}

REGISTER_NAMED_TEST( "VProfLite_GetHistoricalData.TotalFirst", VProfLite_GetHistoricalData_TotalFirst )
{
	CTestVProfLiteFrames frames;

	VProfLiteHistoricalData_t data;
	VProfLite_GetHistoricalData( &data );
	TEST_TRUE( data.m_Items.Count() >= 1 );

	// The total, budget group 1, comes first.
	const VProfLiteReportItem_t &total = data.m_Items[ 0 ];
	TEST_EQ( total.m_nBudgetGroup, 1 );
	TEST_NOT_NULL( total.m_pszName );
	TEST_EQ( V_strcmp( total.m_pszName, "FrameTotal" ), 0 );
	TEST_TRUE( total.m_nActiveSamples >= 0 );

	// The other items are groups with samples.
	for ( int i = 1; i < data.m_Items.Count(); i++ )
	{
		TEST_TRUE( data.m_Items[ i ].m_nBudgetGroup != 1 );
		TEST_TRUE( data.m_Items[ i ].m_nActiveSamples > 0 );
	}

	// VProfLite_Clear drops the samples.
	VProfLite_Clear();
	VProfLite_GetHistoricalData( &data );
	TEST_EQ( data.m_nDiscardedFrames, 0 );
}

REGISTER_NAMED_TEST( "VProfLiteReportItem_t.Layout", VProfLiteReportItem_t_Layout )
{
	TEST_EQ( sizeof( VProfLiteFrameStats_t ), ( size_t )8 );
	TEST_EQ( sizeof( VProfLitePercentiles_t ), ( size_t )20 );
	TEST_EQ( sizeof( VProfLiteReportItem_t ), ( size_t )112 );
	TEST_EQ( offsetof( VProfLiteReportItem_t, m_Active ), ( size_t )24 );
	TEST_EQ( offsetof( VProfLiteReportItem_t, m_All ), ( size_t )44 );
	TEST_EQ( offsetof( VProfLiteReportItem_t, m_Active1SecMax ), ( size_t )64 );
	TEST_EQ( offsetof( VProfLiteReportItem_t, m_All1SecMax ), ( size_t )84 );
	TEST_EQ( offsetof( VProfLiteReportItem_t, m_flMax ), ( size_t )104 );
	TEST_EQ( offsetof( VProfLiteHistoricalData_t, m_Items ), ( size_t )8 );
}
