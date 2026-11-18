#include "common/assert.h"
#include "common/macros.h"

#include <tier0/platform.h>
#include <tier0/interface.h>
#include <tier0/memalloc.h>
#include <tier0/bufferstring.h>
#include <tier0/stacktools.h>
#include <tier0/appinstance.h>
#include <tier0/mem.h>
#include <tier0/platwindow.h>
#include <tier0/random.h>
#include <tier0/threadtools.h>
#include <tier0/utlleanvector.h>
#include <tier0/utlvector.h>
#include <tier0/win32consoleio.h>

#include <cmath>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cstddef>
#include <xmmintrin.h>

#ifdef PLATFORM_WINDOWS
#include <windows.h>
#include <evntprov.h>
#else
#include <dlfcn.h>
#include <stdlib.h>
#endif

REGISTER_NAMED_TEST( "Plat.RelativeTicks", Plat_RelativeTicks_Conversions )
{
	// Conversions go through the relative tick frequency, so one second of ticks reads back exactly.
	const uint64 nFrequency = Plat_RelativeTickFrequency();

	TEST_TRUE( nFrequency > 0 );

	const uint64 nStart = Plat_RelativeTicks();

	TEST_TRUE( Plat_RelativeTicks() >= nStart );

	TEST_EQ( Plat_TickDiffSec( nStart, nStart + nFrequency ), static_cast< uint64 >( 1 ) );
	TEST_EQ( Plat_TickDiffMilliSec( nStart, nStart + nFrequency ), static_cast< uint64 >( 1000 ) );
	TEST_EQ( Plat_TickDiffMicroSec( nStart, nStart + nFrequency ), static_cast< uint64 >( 1000000 ) );
	TEST_TRUE( std::fabs( Plat_TickDiffSecF( nStart, nStart + nFrequency ) - 1.0 ) < 1e-9 );
	TEST_TRUE( std::fabs( Plat_TickDiffMilliSecF( nStart, nStart + nFrequency ) - 1000.0 ) < 1e-6 );
	TEST_TRUE( std::fabs( Plat_TickDiffMicroSecF( nStart, nStart + nFrequency ) - 1000000.0 ) < 1e-3 );

	TEST_EQ( Plat_SecTickDiff( 2 ), nFrequency * 2 );
	TEST_EQ( Plat_MilliSecTickDiff( 2000 ), nFrequency * 2 );
	TEST_EQ( Plat_MicroSecTickDiff( 2000000 ), nFrequency * 2 );

	// The TicksAfter functions add a duration to the current tick count.
	const uint64 nBefore = Plat_RelativeTicks();
	const uint64 nAfter = Plat_TicksAfterSecs( 1 );

	TEST_TRUE( nAfter >= nBefore + nFrequency );
	TEST_TRUE( nAfter <= Plat_RelativeTicks() + nFrequency );
	TEST_TRUE( Plat_TicksAfterMilliSecs( 1000 ) >= nBefore + nFrequency );
	TEST_TRUE( Plat_TicksAfterMicroSecs( 1000000 ) >= nBefore + nFrequency );
}

REGISTER_NAMED_TEST( "Plat.CurrentFrame", Plat_CurrentFrame )
{
	const uint64 nSaved = Plat_GetCurrentFrame();

	Plat_SetCurrentFrame( 100 );
	TEST_EQ( Plat_GetCurrentFrame(), static_cast< uint64 >( 100 ) );

	Plat_ChangeCurrentFrame( 5 );
	TEST_EQ( Plat_GetCurrentFrame(), static_cast< uint64 >( 105 ) );

	Plat_ChangeCurrentFrame( -5 );
	TEST_EQ( Plat_GetCurrentFrame(), static_cast< uint64 >( 100 ) );

	Plat_SetCurrentFrame( nSaved );
}

REGISTER_NAMED_TEST( "Plat.CalendarTime", Plat_CalendarTime )
{
	struct tm time = {};

	time.tm_year = 2020 - 1900;
	time.tm_mon = 0;
	time.tm_mday = 2;
	time.tm_hour = 3;
	time.tm_min = 4;
	time.tm_sec = 5;
	time.tm_isdst = -1;

	TEST_TRUE( Plat_mktime64( &time ) != static_cast< time_t >( -1 ) );

	char szBuffer[ 64 ];

	TEST_NOT_NULL( Plat_asctime( &time, szBuffer, sizeof( szBuffer ) ) );
	TEST_EQ( std::strncmp( szBuffer, "Thu Jan  2 03:04:05 2020", 24 ), 0 );
}

REGISTER_NAMED_TEST( "Plat.Environment", Plat_Environment )
{
	const char *pName = "SOURCESDK_TIER0_PLATFORM_TEST";

	Plat_RemoveEnv( pName );
	TEST_FALSE( Plat_HasEnv( pName ) );

	TEST_TRUE( Plat_SetEnv( pName, "value", true ) );
	TEST_TRUE( Plat_HasEnv( pName ) );
	TEST_NOT_NULL( Plat_GetCrtEnv( pName ) );
	TEST_EQ( std::strcmp( Plat_GetCrtEnv( pName ), "value" ), 0 );

	// Without bOverwrite an existing variable is kept.
	TEST_FALSE( Plat_SetEnv( pName, "other", false ) );
	TEST_EQ( std::strcmp( Plat_GetCrtEnv( pName ), "value" ), 0 );

	// Syntax 1 expands ${NAME} references; bAppend keeps the previous contents of the output.
	CBufferString expanded;

	TEST_EQ( std::strcmp( Plat_ExpandEnvVars( "a${SOURCESDK_TIER0_PLATFORM_TEST}b", expanded, false, 1 ), "avalueb" ), 0 );
	TEST_EQ( std::strcmp( Plat_ExpandEnvVars( "c", expanded, true, 1 ), "avaluebc" ), 0 );

	TEST_TRUE( Plat_RemoveEnv( pName ) );
	TEST_FALSE( Plat_HasEnv( pName ) );
}

