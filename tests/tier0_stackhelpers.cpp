#include "common/assert.h"
#include "common/macros.h"

#include <tier0/platform.h>

// dbghelp.h comes before tier0/stackhelpers.h, which then takes its SYMSRV_INDEX_INFOW
#ifdef PLATFORM_WINDOWS
#include <windows.h>
#include <dbghelp.h>
#include <psapi.h>
#else
#include <dlfcn.h>
#endif

#include <tier0/stackhelpers.h>

#include <cstring>
#include <new>

#include <tier0/bufferstring.h>
#include <tier0/memalloc.h>
#include <tier0/strtools.h>
#include <tier0/utlbuffer.h>


REGISTER_NAMED_TEST( "CDwarfModule.GetSuggestedSymbolPath", CDwarfModule_GetSuggestedSymbolPath )
{
	const char *pszPath = CDwarfModule::GetSuggestedSymbolPath();
	TEST_NOT_NULL( pszPath );
	TEST_NOT_NULL( V_strstr( pszPath, "symbols" ) );
}

REGISTER_NAMED_TEST( "CDwarfModule.GetValveSymbolPaths", CDwarfModule_GetValveSymbolPaths )
{
	CBufferString paths;
	const char *pszPaths = CDwarfModule::GetValveSymbolPaths( &paths );
	TEST_TRUE( pszPaths == paths.Get() );
	TEST_NOT_NULL( V_strstr( pszPaths, CDwarfModule::GetSuggestedSymbolPath() ) );
}

#ifndef PLATFORM_WINDOWS
// The path and the load address of tier0
static bool GetTier0Module( Dl_info *pInfo )
{
	return dladdr( reinterpret_cast< void * >( &Plat_FloatTime ), pInfo ) != 0;
}

// A DWARF module of tier0, the stripped library has no symbols of its own
static CVDwarfSymbolModule *CreateTier0DwarfModule()
{
	Dl_info info;

	if ( !GetTier0Module( &info ) )
		return nullptr;

	CVDwarfSymbolModule *pModule = new CVDwarfSymbolModule;

	if ( !pModule->Init( info.dli_fname, 0, VWMD_CPU_ARCHITECTURE_AMD64, reinterpret_cast< uint64 >( info.dli_fbase ), 0x600000, ( StackTools_ModuleIDFormat_t )0, nullptr, 0 ) )
	{
		delete pModule;
		return nullptr;
	}

	return pModule;
}

REGISTER_NAMED_TEST( "CVDwarfSymbolModule.Init", CVDwarfSymbolModule_Init )
{
	CVDwarfSymbolModule *pModule = CreateTier0DwarfModule();
	TEST_NOT_NULL( pModule );

	// The symbols are read when they are needed.
	TEST_EQ( V_strcmp( pModule->GetSymbolsDescription(), "DWARF deferred symbols" ), 0 );

	// No path fails.
	CVDwarfSymbolModule *pEmpty = new CVDwarfSymbolModule;
	TEST_FALSE( pEmpty->Init( nullptr, 0, VWMD_CPU_ARCHITECTURE_AMD64, 0, 0, ( StackTools_ModuleIDFormat_t )0, nullptr, 0 ) );
	TEST_FALSE( pEmpty->Init( "", 0, VWMD_CPU_ARCHITECTURE_AMD64, 0, 0, ( StackTools_ModuleIDFormat_t )0, nullptr, 0 ) );
	delete pEmpty;
	delete pModule;
}

REGISTER_NAMED_TEST( "CVDwarfSymbolModule.OpenAndParseSymbols", CVDwarfSymbolModule_OpenAndParseSymbols )
{
	CVDwarfSymbolModule *pModule = CreateTier0DwarfModule();
	TEST_NOT_NULL( pModule );
	TEST_TRUE( pModule->OpenAndParseSymbols( nullptr, 0, nullptr ) );
	TEST_NOT_NULL( pModule->GetSymbolsDescription() );
	delete pModule;

	// Without Init there is nothing to parse.
	CVDwarfSymbolModule *pEmpty = new CVDwarfSymbolModule;
	TEST_FALSE( pEmpty->OpenAndParseSymbols( nullptr, 0, nullptr ) );
	delete pEmpty;
}

REGISTER_NAMED_TEST( "CVSymbolModule.KindSlots", CVSymbolModule_KindSlots )
{
	CVDwarfSymbolModule *pModule = CreateTier0DwarfModule();
	TEST_NOT_NULL( pModule );

	// A DWARF module has its reader and no Breakpad or DbgHelp parts.
	TEST_NOT_NULL( pModule->GetDwarfModule( false ) );
	TEST_NULL( pModule->GetBreakpadModule() );
	TEST_NULL( pModule->GetFunctionTableEntry( 0 ) );

	uint8 nIDFormat = 0xff;
	uint32 nAge = 1;
	CUtlVector< uint8 > moduleID;
	pModule->GetModuleIdentity( &nIDFormat, &moduleID, &nAge );
	TEST_EQ( ( int )nIDFormat, 0 );
	TEST_EQ( moduleID.Count(), 0 );

	CUtlBuffer buffer;
	pModule->WriteSourceServerFooter( &buffer );
	TEST_EQ( buffer.TellPut(), 0 );
	delete pModule;

	// A Breakpad module is its own Breakpad part and has no symbols before Open.
	CVBreakpadSymbolModule *pBreakpad = new CVBreakpadSymbolModule;
	TEST_TRUE( pBreakpad->GetBreakpadModule() == pBreakpad );
	TEST_NULL( pBreakpad->GetDwarfModule( false ) );
	TEST_FALSE( pBreakpad->HasSymbols() );
	delete pBreakpad;
}

