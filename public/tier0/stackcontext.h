#ifndef STACKCONTEXT_H
#define STACKCONTEXT_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "tier0/threadtools.h"

typedef uintp IntegerRegisterValue_t;

struct MemoryRange_t
{
	// The bounds are raw addresses, so they are compared as integers
	size_t GetSize() const { return ( size_t )( ( uintp )pHigh - ( uintp )pLow ); }
	bool Contains( const void *p ) const { return ( uintp )p >= ( uintp )pLow && ( uintp )p < ( uintp )pHigh; }

	void *pLow;
	void *pHigh;
};

struct JmpBuf_AlternateStack_t;
struct StackContext_StartData_t;

struct StackContext_LongJumpLink_t
{
	enum JumpFlags_t : uintp
	{
		UNBREAKABLE = 1,
	};

	// The link is the first member of the frame
	inline JmpBuf_AlternateStack_t *GetEnclosingJmpBuf();

	StackContext_StartData_t *m_pExitContext;
	StackContext_StartData_t *m_pEnterContext;
	uintp m_nJumpFlags;
};

//-----------------------------------------------------------------------------
// Registers that a call preserves, in the order the assembly of tier0 pushes them
//-----------------------------------------------------------------------------
struct NonVolatileSIMDRegisters_t
{
#ifdef PLATFORM_WINDOWS
	ALIGN16 uint8 m_XMM6[ 16 ] ALIGN16_POST;
	uint8 m_XMM7[ 16 ];
	uint8 m_XMM8[ 16 ];
	uint8 m_XMM9[ 16 ];
	uint8 m_XMM10[ 16 ];
	uint8 m_XMM11[ 16 ];
	uint8 m_XMM12[ 16 ];
	uint8 m_XMM13[ 16 ];
	uint8 m_XMM14[ 16 ];
	uint8 m_XMM15[ 16 ];
#endif
};

struct _SIMDRegisterAlignmentPadding_t
{
	void *alignmentPadding1;
};

struct NonVolatileIntegerRegisters_t
{
	IntegerRegisterValue_t m_R12;
	IntegerRegisterValue_t m_R13;
	IntegerRegisterValue_t m_R14;
	IntegerRegisterValue_t m_R15;
	IntegerRegisterValue_t m_RBX;
#ifdef PLATFORM_WINDOWS
	IntegerRegisterValue_t m_RSI;
	IntegerRegisterValue_t m_RDI;
#endif
	IntegerRegisterValue_t m_RBP;
};

struct NonVolatileRegisters_t : public NonVolatileSIMDRegisters_t, public _SIMDRegisterAlignmentPadding_t, public NonVolatileIntegerRegisters_t
{
};

//-----------------------------------------------------------------------------
// Frame that RunWithAlternateStack and JmpToAlternateStack build on the stack they leave
//-----------------------------------------------------------------------------
struct JmpBuf_AlternateStack_t
{
	StackContext_LongJumpLink_t m_ExitLink;
#ifdef PLATFORM_WINDOWS
	// Stack limits of the TEB, restored on return
	void *m_pStackLimit;
	void *m_pStackBase;
#endif
	void *alignmentPadding1;
	NonVolatileRegisters_t m_Registers;
};

inline JmpBuf_AlternateStack_t *StackContext_LongJumpLink_t::GetEnclosingJmpBuf()
{
	return reinterpret_cast< JmpBuf_AlternateStack_t * >( this );
}

//-----------------------------------------------------------------------------
// Start of a stack context, a stack that code runs on through RunWithAlternateStack
//-----------------------------------------------------------------------------
struct StackContext_StartData_t
{
	enum ContextFlags_t : uintp
	{
		THREAD_TOP = 1,
	};

	DLL_CLASS_IMPORT void UnlinkFromLongJumpHierarchy();
	DLL_CLASS_IMPORT void ReplaceReturnContext( JmpBuf_AlternateStack_t *pReturnTo );

	IntegerRegisterValue_t m_ParentFramePointer;
	IntegerRegisterValue_t m_ReturnAddress;
	MemoryRange_t m_StackRange;
	StackContext_LongJumpLink_t *m_pReturnLink;
	StackContext_LongJumpLink_t *m_pExitLink;
	uintp m_nContextFlags;
};

//-----------------------------------------------------------------------------
// Thread that other threads can hand work to, or continue their own stack on
//-----------------------------------------------------------------------------
class CThreadSwapTarget
{
public:
	DLL_CLASS_IMPORT CThreadSwapTarget();

	DLL_CLASS_IMPORT bool ReserveOwnership();
	DLL_CLASS_IMPORT void OwnerThreadRun();
	DLL_CLASS_IMPORT void ReleaseOwnerThread();