REGISTER_NAMED_TEST( "Plat.FilesAndDirectories", Plat_FilesAndDirectories )
{
	CBufferString tempDir;

	TEST_TRUE( Plat_GetTempDirectory( tempDir ) );
	TEST_TRUE( Plat_IsDirectory( tempDir.Get() ) );

	CBufferString root;

	TEST_TRUE( Plat_CreateTempDirectory( root, "sourcesdk", false, nullptr ) );
	TEST_TRUE( Plat_IsDirectory( root.Get() ) );

	CBufferString nested( root.Get() );

	nested.Append( "/a/b" );
	TEST_TRUE( Plat_CreateDirectoryTree( nested.Get(), false ) );
	TEST_TRUE( Plat_IsDirectory( nested.Get() ) );
	TEST_TRUE( Plat_EnsureDirectory( nested.Get(), false ) );

	CBufferString forFile( root.Get() );

	forFile.Append( "/c/file.txt" );
	TEST_TRUE( Plat_CreateDirectoryTreeForFile( forFile.Get(), false ) );
	TEST_FALSE( Plat_IsFile( forFile.Get() ) );

	CBufferString file( nested.Get() );

	file.Append( "/file.txt" );

	FILE *pFile = std::fopen( file.Get(), "wb" );

	TEST_NOT_NULL( pFile );
	std::fputs( "tier0", pFile );
	std::fclose( pFile );

	TEST_TRUE( Plat_IsFile( file.Get() ) );
	TEST_FALSE( Plat_IsDirectory( file.Get() ) );
	TEST_FALSE( Plat_IsSymlink( file.Get() ) );

	// The whole file read allocates through g_pMemAlloc and can append a terminator.
	void *pBuffer = nullptr;
	uint32 nSize = 0;

	TEST_EQ( PlatC_ReadFileToMemory( file.Get(), &pBuffer, &nSize, true, 1024 ), 0 );
	TEST_NOT_NULL( pBuffer );
	TEST_EQ( nSize, static_cast< uint32 >( 5 ) );
	TEST_EQ( std::strcmp( static_cast< const char * >( pBuffer ), "tier0" ), 0 );
	g_pMemAlloc->Free( pBuffer );

	// A file larger than nMaxSize is rejected without a buffer.
	pBuffer = nullptr;
	TEST_TRUE( PlatC_ReadFileToMemory( file.Get(), &pBuffer, &nSize, false, 2 ) != 0 );
	TEST_NULL( pBuffer );

	TEST_TRUE( Plat_SetFileReadOnly( file.Get(), true ) );
	TEST_TRUE( Plat_FileIsReadOnly( file.Get() ) );
	TEST_TRUE( Plat_SetFileReadOnly( file.Get(), false ) );
	TEST_FALSE( Plat_FileIsReadOnly( file.Get() ) );

	CBufferString renamed( nested.Get() );

	renamed.Append( "/renamed.txt" );
	TEST_TRUE( Plat_RenameFile( file.Get(), renamed.Get() ) );
	TEST_FALSE( Plat_IsFile( file.Get() ) );
	TEST_TRUE( Plat_IsFile( renamed.Get() ) );

	TEST_TRUE( Plat_RemoveFile( renamed.Get() ) );
	TEST_FALSE( Plat_IsFile( renamed.Get() ) );

	TEST_TRUE( Plat_RemoveDirectoryTree( root.Get() ) );
	TEST_FALSE( Plat_IsDirectory( root.Get() ) );
}

REGISTER_NAMED_TEST( "Plat.CurrentDirectory", Plat_CurrentDirectory )
{
	CBufferString current;

	TEST_TRUE( Plat_GetCurrentDirectory( current, false ) );
	TEST_TRUE( current.Length() > 0 );
	TEST_TRUE( Plat_IsDirectory( current.Get() ) );
	TEST_TRUE( Plat_SetCurrentDirectory( current.Get() ) );
}

REGISTER_NAMED_TEST( "Plat.UUID", Plat_UUID )
{
	const V_uuid_t uuid = { 0x12345678, 0x9abc, 0xdef0, { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08 } };
	char szText[ 64 ];

	Plat_UUIDToString( &uuid, szText, sizeof( szText ) );
	TEST_EQ( std::strcmp( szText, "12345678-9abc-def0-0102-030405060708" ), 0 );

	V_uuid_t parsed;

	TEST_TRUE( Plat_UUIDFromString( &parsed, szText ) );
	TEST_EQ( std::memcmp( &parsed, &uuid, sizeof( uuid ) ), 0 );

	TEST_FALSE( Plat_UUIDFromString( &parsed, "not-a-uuid" ) );

	// Generated identifiers are version 4 with the RFC 4122 variant.
	V_uuid_t generated;

	Plat_CreateUUIDImpl( &generated, []( int nMin, int nMax ) { return nMin + ( nMax - nMin ) / 3; } );
	TEST_EQ( generated.Data3 & 0xF000, 0x4000 );
	TEST_EQ( generated.Data4[ 0 ] & 0xC0, 0x80 );
}

REGISTER_NAMED_TEST( "Plat.MiniDumpErrorString", Plat_MiniDumpErrorString )
{
	Plat_ClearMiniDumpErrorString();
	TEST_EQ( std::strcmp( Plat_GetMiniDumpErrorString(), "" ), 0 );

	Plat_AppendMiniDumpErrorString( "a%d", 1 );
	Plat_AppendMiniDumpErrorString( "b" );
	TEST_EQ( std::strcmp( Plat_GetMiniDumpErrorString(), "a1b" ), 0 );

	Plat_ClearMiniDumpErrorString();
	TEST_EQ( std::strcmp( Plat_GetMiniDumpErrorString(), "" ), 0 );
}

