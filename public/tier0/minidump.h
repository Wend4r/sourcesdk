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

// Set prefix to use for minidump files.  If you don't set one, it is defaulted for you,
// using the current module name
PLATFORM_INTERFACE void SetMinidumpFilenamePrefix( const char *pszPrefix );

// Set comment to put into minidump file upon next call of WriteMiniDump.  (Most common use is the assert text.)
PLATFORM_INTERFACE void SetMinidumpComment( const char *pszComment );

// writes out a minidump of the current stack trace with a unique filename
PLATFORM_INTERFACE void WriteMiniDump( const char *pszFilenameSuffix = NULL );

typedef void( *FnWMain )(int, tchar *[]);
typedef void( *FnVoidPtrFn )(void *);

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
typedef _EXCEPTION_POINTERS ExceptionInfo_t;

// Replaces the current function pointer with the one passed in.
// Returns the previously-set function.
// The function is called internally by WriteMiniDump() and CatchAndWriteMiniDump()
// The default is the built-in function that uses DbgHlp.dll's MiniDumpWriteDump function
typedef void( *FnMiniDump )(unsigned int uStructuredExceptionCode, _EXCEPTION_POINTERS * pExceptionInfo, const char *pszFilenameSuffix);
PLATFORM_INTERFACE FnMiniDump SetMiniDumpFunction( FnMiniDump pfn );

// Use this to write a minidump explicitly.
// Some of the tools choose to catch the minidump themselves instead of using CatchAndWriteMinidump
// so they can show their own dialog.
//
// ptchMinidumpFileNameBuffer if not-NULL should be a writable tchar buffer of length at
// least _MAX_PATH and on return will contain the name of the minidump file written.
// If ptchMinidumpFileNameBuffer is NULL the name of the minidump file written will not
// be available after the function returns.
//

// NOTE: Matches windows.h
enum MinidumpType_t 
{
	MINIDUMP_Normal                           = 0x00000000,
	MINIDUMP_WithDataSegs                     = 0x00000001,
	MINIDUMP_WithFullMemory                   = 0x00000002,
	MINIDUMP_WithHandleData                   = 0x00000004,
	MINIDUMP_FilterMemory                     = 0x00000008,
	MINIDUMP_ScanMemory                       = 0x00000010,
	MINIDUMP_WithUnloadedModules              = 0x00000020,
	MINIDUMP_WithIndirectlyReferencedMemory   = 0x00000040,
	MINIDUMP_FilterModulePaths                = 0x00000080,
	MINIDUMP_WithProcessThreadData            = 0x00000100,
	MINIDUMP_WithPrivateReadWriteMemory       = 0x00000200,
	MINIDUMP_WithoutOptionalData              = 0x00000400,
	MINIDUMP_WithFullMemoryInfo               = 0x00000800,
	MINIDUMP_WithThreadInfo                   = 0x00001000,
	MINIDUMP_WithCodeSegs                     = 0x00002000 
};

PLATFORM_INTERFACE bool WriteMiniDumpUsingExceptionInfo(
	unsigned int uStructuredExceptionCode,
	_EXCEPTION_POINTERS * pExceptionInfo,
	int /* MINIDUMP_TYPE */ minidumpType,
	const char *pszFilenameSuffix = NULL,
	tchar *ptchMinidumpFileNameBuffer = NULL
	);

// Call this to enable a handler for unhandled exceptions.
PLATFORM_INTERFACE void MinidumpSetUnhandledExceptionFunction( FnMiniDump pfn );

// Call this to prevent crashes in kernel callbacks such as window procs from
// being silently swallowed. We should always call this at startup.
PLATFORM_INTERFACE void EnableCrashingOnCrashes();

#endif // MINIDUMP_H
