#ifndef TIER0_STACKHELPERS_H
#define TIER0_STACKHELPERS_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "tier0/bufferstring.h"
#include "tier0/threadtools.h"
#include "tier0/utlscratchmemory.h"
#include "tier0/utlsymbol.h"
#include "tier0/utlvector.h"

class CUtlBuffer;
class CUtlString;

// The format of the identity of a module given to the symbol loaders: 1 is a CodeView RSDS record
// (GUID and age), 2 a CodeView NB10 record (signature and age), 8 a 16 byte build ID shown as a GUID
enum StackTools_ModuleIDFormat_t : int;

// The CPU of a module, as in the minidumps
enum VWMD_CPU_ARCHITECTURE : int;

// The kind of a resolved symbol
enum VSymbolKind_t : int;

struct VSymbolInlineInfo_t;
struct BreakpadWinFpoData_t;
struct BreakpadCfiFrameData_t;
struct BreakpadWinFrameProgram_t;

//-----------------------------------------------------------------------------
// The symbols of a module for the stack translation: DWARF or Mach-O symbols, a Breakpad symbol
// file or DbgHelp. Reference counted, it caches the source server path of each source file.
// The virtual functions are pure here, the derived classes implement them; the names of the
// unknown ones are ours
//-----------------------------------------------------------------------------
class CVSymbolModule
{
public:
	virtual void unk000();
	virtual void unk001();

	// "ELF DWARF symbols", "Mach-O fallback symbols", "no symbols" and so on; a DWARF module needs
	// its Init first
	virtual const char *GetSymbolsDescription();

	// The path of the symbol file, returns the text of pOut
	virtual const char *GetSymbolFilePath( CBufferString *pOut );

	virtual void unk004();
	virtual void unk005();
	virtual void unk006();
	virtual void unk007();
	virtual void unk008();
	virtual void unk009();
	virtual void unk010();

	// The "VERSIONCONTROL=p4" header of the source server stream and its line of a source file
	virtual bool WriteSourceServerHeader( CUtlBuffer *pBuffer );
	virtual void WriteSourceServerFileLine( CUtlBuffer *pBuffer, const char *pszLocalPath, const char *pszDepotPath, int nRevision );

	virtual void unk013();
	virtual void unk014();

	virtual ~CVSymbolModule();

	// The implementations of the non-virtual functions below, which clear the outputs first
	virtual bool InternalResolveSymbolInfo( uint64 nAddress, uint32 nInlineContext, CBufferString *pSymbolName, VSymbolKind_t *pSymbolKind, uint64 *pSymbolAddress, CBufferString *pSourceFile, uint32 *pSourceLine, uint64 *pSourceLineAddress );
	virtual bool InternalResolveInlineCallInfo( VSymbolInlineInfo_t *pInlineInfo, uint64 nAddress, uint32 nInlineContext, CBufferString *pSymbolName, uint64 *pSymbolAddress, CBufferString *pSourceFile, uint32 *pSourceLine, uint64 *pSourceLineAddress );

	// Asked by GetSourceServerPathForFile() for a file without a cached path
	virtual const char *InternalGetSourceServerPathForFile( const char *pszFile, CBufferString *pOut );

	// The source index stream of the symbol file
	virtual bool ReadSourceIndex( const char *pszSymbolFile, CUtlString *pIndex );
	virtual bool WriteSourceIndex( const char *pszSymbolFile, const void *pIndex, int nIndexSize );

	DLL_CLASS_IMPORT bool ResolveSymbolInfo( uint64 nAddress, uint32 nInlineContext, CBufferString *pSymbolName, VSymbolKind_t *pSymbolKind, uint64 *pSymbolAddress, CBufferString *pSourceFile, uint32 *pSourceLine, uint64 *pSourceLineAddress );
	DLL_CLASS_IMPORT bool ResolveInlineCallInfo( VSymbolInlineInfo_t *pInlineInfo, uint64 nAddress, uint32 nInlineContext, CBufferString *pSymbolName, uint64 *pSymbolAddress, CBufferString *pSourceFile, uint32 *pSourceLine, uint64 *pSourceLineAddress );

	// The cached path of the file, or the one InternalGetSourceServerPathForFile() makes; null
	// for none
	DLL_CLASS_IMPORT const char *GetSourceServerPathForFile( const char *pszFile, CBufferString *pOut );