REGISTER_NAMED_TEST( "Plat.ApplicationState", Plat_ApplicationState )
{
#ifndef PLATFORM_WINDOWS
	// The Windows tier0 ignores the setter and answers whether the standard output is open.
	const bool bSaved = Plat_IsConsoleApplication();

	Plat_SetIsConsoleApplication( true );
	TEST_TRUE( Plat_IsConsoleApplication() );

	Plat_SetIsConsoleApplication( false );
	TEST_FALSE( Plat_IsConsoleApplication() );

	Plat_SetIsConsoleApplication( bSaved );
#endif // !PLATFORM_WINDOWS

	char szName[ 256 ];

	szName[ 0 ] = '\0';
	TEST_TRUE( Plat_GetComputerName( szName, sizeof( szName ) ) );
	TEST_TRUE( szName[ 0 ] != '\0' );
}

REGISTER_NAMED_TEST( "Plat.VirtualMemory", Plat_VirtualMemory )
{
	const uint32 nPageSize = Plat_GetPageSize();

	TEST_TRUE( nPageSize > 0 );
	TEST_EQ( nPageSize & ( nPageSize - 1 ), static_cast< uint32 >( 0 ) );
	TEST_TRUE( Plat_GetAllocationGranularity() >= nPageSize );

	// Access bits: 1 read, 2 write, 4 execute.
	const size_t nBytes = Plat_GetAllocationGranularity();
	uint8 *pMemory = static_cast< uint8 * >( Plat_VirtualAlloc( nullptr, nBytes, PLAT_VIRTUAL_RESERVE_AND_COMMIT, 3, 0 ) );

	TEST_NOT_NULL( pMemory );
	pMemory[ 0 ] = 0x5A;
	pMemory[ nBytes - 1 ] = 0xA5;
	TEST_EQ( pMemory[ 0 ], 0x5A );
	TEST_EQ( pMemory[ nBytes - 1 ], 0xA5 );

	TEST_TRUE( Plat_VirtualProtect( pMemory, nBytes, 1 ) );
	TEST_EQ( pMemory[ 0 ], 0x5A );
	TEST_TRUE( Plat_VirtualFree( pMemory, nBytes, PLAT_VIRTUAL_RELEASE ) );
}

REGISTER_NAMED_TEST( "Plat.MemoryStatus", Plat_MemoryStatus )
{
	OsMemoryStatus_t status;

	TEST_TRUE( Plat_GlobalMemoryStatus( &status ) );
	TEST_TRUE( status.m_nTotalPhys > 0 );
	TEST_TRUE( status.m_nAvailPhys <= status.m_nTotalPhys );
	TEST_TRUE( status.m_nMemoryLoad <= 100 );

	TEST_FALSE( Plat_GlobalMemoryStatus( nullptr ) );

	TEST_TRUE( Plat_GetCurrentRSS() > 0 );
	TEST_TRUE( Plat_GetPeakRSS() > 0 );
}

REGISTER_NAMED_TEST( "Plat.CPU", Plat_CPU )
{
	TEST_TRUE( Plat_GetNumberCpusOnline() > 0 );

	double flLoad = -1.0;

#ifdef PLATFORM_OSX
	// The macOS tier0 has no system load.
	TEST_FALSE( Plat_GetSystemLoad( &flLoad ) );
#else
	TEST_TRUE( Plat_GetSystemLoad( &flLoad ) );
	TEST_TRUE( flLoad >= 0.0 );
#endif // PLATFORM_OSX

	// The previous MXCSR value restores the denormal flags.
	const uint32 nOriginal = _mm_getcsr();
	const uint32 nPrevious = Plat_SetDenormalsAreZero();

	TEST_EQ( nPrevious, nOriginal );
	TEST_EQ( _mm_getcsr() & 0x8040, 0x8040u );

	Plat_RestoreDenormalsAreZero( nPrevious );
	TEST_EQ( _mm_getcsr(), nOriginal );
}

// Module name decoration and lookup were checked against the Linux and Windows tier0 only.
#ifndef PLATFORM_OSX
REGISTER_NAMED_TEST( "Plat.Modules", Plat_Modules )
{
	TEST_NOT_NULL( Plat_GetExeModule() );

	// The module that contains a tier0 export is tier0 itself.
	HMODULE hModule = nullptr;
	CBufferString modulePath;
	void *pBase = nullptr;
	size_t nSize = 0;
	const void *pAddress = reinterpret_cast< const void * >( &Plat_GetPageSize );

	TEST_TRUE( Plat_GetModuleAddressInformation( pAddress, &hModule, &modulePath, &pBase, &nSize ) );
	TEST_NOT_NULL( hModule );
	TEST_NOT_NULL( std::strstr( modulePath.Get(), "tier0" ) );
	TEST_TRUE( static_cast< const uint8 * >( pAddress ) >= static_cast< const uint8 * >( pBase ) );
	TEST_TRUE( static_cast< const uint8 * >( pAddress ) < static_cast< const uint8 * >( pBase ) + nSize );

	void *pInfoBase = nullptr;

	TEST_TRUE( Plat_GetModuleInformation( hModule, nullptr, &pInfoBase, nullptr ) );
	TEST_EQ( pInfoBase, pBase );

	TEST_EQ( Plat_FindModuleByName( "tier0", false, nullptr, nullptr ), hModule );
	TEST_TRUE( Plat_IsModuleLoaded( "tier0", false ) );
	TEST_FALSE( Plat_IsModuleLoaded( "sourcesdk_missing_module", false ) );

	// A batch lookup resolves exports in order.
	const char *pNames[] = { "Plat_GetPageSize", "Plat_GetAllocationGranularity" };
	void *pPointers[ 2 ] = {};
	PlatModule_t hLoaded = Plat_GetProcAddresses( modulePath.Get(), 2, pNames, sizeof( pPointers ), pPointers, 0, nullptr, false );

	TEST_NOT_NULL( hLoaded );
	TEST_EQ( pPointers[ 0 ], reinterpret_cast< void * >( &Plat_GetPageSize ) );
	TEST_EQ( pPointers[ 1 ], reinterpret_cast< void * >( &Plat_GetAllocationGranularity ) );
	Plat_UnloadModule( reinterpret_cast< HMODULE >( hLoaded ) );
}
#endif // !PLATFORM_OSX