REGISTER_NAMED_TEST( "CVSymbolModule.ResolveSymbolInfo", CVSymbolModule_ResolveSymbolInfo )
{
	CVDwarfSymbolModule *pModule = CreateTier0DwarfModule();
	TEST_NOT_NULL( pModule );

	// The outputs are cleared first; the stripped tier0 has no debug symbols for the address.
	CBufferString symbol( "symbol" ), file( "file" );
	VSymbolKind_t eKind;
	uint64 nSymbolAddress = 1, nLineAddress = 1;
	uint32 nLine = 1;
	const bool bResolved = pModule->ResolveSymbolInfo( reinterpret_cast< uint64 >( &Plat_FloatTime ), 0, &symbol, &eKind, &nSymbolAddress, &file, &nLine, &nLineAddress );

	if ( !bResolved )
	{
		TEST_TRUE( symbol.IsEmpty() );
		TEST_TRUE( file.IsEmpty() );
		TEST_EQ( nLine, 0u );
	}

	// Every output is optional.
	pModule->ResolveSymbolInfo( reinterpret_cast< uint64 >( &Plat_FloatTime ), 0, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr );
	delete pModule;
}

REGISTER_NAMED_TEST( "CVSymbolModule.ResolveInlineCallInfo", CVSymbolModule_ResolveInlineCallInfo )
{
	CVDwarfSymbolModule *pModule = CreateTier0DwarfModule();
	TEST_NOT_NULL( pModule );

	// The layout of the inline information is internal, a zeroed block stands in for it.
	static uint8 s_InlineInfo[ 4096 ];
	memset( s_InlineInfo, 0, sizeof( s_InlineInfo ) );

	CBufferString symbol( "symbol" ), file( "file" );
	uint64 nSymbolAddress = 1, nLineAddress = 1;
	uint32 nLine = 1;
	const bool bResolved = pModule->ResolveInlineCallInfo( reinterpret_cast< VSymbolInlineInfo_t * >( s_InlineInfo ), reinterpret_cast< uint64 >( &Plat_FloatTime ), 0, &symbol, &nSymbolAddress, &file, &nLine, &nLineAddress );

	if ( !bResolved )
	{
		TEST_TRUE( symbol.IsEmpty() );
		TEST_TRUE( file.IsEmpty() );
	}

	delete pModule;
}

REGISTER_NAMED_TEST( "CVSymbolModule.GetSourceServerPathForFile", CVSymbolModule_GetSourceServerPathForFile )
{
	CVDwarfSymbolModule *pModule = CreateTier0DwarfModule();
	TEST_NOT_NULL( pModule );

	// tier0 has no source index for its files.
	CBufferString path( "path" );
	TEST_NULL( pModule->GetSourceServerPathForFile( "sourcesdk_file.cpp", &path ) );
	TEST_TRUE( path.IsEmpty() );
	delete pModule;
}

REGISTER_NAMED_TEST( "CVSymbolModule.UpdateSourceIndex", CVSymbolModule_UpdateSourceIndex )
{
	CVDwarfSymbolModule *pModule = CreateTier0DwarfModule();
	TEST_NOT_NULL( pModule );

	// The DWARF module has no symbol file to write the index to.
	bool bWritten = true;
	TEST_FALSE( pModule->UpdateSourceIndex( "x", 1, false, &bWritten ) );
	TEST_FALSE( bWritten );
	delete pModule;
}

REGISTER_NAMED_TEST( "CVSymbolModule.GetSymbolFilePath", CVSymbolModule_GetSymbolFilePath )
{
	CVDwarfSymbolModule *pModule = CreateTier0DwarfModule();
	TEST_NOT_NULL( pModule );

	CBufferString path( "path" );
	pModule->GetSymbolFilePath( &path );
	TEST_TRUE( path.IsEmpty() || V_strstr( path.Get(), "tier0" ) != nullptr );
	delete pModule;
}

REGISTER_NAMED_TEST( "CVSymbolModule.WriteSourceServerHeader", CVSymbolModule_WriteSourceServerHeader )
{
	CVDwarfSymbolModule *pModule = CreateTier0DwarfModule();
	TEST_NOT_NULL( pModule );

	CUtlBuffer buffer( 0, 0, CUtlBuffer::TEXT_BUFFER );
	TEST_TRUE( pModule->WriteSourceServerHeader( &buffer ) );
	buffer.PutChar( '\0' );
	TEST_NOT_NULL( V_strstr( static_cast< const char * >( buffer.Base() ), "VERSIONCONTROL=p4" ) );
	delete pModule;
}

REGISTER_NAMED_TEST( "CVSymbolModule.WriteSourceServerFileLine", CVSymbolModule_WriteSourceServerFileLine )
{
	CVDwarfSymbolModule *pModule = CreateTier0DwarfModule();
	TEST_NOT_NULL( pModule );

	CUtlBuffer buffer( 0, 0, CUtlBuffer::TEXT_BUFFER );
	pModule->WriteSourceServerFileLine( &buffer, "c:\\src\\a.cpp", "//depot/a.cpp", 7 );
	buffer.PutChar( '\0' );
	TEST_EQ( V_strcmp( static_cast< const char * >( buffer.Base() ), "c:\\src\\a.cpp-> //depot/a.cpp#7\n" ), 0 );
	delete pModule;
}

REGISTER_NAMED_TEST( "CVBreakpadSymbolModule.Open", CVBreakpadSymbolModule_Open )
{
	CVBreakpadSymbolModule *pModule = new CVBreakpadSymbolModule;
	TEST_EQ( V_strcmp( pModule->GetSymbolsDescription(), "no symbols" ), 0 );

	// A path and a module name are needed, and a known kind of identity.
	const uint8 id[ 16 ] = {};
	TEST_FALSE( pModule->Open( nullptr, "module.so", nullptr, 0x400000, 0x10000, 0, ( StackTools_ModuleIDFormat_t )8, id, sizeof( id ), 0 ) );
	TEST_FALSE( pModule->Open( "/tmp", "", nullptr, 0x400000, 0x10000, 0, ( StackTools_ModuleIDFormat_t )8, id, sizeof( id ), 0 ) );
	TEST_FALSE( pModule->Open( "/tmp", "module.so", nullptr, 0x400000, 0x10000, 0, ( StackTools_ModuleIDFormat_t )0, id, sizeof( id ), 0 ) );

	// No symbol file of the module there.
	TEST_FALSE( pModule->Open( "/tmp/sourcesdk_no_symbols", "module.so", nullptr, 0x400000, 0x10000, 0, ( StackTools_ModuleIDFormat_t )8, id, sizeof( id ), 0 ) );
	delete pModule;
}

