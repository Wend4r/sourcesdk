//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: 
//
//===========================================================================//

#ifndef IPROCESSUTILS_H
#define IPROCESSUTILS_H

#ifdef _WIN32
#pragma once
#endif


#include "appframework/iappsystem.h"
#include "tier0/utlstring.h"
#include "tier0/utlbuffer.h"


//-----------------------------------------------------------------------------
// Handle to a process. This is only for b/w compatibility.
//-----------------------------------------------------------------------------
class IProcess;
class CBufferString;
class CEnvironmentBlock;


//-----------------------------------------------------------------------------
// Interface version
//-----------------------------------------------------------------------------
#define PIPEREAD_INFINITE	INT32_MAX

abstract_class IPipeRead
{
public:
	virtual bool IsValid() const = 0;

	// NONBLOCKING FUNCTIONS

	// Returns how much you can Read() without blocking.
	virtual int GetNumBytesAvailable() = 0;

	// Read whatever is available without blocking.
	// This is the same as Read( sStr, GetNumBytesAvailable() ).
	virtual void ReadAvailable( CUtlString &sStr, int32 nMaxBytes=PIPEREAD_INFINITE  ) = 0;
	virtual void ReadAvailable( CUtlBuffer* pOutBuffer, int32 nMaxBytes=PIPEREAD_INFINITE  ) = 0;


	// (POTENTIALLY) BLOCKING FUNCTIONS

	// Read one line of output (also returns when the process quits).
	// sStr will not include the \n (or \r\n) at the end of the line.
	virtual void ReadLine( CBufferString &sStr ) = 0;
	virtual void ReadLine( CUtlString &sStr ) = 0;

	// This will block the calling thread until it gets the number of bytes specified
	// or until the process exits. If sStr.Length() != nBytes, then you know the process exited.
	//
	// The returned string will always be null-terminated.
	// If you call with nBytes=PIPEREAD_INFINITE, it'll read until the process exits.
	virtual void Read( CUtlString &sStr, int32 nBytes=PIPEREAD_INFINITE ) = 0;
};


enum ProcessPriority_t
{
	PROCESS_PRIORITY_IDLE,
	PROCESS_PRIORITY_LOW,
	PROCESS_PRIORITY_NORMAL,
	PROCESS_PRIORITY_HIGH,
	PROCESS_PRIORITY_REALTIME
};

abstract_class IProcess
{
public:
	virtual ~IProcess() {}

	virtual const char *GetCommandLine() = 0;
	virtual int GetFlags() = 0;
	virtual void *GetProcessHandle() = 0;
	virtual uint32 GetProcessId() = 0;

	virtual void SetPriority( ProcessPriority_t ePriority ) = 0;

	// Note: If the process is still running, this will auto kill it unless you started the process with
	// STARTPROCESS_NOAUTOKILL.
	virtual void Release() = 0;

	// Kill the running process. You still must call IProcess::Release to free the resources.
	virtual void Abort() = 0;

	// Returns true if a process is complete
	virtual bool IsComplete() = 0;

	// Waits until a process is complete.
	// Returns the return value from the process.
	virtual int WaitUntilComplete() = 0;

	// Write to the process' stdin.
	// This blocks until the process has read it.
	virtual int WriteStdin( char *pBuf, int nBufLen ) = 0;
	virtual void CloseStdin() = 0;

	// Get stuff to read the outputs.
	virtual IPipeRead* GetStdout() = 0;
	virtual IPipeRead* GetStderr() = 0;	// NOTE: Only valid if you used STARTPROCESS_SEPARATE_STDERR.
	
	// Returns the exit code for the process. Doesn't work unless the process is complete.
	// Returns -1 on error or if the process isn't complete.
	virtual int GetExitCode() = 0;

	virtual bool DidCrash() = 0;
};


// Flags to IProcessUtils::StartProcess.
#define STARTPROCESS_CONNECTSTDPIPES		0x01	// Necessary to use the stdin/stdout/stderr io functions.
#define STARTPROCESS_SHARE_CONSOLE			0x02	// The process writes directly to your console. The pipe objects returned by 
													// IProcess::GetStdout and GetStderr won't do anything.
