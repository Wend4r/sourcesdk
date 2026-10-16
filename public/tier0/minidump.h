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

// Incomplete, only its start is known
struct MiniDumpStandardData_t
{
	DLL_CLASS_IMPORT void HandlerQueueHeartBeat() const;

	uint32 m_uStructuredExceptionCode;
	// 1 indicates a forced minidump, nonzero makes MiniDumpExceptionHandler skip its debugger and -nominidumps checks
	int32 m_nUnknown;
	// Null on Linux
	_EXCEPTION_POINTERS *m_pExceptionInfo;
};

typedef int ( *FnMiniDumpHandler )( MiniDumpStandardData_t *pData );

// Returns the previous handler. Linux ignores the handler and returns null.
PLATFORM_INTERFACE FnMiniDumpHandler SetDefaultMiniDumpHandler( FnMiniDumpHandler pfn, bool bRegisterForUnhandledExceptions );
PLATFORM_INTERFACE FnMiniDumpHandler GetDefaultMiniDumpHandler();

// The generic minidump handler of tier0, which writes the minidump of pData
PLATFORM_INTERFACE int Tier0GenericMiniDumpHandler( MiniDumpStandardData_t *pData );
PLATFORM_INTERFACE void Tier0GenericMiniDumpHandler_SetFilenamePrefix( const char *pszPrefix );

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
