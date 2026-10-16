#include "common/assert.h"
#include "common/macros.h"

#include <tier0/platform.h>
#include <tier0/interface.h>
#include <tier0/memalloc.h>
#include <tier0/bufferstring.h>
#include <tier0/stacktools.h>

#include <cmath>
#include <cstdio>
#include <cstring>
#include <xmmintrin.h>

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

	TEST_TRUE( Plat_GetSystemLoad( &flLoad ) );
	TEST_TRUE( flLoad >= 0.0 );

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
