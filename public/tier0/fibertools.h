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
	CEventListNode() : m_pNext( nullptr ), m_pUserData( nullptr ) {}

	DLL_CLASS_IMPORT static CEventListNode *AllocTempNode();
	DLL_CLASS_IMPORT static void FreeTempNode( CEventListNode *pNode );

	CEventListNode *m_pNext;

	// Free for the owner of a node that it queues itself, tier0 only clears it
	// (CMovieRecorder of engine2 keeps the pixels of a screenshot there)
	void *m_pUserData;
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
	// Queue of the nodes waiting for the lock, its pop slot holds EVENTLOCK_EMPTY_BUT_OWNED
	// while the lock is owned and nobody waits, and null while the lock is free
	class CLockList : public CTSFastPushQueue_Base< CEventListNode, CEventListNode, CEventListNode, &CEventListNode::m_pNext >
	{
	public:
		enum
		{
			EVENTLOCK_EMPTY_BUT_OWNED = 2,
		};

		CLockList( bool bLocked )
		{
			if ( bLocked )
				m_pPop = GetEmptyButOwned();
		}

		// Takes the lock when it is free
		bool TryLock()
		{
			return ThreadInterlockedCompareExchangePointer( PopSlot(), GetEmptyButOwned(), nullptr ) == nullptr;
		}

		// Queues the node behind the owner; false when the lock was released meanwhile, so
		// the node is the first one in the queue and owns the lock
		bool InternalPush( CEventListNode &node )
		{
			CEventListNode *pPrev = static_cast< CEventListNode * >( ThreadInterlockedExchangePointer( ( void *volatile * )&m_pPush, &node ) );
			void *pPrevNext = ThreadInterlockedExchangePointer( ( void *volatile * )NextSlot( pPrev ), &node );

			return pPrev != GetLoopbackAddress() || pPrevNext != nullptr;
		}

		// Removes the first node, which owns the lock from then on; frees the lock and
		// returns null when nobody waits
		CEventListNode *InternalPop()
		{
			CEventListNode *pFirst = static_cast< CEventListNode * >( ThreadInterlockedCompareExchangePointer( PopSlot(), nullptr, GetEmptyButOwned() ) );

			if ( pFirst == GetEmptyButOwned() )
				return nullptr;

			if ( ThreadInterlockedCompareExchangePointer( ( void *volatile * )&m_pPush, GetLoopbackAddress(), pFirst ) == pFirst )
			{
				ThreadInterlockedCompareExchangePointer( PopSlot(), GetEmptyButOwned(), pFirst );
			}
			else
			{
				while ( !*NextSlot( pFirst ) )
					;

				m_pPop = *NextSlot( pFirst );
			}

			pFirst->m_pNext = nullptr;

			return pFirst;
		}

	private:
		void *volatile *PopSlot() { return ( void *volatile * )&m_pPop; }
		static CEventListNode *GetEmptyButOwned() { return reinterpret_cast< CEventListNode * >( EVENTLOCK_EMPTY_BUT_OWNED ); }
	};

	// bLocked makes the lock start owned, with no owner to hand it back
	CEventLock( bool bLocked = false ) : m_LockList( bLocked ) {}

	bool TryLock() { return m_LockList.TryLock(); }

	// Waits in node until the owner hands the lock over, yielding when called on a fiber
	inline void Lock( CEventListNode &node );

	// Waits in the node of the current fiber or in a temporary node of the thread
	inline void Lock();

	// Hands the lock to the first waiting node or frees it
	void Unlock()
	{
		if ( CEventListNode *pNext = m_LockList.InternalPop() )
			pNext->m_Event.Set();
	}

private:
	CLockList m_LockList;
};

//-----------------------------------------------------------------------------
// Read-write lock that queues the waiting threads and fibers
//-----------------------------------------------------------------------------
class CEventRWLock
{
public:
	// The write lock queue stays owned while nobody waits, the writers that find the write
	// lock taken wait in it until UnlockWrite hands it over
	CEventRWLock() : m_WriteLockQueue( true ), m_pWritingEvent( nullptr ), m_nWritingThreadLockCount( 0 ) {}

	// Takes a read lock, waiting for the writer when there is one
	void LockForRead( CEventListNode &node )
	{
		uint32 nInterlock = ++m_Interlock;

		if ( nInterlock & WRITE_LOCK_MASK )
			WaitForReadLockTransition( node, nInterlock );
	}