REGISTER_NAMED_TEST( "CVBreakpadSymbolModule.FindFrameData", CVBreakpadSymbolModule_FindFrameData )
{
	// Without a symbol file there are no unwind records.
	CVBreakpadSymbolModule *pModule = new CVBreakpadSymbolModule;
	uint8 data[ 256 ];
	TEST_FALSE( pModule->FindCfiFrameData( 0x1000, reinterpret_cast< BreakpadCfiFrameData_t * >( data ) ) );
	TEST_FALSE( pModule->FindWinFpoData( 0x1000, reinterpret_cast< BreakpadWinFpoData_t * >( data ) ) );
	TEST_FALSE( pModule->FindWinFrameProgram( 0x1000, reinterpret_cast< BreakpadWinFrameProgram_t * >( data ) ) );
	delete pModule;
}

REGISTER_NAMED_TEST( "CVSymbolModule.Layout", CVSymbolModule_Layout )
{
	TEST_EQ( sizeof( CVDwarfSymbolModule ), ( size_t )184 );
	TEST_EQ( sizeof( CVBreakpadSymbolModule ), ( size_t )496 );
}
#endif // !PLATFORM_WINDOWS

#ifdef PLATFORM_WINDOWS
REGISTER_NAMED_TEST( "StackHelpers_Windows_GetDbgHelpWrapper.Shared", StackHelpers_Windows_GetDbgHelpWrapper_Shared )
{
	CDbgHelpFunctionsLoader *pDbgHelp = StackHelpers_Windows_GetDbgHelpWrapper();
	TEST_NOT_NULL( pDbgHelp );
	TEST_TRUE( StackHelpers_Windows_GetDbgHelpWrapper() == pDbgHelp );
}

REGISTER_NAMED_TEST( "StackHelpers_Windows_GetImageHlpWrapper.Shared", StackHelpers_Windows_GetImageHlpWrapper_Shared )
{
	CImageHlpFunctionsLoader *pImageHlp = StackHelpers_Windows_GetImageHlpWrapper();
	TEST_NOT_NULL( pImageHlp );
	TEST_TRUE( StackHelpers_Windows_GetImageHlpWrapper() == pImageHlp );
}

REGISTER_NAMED_TEST( "StackHelpers_Windows_GetPSApiWrapper.Shared", StackHelpers_Windows_GetPSApiWrapper_Shared )
{
	CPSApiFunctionsLoader *pPSApi = StackHelpers_Windows_GetPSApiWrapper();
	TEST_NOT_NULL( pPSApi );
	TEST_TRUE( StackHelpers_Windows_GetPSApiWrapper() == pPSApi );
}

REGISTER_NAMED_TEST( "Win_LoadDbgHelpFromKnownPath.Loads", Win_LoadDbgHelpFromKnownPath_Loads )
{
	HMODULE hDbgHelp = Win_LoadDbgHelpFromKnownPath();
	TEST_TRUE( hDbgHelp != NULL );
	TEST_TRUE( Win_LoadDbgHelpFromKnownPath() == hDbgHelp );
}

REGISTER_NAMED_TEST( "StackHelpers_Windows_GetLocalProcessHandle_NonPseudo.RealHandle", StackHelpers_Windows_GetLocalProcessHandle_NonPseudo_RealHandle )
{
	void *hProcess = StackHelpers_Windows_GetLocalProcessHandle_NonPseudo();
	TEST_NOT_NULL( hProcess );
	TEST_EQ( GetProcessId( hProcess ), GetCurrentProcessId() );
}

REGISTER_NAMED_TEST( "StackHelpers_Windows_AllocateDbgHelpUniqueID.Unique", StackHelpers_Windows_AllocateDbgHelpUniqueID_Unique )
{
	const uint32 nFirst = StackHelpers_Windows_AllocateDbgHelpUniqueID();
	const uint32 nSecond = StackHelpers_Windows_AllocateDbgHelpUniqueID();
	TEST_TRUE( nFirst != nSecond );

	// Never the handle of the process itself.
	TEST_TRUE( reinterpret_cast< void * >( ( uintp )nFirst ) != StackHelpers_Windows_GetLocalProcessHandle_NonPseudo() );
	StackHelpers_Windows_FreeDbgHelpUniqueID( nSecond );
	StackHelpers_Windows_FreeDbgHelpUniqueID( nFirst );
}

REGISTER_NAMED_TEST( "StackHelpers_Windows_FreeDbgHelpUniqueID.Empty", StackHelpers_Windows_FreeDbgHelpUniqueID_Empty )
{
	StackHelpers_Windows_FreeDbgHelpUniqueID( 0 );
}

REGISTER_NAMED_TEST( "StackHelpers_Windows_GenerateSymbolSearchPath.Path", StackHelpers_Windows_GenerateSymbolSearchPath_Path )
{
	CBufferString path;
	const char *pszPath = StackHelpers_Windows_GenerateSymbolSearchPath( &path );
	TEST_TRUE( pszPath == path.Get() );
	TEST_FALSE( path.IsEmpty() );
}

REGISTER_NAMED_TEST( "StackHelpers_Windows_GenerateSymbolHomeDirectory.Directory", StackHelpers_Windows_GenerateSymbolHomeDirectory_Directory )
{
	CBufferString directory;
	const char *pszDirectory = StackHelpers_Windows_GenerateSymbolHomeDirectory( &directory );
	TEST_TRUE( pszDirectory == directory.Get() );
}