REGISTER_NAMED_TEST( "Plat.RawCallStack", Plat_RawCallStack )
{
	// The frame count depends on the unwinder of the platform, the result stays within the array.
	void *pFrames[ 16 ] = {};
	const int nFrames = Plat_GetRawCallStack( pFrames, 16, 0 );

	TEST_TRUE( nFrames >= 0 );
	TEST_TRUE( nFrames <= 16 );

	for ( int i = 0; i < nFrames; i++ )
	{
		TEST_NOT_NULL( pFrames[ i ] );
	}
}

REGISTER_NAMED_TEST( "Plat.GlobalListRegistry", Plat_GlobalListRegistry )
{
	GlobalListTypelessRegistry_t *pRegistry = Plat_GetGlobalListRegistry( 0x5EED );

	TEST_NOT_NULL( pRegistry );
	TEST_EQ( Plat_GetGlobalListRegistry( 0x5EED ), pRegistry );
}

REGISTER_NAMED_TEST( "SingleAppInstance.Shutdown", SingleAppInstance_Shutdown )
{
	// The instance can be released before the destructor, which then does nothing more.
	CSingleAppInstance instance( "sourcesdk_tests_single_app_instance" );
	TEST_TRUE( instance.IsUniqueInstance() );
	instance.ShutdownInstance();
}

REGISTER_NAMED_TEST( "Plat_LoadSDL.TableOrNull", Plat_LoadSDL_TableOrNull )
{
	// Without the library every entry point of SDL is null, with it the table is the same.
	void *pSDL = Plat_LoadSDL();

	if ( pSDL )
	{
		TEST_TRUE( Plat_CheckSDL() == pSDL );
		TEST_TRUE( Plat_RequireSDL() == pSDL );
		TEST_NOT_NULL( Plat_GetModuleSDL() );
	}
	else
	{
		TEST_NULL( Plat_CheckSDL() );
		TEST_TRUE( Plat_GetModuleSDL() == PLAT_MODULE_INVALID );
	}

	// Loading twice gives the same table.
	TEST_TRUE( Plat_LoadSDL() == pSDL );
}

REGISTER_NAMED_TEST( "Plat_CheckSDL.OnlyWhenLoaded", Plat_CheckSDL_OnlyWhenLoaded )
{
	TEST_TRUE( Plat_CheckSDL() == nullptr || Plat_CheckSDL() == Plat_LoadSDL() );
}

REGISTER_NAMED_TEST( "Plat_RequireSDL.SameTable", Plat_RequireSDL_SameTable )
{
	// Plat_RequireSDL fails fatally without the library.
	if ( !Plat_LoadSDL() )
		return;

	TEST_TRUE( Plat_RequireSDL() == Plat_LoadSDL() );
}

REGISTER_NAMED_TEST( "Plat_GetModuleSDL.MatchesTable", Plat_GetModuleSDL_MatchesTable )
{
	TEST_EQ( Plat_GetModuleSDL() != PLAT_MODULE_INVALID, Plat_LoadSDL() != nullptr );
}

REGISTER_NAMED_TEST( "Plat_UnloadSDL.KeepsTable", Plat_UnloadSDL_KeepsTable )
{
	// The function is empty, the table stays.
	void *pSDL = Plat_LoadSDL();
	Plat_UnloadSDL();
	TEST_TRUE( Plat_CheckSDL() == pSDL );
}

REGISTER_NAMED_TEST( "Plat_RegisterModule.Appends", Plat_RegisterModule_Appends )
{
	void *const *ppModules = Plat_GetRegisteredModules();
	TEST_NOT_NULL( ppModules );

	int nModules = 0;
	while ( nModules < 100 && ppModules[ nModules ] )
		nModules++;

	TEST_TRUE( nModules < 100 );

	static int s_nModuleBase;
	Plat_RegisterModule( &s_nModuleBase );
	TEST_TRUE( ppModules[ nModules ] == &s_nModuleBase );
}

REGISTER_NAMED_TEST( "Plat_GetRegisteredModules.NullTerminated", Plat_GetRegisteredModules_NullTerminated )
{
	void *const *ppModules = Plat_GetRegisteredModules();
	TEST_NOT_NULL( ppModules );

	bool bFoundEnd = false;
	for ( int i = 0; i < 100 && !bFoundEnd; i++ )
		bFoundEnd = ppModules[ i ] == nullptr;

	TEST_TRUE( bFoundEnd );
	TEST_TRUE( Plat_GetRegisteredModules() == ppModules );
}

REGISTER_NAMED_TEST( "EarlyInit_Tier0.NullReference", EarlyInit_Tier0_NullReference )
{
	// Without a dependency to link there is nothing to do.
	EarlyInit_Tier0( nullptr );
}

#ifndef PLATFORM_WINDOWS
class CTestImportLibraryVisitor : public IPlatModuleImportLibraryVisitor
{
public:
	virtual bool VisitImportLibrary( const char *pszImportLibrary, const char *pszModule )
	{
		m_nImports++;
		m_bFoundLibDl = m_bFoundLibDl || V_stristr( pszImportLibrary, "libdl" ) != nullptr || V_stristr( pszImportLibrary, "libSystem" ) != nullptr;
		m_bModuleMatches = m_bModuleMatches && pszModule && V_strcmp( pszModule, m_pszModule ) == 0;
		return true;
	}

