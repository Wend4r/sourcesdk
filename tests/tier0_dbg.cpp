#include "common/assert.h"
#include "common/macros.h"

#include <tier0/platform.h>
#include <tier0/dbg.h>

#include <cstddef>
#include <cstring>

// Records the failed assertions while it is registered
class CTestAssertionListener : public CAutoAssertionFailureListener
{
public:
	CTestAssertionListener( bool bOnlyOnce = false ) : CAutoAssertionFailureListener( bOnlyOnce ) {}

	virtual void *AssertionFailure( AssertionFailureInfo_t *pInfo )
	{
		m_nFailures++;
		m_nLine = pInfo->m_nLine;
		m_bIgnored = pInfo->m_bIgnored;
		m_bFatal = pInfo->m_bFatal;
		m_eType = pInfo->m_type;
		m_nInstanceReportCount = pInfo->m_nInstanceReportCount;
		V_strncpy( m_szFile, pInfo->m_pFile ? pInfo->m_pFile : "", sizeof( m_szFile ) );
		V_strncpy( m_szExpression, pInfo->m_pRawExpression ? pInfo->m_pRawExpression : "", sizeof( m_szExpression ) );
		V_strncpy( m_szFormattedMsg, pInfo->m_pFormattedMsg ? pInfo->m_pFormattedMsg : "", sizeof( m_szFormattedMsg ) );
		return nullptr;
	}

	int m_nFailures = 0;
	int m_nLine = 0;
	int m_nInstanceReportCount = 0;
	bool m_bIgnored = false;
	bool m_bFatal = true;
	AssertionType_t m_eType = ASSERTION_TYPE_ALL_TYPES;
	char m_szFile[ 64 ] = {};
	char m_szExpression[ 64 ] = {};
	char m_szFormattedMsg[ 256 ] = {};
};

// No dialog and no break in the tests.
static void DisableAssertionPrompts()
{
	SetAssertionPrompt( ASSERTION_TYPE_ALL_TYPES, AP_NOTHING );
}

static int s_nAssertionCallbacks;

static void TestAssertionCallback( const char *pszMessage, bool bFatal )
{
	s_nAssertionCallbacks++;
}

REGISTER_NAMED_TEST( "SetAssertionPrompt.NothingOnlyLogs", SetAssertionPrompt_NothingOnlyLogs )
{
	DisableAssertionPrompts();
	CTestAssertionListener listener;

	// The failure is reported and the caller is not asked to break.
	static const _AssertCompileTimeConstantStruct_t s_Assertion = { "sourcesdk_dbg_prompt.cpp", "PromptTest", "bPrompt", 11, 0 };
	TEST_FALSE( Assert_ConditionFailed( s_Assertion ) );
	TEST_EQ( listener.m_nFailures, 1 );
}

REGISTER_NAMED_TEST( "RegisterAssertionFailureListener.ReceivesFailures", RegisterAssertionFailureListener_ReceivesFailures )
{
	DisableAssertionPrompts();
	CTestAssertionListener listener;
	TEST_FALSE( listener.OnlyMonitorsEachAssertionOnce() );

	static const _AssertCompileTimeConstantStruct_t s_Assertion = { "sourcesdk_dbg_listener.cpp", "ListenerTest", "x == 1", 12, 0 };
	Assert_ConditionFailed( s_Assertion );
	TEST_EQ( listener.m_nFailures, 1 );
	TEST_EQ( listener.m_nLine, 12 );
	TEST_EQ( V_strcmp( listener.m_szFile, "sourcesdk_dbg_listener.cpp" ), 0 );
	TEST_EQ( V_strcmp( listener.m_szExpression, "x == 1" ), 0 );
	TEST_EQ( ( int )listener.m_eType, ( int )ASSERTION_TYPE_CODE );
	TEST_FALSE( listener.m_bFatal );
	TEST_FALSE( listener.m_bIgnored );
	TEST_TRUE( listener.m_nInstanceReportCount >= 1 );

	// The formatted message carries the text of the caller.
	Assert_ConditionFailed( s_Assertion, "sourcesdk %d", 5 );
	TEST_EQ( listener.m_nFailures, 2 );
	TEST_NOT_NULL( V_strstr( listener.m_szFormattedMsg, "sourcesdk 5" ) );
}

