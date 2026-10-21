//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#ifndef MINIDUMP_H
#define MINIDUMP_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "tier0/memalloc.h"

// calls the passed in function pointer and catches any exceptions/crashes thrown by it, and writes a minidump
// use from wmain() to protect the whole program
typedef void( *FnWMain )(int, tchar *[]);
typedef int( *FnWMainIntRet )(int, tchar *[]);
typedef void( *FnVoidPtrFn )(void *);

enum ECatchAndWriteMinidumpAction
{
	k_ECatchAndWriteMiniDumpAbort = 0,
	k_ECatchAndWriteMiniDumpReThrow = 1,
	k_ECatchAndWriteMiniDumpIgnore = 2,
};

PLATFORM_INTERFACE void CatchAndWriteMiniDump( FnWMain pfn, int argc, tchar *argv[] ); // action = Abort
PLATFORM_INTERFACE void CatchAndWriteMiniDumpForVoidPtrFn( FnVoidPtrFn pfn, void *pv, bool bExitQuietly ); // action = abort if bExitQuietly, Rethrow otherwise

PLATFORM_INTERFACE void CatchAndWriteMiniDumpEx( FnWMain pfn, int argc, tchar *argv[], ECatchAndWriteMinidumpAction eAction );
PLATFORM_INTERFACE int CatchAndWriteMiniDumpExReturnsInt( FnWMainIntRet pfn, int argc, tchar *argv[], ECatchAndWriteMinidumpAction eAction );
PLATFORM_INTERFACE void CatchAndWriteMiniDumpExForVoidPtrFn( FnVoidPtrFn pfn, void *pv, ECatchAndWriteMinidumpAction eAction );

// Let's not include this.  We'll use forwards instead.
//#include <dbghelp.h>
struct _EXCEPTION_POINTERS;

// What the caller of a minidump asks for; the names of the type and its members are ours,
// InvokeMiniDumpHandler, AlwaysInvokeMiniDumpHandler and MiniDumpExceptionHandler copy it
// into MiniDumpStandardData_t::m_Options
struct MiniDumpOptions_t
{
	MiniDumpOptions_t() { memset( this, 0, sizeof( *this ) ); }

	// The options of the callers: the reason and the notes of the file, flags 0x40
	MiniDumpOptions_t( const char *pszReason, const char *pszNotes = NULL )
	{
		memset( this, 0, sizeof( *this ) );
		m_pszReason = pszReason;
		m_pszNotes = pszNotes;
		m_nFlags = 0x40;
	}

	// Part of the file name, "crash" or "forced" when null
	const char *m_pszReason;

	// The type flags of the minidump, GetDefaultMiniDumpTypeFlags when the caller has none
	uint32 m_nMiniDumpTypeFlags;

	// The exception that InvokeMiniDumpHandler raises on Windows, 0 for the default one
	uint32 m_uExceptionCode;

	// 0x20 adds _-report-_ to the file name; the callers set 0x40
	uint8 m_nFlags;
	uint8 unk011[ 7 ];
	uint8 unk018[ 24 ];

	// Notes of the minidump, the command line value of -minidumpnotes is used when null
	const char *m_pszNotes;
	uint8 unk038[ 48 ];
};

// The minidump that a handler writes
struct MiniDumpStandardData_t
{
	DLL_CLASS_IMPORT void HandlerQueueHeartBeat() const;

	uint32 m_uStructuredExceptionCode;
	// 1 indicates a forced minidump, nonzero makes MiniDumpExceptionHandler skip its debugger and -nominidumps checks
	int32 m_nForced;
	// Null on Linux
	_EXCEPTION_POINTERS *m_pExceptionInfo;
	// State of the handler queue, a byte at +5 tells whether a debugger may be attached
	void *m_pHandlerState;
	MiniDumpOptions_t m_Options;
};

typedef int ( *FnMiniDumpHandler )( MiniDumpStandardData_t *pData );

// Returns the previous handler. Linux ignores the handler and returns null.
PLATFORM_INTERFACE FnMiniDumpHandler SetDefaultMiniDumpHandler( FnMiniDumpHandler pfn, bool bRegisterForUnhandledExceptions );
PLATFORM_INTERFACE FnMiniDumpHandler GetDefaultMiniDumpHandler();

// The generic minidump handler of tier0, which writes the minidump of pData
PLATFORM_INTERFACE int Tier0GenericMiniDumpHandler( MiniDumpStandardData_t *pData );
PLATFORM_INTERFACE void Tier0GenericMiniDumpHandler_SetFilenamePrefix( const char *pszPrefix );

// Writes the minidump of pData and copies its file name to pszFileNameOut; nFlags bit 0 prints
// the result to stderr, bit 3 asks for notes, -1 returns at once
PLATFORM_INTERFACE bool Tier0GenericMiniDumpHandlerEx( MiniDumpStandardData_t *pData, char *pszFileNameOut, int nFlags );

// The type of the minidumps of the generic handler: bFullMemory adds the full memory, bFilterMemory
// keeps the memory filter; bRegisterForUnhandledExceptions calls MiniDumpRegisterForUnhandledExceptions
PLATFORM_INTERFACE void Tier0GenericMiniDumpHandler_SetOptions( bool bFullMemory, bool bFilterMemory, bool bRegisterForUnhandledExceptions );