	const char *m_pszModule = nullptr;
	int m_nImports = 0;
	bool m_bFoundLibDl = false;
	bool m_bModuleMatches = true;
};

REGISTER_NAMED_TEST( "Plat_EnumerateModuleImportLibraries.Tier0", Plat_EnumerateModuleImportLibraries_Tier0 )
{
	Dl_info info;
	TEST_TRUE( dladdr( reinterpret_cast< void * >( &Plat_LoadSDL ), &info ) != 0 );

	// The libraries tier0 links, each with the path of tier0.
	CTestImportLibraryVisitor visitor;
	visitor.m_pszModule = info.dli_fname;
	TEST_TRUE( visitor.ReadFileOfLoadedModule() );
	Plat_EnumerateModuleImportLibraries( info.dli_fname, &visitor );
	TEST_TRUE( visitor.m_nImports > 0 );
	TEST_TRUE( visitor.m_bFoundLibDl );
	TEST_TRUE( visitor.m_bModuleMatches );
}
#endif // !PLATFORM_WINDOWS

REGISTER_NAMED_TEST( "Plat_ReportDllLoadDenial.CountsAndNames", Plat_ReportDllLoadDenial_CountsAndNames )
{
	int nBefore = 0;
	Plat_GetLastDllLoadDenial( &nBefore );

	Plat_ReportDllLoadDenial( "sourcesdk_denier", nullptr, "sourcesdk_denied_module", nullptr );
	int nCount = 0;
	const char *pszModule = Plat_GetLastDllLoadDenial( &nCount );
	TEST_NOT_NULL( pszModule );
	TEST_EQ( V_strcmp( pszModule, "sourcesdk_denied_module" ), 0 );
	TEST_EQ( nCount, nBefore + 1 );

	// UTF-16 names are converted.
	Plat_ReportDllLoadDenial( nullptr, L"sourcesdk_wide_denier", nullptr, L"sourcesdk_wide_module" );
	pszModule = Plat_GetLastDllLoadDenial( &nCount );
	TEST_NOT_NULL( pszModule );
	TEST_EQ( V_strcmp( pszModule, "sourcesdk_wide_module" ), 0 );
	TEST_EQ( nCount, nBefore + 2 );
}

REGISTER_NAMED_TEST( "Plat_GetLastDllLoadDenial.CountWithoutName", Plat_GetLastDllLoadDenial_CountWithoutName )
{
	int nCount = -1;
	const char *pszModule = Plat_GetLastDllLoadDenial( &nCount );
	TEST_TRUE( nCount >= 0 );
	TEST_EQ( pszModule == nullptr, nCount == 0 );

	// The count is optional.
	TEST_TRUE( Plat_GetLastDllLoadDenial( nullptr ) == pszModule );
}

REGISTER_NAMED_TEST( "Plat_GetDebugMonitor.PlatformMonitor", Plat_GetDebugMonitor_PlatformMonitor )
{
#ifdef PLATFORM_WINDOWS
	TEST_TRUE( Plat_GetDebugMonitor() == Plat_GetDebugMonitor() );
#else
	TEST_NULL( Plat_GetDebugMonitor() );
#endif
}

static const void *TestClipboardData( void *pUserData, const char *pszMimeType, size_t *pSize )
{
	*pSize = 4;
	return "text";
}

static void TestClipboardCleanup( void *pUserData )
{
}

REGISTER_NAMED_TEST( "Plat_SetClipboardData.ThroughSDL", Plat_SetClipboardData_ThroughSDL )
{
	// tier0 calls SDL_SetClipboardData without checking that SDL is loaded.
	if ( !Plat_LoadSDL() )
		return;

	const char *pszMimeTypes[] = { "text/plain" };
	Plat_SetClipboardData( TestClipboardData, TestClipboardCleanup, nullptr, pszMimeTypes, 1 );
}

REGISTER_NAMED_TEST( "OSTypesAreCompatible.Families", OSTypesAreCompatible_Families )
{
	// A version is compatible with itself and the older ones of its family.
	TEST_TRUE( OSTypesAreCompatible( k_eLinux50, k_eLinux50 ) );
	TEST_TRUE( OSTypesAreCompatible( k_eLinux50, k_eLinux26 ) );
	TEST_FALSE( OSTypesAreCompatible( k_eLinux26, k_eLinux50 ) );

	// The unknown version of a family takes any of it.
	TEST_TRUE( OSTypesAreCompatible( k_eLinux418, k_eLinuxUnknown ) );
	TEST_TRUE( OSTypesAreCompatible( k_eMacOS1014, k_eMacOSUnknown ) );
	TEST_FALSE( OSTypesAreCompatible( k_eMacOS1014, k_eLinuxUnknown ) );

	// Windows versions are positive.
	TEST_TRUE( OSTypesAreCompatible( k_eWindows10, k_eWindows7 ) );
	TEST_FALSE( OSTypesAreCompatible( k_eWindows7, k_eWindows10 ) );

	// Any OS takes k_eOSUnknown.
	TEST_TRUE( OSTypesAreCompatible( k_eLinux50, k_eOSUnknown ) );
}

REGISTER_NAMED_TEST( "GetAlternateLaunchLocation.Stable", GetAlternateLaunchLocation_Stable )
{
	AlternateLaunchLocation_t *pLocation = GetAlternateLaunchLocation();
	TEST_NOT_NULL( pLocation );
	TEST_TRUE( GetAlternateLaunchLocation() == pLocation );
}