REGISTER_NAMED_TEST( "StackHelpers_Windows_InitModLoadData.CodeViewRecords", StackHelpers_Windows_InitModLoadData_CodeViewRecords )
{
	MODLOAD_DATA data;
	uint8 buffer[ 256 ];

	// RSDS: GUID and age.
	const uint8 rsds[ 24 ] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 0, 0, 1 };
	TEST_TRUE( StackHelpers_Windows_InitModLoadData( reinterpret_cast< _MODLOAD_DATA * >( &data ), buffer, sizeof( buffer ), "module.pdb", ( StackTools_ModuleIDFormat_t )1, rsds ) );
	TEST_EQ( data.ssize, ( DWORD )sizeof( MODLOAD_DATA ) );
	TEST_EQ( data.ssig, ( DWORD )DBHHEADER_DEBUGDIRS );
	TEST_TRUE( data.data == buffer );

	// NB10: signature and age.
	const uint8 nb10[ 12 ] = {};
	TEST_TRUE( StackHelpers_Windows_InitModLoadData( reinterpret_cast< _MODLOAD_DATA * >( &data ), buffer, sizeof( buffer ), "module.pdb", ( StackTools_ModuleIDFormat_t )2, nb10 ) );

	// Other formats and small buffers fail.
	TEST_FALSE( StackHelpers_Windows_InitModLoadData( reinterpret_cast< _MODLOAD_DATA * >( &data ), buffer, sizeof( buffer ), "module.pdb", ( StackTools_ModuleIDFormat_t )8, rsds ) );
	TEST_FALSE( StackHelpers_Windows_InitModLoadData( reinterpret_cast< _MODLOAD_DATA * >( &data ), buffer, 16, "module.pdb", ( StackTools_ModuleIDFormat_t )1, rsds ) );
}

REGISTER_NAMED_TEST( "GetModuleNTHeaders.Tier0", GetModuleNTHeaders_Tier0 )
{
	HMODULE hTier0 = GetModuleHandleA( "tier0.dll" );
	MODULEINFO info = {};
	TEST_TRUE( GetModuleInformation( GetCurrentProcess(), hTier0, &info, sizeof( info ) ) );

	const IMAGE_NT_HEADERS *pHeaders = static_cast< const IMAGE_NT_HEADERS * >( GetModuleNTHeaders( hTier0, info.SizeOfImage ) );
	TEST_NOT_NULL( pHeaders );
	TEST_EQ( pHeaders->Signature, ( DWORD )IMAGE_NT_SIGNATURE );

	// Too small or not an image.
	TEST_NULL( GetModuleNTHeaders( hTier0, 16 ) );
	char szText[ 512 ] = "not an image";
	TEST_NULL( GetModuleNTHeaders( szText, sizeof( szText ) ) );
	TEST_NULL( GetModuleNTHeaders( nullptr, 4096 ) );
}

REGISTER_NAMED_TEST( "GetThreadInformationBlock.CurrentThread", GetThreadInformationBlock_CurrentThread )
{
	TEST_TRUE( GetThreadInformationBlock() == reinterpret_cast< _TEB * >( NtCurrentTeb() ) );
}

REGISTER_NAMED_TEST( "Hack_WatchWindow_t._FooHacked_DontEverCallMe", Hack_WatchWindow_t__FooHacked_DontEverCallMe )
{
	// Only the debugger calls it.
	TEST_NOT_NULL( reinterpret_cast< void * >( &Hack_WatchWindow_t::_FooHacked_DontEverCallMe ) );
}

// A DbgHelp session of a unique ID for the tests
class CTestDbgHelpSession
{
public:
	CTestDbgHelpSession()
	{
		m_pDbgHelp = StackHelpers_Windows_GetDbgHelpWrapper();
		m_pDbgHelp->EnsureReady();
		m_hProcess = m_pDbgHelp->EitherSymInitialize( nullptr, false );
	}

	~CTestDbgHelpSession()
	{
		m_pDbgHelp->EitherSymCleanup( m_hProcess );
	}

	CDbgHelpFunctionsLoader *m_pDbgHelp;
	void *m_hProcess;
};

REGISTER_NAMED_TEST( "CDbgHelpFunctionsLoader.EnsureReady", CDbgHelpFunctionsLoader_EnsureReady )
{
	CDbgHelpFunctionsLoader *pDbgHelp = StackHelpers_Windows_GetDbgHelpWrapper();
	pDbgHelp->EnsureReady();
	pDbgHelp->EnsureReady();
	TEST_TRUE( pDbgHelp->SymGetOptions_() != ( unsigned long )-1 );
}

REGISTER_NAMED_TEST( "CDbgHelpFunctionsLoader.LockMutex", CDbgHelpFunctionsLoader_LockMutex )
{
	// The lock is recursive.
	CDbgHelpFunctionsLoader *pDbgHelp = StackHelpers_Windows_GetDbgHelpWrapper();
	pDbgHelp->LockMutex();
	pDbgHelp->LockMutex();
	pDbgHelp->UnlockMutex();
	pDbgHelp->UnlockMutex();
}

REGISTER_NAMED_TEST( "CDbgHelpFunctionsLoader.HasStackWalkEx_", CDbgHelpFunctionsLoader_HasStackWalkEx_ )
{
	CDbgHelpFunctionsLoader *pDbgHelp = StackHelpers_Windows_GetDbgHelpWrapper();
	TEST_EQ( pDbgHelp->HasStackWalkEx_(), pDbgHelp->HasStackWalkEx_() );
	TEST_EQ( pDbgHelp->SupportsInlineContext_(), pDbgHelp->SupportsInlineContext_() );
}

REGISTER_NAMED_TEST( "CDbgHelpFunctionsLoader.SymOptions", CDbgHelpFunctionsLoader_SymOptions )
{
	CDbgHelpFunctionsLoader *pDbgHelp = StackHelpers_Windows_GetDbgHelpWrapper();
	const unsigned long nOptions = pDbgHelp->SymGetOptions_();
	TEST_EQ( pDbgHelp->SymSetOptions_( nOptions | SYMOPT_UNDNAME ), nOptions | SYMOPT_UNDNAME );
	TEST_TRUE( ( pDbgHelp->SymGetOptions_() & SYMOPT_UNDNAME ) != 0 );
	pDbgHelp->SymSetOptions_( nOptions );
}