	// Writes pIndex to the source index of the symbol file unless it already holds it, or always
	// with bForce; *pbWritten tells whether it was written
	DLL_CLASS_IMPORT bool UpdateSourceIndex( const void *pIndex, int nIndexSize, bool bForce, bool *pbWritten );

protected:
	uint64 m_nModuleBase;
	uint64 m_nModuleSize;

	// The source server path of each file of m_SourceFiles, by its symbol
	CUtlVector< const char * > m_SourceServerPaths;
	CUtlSymbolTable m_SourceFiles;
	CAtomicMutex m_SourceServerMutex;

	CInterlockedInt m_nRefCount;
};

//-----------------------------------------------------------------------------
// The DWARF symbols of an ELF or the Mach-O symbols of a module
//-----------------------------------------------------------------------------
class CDwarfModule;

class CVDwarfSymbolModule : public CVSymbolModule
{
public:
	DLL_CLASS_IMPORT CVDwarfSymbolModule();
	virtual ~CVDwarfSymbolModule();

	// Opens the module at pszModulePath, false when it is already open or cannot be read
	DLL_CLASS_IMPORT bool Init( const char *pszModulePath, uint32 nUnknown, VWMD_CPU_ARCHITECTURE eArchitecture, uint64 nModuleBase, uint64 nModuleSize, StackTools_ModuleIDFormat_t eIDFormat, const void *pModuleID, int nModuleIDSize );

	// Reads the symbols of the opened module
	DLL_CLASS_IMPORT bool OpenAndParseSymbols( const char *pszSearchPath, uint32 nUnknown, const char *pszUnknown );

private:
	CDwarfModule *m_pDwarfModule;
	bool m_bSourceFilesEnumerated;
};

//-----------------------------------------------------------------------------
// A Breakpad symbol file (.sym) of a module
//-----------------------------------------------------------------------------
class CVBreakpadSymbolModule : public CVSymbolModule
{
public:
	DLL_CLASS_IMPORT CVBreakpadSymbolModule();
	virtual ~CVBreakpadSymbolModule();

	// Looks the symbol file up under pszSymbolPath by the name of the module (pszDebugFileName
	// when given) and its identity with nAge; eIDFormat must be 1, 2 or 5 to 8
	DLL_CLASS_IMPORT bool Open( const char *pszSymbolPath, const char *pszModuleName, const char *pszDebugFileName, uint64 nModuleBase, uint64 nModuleSize, uint32 nUnknown, StackTools_ModuleIDFormat_t eIDFormat, const void *pModuleID, int nModuleIDSize, uint32 nAge );

	// The unwind records of the address
	DLL_CLASS_IMPORT bool FindWinFrameProgram( uint64 nAddress, BreakpadWinFrameProgram_t *pData ) const;
	DLL_CLASS_IMPORT bool FindWinFpoData( uint64 nAddress, BreakpadWinFpoData_t *pData ) const;
	DLL_CLASS_IMPORT bool FindCfiFrameData( uint64 nAddress, BreakpadCfiFrameData_t *pData ) const;

private:
	uint8 unk0a8[ 40 ];
	CBufferString unk0d0;
	uint8 unk0e0[ 64 ];
	CUtlScratchMemoryPool unk120;
	uint8 unk130[ 192 ];
};

//-----------------------------------------------------------------------------
// The ELF and Mach-O reader of the DWARF symbols
//-----------------------------------------------------------------------------
class CDwarfModule
{
public:
	// "\\symbols" on Windows
	DLL_CLASS_IMPORT static const char *GetSuggestedSymbolPath();

	// Writes the symbol paths of Valve to pPaths, returns its text
	DLL_CLASS_IMPORT static const char *GetValveSymbolPaths( CBufferString *pPaths );
};

#ifdef _WIN32
struct _GUID;
struct _IMAGE_NT_HEADERS64;
struct _IMAGE_SECTION_HEADER;
struct _IMAGEHLP_LINEW64;
struct _IMAGEHLP_MODULEW64;
struct _LOADED_IMAGE;
struct _MODLOAD_DATA;
struct _MODULEINFO;
struct _SOURCEFILEW;
struct _SRCCODEINFOW;
struct _SYMBOL_INFOW;
struct _tagADDRESS64;
struct _tagSTACKFRAME64;
struct _tagSTACKFRAME_EX;
struct _TEB;

