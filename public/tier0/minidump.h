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
	bool EnsureEndsWithNumCharacters( char, int, bool );
	bool RemoveTrailingCharacters( char );
	void OnExceptionCaught();

private:
	[[maybe_unused]] char pad0[0x28];
};
static_assert(sizeof(CMiniDumpComment) == 0x28, "CMiniDumpComment - incorrect size on this compiler");

#endif // MINIDUMP_H