#ifndef PLATFORM_WINDOWS
REGISTER_NAMED_TEST( "CopyDependencyToAlternateLocation.Results", CopyDependencyToAlternateLocation_Results )
{
	char szSource[] = "/tmp/sourcesdk_alt_src_XXXXXX";
	char szDest[] = "/tmp/sourcesdk_alt_dst_XXXXXX";
	TEST_NOT_NULL( mkdtemp( szSource ) );
	TEST_NOT_NULL( mkdtemp( szDest ) );

	// No source file.
	TEST_EQ( CopyDependencyToAlternateLocation( szSource, "dep.bin", szDest, false, nullptr ), 1 );

	char szPath[ 128 ];
	V_snprintf( szPath, sizeof( szPath ), "%s/dep.bin", szSource );
	FILE *pFile = fopen( szPath, "wb" );
	TEST_NOT_NULL( pFile );
	fputs( "dependency", pFile );
	fclose( pFile );

	// The destination is missing: it differs and the POSIX build does not copy.
	TEST_EQ( CopyDependencyToAlternateLocation( szSource, "dep.bin", szDest, false, nullptr ), 2 );

	// The fallback directory is used when the source has no file.
	char szEmpty[] = "/tmp/sourcesdk_alt_empty_XXXXXX";
	TEST_NOT_NULL( mkdtemp( szEmpty ) );
	TEST_EQ( CopyDependencyToAlternateLocation( szEmpty, "dep.bin", szDest, false, szSource ), 2 );

	// A copy with the same size and time is up to date.
	char szCommand[ 512 ];
	V_snprintf( szCommand, sizeof( szCommand ), "cp -p %s %s/dep.bin", szPath, szDest );
	TEST_EQ( system( szCommand ), 0 );
	TEST_EQ( CopyDependencyToAlternateLocation( szSource, "dep.bin", szDest, false, nullptr ), 0 );

	V_snprintf( szCommand, sizeof( szCommand ), "rm -rf %s %s %s", szSource, szDest, szEmpty );
	( void )!system( szCommand );
}
#endif // !PLATFORM_WINDOWS

REGISTER_NAMED_TEST( "RestartSelfFromAlternateLocation.Exported", RestartSelfFromAlternateLocation_Exported )
{
	// The call copies the dependencies and starts the executable again, which a test cannot do.
	TEST_NOT_NULL( reinterpret_cast< void * >( &RestartSelfFromAlternateLocation ) );
}

REGISTER_NAMED_TEST( "RestartSelf_FixTaskbarPinShortcut.IgnoredOutsideWindows", RestartSelf_FixTaskbarPinShortcut_IgnoredOutsideWindows )
{
	// Windows rewrites the shortcut of a pinned window, the other builds only build the command line.
#ifndef PLATFORM_WINDOWS
	RestartSelf_FixTaskbarPinShortcut( nullptr, nullptr, nullptr );
#endif
	TEST_NOT_NULL( reinterpret_cast< void * >( &RestartSelf_FixTaskbarPinShortcut ) );
}

REGISTER_NAMED_TEST( "RestartSelf_ShowGenericRestartErrorForBadShortcut.NeedsSDL", RestartSelf_ShowGenericRestartErrorForBadShortcut_NeedsSDL )
{
	// The message box of SDL waits for the user, so the test only calls it without SDL, where
	// it returns at once.
	if ( Plat_LoadSDL() )
		return;

	RestartSelf_ShowGenericRestartErrorForBadShortcut();
}

REGISTER_NAMED_TEST( "Plat_GetCurrentDisplayMode.NeedsSDL", Plat_GetCurrentDisplayMode_NeedsSDL )
{
	// A null mode fails before SDL is needed.
	TEST_FALSE( Plat_GetCurrentDisplayMode( 0, nullptr ) );

	// Plat_RequireSDL fails fatally without the library.
	if ( !Plat_LoadSDL() )
		return;

	PlatDisplayMode_t mode;
	if ( Plat_GetCurrentDisplayMode( 0, &mode ) )
	{
		TEST_TRUE( mode.m_nWidth > 0 );
		TEST_TRUE( mode.m_nHeight > 0 );
	}
}

REGISTER_NAMED_TEST( "Plat_GetFullscreenDisplayModes.NeedsSDL", Plat_GetFullscreenDisplayModes_NeedsSDL )
{
	if ( !Plat_LoadSDL() )
		return;

	int nModes = 0;
	if ( Plat_GetFullscreenDisplayModes( 0, &nModes, nullptr ) && nModes > 0 )
	{
		PlatDisplayMode_t modes[ 64 ];
		nModes = nModes < 64 ? nModes : 64;
		TEST_TRUE( Plat_GetFullscreenDisplayModes( 0, &nModes, modes ) );
		TEST_TRUE( modes[ 0 ].m_nWidth > 0 );
	}
}

REGISTER_NAMED_TEST( "PlatDisplayMode_t.Layout", PlatDisplayMode_t_Layout )
{
	TEST_EQ( sizeof( PlatDisplayMode_t ), ( size_t )12 );
	TEST_EQ( offsetof( PlatDisplayMode_t, m_flRefreshRate ), ( size_t )8 );
}

REGISTER_NAMED_TEST( "GetThreadLocalStackAllocator.PerThreadBlock", GetThreadLocalStackAllocator_PerThreadBlock )
{
	ThreadLocalStackAllocator_t *pAllocator = GetThreadLocalStackAllocator();
	TEST_NOT_NULL( pAllocator );
	TEST_TRUE( GetThreadLocalStackAllocator() == pAllocator );

	// 256 KB of g_pMemAlloc, the next allocation inside it.
	TEST_EQ( ( size_t )( pAllocator->m_pLimit - pAllocator->m_pBase ), ( size_t )0x40000 );
	TEST_TRUE( pAllocator->m_pNext >= pAllocator->m_pBase );
	TEST_TRUE( pAllocator->m_pNext <= pAllocator->m_pLimit );
	TEST_TRUE( pAllocator->m_nAllocations >= 0 );
}