REGISTER_NAMED_TEST( "CDbgHelpFunctionsLoader.EitherSymInitialize", CDbgHelpFunctionsLoader_EitherSymInitialize )
{
	CTestDbgHelpSession session;
	TEST_NOT_NULL( session.m_hProcess );
}

REGISTER_NAMED_TEST( "CDbgHelpFunctionsLoader.BaseSymInitialize", CDbgHelpFunctionsLoader_BaseSymInitialize )
{
	CDbgHelpFunctionsLoader *pDbgHelp = StackHelpers_Windows_GetDbgHelpWrapper();
	void *hProcess = reinterpret_cast< void * >( ( uintp )StackHelpers_Windows_AllocateDbgHelpUniqueID() );
	pDbgHelp->BaseSymInitialize( hProcess, nullptr, false );
	pDbgHelp->ApplySymSrvDebugOutputWorkaround( hProcess );
	pDbgHelp->BaseSymCleanup( hProcess );
}

REGISTER_NAMED_TEST( "CDbgHelpFunctionsLoader.LocalSymInitialize", CDbgHelpFunctionsLoader_LocalSymInitialize )
{
	CDbgHelpFunctionsLoader *pDbgHelp = StackHelpers_Windows_GetDbgHelpWrapper();
	pDbgHelp->LocalSymInitialize( nullptr );
	pDbgHelp->LocalSymCleanup();
}

REGISTER_NAMED_TEST( "CDbgHelpFunctionsLoader.SymInitialize_", CDbgHelpFunctionsLoader_SymInitialize_ )
{
	CDbgHelpFunctionsLoader *pDbgHelp = StackHelpers_Windows_GetDbgHelpWrapper();
	void *hProcess = reinterpret_cast< void * >( ( uintp )StackHelpers_Windows_AllocateDbgHelpUniqueID() );
	TEST_TRUE( pDbgHelp->SymInitialize_( hProcess, nullptr, FALSE ) != 0 );

	wchar_t szPath[ 1024 ] = {};
	TEST_TRUE( pDbgHelp->SymSetSearchPath_( hProcess, L"." ) != 0 );
	TEST_TRUE( pDbgHelp->SymGetSearchPath_( hProcess, szPath, 1024 ) != 0 );
	TEST_EQ( wcscmp( szPath, L"." ), 0 );
	TEST_TRUE( pDbgHelp->SymRefreshModuleList_( hProcess ) != 0 || GetLastError() != 0 );
	TEST_TRUE( pDbgHelp->SymCleanup_( hProcess ) != 0 );
}

REGISTER_NAMED_TEST( "CDbgHelpFunctionsLoader.SymSetHomeDirectory_", CDbgHelpFunctionsLoader_SymSetHomeDirectory_ )
{
	CTestDbgHelpSession session;
	wchar_t szDirectory[ MAX_PATH ];
	GetTempPathW( MAX_PATH, szDirectory );
	TEST_NOT_NULL( session.m_pDbgHelp->SymSetHomeDirectory_( session.m_hProcess, szDirectory ) );
}

REGISTER_NAMED_TEST( "CDbgHelpFunctionsLoader.SymLoadModuleEx_", CDbgHelpFunctionsLoader_SymLoadModuleEx_ )
{
	CTestDbgHelpSession session;
	CDbgHelpFunctionsLoader *pDbgHelp = session.m_pDbgHelp;

	HMODULE hTier0 = GetModuleHandleA( "tier0.dll" );
	wchar_t szPath[ MAX_PATH ];
	GetModuleFileNameW( hTier0, szPath, MAX_PATH );
	MODULEINFO info = {};
	GetModuleInformation( GetCurrentProcess(), hTier0, &info, sizeof( info ) );

	const DWORD64 nBase = pDbgHelp->SymLoadModuleEx_( session.m_hProcess, nullptr, szPath, nullptr, ( DWORD64 )hTier0, info.SizeOfImage, nullptr, 0 );
	TEST_EQ( nBase, ( DWORD64 )hTier0 );

	// The exports of tier0 are symbols.
	TEST_EQ( pDbgHelp->SymGetModuleBase64_( session.m_hProcess, ( DWORD64 )&Plat_FloatTime ), ( DWORD64 )hTier0 );

	IMAGEHLP_MODULEW64 module = { sizeof( module ) };
	TEST_TRUE( pDbgHelp->SymGetModuleInfo64_( session.m_hProcess, ( DWORD64 )hTier0, reinterpret_cast< _IMAGEHLP_MODULEW64 * >( &module ) ) != 0 );

	uint64 symbolBuffer[ ( sizeof( SYMBOL_INFOW ) + 256 * sizeof( wchar_t ) ) / sizeof( uint64 ) + 1 ];
	SYMBOL_INFOW *pSymbol = reinterpret_cast< SYMBOL_INFOW * >( symbolBuffer );
	memset( symbolBuffer, 0, sizeof( symbolBuffer ) );
	pSymbol->SizeOfStruct = sizeof( SYMBOL_INFOW );
	pSymbol->MaxNameLen = 256;
	DWORD64 nDisplacement = 0;

	// DbgHelp of the CI machines finds no symbols of tier0, not even its exports.
	if ( pDbgHelp->SymFromAddr_( session.m_hProcess, ( DWORD64 )&Plat_FloatTime, &nDisplacement, pSymbol ) != 0 )
	{
		TEST_TRUE( pDbgHelp->SymFromInlineContext_( session.m_hProcess, ( DWORD64 )&Plat_FloatTime, 0, &nDisplacement, pSymbol ) != 0 );
		pDbgHelp->SymNext_( session.m_hProcess, pSymbol );
		pDbgHelp->SymPrev_( session.m_hProcess, pSymbol );
	}

	TEST_TRUE( pDbgHelp->SymUnloadModule64_( session.m_hProcess, ( DWORD64 )hTier0 ) != 0 );
}