REGISTER_NAMED_TEST( "UnregisterAssertionFailureListener.StopsFailures", UnregisterAssertionFailureListener_StopsFailures )
{
	DisableAssertionPrompts();
	static const _AssertCompileTimeConstantStruct_t s_Assertion = { "sourcesdk_dbg_unregister.cpp", "UnregisterTest", "y", 13, 0 };

	CTestAssertionListener *pListener = new CTestAssertionListener;
	Assert_ConditionFailed( s_Assertion );
	TEST_EQ( pListener->m_nFailures, 1 );

	UnregisterAssertionFailureListener( pListener );
	Assert_ConditionFailed( s_Assertion );
	TEST_EQ( pListener->m_nFailures, 1 );

	// A second unregistration in the destructor is harmless.
	delete pListener;
}

REGISTER_NAMED_TEST( "IAssertionFailureListener.OnlyMonitorsEachAssertionOnce", IAssertionFailureListener_OnlyMonitorsEachAssertionOnce )
{
	CTestAssertionListener once( true );
	TEST_TRUE( once.OnlyMonitorsEachAssertionOnce() );

	CTestAssertionListener always( false );
	TEST_FALSE( always.OnlyMonitorsEachAssertionOnce() );
}

REGISTER_NAMED_TEST( "SetAssertionCallback.OnlyWithPrompts", SetAssertionCallback_OnlyWithPrompts )
{
	DisableAssertionPrompts();
	s_nAssertionCallbacks = 0;
	SetAssertionCallback( TestAssertionCallback );

	// The callback runs around the prompts, which AP_NOTHING turns off.
	static const _AssertCompileTimeConstantStruct_t s_Assertion = { "sourcesdk_dbg_callback.cpp", "CallbackTest", "z", 14, 0 };
	Assert_ConditionFailed( s_Assertion );
	TEST_EQ( s_nAssertionCallbacks, 0 );

	RemoveAssertionCallback( TestAssertionCallback );
}

REGISTER_NAMED_TEST( "RemoveAssertionCallback.OnlyTheCurrentOne", RemoveAssertionCallback_OnlyTheCurrentOne )
{
	// Removing another callback leaves the current one, removing it twice is harmless.
	SetAssertionCallback( TestAssertionCallback );
	RemoveAssertionCallback( nullptr );
	RemoveAssertionCallback( TestAssertionCallback );
	RemoveAssertionCallback( TestAssertionCallback );
}

REGISTER_NAMED_TEST( "SendActionToAssertDialog.NoDialog", SendActionToAssertDialog_NoDialog )
{
	// Without an open dialog there is nothing to act on.
	SendActionToAssertDialog( ASSERT_ACTION_IGNORE_THIS, "sourcesdk notes" );
	SendActionToAssertDialog( ASSERT_ACTION_INVALID, nullptr );
}

REGISTER_NAMED_TEST( "IgnoreAssertsInCurrentFile.WholeFile", IgnoreAssertsInCurrentFile_WholeFile )
{
	DisableAssertionPrompts();
	DialogInitInfo_t info;
	memset( &info, 0, sizeof( info ) );
	info.m_pFilename = "sourcesdk_dbg_ignore_file.cpp";
	info.m_iLine = 20;
	info.m_pExpression = "a";
	IgnoreAssertsInCurrentFile( &info );

	// Every line of the file is ignored, the listeners still hear of it.
	CTestAssertionListener listener;
	static const _AssertCompileTimeConstantStruct_t s_Assertion = { "sourcesdk_dbg_ignore_file.cpp", "IgnoreFileTest", "b", 900, 0 };
	Assert_ConditionFailed( s_Assertion );
	TEST_EQ( listener.m_nFailures, 1 );
	TEST_TRUE( listener.m_bIgnored );
}