REGISTER_NAMED_TEST( "DebugBuild_InitFillMemory.ReturnsMemory", DebugBuild_InitFillMemory_ReturnsMemory )
{
	// The release build leaves the memory as it is.
	char szBuffer[ 8 ] = "abcdefg";
	TEST_TRUE( DebugBuild_InitFillMemory( szBuffer ) == szBuffer );
	TEST_EQ( V_strcmp( szBuffer, "abcdefg" ), 0 );
}

REGISTER_NAMED_TEST( "TransferMemory.CopiesStreams", TransferMemory_CopiesStreams )
{
	struct Src_t
	{
		int a;
		float b;
		short c;
	};

	struct Dst_t
	{
		float b;
		int a;
	};

	Src_t src[ 3 ] = { { 1, 1.5f, 10 }, { 2, 2.5f, 20 }, { 3, 3.5f, 30 } };
	Dst_t dst[ 3 ] = {};

	uint8 *pSrcStreams[] = { reinterpret_cast< uint8 * >( src ) };
	uint8 *pDstStreams[] = { reinterpret_cast< uint8 * >( dst ) };
	const int nSrcStrides[] = { sizeof( Src_t ) };
	const int nDstStrides[] = { sizeof( Dst_t ) };

	TransferMemoryStep_t steps[ 3 ];
	memset( steps, 0, sizeof( steps ) );
	steps[ 0 ].m_nOp = TransferMemoryStep_t::OP_COPY;
	steps[ 0 ].m_nSrcOffset = offsetof( Src_t, a );
	steps[ 0 ].m_nDstOffset = offsetof( Dst_t, a );
	steps[ 0 ].m_nBytes = sizeof( int );
	steps[ 1 ].m_nOp = TransferMemoryStep_t::OP_COPY;
	steps[ 1 ].m_nSrcOffset = offsetof( Src_t, b );
	steps[ 1 ].m_nDstOffset = offsetof( Dst_t, b );
	steps[ 1 ].m_nBytes = sizeof( float );
	steps[ 2 ].m_nOp = TransferMemoryStep_t::OP_END;

	TransferMemory( steps, nullptr, pSrcStreams, nSrcStrides, 0, pDstStreams, nDstStrides, 3 );

	for ( int i = 0; i < 3; i++ )
	{
		TEST_EQ( dst[ i ].a, src[ i ].a );
		TEST_EQ( dst[ i ].b, src[ i ].b );
	}
}

REGISTER_NAMED_TEST( "TransferMemoryStep_t.Layout", TransferMemoryStep_t_Layout )
{
	TEST_EQ( sizeof( TransferMemoryStep_t ), ( size_t )16 );
	TEST_EQ( offsetof( TransferMemoryStep_t, m_nSrcOffset ), ( size_t )4 );
	TEST_EQ( offsetof( TransferMemoryStep_t, m_nBytes ), ( size_t )8 );
}

REGISTER_NAMED_TEST( "ThreadLocalStackAllocator_t.Layout", ThreadLocalStackAllocator_t_Layout )
{
	TEST_EQ( sizeof( ThreadLocalStackAllocator_t ), ( size_t )40 );
	TEST_EQ( offsetof( ThreadLocalStackAllocator_t, m_pBase ), ( size_t )24 );
}

REGISTER_NAMED_TEST( "GetWin32ConsoleCursorColumnIndex.Column", GetWin32ConsoleCursorColumnIndex_Column )
{
#ifdef PLATFORM_WINDOWS
	TEST_TRUE( GetWin32ConsoleCursorColumnIndex() < 0x10000 );
#else
	TEST_EQ( GetWin32ConsoleCursorColumnIndex(), 0u );
#endif
}

#ifdef PLATFORM_WINDOWS
REGISTER_NAMED_TEST( "Plat_LoadCOM.TableAndCheck", Plat_LoadCOM_TableAndCheck )
{
	void *pCOM = Plat_LoadCOM();
	TEST_NOT_NULL( pCOM );
	TEST_TRUE( Plat_CheckCOM() == pCOM );
	TEST_TRUE( Plat_RequireCOM() == pCOM );
	Plat_UnloadCOM();
	TEST_TRUE( Plat_CheckCOM() == pCOM );
}

REGISTER_NAMED_TEST( "Plat_LoadOleAut.TableAndCheck", Plat_LoadOleAut_TableAndCheck )
{
	void *pOleAut = Plat_LoadOleAut();
	TEST_NOT_NULL( pOleAut );
	TEST_TRUE( Plat_CheckOleAut() == pOleAut );
	TEST_TRUE( Plat_RequireOleAut() == pOleAut );
	Plat_UnloadOleAut();
	TEST_TRUE( Plat_CheckOleAut() == pOleAut );
}

REGISTER_NAMED_TEST( "Plat_GetLoadedModules.HasTier0", Plat_GetLoadedModules_HasTier0 )
{
	CUtlLeanVector< HMODULE > modules;
	TEST_TRUE( Plat_GetLoadedModules( &modules ) );
	TEST_TRUE( modules.Find( GetModuleHandleA( "tier0.dll" ) ) != -1 );
}

REGISTER_NAMED_TEST( "Plat_IsSafeToLoadModules.Result", Plat_IsSafeToLoadModules_Result )
{
	const int nResult = Plat_IsSafeToLoadModules( 0 );
	TEST_TRUE( nResult >= 0 && nResult <= 2 );
}

REGISTER_NAMED_TEST( "Plat_NotifyModuleLoad.Exported", Plat_NotifyModuleLoad_Exported )
{
	// The notification data comes from the loader of Windows.
	TEST_NOT_NULL( reinterpret_cast< void * >( &Plat_NotifyModuleLoad ) );
}

