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
	void *pLow;
	void *pHigh;
};

// Saved registers and links of a jump between stacks, their layout depends on the platform
struct JmpBuf_AlternateStack_t;
struct StackContext_StartData_t;

struct StackContext_LongJumpLink_t
{
	StackContext_StartData_t *m_pExitContext;
	StackContext_StartData_t *m_pEnterContext;
	uintp m_nJumpFlags;
};

//-----------------------------------------------------------------------------
// Start of a stack context, a stack that code runs on through RunWithAlternateStack
//-----------------------------------------------------------------------------
struct StackContext_StartData_t
{
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

private:
	// Queue of the pending swap contexts and jobs, the tail points at the link of the last item
	void **m_ppQueueTail;
	void *m_pQueueHead;
	CThreadSemaphore m_Semaphore;
	StackContext_LongJumpLink_t *m_pParkedTargetReturnLink;
	void *m_pActiveSwapContext;
	ThreadId_t m_hRunningThreadID;
};

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
};

//-----------------------------------------------------------------------------
// Runs functions on the target thread and waits for them
//-----------------------------------------------------------------------------
class CThreadSwapJob
{
public:
	DLL_CLASS_IMPORT CThreadSwapJob( CThreadSwapTarget *pTarget );

private:
	DLL_CLASS_IMPORT void InternalRunJob( void (*pfnJob)( void * ), void *pContext );

	CThreadSwapJob *m_pNext;
	CThreadSwapTarget *m_pTarget;
	int m_nKind; // 1 for a swap context, 2 for a job
	CThreadManualEvent m_JobDone;
	void (*m_pfnJob)( void * );
	void *m_pJobContext;
};

#endif // STACKCONTEXT_H
