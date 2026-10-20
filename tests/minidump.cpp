#include "common/assert.h"
#include "common/macros.h"

#include <tier0/platform.h>
#include <tier0/dbg.h>
#include <tier0/minidump.h>

#include <cstring>
#include <cstddef>
#include <cstdio>
#include <cstdlib>

#include <tier0/bufferstring.h>
#include <tier0/fileio.h>
#include <tier0/strtools.h>

#ifdef PLATFORM_POSIX
#include <signal.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#endif

REGISTER_NAMED_TEST( "CMiniDumpComment.Append", CMiniDumpComment_Append )
{
	// The comment buffer should hold appended text within the requested size.
	CMiniDumpComment comment( 64 );

	TEST_TRUE( comment.GetAvailableBufferSize() > 0 );

	comment.AppendComment( "tier0" );
	TEST_NOT_NULL( std::strstr( comment.GetStartPointer(), "tier0" ) );
	TEST_TRUE( comment.GetCurrentPointer() > comment.GetStartPointer() );
	TEST_TRUE( comment.GetCurrentPointer() <= comment.GetEndPointer() );

	comment.Reset();
	TEST_EQ( comment.GetCurrentPointer(), comment.GetStartPointer() );
}

#ifdef PLATFORM_POSIX
// Runs pfnTest in a child process, whose exit code is the result; minidumps of the child do not
// touch the process of the tests, a child that hangs for 15 seconds is killed
static int RunInChildProcessOnce( int ( *pfnTest )() )
{
	const pid_t nChild = fork();

	// Without a child, the kills below would signal every process of the user
	if ( nChild < 0 )
		return -1;

	if ( nChild == 0 )
	{
		setpgid( 0, 0 );
		const int nResult = pfnTest();
		fflush( stdout );
		_exit( nResult );
	}

	int nStatus = 0;

	for ( int i = 0; i < 1500; i++ )
	{
		if ( waitpid( nChild, &nStatus, WNOHANG ) == nChild )
			return WIFEXITED( nStatus ) ? WEXITSTATUS( nStatus ) : 128 + WTERMSIG( nStatus );

		usleep( 10 * 1000 );
	}

	kill( -nChild, SIGKILL );
	kill( nChild, SIGKILL );
	waitpid( nChild, &nStatus, 0 );
	return 124;
}

// A minidump of the process stops its threads and now and then never resumes them; such a run
// is retried
static int RunInChildProcess( int ( *pfnTest )() )
{
	int nResult = 124;

	for ( int i = 0; i < 3 && nResult == 124; i++ )
		nResult = RunInChildProcessOnce( pfnTest );

	return nResult;
}

// A temporary directory for the files of the minidumps, the current one of the child
static char s_szMiniDumpDir[ 64 ];

static bool MakeMiniDumpDir()
{
	V_strncpy( s_szMiniDumpDir, "/tmp/sourcesdk_minidump_XXXXXX", sizeof( s_szMiniDumpDir ) );
	return mkdtemp( s_szMiniDumpDir ) != NULL;
}

static void RemoveMiniDumpDir()
{
	char szCommand[ 128 ];
	V_snprintf( szCommand, sizeof( szCommand ), "rm -rf %s", s_szMiniDumpDir );
	( void )!system( szCommand );
}

static int s_nTestHandlerCalls;
static MiniDumpStandardData_t s_LastHandlerData;

static int TestMiniDumpHandler( MiniDumpStandardData_t *pData )
{
	s_nTestHandlerCalls++;
	s_LastHandlerData = *pData;
	return 1;
}

REGISTER_NAMED_TEST( "GetNewMiniDumpFileName.Parts", GetNewMiniDumpFileName_Parts )
{
	CBufferString fileName;
	const char *pszName = GetNewMiniDumpFileName( "sourcesdk", &fileName, "/tmp", "prefix_", ".dmp" );
	TEST_TRUE( pszName == fileName.Get() );
	TEST_EQ( V_strncmp( pszName, "/tmp/prefix_", 12 ), 0 );
	TEST_NOT_NULL( V_strstr( pszName, "_sourcesdk.dmp" ) );

	// Without a directory the name is relative, without a reason it has no suffix.
	pszName = GetNewMiniDumpFileName( nullptr, &fileName, nullptr, nullptr, nullptr );
	TEST_EQ( V_strncmp( pszName, "./", 2 ), 0 );
	TEST_NULL( V_strstr( pszName, "sourcesdk" ) );
}