#define STARTPROCESS_SEPARATE_STDERR		0x04	// Rather than having to read stdout and stderr to get the output, the default is to put the stderr output into stdout.
													// This flag can change that behavior so you can get that output separately.
													// Warning: There may be deadlock problems with this, specifically in CProcessPipeRead::GetActualProcessOutput if
													//          it's blocked reading stdout's pipe but the process is blocked waiting for us to flush stderr's pipe first.
													//			To fully support that case, we'd need threads, overlapped IO, or a more careful (and slower) GetActualProcessOutput call
													//			that bounces between the two pipes and never stalls.
													//			
													//			You can also get around this on the client side by reading the pipes from threads.
#define STARTPROCESS_NOAUTOKILL				0x08	// Prevents the process from being auto-terminated in IProcess::Release()
													// or when IProcessUtils' Shutdown function is called.
#define STARTPROCESS_FATPIPES				0x10	// Use I/O pipes larger than the default size for processes that do lots of stdio
													// (Only works with STARTPROCESS_CONNECTSTDPIPES)
// AMNOTE: Other flags the engine checks: 0x20 don't inherit handles, 0x40 adds -attachtodebugger, 0x80 adds -breakimmediately,
// 0x100 kill with parent process, 0x200 no console window, 0x400 run under crash monitor, 0x800 no stdout pipe, 0x1000 no stderr pipe

#ifdef _WIN32
typedef void *OSFILEHANDLE;
#else
typedef int OSFILEHANDLE;
#endif

struct StartProcessInfo_t
{
	PLATFORM_CLASS void CloseIo();

	const char *m_pWorkingDir = nullptr;
	CEnvironmentBlock *m_pEnvironment = nullptr;

	// AMNOTE: Duplicated into the process as its stdin, stdout and stderr, -1 leaves one unused
	OSFILEHANDLE m_hStdin = ( OSFILEHANDLE )-1;
	OSFILEHANDLE m_hStdout = ( OSFILEHANDLE )-1;
	OSFILEHANDLE m_hStderr = ( OSFILEHANDLE )-1;

	// AMNOTE: Closes the handles above with CloseIo once the process is started
	bool m_bCloseIo = false;
};

//-----------------------------------------------------------------------------
// Interface for makefiles to build differently depending on where they are run from
//-----------------------------------------------------------------------------
abstract_class IProcessUtils : public IAppSystem
{
public:
	// Starts, stops a process.
	// If pWorkingDir is left at NULL, it'll use this process' working directory.
	virtual IProcess* StartProcess( const char *pCommandLine, int fFlags, const char *pWorkingDir=NULL )= 0;
	virtual IProcess* StartProcess( int argc, const char **argv, int fFlags, const char *pWorkingDir=NULL ) = 0;
	virtual IProcess* StartProcessWithEnv( const char *pCommandLine, int fFlags, const char *pWorkingDir, CEnvironmentBlock *pEnvironment ) = 0;
	virtual IProcess* StartProcess( const char *pCommandLine, int fFlags, StartProcessInfo_t *pInfo ) = 0;

	// Run a process and get its output.
	// If pStdout is set, then stdout AND stderr are put into pStdout.
	// If not, then the text output is ignored.
	//
	// Returns -1 if it was unable to run the process. Otherwise, returns the exit code from the process.
	virtual int SimpleRunProcess( const char *pCommandLine, const char *pWorkingDir=NULL, CUtlString *pStdout=NULL ) = 0;

	virtual const char *GetMiniDumpPath() = 0;
	virtual void SetMiniDumpPath( const char *pPath ) = 0;
	virtual const char *GetMiniDumpPrefix() = 0;
	virtual void SetMiniDumpPrefix( const char *pPrefix ) = 0;
	virtual uint32 GetMiniDumpFlags() = 0;
	virtual void SetMiniDumpFlags( uint32 nFlags ) = 0;
	virtual int GetNumMiniDumpsWritten() = 0;
	virtual int GetMiniDumpWriteLimit() = 0;
	virtual void SetMiniDumpWriteLimit( int nLimit ) = 0;
};

DECLARE_TIER1_INTERFACE( IProcessUtils, g_pProcessUtils );


#endif // IPROCESSUTILS_H