REGISTER_NAMED_TEST( "Plat_EventRegister.RegisterWriteUnregister", Plat_EventRegister_RegisterWriteUnregister )
{
	// {5C1A3E2F-6A0B-4B5E-9D4E-2F1A7C3B9E01}, a provider of the test.
	static const GUID s_ProviderId = { 0x5c1a3e2f, 0x6a0b, 0x4b5e, { 0x9d, 0x4e, 0x2f, 0x1a, 0x7c, 0x3b, 0x9e, 0x01 } };

	uint64 hRegistration = 0;
	TEST_EQ( Plat_EventRegister( reinterpret_cast< const _GUID * >( &s_ProviderId ), nullptr, nullptr, &hRegistration ), 0ul );
	TEST_TRUE( hRegistration != 0 );

	EVENT_DESCRIPTOR descriptor = {};
	TEST_EQ( Plat_EventWriteTransfer( hRegistration, reinterpret_cast< const _EVENT_DESCRIPTOR * >( &descriptor ), nullptr, nullptr, 0, nullptr ), 0ul );
	TEST_EQ( Plat_EventUnregister( hRegistration ), 0ul );
}

REGISTER_NAMED_TEST( "Plat_EventActivityIdControl.CreateId", Plat_EventActivityIdControl_CreateId )
{
	// EVENT_ACTIVITY_CTRL_CREATE_ID
	GUID activityId = {};
	TEST_EQ( Plat_EventActivityIdControl( 3, reinterpret_cast< _GUID * >( &activityId ) ), 0ul );
	TEST_TRUE( activityId.Data1 != 0 || activityId.Data2 != 0 || activityId.Data3 != 0 );
}
#endif // PLATFORM_WINDOWS

#ifdef PLATFORM_OSX
REGISTER_NAMED_TEST( "Plat_GetAppleMajorMinorPatchVersion.Version", Plat_GetAppleMajorMinorPatchVersion_Version )
{
	int nMajor = 0, nMinor = 0, nPatch = 0;
	TEST_TRUE( Plat_GetAppleMajorMinorPatchVersion( &nMajor, &nMinor, &nPatch ) );
	TEST_TRUE( nMajor >= 10 );
	TEST_TRUE( nMinor >= 0 && nPatch >= 0 );
}

REGISTER_NAMED_TEST( "IsAppleOSVersionSupported.Versions", IsAppleOSVersionSupported_Versions )
{
	int nMajor = 0, nMinor = 0, nPatch = 0;
	TEST_TRUE( Plat_GetAppleMajorMinorPatchVersion( &nMajor, &nMinor, &nPatch ) );
	TEST_TRUE( IsAppleOSVersionSupported( nMajor, nMinor, nPatch ) );
	TEST_TRUE( IsAppleOSVersionSupported( 10, 0, 0 ) );
	TEST_FALSE( IsAppleOSVersionSupported( nMajor + 1, 0, 0 ) );
}

REGISTER_NAMED_TEST( "GetMoltenVKSupportsMetal.Answers", GetMoltenVKSupportsMetal_Answers )
{
	TEST_EQ( GetMoltenVKSupportsMetal(), GetMoltenVKSupportsMetal() );
}

REGISTER_NAMED_TEST( "GetMoltenVKMonitorContentsScale.Positive", GetMoltenVKMonitorContentsScale_Positive )
{
	TEST_TRUE( GetMoltenVKMonitorContentsScale() >= 0.0f );
}

REGISTER_NAMED_TEST( "GetMoltenVKMetalLayer.NeedsWindow", GetMoltenVKMetalLayer_NeedsWindow )
{
	// Both take an NSWindow, which the tests do not create.
	TEST_NOT_NULL( reinterpret_cast< void * >( &GetMoltenVKMetalLayer ) );
	TEST_NOT_NULL( reinterpret_cast< void * >( &GetMoltenVKWindowContentsScale ) );
}
#endif // PLATFORM_OSX

REGISTER_NAMED_TEST( "Plat_GetProcAddress.LoadsAndResolves", Plat_GetProcAddress_LoadsAndResolves )
{
	// The module name is the one of Plat_LoadModule, without the prefix and the extension.
	typedef double ( *FloatTimeFn_t )();
	FloatTimeFn_t pfnFloatTime = reinterpret_cast< FloatTimeFn_t >( Plat_GetProcAddress( "tier0", "Plat_FloatTime" ) );
	TEST_NOT_NULL( reinterpret_cast< void * >( pfnFloatTime ) );
	TEST_TRUE( pfnFloatTime() > 0.0 );
	TEST_NULL( Plat_GetProcAddress( "tier0", "sourcesdk_missing_export" ) );
	TEST_NULL( Plat_GetProcAddress( "sourcesdk_missing_module", "Plat_FloatTime" ) );
}

REGISTER_NAMED_TEST( "Plat_AlertUser.Overloads", Plat_AlertUser_Overloads )
{
	// Both need a window or a desktop session, which the tests do not have.
	void ( *pfnUTF8 )( PlatWindow_t, const char *, const char * ) = &Plat_AlertUser;
	void ( *pfnWide )( PlatWindow_t, const wchar_t *, const wchar_t * ) = &Plat_AlertUser;
	TEST_NOT_NULL( reinterpret_cast< void * >( pfnUTF8 ) );
	TEST_NOT_NULL( reinterpret_cast< void * >( pfnWide ) );
	TEST_NOT_NULL( reinterpret_cast< void * >( &Plat_MessageBox ) );
}

REGISTER_NAMED_TEST( "RandomizeBits.FillsTheBuffer", RandomizeBits_FillsTheBuffer )
{
	uint8 bytes[ 64 ] = {};
	RandomizeBits( bytes, sizeof( bytes ) );

	int nNonZero = 0;
	for ( uint8 nByte : bytes )
		nNonZero += nByte != 0;

	TEST_TRUE( nNonZero > 32 );

	// A count that is not a multiple of 4 leaves the bytes after it alone.
	uint8 tail[ 8 ] = {};
	RandomizeBits( tail, 5 );
	TEST_EQ( ( int )tail[ 5 ], 0 );
	TEST_EQ( ( int )tail[ 7 ], 0 );
}