REGISTER_NAMED_TEST( "CreateNewMiniDumpFile.CreatesFile", CreateNewMiniDumpFile_CreatesFile )
{
	TEST_TRUE( MakeMiniDumpDir() );

	CBufferString fileName;
	OSFILEHANDLE hFile = CreateNewMiniDumpFile( "sourcesdk", &fileName, s_szMiniDumpDir, "prefix_", ".dmp", 3 );
	TEST_TRUE( hFile >= 0 );
	TEST_EQ( V_strncmp( fileName.Get(), s_szMiniDumpDir, V_strlen( s_szMiniDumpDir ) ), 0 );
	TEST_EQ( access( fileName.Get(), F_OK ), 0 );
	FioClose( hFile );

	RemoveMiniDumpDir();
}

REGISTER_NAMED_TEST( "InvokeMiniDumpHandler.FalseOnPosix", InvokeMiniDumpHandler_FalseOnPosix )
{
	// Only Windows raises the exception for the handler.
	s_nTestHandlerCalls = 0;
	const MiniDumpOptions_t options( "sourcesdk", "sourcesdk notes" );
	TEST_FALSE( InvokeMiniDumpHandler( TestMiniDumpHandler, &options, false ) );
	TEST_EQ( s_nTestHandlerCalls, 0 );
}

static int TestAlwaysInvoke()
{
	s_nTestHandlerCalls = 0;
	const MiniDumpOptions_t options( "sourcesdk", "sourcesdk notes" );

	if ( !AlwaysInvokeMiniDumpHandler( TestMiniDumpHandler, &options ) )
		return 1;

	// A forced breakpoint minidump with the options of the caller.
	if ( s_nTestHandlerCalls != 1 || s_LastHandlerData.m_uStructuredExceptionCode != 0x80000003 || s_LastHandlerData.m_nForced != 1 )
		return 2;

	if ( V_strcmp( s_LastHandlerData.m_Options.m_pszReason, "sourcesdk" ) != 0 || V_strcmp( s_LastHandlerData.m_Options.m_pszNotes, "sourcesdk notes" ) != 0 )
		return 3;

	return ( s_LastHandlerData.m_Options.m_nFlags & 0x40 ) ? 0 : 4;
}

REGISTER_NAMED_TEST( "AlwaysInvokeMiniDumpHandler.CallsHandler", AlwaysInvokeMiniDumpHandler_CallsHandler )
{
	TEST_EQ( RunInChildProcess( TestAlwaysInvoke ), 0 );
}

static int TestExceptionHandler()
{
	const MiniDumpOptions_t options( "sourcesdk", "sourcesdk notes" );

	// The POSIX builds only warn that the handler is a Windows one.
	return MiniDumpExceptionHandler( 0x80000003, nullptr, 0, nullptr, 1, &options ) == 0 && MiniDumpExceptionHandler( 0x80000003, nullptr, 1, nullptr, 1, &options ) == 0 ? 0 : 1;
}

REGISTER_NAMED_TEST( "MiniDumpExceptionHandler.UndefinedOnPosix", MiniDumpExceptionHandler_UndefinedOnPosix )
{
	TEST_EQ( RunInChildProcess( TestExceptionHandler ), 0 );
}

static int TestGenericHandlerEx()
{
	if ( chdir( s_szMiniDumpDir ) != 0 )
		return 1;

	MiniDumpStandardData_t data = {};
	data.m_uStructuredExceptionCode = 0x80000003;
	data.m_nForced = 1;
	data.m_Options = MiniDumpOptions_t( "sourcesdk", "sourcesdk notes" );

	char szFileName[ 512 ] = {};
	if ( !Tier0GenericMiniDumpHandlerEx( &data, szFileName, 0 ) )
		return 2;

	// The minidump of the child is in its current directory.
	if ( !V_strstr( szFileName, "sourcesdk" ) || access( szFileName, F_OK ) != 0 )
		return 3;

	// -1 returns at once and leaves the name alone.
	char szUntouched[ 8 ] = "x";
	if ( !Tier0GenericMiniDumpHandlerEx( &data, szUntouched, -1 ) || V_strcmp( szUntouched, "x" ) != 0 )
		return 4;

	return 0;
}

