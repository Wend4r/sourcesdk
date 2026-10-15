#ifndef FIBERTOOLS_H
#define FIBERTOOLS_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "tier0/threadtools.h"
#include "tier0/stackcontext.h"
#include "tier0/tsfastpushqueue.h"

class CFiber;

//-----------------------------------------------------------------------------
// Event that a thread or a fiber waits on in an event list
//-----------------------------------------------------------------------------
class CEventListNode
{
public:
	CEventListNode() : m_pNext( nullptr ), unk008( nullptr ) {}

	DLL_CLASS_IMPORT static CEventListNode *AllocTempNode();
	DLL_CLASS_IMPORT static void FreeTempNode( CEventListNode *pNode );

	CEventListNode *m_pNext;
	void *unk008;
	CThreadManualEvent m_Event;
};

class CEventList : public CTSFastPushQueue_Base< CEventListNode, CEventListNode, CEventListNode, &CEventListNode::m_pNext >
{
};

//-----------------------------------------------------------------------------
// Lock that queues the waiting threads and fibers
//-----------------------------------------------------------------------------
class CEventLock
{
public:
	class CLockList : public CTSFastPushQueue_Base< CEventListNode, CEventListNode, CEventListNode, &CEventListNode::m_pNext >
	{
	};

private:
	CLockList m_LockList;
};

//-----------------------------------------------------------------------------
// Read-write lock that queues the waiting threads and fibers
//-----------------------------------------------------------------------------
class CEventRWLock
{
public:
	CEventRWLock() : m_pWritingEvent( nullptr ), m_nWritingThreadLockCount( 0 ) {}

	DLL_CLASS_IMPORT void DuplicateReadLock();

	// Switches the held lock, RWLTR_STATE_INVALIDATED means another writer had it in between
	DLL_CLASS_IMPORT RWLockTranstionResult_t UnlockRead_LockForWrite( CEventListNode &node );
	DLL_CLASS_IMPORT RWLockTranstionResult_t UnlockWrite_LockForRead( CEventListNode &node );

	const CEventListNode *GetWriteLockEvent() const { return m_pWritingEvent; }

protected:
	DLL_CLASS_IMPORT void HandleFirstWriteLockTransition( CEventListNode &node );
	DLL_CLASS_IMPORT void WaitForReadLockTransition( CEventListNode &node, uint32 nFlags );

	CInterlockedUInt m_Interlock;
	CInterlockedInt m_ReadlockAccounting;
	CEventLock m_WriteLockQueue;
	CEventList m_WaitingReadLocks;
	CEventListNode *volatile m_pWritingEvent;
	uint32 m_nWritingThreadLockCount;
	CInterlockedUInt m_nSecondaryFlags;

private:
	CEventRWLock( const CEventRWLock & ) = delete;
	CEventRWLock &operator=( const CEventRWLock & ) = delete;
};

template< bool bDebugEnabled >
class CEventRWLock_Debuggable : public CEventRWLock
{
};

//-----------------------------------------------------------------------------
// Read-write lock that also tracks the nodes that hold its read locks
//-----------------------------------------------------------------------------
template<>
class CEventRWLock_Debuggable< true > : public CEventRWLock
{
public:
	using CEventRWLock::DuplicateReadLock;
	DLL_CLASS_IMPORT void DuplicateReadLock( CEventListNode &node );

private:
#pragma pack( push, 4 )
	struct TrackedReadLock_t
	{
		TrackedReadLock_t *pNext;
		CEventListNode *pNode;
		CInterlockedUInt nCopies;
	};
#pragma pack( pop )

	typedef CTSFastPushQueue< TrackedReadLock_t, TrackedReadLock_t, TrackedReadLock_t, &TrackedReadLock_t::pNext > TrackedReadLocks_t;

	DLL_CLASS_IMPORT bool DoesHaveAnyReadLocks( CEventListNode &node );

	// Allocates a tracking node when pFree is null, frees pFree otherwise
	DLL_CLASS_IMPORT TrackedReadLock_t *AllocOrFreeReadLockMem( TrackedReadLock_t *pFree );

	TrackedReadLocks_t m_ActiveReadLocks;
};

//-----------------------------------------------------------------------------
// Job that runs on its own stack and can yield back to the thread that resumed it
//-----------------------------------------------------------------------------
class CFiber
{
public:
	enum SizeBuckets_t
	{
		STACKSIZE_NONE = 0,
		STACKSIZE_2KB,
		STACKSIZE_4KB,
		STACKSIZE_8KB,
		STACKSIZE_16KB,
		STACKSIZE_32KB,
		STACKSIZE_64KB,
		STACKSIZE_128KB,
	};