REGISTER_NAMED_TEST( "CDbgHelpFunctionsLoader.SymLoadModule64_", CDbgHelpFunctionsLoader_SymLoadModule64_ )
{
	CTestDbgHelpSession session;
	HMODULE hTier0 = GetModuleHandleA( "tier0.dll" );
	wchar_t szPath[ MAX_PATH ];
	GetModuleFileNameW( hTier0, szPath, MAX_PATH );
	MODULEINFO info = {};
	GetModuleInformation( GetCurrentProcess(), hTier0, &info, sizeof( info ) );
	TEST_EQ( session.m_pDbgHelp->SymLoadModule64_( session.m_hProcess, nullptr, szPath, nullptr, ( DWORD64 )hTier0, info.SizeOfImage ), ( DWORD64 )hTier0 );
	session.m_pDbgHelp->SymUnloadModule64_( session.m_hProcess, ( DWORD64 )hTier0 );
}

static int s_nDbgHelpCallbacks;

static int __cdecl TestEnumLoadedModules( const wchar_t *pszName, unsigned __int64 nBase, unsigned long nSize, void *pContext )
{
	s_nDbgHelpCallbacks++;
	return TRUE;
}

static int __cdecl TestEnumModules( const wchar_t *pszName, unsigned __int64 nBase, void *pContext )
{
	s_nDbgHelpCallbacks++;
	return TRUE;
}

static int __cdecl TestEnumSymbols( _SYMBOL_INFOW *pSymbol, unsigned long nSize, void *pContext )
{
	s_nDbgHelpCallbacks++;
	return s_nDbgHelpCallbacks < 16;
}

static int __cdecl TestEnumSourceFiles( _SOURCEFILEW *pSourceFile, void *pContext )
{
	s_nDbgHelpCallbacks++;
	return TRUE;
}

static int __cdecl TestEnumLines( _SRCCODEINFOW *pLine, void *pContext )
{
	s_nDbgHelpCallbacks++;
	return TRUE;
}

REGISTER_NAMED_TEST( "CDbgHelpFunctionsLoader.EnumerateLoadedModules64_", CDbgHelpFunctionsLoader_EnumerateLoadedModules64_ )
{
	CDbgHelpFunctionsLoader *pDbgHelp = StackHelpers_Windows_GetDbgHelpWrapper();
	s_nDbgHelpCallbacks = 0;
	TEST_TRUE( pDbgHelp->EnumerateLoadedModules64_( GetCurrentProcess(), TestEnumLoadedModules, nullptr ) != 0 );
	TEST_TRUE( s_nDbgHelpCallbacks > 0 );
}

REGISTER_NAMED_TEST( "CDbgHelpFunctionsLoader.Enumerations", CDbgHelpFunctionsLoader_Enumerations )
{
	CTestDbgHelpSession session;
	CDbgHelpFunctionsLoader *pDbgHelp = session.m_pDbgHelp;
	HMODULE hTier0 = GetModuleHandleA( "tier0.dll" );
	wchar_t szPath[ MAX_PATH ];
	GetModuleFileNameW( hTier0, szPath, MAX_PATH );
	MODULEINFO info = {};
	GetModuleInformation( GetCurrentProcess(), hTier0, &info, sizeof( info ) );
	pDbgHelp->SymLoadModuleEx_( session.m_hProcess, nullptr, szPath, nullptr, ( DWORD64 )hTier0, info.SizeOfImage, nullptr, 0 );

	s_nDbgHelpCallbacks = 0;
	TEST_TRUE( pDbgHelp->SymEnumerateModules64_( session.m_hProcess, TestEnumModules, nullptr ) != 0 );
	TEST_EQ( s_nDbgHelpCallbacks, 1 );

	// The symbols of tier0 are enumerated when DbgHelp finds them, which it does not on CI machines.
	uint64 symbolBuffer[ ( sizeof( SYMBOL_INFOW ) + 256 * sizeof( wchar_t ) ) / sizeof( uint64 ) + 1 ] = {};
	SYMBOL_INFOW *pSymbol = reinterpret_cast< SYMBOL_INFOW * >( symbolBuffer );
	pSymbol->SizeOfStruct = sizeof( SYMBOL_INFOW );
	pSymbol->MaxNameLen = 256;
	DWORD64 nSymbolDisplacement = 0;
	const bool bHasSymbols = pDbgHelp->SymFromAddr_( session.m_hProcess, ( DWORD64 )&Plat_FloatTime, &nSymbolDisplacement, pSymbol ) != 0;

	s_nDbgHelpCallbacks = 0;
	pDbgHelp->SymEnumSymbolsEx_( session.m_hProcess, ( DWORD64 )hTier0, L"Plat_*", TestEnumSymbols, nullptr, 1 /* SYMENUM_OPTIONS_DEFAULT */ );
	TEST_TRUE( !bHasSymbols || s_nDbgHelpCallbacks > 0 );
	pDbgHelp->SymEnumSymbolsForAddr_( session.m_hProcess, ( DWORD64 )&Plat_FloatTime, TestEnumSymbols, nullptr );

	// Without private symbols there are no source files or lines; the calls only report it.
	pDbgHelp->SymEnumSourceFiles_( session.m_hProcess, ( DWORD64 )hTier0, nullptr, TestEnumSourceFiles, nullptr );
	pDbgHelp->SymEnumSourceLines_( session.m_hProcess, ( DWORD64 )hTier0, nullptr, nullptr, 0, 0, TestEnumLines, nullptr );

	IMAGEHLP_LINEW64 line = { sizeof( line ) };
	DWORD nDisplacement = 0;
	pDbgHelp->SymGetLineFromAddr64_( session.m_hProcess, ( DWORD64 )&Plat_FloatTime, &nDisplacement, &line );
	pDbgHelp->SymGetLineFromInlineContext_( session.m_hProcess, ( DWORD64 )&Plat_FloatTime, 0, ( DWORD64 )hTier0, &nDisplacement, &line );

	DWORD nContext = 0, nFrame = 0;
	pDbgHelp->SymQueryInlineTrace_( session.m_hProcess, ( DWORD64 )&Plat_FloatTime, 0, ( DWORD64 )&Plat_FloatTime, ( DWORD64 )&Plat_FloatTime, &nContext, &nFrame );

	void *pToken = nullptr;
	DWORD nTokenSize = 0;
	if ( pDbgHelp->SymGetSourceFileToken_( session.m_hProcess, ( DWORD64 )hTier0, L"sourcesdk.cpp", &pToken, &nTokenSize ) )
	{
		wchar_t szValue[ 256 ];
		pDbgHelp->SymGetSourceVarFromToken_( session.m_hProcess, pToken, nullptr, L"SRCSRVTRG", szValue, 256 );
	}
}