REGISTER_NAMED_TEST( "Tier0GenericMiniDumpHandlerEx.WritesMinidump", Tier0GenericMiniDumpHandlerEx_WritesMinidump )
{
	TEST_TRUE( MakeMiniDumpDir() );
#ifdef PLATFORM_OSX
	// The macOS tier0 has no minidump writer, the handler fails
	TEST_EQ( RunInChildProcess( TestGenericHandlerEx ), 2 );
#else
	TEST_EQ( RunInChildProcess( TestGenericHandlerEx ), 0 );
#endif
	RemoveMiniDumpDir();
}

REGISTER_NAMED_TEST( "Tier0GenericMiniDumpHandler_SetOptions.Accepts", Tier0GenericMiniDumpHandler_SetOptions_Accepts )
{
	// The defaults of tier0: no full memory, the memory filter, no handler registration.
	Tier0GenericMiniDumpHandler_SetOptions( false, true, false );
}

static int s_nPreHandlerCalls;

static void TestPreHandler( void *pContext )
{
	s_nPreHandlerCalls++;
}

REGISTER_NAMED_TEST( "Tier0GenericMiniDumpHandler_SetPreHandlerCallback.SetAndClear", Tier0GenericMiniDumpHandler_SetPreHandlerCallback_SetAndClear )
{
	s_nPreHandlerCalls = 0;
	Tier0GenericMiniDumpHandler_SetPreHandlerCallback( TestPreHandler );

	// It runs only when the generic handler writes a minidump.
	TEST_EQ( s_nPreHandlerCalls, 0 );
	Tier0GenericMiniDumpHandler_SetPreHandlerCallback( nullptr );
}

static int TestVWMDWriteDump()
{
	char szFileName[ 128 ];
	V_snprintf( szFileName, sizeof( szFileName ), "%s/self.dmp", s_szMiniDumpDir );

	if ( !VWMD_tier0_MiniDumpWriteDump( ( uint32 )getpid(), szFileName, false, ( VWMD_MINIDUMP_TYPE )0, nullptr, nullptr, nullptr ) )
		return 1;

	struct stat info;
	return stat( szFileName, &info ) == 0 && info.st_size > 0 ? 0 : 2;
}

REGISTER_NAMED_TEST( "VWMD_tier0_MiniDumpWriteDump.ProcessIdAndFileName", VWMD_tier0_MiniDumpWriteDump_ProcessIdAndFileName )
{
	TEST_TRUE( MakeMiniDumpDir() );
#ifdef PLATFORM_OSX
	// The macOS tier0 has no minidump writer
	TEST_EQ( RunInChildProcess( TestVWMDWriteDump ), 1 );
#else
	TEST_EQ( RunInChildProcess( TestVWMDWriteDump ), 0 );
#endif
	RemoveMiniDumpDir();
}

static int TestVWMDWriteDumpHandle()
{
	char szFileName[ 128 ];
	V_snprintf( szFileName, sizeof( szFileName ), "%s/handle.dmp", s_szMiniDumpDir );

	OSFILEHANDLE hFile;
	if ( FioCreateFile( szFileName, k_EFioReadWrite, k_EFioShareNone, k_EFioCreateAlways, k_EFioNoModifiers, &hFile ) != 0 )
		return 1;

	// The handle form takes the process handle and the id.
	const bool bWritten = VWMD_tier0_MiniDumpWriteDump( nullptr, ( uint32 )getpid(), hFile, ( VWMD_MINIDUMP_TYPE )0, nullptr, nullptr, nullptr );
	uint64 nBytes = 0;
	FioGetSize( hFile, &nBytes );
	FioClose( hFile );
	return bWritten && nBytes > 0 ? 0 : 2;
}

REGISTER_NAMED_TEST( "VWMD_tier0_MiniDumpWriteDump.Handle", VWMD_tier0_MiniDumpWriteDump_Handle )
{
	TEST_TRUE( MakeMiniDumpDir() );
#ifdef PLATFORM_OSX
	// The macOS tier0 has no minidump writer
	TEST_EQ( RunInChildProcess( TestVWMDWriteDumpHandle ), 2 );
#else
	TEST_EQ( RunInChildProcess( TestVWMDWriteDumpHandle ), 0 );
#endif
	RemoveMiniDumpDir();
}

REGISTER_NAMED_TEST( "VWMD_tier0_MiniDumpWriteDump.DebugProcess", VWMD_tier0_MiniDumpWriteDump_DebugProcess )
{
	// CVDebugProcess is internal to tier0, the tests have none to give it.
	bool ( *pfnWrite )( CVDebugProcess *, const char *, bool, VWMD_MINIDUMP_TYPE, _VWMD_MINIDUMP_EXCEPTION_INFORMATION *, _VWMD_MINIDUMP_USER_STREAM_INFORMATION *, _VWMD_MINIDUMP_CALLBACK_INFORMATION * ) = VWMD_tier0_MiniDumpWriteDump;
	TEST_NOT_NULL( reinterpret_cast< void * >( pfnWrite ) );
}
#endif // PLATFORM_POSIX