	inline void LockForRead();

	void UnlockRead( const char *pFileName = nullptr, int nLine = -1 )
	{
		uint32 nInterlock = --m_Interlock;

		if ( !( nInterlock & ( WRITE_LOCK_MASK | READ_LOCK_ACCOUNTING_BIT ) ) )
			return;

		if ( nInterlock & READ_LOCK_ACCOUNTING_BIT )
		{
			// The writer waits for the last of the read locks that it counts
			if ( ++m_ReadlockAccounting == 0 )
				m_pWritingEvent->m_Event.Set();
		}
		else if ( m_nSecondaryFlags & SF_READ_LOCKS_ACTIVE_WHILE_HOLDING_WRITE_LOCK )
		{
			UnlockWrite( pFileName, nLine );
		}
	}

	// Takes the write lock, again when node already holds it
	void LockForWrite( CEventListNode &node )
	{
		if ( m_pWritingEvent == &node )
		{
			m_nWritingThreadLockCount++;
			return;
		}

		uint32 nInterlock = m_Interlock.AtomicAdd( WRITE_LOCK_INCREMENT );

		if ( nInterlock & WRITE_LOCK_MASK )
			m_WriteLockQueue.Lock();
		else if ( nInterlock & READ_LOCK_MASK )
			HandleFirstWriteLockTransition( node );

		m_pWritingEvent = &node;
		m_nWritingThreadLockCount = 1;
	}

	// Takes the write lock with the node of the current fiber or with a temporary node
	inline void LockForWrite( const char *pFileName = nullptr, int nLine = -1 );

	// Releases one level of the write lock, then wakes the waiting readers or the next writer
	void UnlockWrite( const char *pFileName = nullptr, int nLine = -1 )
	{
		if ( --m_nWritingThreadLockCount != 0 )
			return;

		m_pWritingEvent = nullptr;

		uint32 nFlags;

		do
		{
			nFlags = m_nSecondaryFlags;
		}
		while ( !m_nSecondaryFlags.AssignIf( nFlags, nFlags & ~SF_READ_LOCKS_ACTIVE_WHILE_HOLDING_WRITE_LOCK ) );

		if ( ( m_Interlock.AtomicAdd( -WRITE_LOCK_INCREMENT ) & WRITE_LOCK_MASK ) != WRITE_LOCK_INCREMENT )
		{
			m_WriteLockQueue.Unlock();
			return;
		}

		for ( CEventListNode *pReader = m_WaitingReadLocks.Detach(); pReader; )
		{
			CEventListNode *pNext = pReader->m_pNext;

			pReader->m_pNext = nullptr;
			pReader->m_Event.Set();

			pReader = pNext;
		}
	}

	DLL_CLASS_IMPORT void DuplicateReadLock();

	// Switches the held lock, RWLTR_STATE_INVALIDATED means another writer had it in between
	DLL_CLASS_IMPORT RWLockTranstionResult_t UnlockRead_LockForWrite( CEventListNode &node );
	DLL_CLASS_IMPORT RWLockTranstionResult_t UnlockWrite_LockForRead( CEventListNode &node );

	const CEventListNode *GetWriteLockEvent() const { return m_pWritingEvent; }

protected:
	enum InterlockBits_t
	{
		READ_LOCK_MASK = 0x0000FFFF,
		WRITE_LOCK_INCREMENT = 0x00010000,
		WRITE_LOCK_MASK = 0x7FFF0000,
		READ_LOCK_ACCOUNTING_BIT = 0x80000000,
	};

	enum SecondaryFlags_t
	{
		SF_READ_LOCKS_ACTIVE_WHILE_HOLDING_WRITE_LOCK = 1 << 0,
		SF_READ_LOCK_ACCOUNTING_NOTIFY_WRITER_WHEN_OFF = 1 << 1,
	};

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
	void *FiberYield() { return FiberYield( nullptr, nullptr ); }

	// Sets the job that ResumeJob starts, nJobContextSize bytes of pJobContext are copied
	// to the top of the fiber stack when it is not zero; returns the context the job gets
	void *SetFiberJob( void (*pfnJob)( void * ), void *pJobContext = nullptr, size_t nJobContextSize = 0 ) { return InternalSetFiberJob( pfnJob, pJobContext, nJobContextSize, false ); }