REGISTER_NAMED_TEST( "CDbgHelpFunctionsLoader.SymRegisterCallbackW64_", CDbgHelpFunctionsLoader_SymRegisterCallbackW64_ )
{
	CTestDbgHelpSession session;
	TEST_TRUE( session.m_pDbgHelp->SymRegisterCallbackW64_( session.m_hProcess, []( void *, unsigned long, unsigned __int64, unsigned __int64 ) -> int { return FALSE; }, 0 ) != 0 );
}

// The routines of the stack walk through the loader, the tests do not link dbghelp.lib
static void *__cdecl TestFunctionTableAccess( void *hProcess, unsigned __int64 nAddrBase )
{
	return StackHelpers_Windows_GetDbgHelpWrapper()->SymFunctionTableAccess64_( hProcess, nAddrBase );
}

static unsigned __int64 __cdecl TestGetModuleBase( void *hProcess, unsigned __int64 nAddress )
{
	return StackHelpers_Windows_GetDbgHelpWrapper()->SymGetModuleBase64_( hProcess, nAddress );
}

REGISTER_NAMED_TEST( "CDbgHelpFunctionsLoader.StackWalk64_", CDbgHelpFunctionsLoader_StackWalk64_ )
{
	CDbgHelpFunctionsLoader *pDbgHelp = StackHelpers_Windows_GetDbgHelpWrapper();
	void *hProcess = GetCurrentProcess();
	pDbgHelp->SymInitialize_( hProcess, nullptr, TRUE );

	CONTEXT context = {};
	RtlCaptureContext( &context );

	STACKFRAME64 frame = {};
	frame.AddrPC.Offset = context.Rip;
	frame.AddrPC.Mode = AddrModeFlat;
	frame.AddrStack.Offset = context.Rsp;
	frame.AddrStack.Mode = AddrModeFlat;
	frame.AddrFrame.Offset = context.Rbp;
	frame.AddrFrame.Mode = AddrModeFlat;

	// The function table and the module bases come from the loader too.
	TEST_NOT_NULL( pDbgHelp->SymFunctionTableAccess64_( hProcess, context.Rip ) );
	TEST_TRUE( pDbgHelp->StackWalk64_( IMAGE_FILE_MACHINE_AMD64, hProcess, GetCurrentThread(), reinterpret_cast< _tagSTACKFRAME64 * >( &frame ), &context, nullptr, TestFunctionTableAccess, TestGetModuleBase, nullptr ) != 0 );

	if ( pDbgHelp->HasStackWalkEx_() )
	{
		RtlCaptureContext( &context );
		STACKFRAME_EX frameEx = {};
		frameEx.StackFrameSize = sizeof( frameEx );
		frameEx.AddrPC.Offset = context.Rip;
		frameEx.AddrPC.Mode = AddrModeFlat;
		frameEx.AddrStack.Offset = context.Rsp;
		frameEx.AddrStack.Mode = AddrModeFlat;
		TEST_TRUE( pDbgHelp->StackWalkEx_( IMAGE_FILE_MACHINE_AMD64, hProcess, GetCurrentThread(), reinterpret_cast< _tagSTACKFRAME_EX * >( &frameEx ), &context, nullptr, TestFunctionTableAccess, TestGetModuleBase, nullptr, 0 /* SYM_STKWALK_DEFAULT */ ) != 0 );
	}

	pDbgHelp->SymCleanup_( hProcess );
}

REGISTER_NAMED_TEST( "CDbgHelpFunctionsLoader.SymSrvGetFileIndexes_", CDbgHelpFunctionsLoader_SymSrvGetFileIndexes_ )
{
	CDbgHelpFunctionsLoader *pDbgHelp = StackHelpers_Windows_GetDbgHelpWrapper();
	pDbgHelp->EnsureReady();

	wchar_t szPath[ MAX_PATH ];
	GetModuleFileNameW( GetModuleHandleA( "tier0.dll" ), szPath, MAX_PATH );

	GUID id = {};
	DWORD nValue1 = 0, nValue2 = 0;
	TEST_TRUE( pDbgHelp->SymSrvGetFileIndexes_( szPath, reinterpret_cast< _GUID * >( &id ), &nValue1, &nValue2, 0 ) != 0 );

	SYMSRV_INDEX_INFOW indexInfo = { sizeof( indexInfo ) };
	TEST_TRUE( pDbgHelp->SymSrvGetFileIndexInfo_( szPath, reinterpret_cast< SYMSRV_INDEX_INFOW * >( &indexInfo ), 0 ) != 0 );
}

REGISTER_NAMED_TEST( "CDbgHelpFunctionsLoader.SymFindFileInPath_", CDbgHelpFunctionsLoader_SymFindFileInPath_ )
{
	CTestDbgHelpSession session;
	wchar_t szFound[ MAX_PATH ] = {};

	// A file that is not there.
	TEST_FALSE( session.m_pDbgHelp->SymFindFileInPath_( session.m_hProcess, L".", L"sourcesdk_missing.pdb", nullptr, 0, 0, SSRVOPT_DWORD, szFound, nullptr, nullptr ) != 0 );

	char szFoundA[ MAX_PATH ] = {};
	TEST_FALSE( session.m_pDbgHelp->SearchForImageOrSymbolFile( session.m_hProcess, "sourcesdk_missing.pdb", nullptr, nullptr, 0, 0, SSRVOPT_DWORD, szFoundA, sizeof( szFoundA ) ) );
}