// An unnamed struct of dbghelp.h, which therefore has to be included first when both are used
#ifndef _DBGHELP_
struct SYMSRV_INDEX_INFOW;
#endif

//-----------------------------------------------------------------------------
// The functions of dbghelp.dll, loaded on first use from a known path; a function the loaded
// version lacks fails. The methods take the process handle first like the DbgHelp ones
//-----------------------------------------------------------------------------
class CDbgHelpFunctionsLoader
{
public:
	typedef int (__cdecl *PENUMLOADED_MODULES_CALLBACKW64)( const wchar_t *ModuleName, unsigned __int64 ModuleBase, unsigned long ModuleSize, void *UserContext );
	typedef int (__cdecl *PSYM_ENUMMODULES_CALLBACKW64)( const wchar_t *ModuleName, unsigned __int64 BaseOfDll, void *UserContext );
	typedef int (__cdecl *PSYM_ENUMSOURCEFILES_CALLBACKW)( _SOURCEFILEW *pSourceFile, void *UserContext );
	typedef int (__cdecl *PSYM_ENUMLINES_CALLBACKW)( _SRCCODEINFOW *LineInfo, void *UserContext );
	typedef int (__cdecl *PSYM_ENUMERATESYMBOLS_CALLBACKW)( _SYMBOL_INFOW *pSymInfo, unsigned long SymbolSize, void *UserContext );
	typedef int (__cdecl *PFINDFILEINPATHCALLBACKW)( const wchar_t *filename, void *context );
	typedef int (__cdecl *PSYMBOL_REGISTERED_CALLBACK64)( void *hProcess, unsigned long ActionCode, unsigned __int64 CallbackData, unsigned __int64 UserContext );
	typedef int (__cdecl *PREAD_PROCESS_MEMORY_ROUTINE64)( void *hProcess, unsigned __int64 qwBaseAddress, void *lpBuffer, unsigned long nSize, unsigned long *lpNumberOfBytesRead );
	typedef void *(__cdecl *PFUNCTION_TABLE_ACCESS_ROUTINE64)( void *ahProcess, unsigned __int64 AddrBase );
	typedef unsigned __int64 (__cdecl *PGET_MODULE_BASE_ROUTINE64)( void *hProcess, unsigned __int64 Address );
	typedef unsigned __int64 (__cdecl *PTRANSLATE_ADDRESS_ROUTINE64)( void *hProcess, void *hThread, _tagADDRESS64 *lpaddr );

	DLL_CLASS_IMPORT CDbgHelpFunctionsLoader();
	DLL_CLASS_IMPORT ~CDbgHelpFunctionsLoader();

	DLL_CLASS_IMPORT void EnsureReady();
	DLL_CLASS_IMPORT void LockMutex();
	DLL_CLASS_IMPORT void UnlockMutex();

	DLL_CLASS_IMPORT bool HasStackWalkEx_();
	DLL_CLASS_IMPORT bool SupportsInlineContext_();

	// SymInitialize of the local process or of another one, with the search path pszSearchPath
	DLL_CLASS_IMPORT void LocalSymInitialize( const char *pszSearchPath );
	DLL_CLASS_IMPORT void LocalSymCleanup();
	DLL_CLASS_IMPORT void BaseSymInitialize( void *hProcess, const char *pszSearchPath, bool bInvadeProcess );
	DLL_CLASS_IMPORT void BaseSymCleanup( void *hProcess );
	DLL_CLASS_IMPORT void *EitherSymInitialize( const char *pszSearchPath, bool bInvadeProcess );
	DLL_CLASS_IMPORT void EitherSymCleanup( void *hProcess );
	DLL_CLASS_IMPORT void ApplySymSrvDebugOutputWorkaround( void *hProcess );

	DLL_CLASS_IMPORT bool SearchForImageOrSymbolFile( void *hProcess, const char *pszSearchPath, const char *pszFileName, const void *pID, unsigned long nTwo, unsigned long nThree, unsigned long nFlags, char *pszFoundFile, unsigned __int64 nFoundFileSize );
	DLL_CLASS_IMPORT void FindDebugFiles( void *hProcess, const char *pszModulePath, uint32 nUnknown, const char *pszSearchPath, StackTools_ModuleIDFormat_t eIDFormat, const void *pModuleID, unsigned __int64 nUnknown2, char *pszImageFile, unsigned __int64 nImageFileSize, char *pszSymbolFile, unsigned __int64 nSymbolFileSize );