REGISTER_NAMED_TEST( "MinidumpDefaultWriteCallbackInit.FillsCallback", MinidumpDefaultWriteCallbackInit_FillsCallback )
{
	// The callback and its parameter, like MINIDUMP_CALLBACK_INFORMATION.
	void *pCallbackInfo[ 2 ] = { nullptr, reinterpret_cast< void * >( 1 ) };
	VWMD_MINIDUMP_CALLBACK_ROUTINE pfnCallback = MinidumpDefaultWriteCallbackInit( reinterpret_cast< _VWMD_MINIDUMP_CALLBACK_INFORMATION * >( pCallbackInfo ) );
	TEST_TRUE( pfnCallback == MinidumpDefaultWriteCallback );
	TEST_TRUE( pCallbackInfo[ 0 ] == reinterpret_cast< void * >( MinidumpDefaultWriteCallback ) );
	TEST_NULL( pCallbackInfo[ 1 ] );
}

REGISTER_NAMED_TEST( "MinidumpDefaultWriteCallback.Exported", MinidumpDefaultWriteCallback_Exported )
{
	// The inputs come from the writer of the minidump.
	TEST_NOT_NULL( reinterpret_cast< void * >( &MinidumpDefaultWriteCallback ) );
}

REGISTER_NAMED_TEST( "Tier0GenericMiniDumpHandler_SetGetAdditionalFileAttachmentCallback.Empty", Tier0GenericMiniDumpHandler_SetGetAdditionalFileAttachmentCallback_Empty )
{
	Tier0GenericMiniDumpHandler_SetGetAdditionalFileAttachmentCallback();
}

REGISTER_NAMED_TEST( "InitMiniDumpReportOptions.Empty", InitMiniDumpReportOptions_Empty )
{
	InitMiniDumpReportOptions();
}

REGISTER_NAMED_TEST( "MailMiniDump.Empty", MailMiniDump_Empty )
{
	MailMiniDump();
}

REGISTER_NAMED_TEST( "SendMiniDumpReport.Empty", SendMiniDumpReport_Empty )
{
	SendMiniDumpReport();
}

REGISTER_NAMED_TEST( "UploadFilesToAnalyzer.Disabled", UploadFilesToAnalyzer_Disabled )
{
	TEST_FALSE( UploadFilesToAnalyzer() );
}

REGISTER_NAMED_TEST( "UploadTracyFile.Disabled", UploadTracyFile_Disabled )
{
	TEST_FALSE( UploadTracyFile() );
}

REGISTER_NAMED_TEST( "MiniDumpOptions_t.Constructors", MiniDumpOptions_t_Constructors )
{
	const MiniDumpOptions_t empty;
	TEST_NULL( empty.m_pszReason );
	TEST_NULL( empty.m_pszNotes );
	TEST_EQ( empty.m_nMiniDumpTypeFlags, 0u );
	TEST_EQ( ( int )empty.m_nFlags, 0 );

	// The flags of the callers come with the reason.
	const MiniDumpOptions_t options( "reason" );
	TEST_EQ( V_strcmp( options.m_pszReason, "reason" ), 0 );
	TEST_NULL( options.m_pszNotes );
	TEST_EQ( ( int )options.m_nFlags, 0x40 );
	TEST_EQ( options.m_uExceptionCode, 0u );
}

REGISTER_NAMED_TEST( "MiniDumpOptions_t.Layout", MiniDumpOptions_t_Layout )
{
	TEST_EQ( sizeof( MiniDumpOptions_t ), ( size_t )0x68 );
	TEST_EQ( offsetof( MiniDumpOptions_t, m_nFlags ), ( size_t )0x10 );
	TEST_EQ( offsetof( MiniDumpOptions_t, m_pszNotes ), ( size_t )0x30 );
	TEST_EQ( offsetof( MiniDumpStandardData_t, m_Options ), ( size_t )0x18 );
}

#ifdef PLATFORM_WINDOWS
REGISTER_NAMED_TEST( "MinidumpLog.Writes", MinidumpLog_Writes )
{
	MinidumpLog( "sourcesdk %d\n", 1 );
	MinidumpAgeLogs();
}
#endif