REGISTER_NAMED_TEST( "CDbgHelpFunctionsLoader.FindDebugFiles", CDbgHelpFunctionsLoader_FindDebugFiles )
{
	CTestDbgHelpSession session;
	char szImage[ MAX_PATH ] = {};
	char szSymbols[ MAX_PATH ] = {};
	const uint8 rsds[ 20 ] = {};

	// Nothing is found for an unknown identity, the outputs stay strings.
	session.m_pDbgHelp->FindDebugFiles( session.m_hProcess, "sourcesdk_missing.dll", 0, ".", ( StackTools_ModuleIDFormat_t )1, rsds, 0, szImage, sizeof( szImage ), szSymbols, sizeof( szSymbols ) );
	TEST_TRUE( strlen( szImage ) < sizeof( szImage ) );
	TEST_TRUE( strlen( szSymbols ) < sizeof( szSymbols ) );
}

REGISTER_NAMED_TEST( "CDbgHelpFunctionsLoader.ConstructDestruct", CDbgHelpFunctionsLoader_ConstructDestruct )
{
	// A loader of its own loads nothing before EnsureReady.
	CDbgHelpFunctionsLoader *pLoader = new CDbgHelpFunctionsLoader;
	delete pLoader;
}

REGISTER_NAMED_TEST( "CImageHlpFunctionsLoader.ImageLoad_", CImageHlpFunctionsLoader_ImageLoad_ )
{
	CImageHlpFunctionsLoader *pImageHlp = StackHelpers_Windows_GetImageHlpWrapper();
	LOADED_IMAGE *pImage = reinterpret_cast< LOADED_IMAGE * >( pImageHlp->ImageLoad_( "tier0.dll", nullptr ) );
	TEST_NOT_NULL( pImage );

	// The first section maps into the image.
	PIMAGE_SECTION_HEADER pLastSection = nullptr;
	TEST_NOT_NULL( pImageHlp->ImageRvaToVa_( reinterpret_cast< _IMAGE_NT_HEADERS64 * >( pImage->FileHeader ), pImage->MappedAddress, pImage->Sections[ 0 ].VirtualAddress, reinterpret_cast< _IMAGE_SECTION_HEADER ** >( &pLastSection ) ) );
	TEST_TRUE( pImageHlp->ImageUnload_( reinterpret_cast< _LOADED_IMAGE * >( pImage ) ) != 0 );
}

REGISTER_NAMED_TEST( "CImageHlpFunctionsLoader.ConstructDestruct", CImageHlpFunctionsLoader_ConstructDestruct )
{
	CImageHlpFunctionsLoader *pLoader = new CImageHlpFunctionsLoader;
	delete pLoader;
}

REGISTER_NAMED_TEST( "CPSApiFunctionsLoader.GetModuleInformation_", CPSApiFunctionsLoader_GetModuleInformation_ )
{
	CPSApiFunctionsLoader *pPSApi = StackHelpers_Windows_GetPSApiWrapper();
	MODULEINFO info = {};
	HMODULE hTier0 = GetModuleHandleA( "tier0.dll" );
	TEST_TRUE( pPSApi->GetModuleInformation_( GetCurrentProcess(), hTier0, reinterpret_cast< _MODULEINFO * >( &info ), sizeof( info ) ) != 0 );
	TEST_TRUE( info.lpBaseOfDll == hTier0 );
}

REGISTER_NAMED_TEST( "CPSApiFunctionsLoader.Assignment", CPSApiFunctionsLoader_Assignment )
{
	CPSApiFunctionsLoader *pFirst = new CPSApiFunctionsLoader;
	CPSApiFunctionsLoader *pSecond = new CPSApiFunctionsLoader;
	*pSecond = *StackHelpers_Windows_GetPSApiWrapper();

	MODULEINFO info = {};
	TEST_TRUE( pSecond->GetModuleInformation_( GetCurrentProcess(), GetModuleHandleA( "tier0.dll" ), reinterpret_cast< _MODULEINFO * >( &info ), sizeof( info ) ) != 0 );
	delete pSecond;
	delete pFirst;
}

REGISTER_NAMED_TEST( "CVDbgHelpSymbolModule.Init", CVDbgHelpSymbolModule_Init )
{
	CTestDbgHelpSession session;
	HMODULE hTier0 = GetModuleHandleA( "tier0.dll" );
	char szPath[ MAX_PATH ];
	GetModuleFileNameA( hTier0, szPath, MAX_PATH );
	MODULEINFO info = {};
	GetModuleInformation( GetCurrentProcess(), hTier0, &info, sizeof( info ) );

	// The deleting destructor of tier0 frees the module with g_pMemAlloc.
	CVDbgHelpSymbolModule *pModule = new ( MemAlloc_Alloc( sizeof( CVDbgHelpSymbolModule ) ) ) CVDbgHelpSymbolModule;
	TEST_TRUE( pModule->Init( session.m_pDbgHelp, session.m_hProcess, szPath, "tier0.dll", nullptr, ( uint64 )hTier0, info.SizeOfImage, ( StackTools_ModuleIDFormat_t )0, nullptr, 0, false ) );
	TEST_NOT_NULL( pModule->GetSymbolsDescription() );

	CBufferString symbol;
	VSymbolKind_t eKind;
	uint64 nSymbolAddress = 0;
	if ( pModule->ResolveSymbolInfo( ( uint64 )&Plat_FloatTime, 0, &symbol, &eKind, &nSymbolAddress, nullptr, nullptr, nullptr ) )
		TEST_NOT_NULL( V_strstr( symbol.Get(), "Plat_FloatTime" ) );

	delete pModule;
}
#endif // PLATFORM_WINDOWS