	DLL_CLASS_IMPORT int SymInitialize_( void *hProcess, const wchar_t *UserSearchPath, int fInvadeProcess );
	DLL_CLASS_IMPORT int SymCleanup_( void *hProcess );
	DLL_CLASS_IMPORT unsigned long SymGetOptions_();
	DLL_CLASS_IMPORT unsigned long SymSetOptions_( unsigned long SymOptions );
	DLL_CLASS_IMPORT int SymGetSearchPath_( void *hProcess, wchar_t *SearchPath, unsigned long SearchPathLength );
	DLL_CLASS_IMPORT int SymSetSearchPath_( void *hProcess, const wchar_t *SearchPath );
	DLL_CLASS_IMPORT wchar_t *SymSetHomeDirectory_( void *hProcess, const wchar_t *dir );
	DLL_CLASS_IMPORT int SymRefreshModuleList_( void *hProcess );
	DLL_CLASS_IMPORT int SymRegisterCallbackW64_( void *hProcess, PSYMBOL_REGISTERED_CALLBACK64 CallbackFunction, unsigned __int64 UserContext );

	DLL_CLASS_IMPORT unsigned __int64 SymLoadModule64_( void *hProcess, void *hFile, const wchar_t *ImageName, const wchar_t *ModuleName, unsigned __int64 BaseOfDll, unsigned long SizeOfDll );
	DLL_CLASS_IMPORT unsigned __int64 SymLoadModuleEx_( void *hProcess, void *hFile, const wchar_t *ImageName, const wchar_t *ModuleName, unsigned __int64 BaseOfDll, unsigned long DllSize, _MODLOAD_DATA *Data, unsigned long Flags );
	DLL_CLASS_IMPORT int SymUnloadModule64_( void *hProcess, unsigned __int64 BaseOfDll );
	DLL_CLASS_IMPORT unsigned __int64 SymGetModuleBase64_( void *hProcess, unsigned __int64 qwAddr );
	DLL_CLASS_IMPORT int SymGetModuleInfo64_( void *hProcess, unsigned __int64 qwAddr, _IMAGEHLP_MODULEW64 *ModuleInfo );
	DLL_CLASS_IMPORT int EnumerateLoadedModules64_( void *hProcess, PENUMLOADED_MODULES_CALLBACKW64 EnumLoadedModulesCallback, void *UserContext );
	DLL_CLASS_IMPORT int SymEnumerateModules64_( void *hProcess, PSYM_ENUMMODULES_CALLBACKW64 EnumModulesCallback, void *UserContext );

