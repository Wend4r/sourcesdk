#include "common/assert.h"
#include "common/macros.h"

#include <tier0/platform.h>
#include <tier0/threadtools.h>
#include <tier0/vprof.h>

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