	DLL_CLASS_IMPORT CFiber();
	DLL_CLASS_IMPORT ~CFiber();

	DLL_CLASS_IMPORT static CFiber *AllocPooledFiber( SizeBuckets_t nSizeBucket, size_t nStackSize );

	uint32 AddRef() { return ++m_nRefCount; }
	uint32 Release()
	{
		uint32 nRefCount = --m_nRefCount;

		if ( nRefCount == 0 )
			ReachedZeroReferences();

		return nRefCount;
	}

	DLL_CLASS_IMPORT void InitStackContext( void *pStackMemory, size_t nStackMemorySize, bool bClearUnusedSpace, size_t nClearedMemoryInterval );
	DLL_CLASS_IMPORT void *DetachFromStackContext();
	DLL_CLASS_IMPORT size_t EstimateBytesUsed( size_t nGranularity ) const;

	DLL_CLASS_IMPORT bool IsReadyToResume();
	DLL_CLASS_IMPORT bool ResumeJob( void *pResumeVal );
	DLL_CLASS_IMPORT bool AbortJob( void *pResumeVal );

	// Returns to the thread that resumed the fiber, pfnResumeHint tells when it is ready to resume
	DLL_CLASS_IMPORT void *FiberYield( bool (*pfnResumeHint)( void * ), void *pResumeHintContext );

	DLL_CLASS_IMPORT static CFiber *GetCurrentFiber();
	DLL_CLASS_IMPORT static CFiber *SetCurrentFiber( CFiber *pFiber );

	DLL_CLASS_IMPORT static bool IsRunningInThreadMemory();
	DLL_CLASS_IMPORT static int64 RunInThreadMemory( int64 (*pfnFunc)( void * ), void *pContext );

	DLL_CLASS_IMPORT void WaitForJobCompletion( CEventListNode &node );

	CFiber *GetPrevFiberInSameThread() const { return m_pPrevFiberInSameThread; }
	StackContext_StartData_t *GetStackContextStartData() const { return m_pStackContextStartData; }
	const MemoryRange_t &GetStackMemoryRange() const { return m_StackMemoryRange; }
	ThreadId_t GetJobOriginatingThreadId() const { return m_hJobsOriginatingThreadId; }

protected:
	DLL_CLASS_IMPORT void ConstructThreadFiber( size_t nStackSize );
	DLL_CLASS_IMPORT void SetThreadFiberMemoryRange( size_t nStackSize );
	DLL_CLASS_IMPORT void ClearUnusedSpaceToFFEEFFEE( void *pStackPointer );

private:
	DLL_CLASS_IMPORT void ReachedZeroReferences();
	// Copies nJobContextSize bytes of pJobContext to the top of the stack when it is not zero
	DLL_CLASS_IMPORT void *InternalSetFiberJob( void (*pfnJob)( void * ), void *pJobContext, size_t nJobContextSize, bool bUnknown );

public:
	CFiber *volatile m_pNext;
	CEventListNode m_EventListNode;

private:
	JmpBuf_AlternateStack_t *volatile m_pJmpBuf_IN;
	JmpBuf_AlternateStack_t *volatile m_pJmpBuf_OUT;
	StackContext_StartData_t *m_pStackContextStartData;
	void (*volatile m_pCurrentJobFunc)( void * );
	void *volatile m_pCurrentJobContext;
	bool (*m_pResumeHint)( void * );
	void *m_pResumeHintContext;
	CEventList m_NotifyOnComplete;
	MemoryRange_t m_StackMemoryRange;
	CInterlockedUInt m_nFlags;
	CInterlockedUInt m_nRefCount;
	CFiber *m_pPrevFiberInSameThread;
	ThreadId_t m_hJobsOriginatingThreadId;
	size_t m_nClearedMemoryInterval;
	void *unk110;

	CFiber( const CFiber & ) = delete;
	CFiber &operator=( const CFiber & ) = delete;
};

//-----------------------------------------------------------------------------
// Turns the current thread into a fiber for the lifetime of the object
//-----------------------------------------------------------------------------
class CThreadFiber
{
public:
	DLL_CLASS_IMPORT CThreadFiber( size_t nStackSize = 0 );
	~CThreadFiber()
	{
		if ( m_pFiber )
			m_pFiber->Release();
	}

private:
	CFiber *m_pFiber;

	CThreadFiber( const CThreadFiber & ) = delete;
	CThreadFiber &operator=( const CThreadFiber & ) = delete;
};

#endif // FIBERTOOLS_H