	DLL_CLASS_IMPORT int SymFromAddr_( void *hProcess, unsigned __int64 Address, unsigned __int64 *Displacement, _SYMBOL_INFOW *Symbol );
	DLL_CLASS_IMPORT int SymFromInlineContext_( void *hProcess, unsigned __int64 Address, unsigned long InlineContext, unsigned __int64 *Displacement, _SYMBOL_INFOW *Symbol );
	DLL_CLASS_IMPORT int SymNext_( void *hProcess, _SYMBOL_INFOW *si );
	DLL_CLASS_IMPORT int SymPrev_( void *hProcess, _SYMBOL_INFOW *si );
	DLL_CLASS_IMPORT int SymEnumSymbolsEx_( void *hProcess, unsigned __int64 BaseOfDll, const wchar_t *Mask, PSYM_ENUMERATESYMBOLS_CALLBACKW EnumSymbolsCallback, void *UserContext, unsigned long Options );
	DLL_CLASS_IMPORT int SymEnumSymbolsForAddr_( void *hProcess, unsigned __int64 Address, PSYM_ENUMERATESYMBOLS_CALLBACKW EnumSymbolsCallback, void *UserContext );
	DLL_CLASS_IMPORT int SymQueryInlineTrace_( void *hProcess, unsigned __int64 StartAddress, unsigned long StartContext, unsigned __int64 StartRetAddress, unsigned __int64 CurAddress, unsigned long *CurContext, unsigned long *CurFrameIndex );
	DLL_CLASS_IMPORT int SymGetLineFromAddr64_( void *hProcess, unsigned __int64 qwAddr, unsigned long *pdwDisplacement, _IMAGEHLP_LINEW64 *Line64 );
	DLL_CLASS_IMPORT int SymGetLineFromInlineContext_( void *hProcess, unsigned __int64 qwAddr, unsigned long InlineContext, unsigned __int64 qwModuleBaseAddress, unsigned long *pdwDisplacement, _IMAGEHLP_LINEW64 *Line64 );
	DLL_CLASS_IMPORT int SymEnumSourceFiles_( void *hProcess, unsigned __int64 ModBase, const wchar_t *Mask, PSYM_ENUMSOURCEFILES_CALLBACKW cbSrcFiles, void *UserContext );
	DLL_CLASS_IMPORT int SymEnumSourceLines_( void *hProcess, unsigned __int64 Base, const wchar_t *Obj, const wchar_t *File, unsigned long Line, unsigned long Flags, PSYM_ENUMLINES_CALLBACKW EnumLinesCallback, void *UserContext );
	DLL_CLASS_IMPORT int SymGetSourceFileToken_( void *hProcess, unsigned __int64 Base, const wchar_t *FileSpec, void **Token, unsigned long *Size );
	DLL_CLASS_IMPORT int SymGetSourceVarFromToken_( void *hProcess, void *Token, const wchar_t *Params, const wchar_t *VarName, wchar_t *Value, unsigned long Size );
	DLL_CLASS_IMPORT int SymFindFileInPath_( void *hprocess, const wchar_t *SearchPath, const wchar_t *FileName, void *id, unsigned long two, unsigned long three, unsigned long flags, wchar_t *FoundFile, PFINDFILEINPATHCALLBACKW callback, void *context );
	DLL_CLASS_IMPORT int SymSrvGetFileIndexes_( const wchar_t *File, _GUID *Id, unsigned long *Val1, unsigned long *Val2, unsigned long Flags );
	DLL_CLASS_IMPORT int SymSrvGetFileIndexInfo_( const wchar_t *File, SYMSRV_INDEX_INFOW *Info, unsigned long Flags );

	DLL_CLASS_IMPORT void *SymFunctionTableAccess64_( void *hProcess, unsigned __int64 AddrBase );
	DLL_CLASS_IMPORT int StackWalk64_( unsigned long MachineType, void *hProcess, void *hThread, _tagSTACKFRAME64 *StackFrame, void *ContextRecord, PREAD_PROCESS_MEMORY_ROUTINE64 ReadMemoryRoutine, PFUNCTION_TABLE_ACCESS_ROUTINE64 FunctionTableAccessRoutine, PGET_MODULE_BASE_ROUTINE64 GetModuleBaseRoutine, PTRANSLATE_ADDRESS_ROUTINE64 TranslateAddress );
	DLL_CLASS_IMPORT int StackWalkEx_( unsigned long MachineType, void *hProcess, void *hThread, _tagSTACKFRAME_EX *StackFrame, void *ContextRecord, PREAD_PROCESS_MEMORY_ROUTINE64 ReadMemoryRoutine, PFUNCTION_TABLE_ACCESS_ROUTINE64 FunctionTableAccessRoutine, PGET_MODULE_BASE_ROUTINE64 GetModuleBaseRoutine, PTRANSLATE_ADDRESS_ROUTINE64 TranslateAddress, unsigned long Flags );

private:
	DLL_CLASS_IMPORT void AttemptToLoad();

	// The functions of the DLL
	void *unk000[ 34 ];

	CAtomicMutex m_Mutex;
	uint8 unk11c[ 20 ];
	bool unk130;
	bool unk131;
};

//-----------------------------------------------------------------------------
// The functions of imagehlp.dll
//-----------------------------------------------------------------------------
class CImageHlpFunctionsLoader
{
public:
	DLL_CLASS_IMPORT CImageHlpFunctionsLoader();
	DLL_CLASS_IMPORT ~CImageHlpFunctionsLoader();

	DLL_CLASS_IMPORT _LOADED_IMAGE *ImageLoad_( const char *DllName, const char *DllPath );
	DLL_CLASS_IMPORT int ImageUnload_( _LOADED_IMAGE *LoadedImage );
	DLL_CLASS_IMPORT void *ImageRvaToVa_( _IMAGE_NT_HEADERS64 *NtHeaders, void *Base, unsigned long Rva, _IMAGE_SECTION_HEADER **LastRvaSection );

private:
	DLL_CLASS_IMPORT void EnsureReady();
	DLL_CLASS_IMPORT void AttemptToLoad();