	ThreadId_t GetOwnerThread() const { return m_hRunningThreadID; }
	bool IsCurrentThread() const { return m_hRunningThreadID == ThreadGetCurrentId(); }

private:
	// Queue of the pending swap contexts and jobs, the tail points at the link of the last item
	void **m_ppQueueTail;
	void *m_pQueueHead;
	CThreadSemaphore m_Semaphore;
	StackContext_LongJumpLink_t *m_pParkedTargetReturnLink;
	void *m_pActiveSwapContext;
	ThreadId_t m_hRunningThreadID;

	CThreadSwapTarget( const CThreadSwapTarget & ) = delete;
	CThreadSwapTarget &operator=( const CThreadSwapTarget & ) = delete;
};

// Runs pfnRun( pPassThroughArg ) on the stack of alternateStackRange; *ppJmpBack receives the
// frame that JmpToAlternateStack jumps back to, null makes the context unbreakable
PLATFORM_INTERFACE intp RunWithAlternateStack( const MemoryRange_t &alternateStackRange, intp (*pfnRun)( void * ), void *pPassThroughArg, JmpBuf_AlternateStack_t **ppJmpBack );

// Continues the stack of jmpTo, which then returns nReturnValue; *ppJmpBack receives the frame
// of the current stack
PLATFORM_INTERFACE intp JmpToAlternateStack( JmpBuf_AlternateStack_t &jmpTo, JmpBuf_AlternateStack_t **ppJmpBack, intp nReturnValue );

template < typename R, typename T >
R RunWithAlternateStack( const MemoryRange_t &alternateStackRange, R (*pFnRun)( T ), T pPassThroughArg, JmpBuf_AlternateStack_t **ppJmpBack )
{
	return ( R )RunWithAlternateStack( alternateStackRange, reinterpret_cast< intp (*)( void * ) >( pFnRun ), ( void * )pPassThroughArg, ppJmpBack );
}

#ifdef PLATFORM_POSIX
// The context returns into JmpToAlternateStack to the frame of pJmpBuf
PLATFORM_INTERFACE void StackContext_SetContextReturn_To_JmpToAlternateStack( StackContext_StartData_t *pContext, JmpBuf_AlternateStack_t *pJmpBuf );

// Checks of the assembly that tier0 runs once
PLATFORM_INTERFACE bool TestStackPointerAlignment_ASM();
PLATFORM_INTERFACE intp TestStackVolatility_ASM( JmpBuf_AlternateStack_t &jmpBuf, intp (*pfnRun)( void * ), void *pPassThroughArg, JmpBuf_AlternateStack_t &jmpBack );
#endif

#ifdef PLATFORM_OSX
// Swap targets of the threads that macOS and SDL require some calls to run on
PLATFORM_INTERFACE CThreadSwapTarget *g_pThreadSwapTarget_Apple_Main_Thread;
PLATFORM_INTERFACE CThreadSwapTarget *g_pThreadSwapTarget_SDL_Main_Thread;
#endif

//-----------------------------------------------------------------------------
// Moves the current stack to the target thread for the lifetime of the object,
// pParkedThreadMemory is the stack the parked thread runs on meanwhile
//-----------------------------------------------------------------------------
class CThreadSwapContext_Base
{
public:
	DLL_CLASS_IMPORT CThreadSwapContext_Base( CThreadSwapTarget *pTarget, void *pParkedThreadMemory, size_t nParkedThreadMemorySize );
	DLL_CLASS_IMPORT ~CThreadSwapContext_Base();

private:
	CThreadSwapContext_Base *m_pNext;
	CThreadSwapTarget *m_pTarget;
	int m_nKind; // 1 for a swap context, 2 for a job
	CThreadManualEvent m_PreSwapParkedThreadWait;
	StackContext_StartData_t *m_pParkedContext;
	void *m_pParkedThreadMemory;
	size_t m_nParkedThreadMemorySize;

	CThreadSwapContext_Base( const CThreadSwapContext_Base & ) = delete;
	CThreadSwapContext_Base &operator=( const CThreadSwapContext_Base & ) = delete;
};

//-----------------------------------------------------------------------------
// Runs functions on the target thread and waits for them
//-----------------------------------------------------------------------------
class CThreadSwapJob
{
public:
	DLL_CLASS_IMPORT CThreadSwapJob( CThreadSwapTarget *pTarget );

	// Runs pfnJob on the thread of the target and waits for it, or runs it right away when
	// the target has no thread or is the current one
	void RunJob( void (*pfnJob)( void * ), void *pContext ) { InternalRunJob( pfnJob, pContext ); }

private:
	// Private in tier0, which MSVC encodes in the exported name
	DLL_CLASS_IMPORT void InternalRunJob( void (*pfnJob)( void * ), void *pContext );

	CThreadSwapJob *m_pNext;
	CThreadSwapTarget *m_pTarget;
	int m_nKind; // 1 for a swap context, 2 for a job
	CThreadManualEvent m_JobDone;
	void (*m_pfnJob)( void * );
	void *m_pJobContext;

	CThreadSwapJob( const CThreadSwapJob & ) = delete;
	CThreadSwapJob &operator=( const CThreadSwapJob & ) = delete;
};

#endif // STACKCONTEXT_H