REGISTER_NAMED_TEST( "IgnoreAssertsNearby.Range", IgnoreAssertsNearby_Range )
{
	DisableAssertionPrompts();
	DialogInitInfo_t info;
	memset( &info, 0, sizeof( info ) );
	info.m_pFilename = "sourcesdk_dbg_ignore_nearby.cpp";
	info.m_iLine = 50;
	info.m_pExpression = "c";
	IgnoreAssertsNearby( &info, 5, 0 );

	CTestAssertionListener listener;
	static const _AssertCompileTimeConstantStruct_t s_Near = { "sourcesdk_dbg_ignore_nearby.cpp", "NearbyTest", "c", 52, 0 };
	static const _AssertCompileTimeConstantStruct_t s_Far = { "sourcesdk_dbg_ignore_nearby.cpp", "NearbyTest", "c", 80, 0 };

	Assert_ConditionFailed( s_Near );
	TEST_TRUE( listener.m_bIgnored );
	Assert_ConditionFailed( s_Far );
	TEST_FALSE( listener.m_bIgnored );
	TEST_EQ( listener.m_nFailures, 2 );
}

REGISTER_NAMED_TEST( "IgnoreAssertForADay.Line", IgnoreAssertForADay_Line )
{
	DisableAssertionPrompts();
	DialogInitInfo_t info;
	memset( &info, 0, sizeof( info ) );
	info.m_pFilename = "sourcesdk_dbg_ignore_day.cpp";
	info.m_iLine = 7;
	info.m_pExpression = "d";
	IgnoreAssertForADay( &info );

	CTestAssertionListener listener;
	static const _AssertCompileTimeConstantStruct_t s_Assertion = { "sourcesdk_dbg_ignore_day.cpp", "DayTest", "d", 7, 0 };
	Assert_ConditionFailed( s_Assertion );
	TEST_TRUE( listener.m_bIgnored );
}

REGISTER_NAMED_TEST( "UtlRBTree_FailedInsertDuplicate.Asserts", UtlRBTree_FailedInsertDuplicate_Asserts )
{
	DisableAssertionPrompts();
	CTestAssertionListener listener;
	UtlRBTree_FailedInsertDuplicate();
	TEST_EQ( listener.m_nFailures, 1 );
	TEST_FALSE( listener.m_bFatal );
}

REGISTER_NAMED_TEST( "SortCheck_Failure.Silent", SortCheck_Failure_Silent )
{
	// Only reads the command line once, it does not assert.
	DisableAssertionPrompts();
	CTestAssertionListener listener;
	SortCheck_Failure();
	SortCheck_Failure();
	TEST_EQ( listener.m_nFailures, 0 );
}

#ifdef PLATFORM_WINDOWS
REGISTER_NAMED_TEST( "GetActiveAssertDialogHwnd.NoDialog", GetActiveAssertDialogHwnd_NoDialog )
{
	TEST_TRUE( GetActiveAssertDialogHwnd() == NULL );
}
#endif

REGISTER_NAMED_TEST( "DialogInitInfo_t.Layout", DialogInitInfo_t_Layout )
{
	TEST_EQ( sizeof( DialogInitInfo_t ), ( size_t )4136 );
	TEST_EQ( offsetof( DialogInitInfo_t, m_szMinidumpNotes ), ( size_t )38 );
}

REGISTER_NAMED_TEST( "AssertionFailureInfo_t.Layout", AssertionFailureInfo_t_Layout )
{
	TEST_EQ( offsetof( AssertionFailureInfo_t, m_nLine ), ( size_t )40 );
	TEST_EQ( offsetof( AssertionFailureInfo_t, m_type ), ( size_t )48 );
	TEST_EQ( offsetof( AssertionFailureInfo_t, m_bIgnored ), ( size_t )53 );
}

REGISTER_NAMED_TEST( "AssertAction_t.Values", AssertAction_t_Values )
{
	TEST_EQ( ( int )ASSERT_ACTION_INVALID, -1 );
	TEST_EQ( ( int )ASSERT_ACTION_IGNORE_ONEDAY, 6 );
	TEST_EQ( ( int )ASSERT_ACTION_OPEN_FULL_DIALOG, 10 );
	TEST_EQ( ( int )BROADCAST_TYPE_FATAL_ERROR, 2 );
	TEST_EQ( ( int )AP_BREAK_OR_EXIT, 6 );
	TEST_EQ( ( int )ASSERTION_TYPE_CONTENT, 1 );
	TEST_EQ( ( int )ASSERTION_TYPE_ALL_TYPES, -1 );
}