	// ImageLoad, ImageUnload and ImageRvaToVa
	void *unk000[ 3 ];

	CAtomicMutex m_Mutex;
	bool unk024;
	void *unk028;
};

//-----------------------------------------------------------------------------
// The functions of psapi.dll
//-----------------------------------------------------------------------------
class CPSApiFunctionsLoader
{
public:
	DLL_CLASS_IMPORT CPSApiFunctionsLoader();
	DLL_CLASS_IMPORT ~CPSApiFunctionsLoader();

	DLL_CLASS_IMPORT CPSApiFunctionsLoader &operator=( const CPSApiFunctionsLoader &other );

	DLL_CLASS_IMPORT int GetModuleInformation_( void *hProcess, HMODULE hModule, _MODULEINFO *lpmodinfo, unsigned long cb );

private:
	DLL_CLASS_IMPORT void EnsureReady();
	DLL_CLASS_IMPORT void AttemptToLoad();

	// GetModuleInformation
	void *unk000;
	bool unk008;
	void *unk010;
	void *unk018;
};

//-----------------------------------------------------------------------------
// The symbols of a module through DbgHelp
//-----------------------------------------------------------------------------
class CVDbgHelpSymbolModule : public CVSymbolModule
{
public:
	DLL_CLASS_IMPORT CVDbgHelpSymbolModule();
	virtual ~CVDbgHelpSymbolModule();

	DLL_CLASS_IMPORT bool Init( CDbgHelpFunctionsLoader *pDbgHelp, void *hProcess, const char *pszModulePath, const char *pszModuleName, const char *pszSearchPath, uint64 nModuleBase, uint64 nModuleSize, StackTools_ModuleIDFormat_t eIDFormat, const void *pModuleID, int nModuleIDSize, bool bUnknown );

private:
	void *unk0a0;
	void *unk0a8;
	void *unk0b0;
};

// For the watch window of the debugger, never called
struct Hack_WatchWindow_t
{
	DLL_CLASS_IMPORT static const Hack_WatchWindow_t *_FooHacked_DontEverCallMe( const void *p );
};

// The shared loaders of tier0, loaded on first use
PLATFORM_INTERFACE CDbgHelpFunctionsLoader *StackHelpers_Windows_GetDbgHelpWrapper();
PLATFORM_INTERFACE CImageHlpFunctionsLoader *StackHelpers_Windows_GetImageHlpWrapper();
PLATFORM_INTERFACE CPSApiFunctionsLoader *StackHelpers_Windows_GetPSApiWrapper();

// Loads dbghelp.dll from a known directory, such as the one of tier0, once
PLATFORM_INTERFACE HMODULE Win_LoadDbgHelpFromKnownPath();

// A process handle for SymInitialize: the real handle of the process, or a new unique ID for each
// session of DbgHelp
PLATFORM_INTERFACE void *StackHelpers_Windows_GetLocalProcessHandle_NonPseudo();
PLATFORM_INTERFACE uint32 StackHelpers_Windows_AllocateDbgHelpUniqueID();
PLATFORM_INTERFACE void StackHelpers_Windows_FreeDbgHelpUniqueID( uint32 nID );

// The symbol search path and the symbol cache directory tier0 gives DbgHelp, returns the text of pOut
PLATFORM_INTERFACE const char *StackHelpers_Windows_GenerateSymbolSearchPath( CBufferString *pOut );
PLATFORM_INTERFACE const char *StackHelpers_Windows_GenerateSymbolHomeDirectory( CBufferString *pOut );

// Fills pData for SymLoadModuleEx with the CodeView record of the module, built in pBuffer;
// false when the format is not 1 or 2 or the buffer is too small
PLATFORM_INTERFACE bool StackHelpers_Windows_InitModLoadData( _MODLOAD_DATA *pData, void *pBuffer, size_t nBufferSize, const char *pszPdbName, StackTools_ModuleIDFormat_t eIDFormat, const void *pModuleID );

// The NT headers of a mapped image of nSize bytes, null when it is not a PE image
PLATFORM_INTERFACE void *GetModuleNTHeaders( const void *pImage, size_t nSize );

PLATFORM_INTERFACE _TEB *GetThreadInformationBlock();
#endif // _WIN32

#endif // TIER0_STACKHELPERS_H