// Called before the generic handler writes a minidump, with an internal state of tier0
PLATFORM_INTERFACE void Tier0GenericMiniDumpHandler_SetPreHandlerCallback( void ( *pfnCallback )( void *pContext ) );

// Empty in every build of tier0, the parameters are not known
PLATFORM_INTERFACE void Tier0GenericMiniDumpHandler_SetGetAdditionalFileAttachmentCallback();

// Raises an exception on Windows so that the default handler writes a minidump with pOptions,
// bBreakOnBreakpoint breaks into the debugger for a breakpoint exception; false on POSIX
PLATFORM_INTERFACE bool InvokeMiniDumpHandler( FnMiniDumpHandler pfnHandler, const MiniDumpOptions_t *pOptions, bool bBreakOnBreakpoint );

#ifdef PLATFORM_POSIX
// Calls pfnHandler with a forced breakpoint minidump of pOptions; true when it wrote one
PLATFORM_INTERFACE bool AlwaysInvokeMiniDumpHandler( FnMiniDumpHandler pfnHandler, const MiniDumpOptions_t *pOptions );
#endif

// The exception handler of the minidumps: nAction 0 returns 1, 1 returns 0 and anything else
// exits the process when minidumps are off; nForced is MiniDumpStandardData_t::m_nForced
PLATFORM_INTERFACE int MiniDumpExceptionHandler( uint32 uStructuredExceptionCode, _EXCEPTION_POINTERS *pExceptionInfo, int nAction, void *pUnknown, int nForced, const MiniDumpOptions_t *pOptions );

// The file name of a new minidump: the directory, pszPrefix, the executable, the date, _pszReason and pszExtension
PLATFORM_INTERFACE const char *GetNewMiniDumpFileName( const char *pszReason, CBufferString *pFileName, const char *pszDirectory, const char *pszPrefix, const char *pszExtension );

// Creates the file of GetNewMiniDumpFileName, trying new names up to nMaxAttempts times
PLATFORM_INTERFACE OSFILEHANDLE CreateNewMiniDumpFile( const char *pszReason, CBufferString *pFileName, const char *pszDirectory, const char *pszPrefix, const char *pszExtension, int nMaxAttempts );

// Empty in every build of tier0, the parameters are not known
PLATFORM_INTERFACE void InitMiniDumpReportOptions();
PLATFORM_INTERFACE void MailMiniDump();
PLATFORM_INTERFACE void SendMiniDumpReport();
PLATFORM_INTERFACE bool UploadFilesToAnalyzer();
PLATFORM_INTERFACE bool UploadTracyFile();

#ifdef _WIN32
// The log of the minidump handler, and the removal of its old files
PLATFORM_INTERFACE void MinidumpLog( PRINTF_FORMAT_STRING const char *pszFormat, ... ) FMTFUNCTION( 1, 2 );
PLATFORM_INTERFACE void MinidumpAgeLogs();
#endif

//-----------------------------------------------------------------------------
// tier0's own MiniDumpWriteDump, which also writes the minidumps of POSIX processes
//-----------------------------------------------------------------------------
enum VWMD_MINIDUMP_TYPE : int;
struct _VWMD_MINIDUMP_EXCEPTION_INFORMATION;
struct _VWMD_MINIDUMP_USER_STREAM_INFORMATION;
struct _VWMD_MINIDUMP_CALLBACK_INFORMATION;
struct _VWMD_MINIDUMP_CALLBACK_INPUT;
struct _VWMD_MINIDUMP_CALLBACK_OUTPUT;
class CVDebugProcess;

// The type of _VWMD_MINIDUMP_CALLBACK_INFORMATION::CallbackRoutine, after MINIDUMP_CALLBACK_ROUTINE
typedef int ( *VWMD_MINIDUMP_CALLBACK_ROUTINE )( void *pContext, const _VWMD_MINIDUMP_CALLBACK_INPUT *pInput, _VWMD_MINIDUMP_CALLBACK_OUTPUT *pOutput );

PLATFORM_OVERLOAD bool VWMD_tier0_MiniDumpWriteDump( void *hProcess, uint32 nProcessId, OSFILEHANDLE hFile, VWMD_MINIDUMP_TYPE eDumpType, _VWMD_MINIDUMP_EXCEPTION_INFORMATION *pExceptionParam, _VWMD_MINIDUMP_USER_STREAM_INFORMATION *pUserStreamParam, _VWMD_MINIDUMP_CALLBACK_INFORMATION *pCallbackParam );
PLATFORM_OVERLOAD bool VWMD_tier0_MiniDumpWriteDump( uint32 nProcessId, const char *pszFileName, bool bUnknown, VWMD_MINIDUMP_TYPE eDumpType, _VWMD_MINIDUMP_EXCEPTION_INFORMATION *pExceptionParam, _VWMD_MINIDUMP_USER_STREAM_INFORMATION *pUserStreamParam, _VWMD_MINIDUMP_CALLBACK_INFORMATION *pCallbackParam );
PLATFORM_OVERLOAD bool VWMD_tier0_MiniDumpWriteDump( CVDebugProcess *pProcess, const char *pszFileName, bool bUnknown, VWMD_MINIDUMP_TYPE eDumpType, _VWMD_MINIDUMP_EXCEPTION_INFORMATION *pExceptionParam, _VWMD_MINIDUMP_USER_STREAM_INFORMATION *pUserStreamParam, _VWMD_MINIDUMP_CALLBACK_INFORMATION *pCallbackParam );