	// Same, the job also gets the value of the ResumeJob that starts it
	void *SetFiberJob( void (*pfnJob)( void *, void * ), void *pJobContext = nullptr, size_t nJobContextSize = 0 ) { return InternalSetFiberJob( reinterpret_cast< void (*)( void * ) >( pfnJob ), pJobContext, nJobContextSize, true ); }
	void *GetFiberJobContext() const { return m_pCurrentJobContext; }
	bool HasAJob() const { return m_pCurrentJobFunc != nullptr; }

	bool IsMemoryInFiberRange( const void *pMemory ) const { return pMemory >= m_StackMemoryRange.pLow && pMemory < m_StackMemoryRange.pHigh; }

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
	// Copies nJobContextSize bytes of pJobContext to the top of the stack when it is not zero;
	// with bJobTakesResumeValue the job is called as void ( void *pContext, void *pResumeVal ),
	// flag 0x80 of m_nFlags
	DLL_CLASS_IMPORT void *InternalSetFiberJob( void (*pfnJob)( void * ), void *pJobContext, size_t nJobContextSize, bool bJobTakesResumeValue );

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


inline void CEventLock::Lock( CEventListNode &node )
{
	node.m_pNext = nullptr;

	if ( m_LockList.TryLock() )
		return;

	if ( !m_LockList.InternalPush( node ) )
	{
		// The lock was released meanwhile, take the node out of the queue again
		m_LockList.InternalPop();
		return;
	}

	if ( CFiber *pFiber = CFiber::GetCurrentFiber() )
		pFiber->FiberYield( []( void *pEvent ) { return static_cast< CThreadEvent * >( pEvent )->Check(); }, &node.m_Event );
	else
		node.m_Event.Wait();

	node.m_Event.Reset();
}

inline void CEventLock::Lock()
{
	if ( CFiber *pFiber = CFiber::GetCurrentFiber() )
	{
		Lock( pFiber->m_EventListNode );
		return;
	}

	CEventListNode *pNode = CEventListNode::AllocTempNode();
	Lock( *pNode );
	CEventListNode::FreeTempNode( pNode );
}

inline void CEventRWLock::LockForRead()
{
	if ( CFiber *pFiber = CFiber::GetCurrentFiber() )
	{
		LockForRead( pFiber->m_EventListNode );
		return;
	}

	CEventListNode *pNode = CEventListNode::AllocTempNode();
	LockForRead( *pNode );
	CEventListNode::FreeTempNode( pNode );
}

inline void CEventRWLock::LockForWrite( const char *pFileName, int nLine )
{
	if ( CFiber *pFiber = CFiber::GetCurrentFiber() )
	{
		LockForWrite( pFiber->m_EventListNode );
		return;
	}

	CEventListNode *pNode = CEventListNode::AllocTempNode();
	LockForWrite( *pNode );
	CEventListNode::FreeTempNode( pNode );
}

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

//-----------------------------------------------------------------------------
// Thread pool that runs the fiber jobs of the job pool
//-----------------------------------------------------------------------------
abstract_class IFiberThreadWorkerPool
{
public:
	// The job pool has a fiber job to run
	virtual void FiberJobAvailable() = 0;

	// The default worker of tier0 sets its quit flag and wakes up; the name is ours
	virtual void Quit() = 0;
};

//-----------------------------------------------------------------------------
// Fibers waiting to run, shared by the worker thread pools
//-----------------------------------------------------------------------------
abstract_class IFiberJobPool
{
public:
	// The pool takes the reference of the caller with bPassReference, otherwise it adds one
	virtual void AddFiberToJobPool( CFiber *pFiber, bool bPassReference ) = 0;

	// Takes the next waiting fiber that is ready to resume, null when there is none; with
	// pbSkippedFibers, it is set when a fiber that is not ready yet was passed over
	virtual CFiber *GetWaitingFiberJob( bool *pbSkippedFibers ) = 0;
	virtual bool RemoveFiberJob( volatile CFiber *pFiber ) = 0;

	virtual void RegisterWorkerThreadPool( IFiberThreadWorkerPool *pPool ) = 0;
	virtual void UnregisterWorkerThreadPool( IFiberThreadWorkerPool *pPool ) = 0;

	// Under the write lock of the pool, has every registered worker pool quit and waits for the
	// default worker thread, then aborts the waiting fibers with a resume value of -1 and
	// releases them; newer than the s&box build, the name is ours
	virtual void Shutdown() = 0;
};

PLATFORM_INTERFACE IFiberJobPool *g_pFiberJobPool;

#endif // FIBERTOOLS_H