// The callback of the minidumps of tier0, which leaves out the modules it does not need;
// MinidumpDefaultWriteCallbackInit puts it into pCallbackInfo with no parameter
PLATFORM_INTERFACE int MinidumpDefaultWriteCallback( void *pContext, const _VWMD_MINIDUMP_CALLBACK_INPUT *pInput, _VWMD_MINIDUMP_CALLBACK_OUTPUT *pOutput );
PLATFORM_INTERFACE VWMD_MINIDUMP_CALLBACK_ROUTINE MinidumpDefaultWriteCallbackInit( _VWMD_MINIDUMP_CALLBACK_INFORMATION *pCallbackInfo );

// The minidump type flags tier0 writes by default
PLATFORM_INTERFACE uint32 GetDefaultMiniDumpTypeFlags();

PLATFORM_INTERFACE void MiniDumpRegisterForUnhandledExceptions();

// The comment of the minidumps; tier0 keeps the pointer, not a copy
PLATFORM_INTERFACE void MinidumpSetMiniDumpComment( const char *pszComment );
PLATFORM_INTERFACE void MiniDumpCommentEnsureOSDescription();

// The directory of the minidumps: the OVERRIDE_MINIDUMP_PATH environment variable, otherwise
// pszDefault or the current directory; returns the text of pOut
PLATFORM_INTERFACE const char *GetMiniDumpDirectory( CBufferString *pOut, const char *pszDefault );

// The user stream of the minidumps: a formatted header and the last 64 timestamped lines.
// MinidumpUserStreamInfoGet returns the header for 0 and a line for 1 to 64
PLATFORM_INTERFACE void MinidumpUserStreamInfoSetHeader( PRINTF_FORMAT_STRING const char *pszFormat, ... ) FMTFUNCTION( 1, 2 );
PLATFORM_INTERFACE void MinidumpUserStreamInfoAppend( PRINTF_FORMAT_STRING const char *pszFormat, ... ) FMTFUNCTION( 1, 2 );
PLATFORM_INTERFACE const char *MinidumpUserStreamInfoGet( uint32 nIndex );

class DLL_CLASS_IMPORT CMiniDumpComment
{
public:
	CMiniDumpComment( int iSize, MemAllocAttribute_t allocAttribute = MemAllocAttribute_Unk0 );
	~CMiniDumpComment();
	char *GetStartPointer();
	const char *GetStartPointer() const;
	const char *GetEndPointer() const;
	char *GetCurrentPointer();
	const char *GetCurrentPointer() const;
	void EnsureOSDescription();
	int GetAvailableBufferSize() const;
	void Reset();
	void AppendOSComment();
	void AppendComment( const char *pszComment );
	void PrependComment( const char *pszComment );
	void AppendFormattedComment( const char *pszComment, ... ) FMTFUNCTION( 2, 3 );

	// Appends ch until the comment ends with nCount of them, returns whether it appended any
	bool EnsureEndsWithNumCharacters( char ch, int nCount, bool bAppendToEmpty );
	// Returns whether it removed any
	bool RemoveTrailingCharacters( char ch );

	void OnExceptionCaught();

private:
	int m_nBufferSize;
	MemAllocAttribute_t m_nAllocAttribute;
	char *m_pBuffer;
	char *m_pOSDescription;
	EOSType m_eOSType;
};

COMPILE_TIME_ASSERT( sizeof( CMiniDumpComment ) == 32 );

//-----------------------------------------------------------------------------
// Adds its own text to the minidump comment, the builders register themselves
// in a list on construction
//-----------------------------------------------------------------------------
class CMiniDumpCommentBuilderBase
{
public:
	DLL_CLASS_IMPORT CMiniDumpCommentBuilderBase();
	DLL_CLASS_IMPORT virtual ~CMiniDumpCommentBuilderBase();

	DLL_CLASS_IMPORT virtual CMiniDumpCommentBuilderBase *GetNextCommentBuilder();
	// Appends the text of this builder to the comment of the minidump being written; tier0 calls
	// it for every registered builder in turn (the client adds its "setpos ...;setang ..." line)
	virtual void BuildComment( MiniDumpStandardData_t *pData, CMiniDumpComment *pComment ) = 0;

private:
	CMiniDumpCommentBuilderBase *m_pNext;

	CMiniDumpCommentBuilderBase( const CMiniDumpCommentBuilderBase & ) = delete;
	CMiniDumpCommentBuilderBase &operator=( const CMiniDumpCommentBuilderBase & ) = delete;
};

#endif // MINIDUMP_H
