//========== Copyright © 2005, Valve Corporation, All rights reserved. ========
//
// Purpose: A collection of utility classes to simplify thread handling, and
//			as much as possible contain portability problems. Here avoiding 
//			including windows.h.
//
//=============================================================================

#ifndef THREADTOOLS_H
#define THREADTOOLS_H

#include <limits.h>
#include <shared_mutex>

#include "basetypes.h"
#include "tier0/platform.h"
#include "tier0/dbg.h"

#ifdef POSIX
#include <pthread.h>
#include <errno.h>
#define WAIT_OBJECT_0 0
#define WAIT_TIMEOUT 0x00000102
#define WAIT_FAILED -1
#define THREAD_PRIORITY_HIGHEST 2
#endif

#ifdef OSX
// Add some missing defines
#define PTHREAD_MUTEX_TIMED_NP         PTHREAD_MUTEX_NORMAL
#define PTHREAD_MUTEX_RECURSIVE_NP     PTHREAD_MUTEX_RECURSIVE
#define PTHREAD_MUTEX_ERRORCHECK_NP    PTHREAD_MUTEX_ERRORCHECK
#define PTHREAD_MUTEX_ADAPTIVE_NP      3
#endif


#if defined( PLATFORM_WINDOWS )
#pragma once
#pragma warning(push)
#pragma warning(disable:4251)
#endif

#ifdef COMPILER_MSVC64
#include <intrin.h>
#endif

// #define THREAD_PROFILER 1

#define THREAD_MUTEX_TRACING_SUPPORTED
#if defined(PLATFORM_WINDOWS) && defined(_DEBUG)
#define THREAD_MUTEX_TRACING_ENABLED
#endif

#ifdef PLATFORM_WINDOWS
typedef void *HANDLE;
#endif

#ifdef _X360
#define MAX_THREADS_SUPPORTED 16
#else
#define MAX_THREADS_SUPPORTED 32
#endif



//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------

const unsigned TT_INFINITE = 0xffffffff;

#ifdef _WIN32
	typedef uint32 ThreadId_t;
#else
	typedef uint64 ThreadId_t;
#endif

//-----------------------------------------------------------------------------
//
// Simple thread creation. Differs from VCR mode/CreateThread/_beginthreadex
// in that it accepts a standard C function rather than compiler specific one.
//
//-----------------------------------------------------------------------------
FORWARD_DECLARE_HANDLE( ThreadHandle_t );
typedef uintp (*ThreadFunc_t)( void *pParam );

PLATFORM_INTERFACE ThreadHandle_t CreateSimpleThread( ThreadFunc_t, void *pParam, unsigned stackSize = 0 );
PLATFORM_INTERFACE bool ReleaseThreadHandle( ThreadHandle_t );

enum SpecialThreadID_t : uint32;
PLATFORM_INTERFACE uintp Plat_HookSpecialThread( SpecialThreadID_t specialThreadID, ThreadFunc_t pfnCallback, void *pParam, size_t nReplacementThreadStackSize, const char *pszReplacementThreadName, size_t nDuplicateParamSize );


//-----------------------------------------------------------------------------
// Processes
//-----------------------------------------------------------------------------
FORWARD_DECLARE_HANDLE( ProcessHandle_t );
enum ThreadProcessPriority_t
{
	THREAD_PROCESS_PRIORITY_IDLE = 0,
	THREAD_PROCESS_PRIORITY_LOW = 1,
	THREAD_PROCESS_PRIORITY_NORMAL = 2,
	THREAD_PROCESS_PRIORITY_HIGH = 3,
	THREAD_PROCESS_PRIORITY_REALTIME = 4,
};

PLATFORM_INTERFACE int CreateSimpleProcess( const char *pCommandLine, uint32 nFlags, ProcessHandle_t *pHandle );
PLATFORM_INTERFACE int ThreadOpenProcess( uint32 nProcessId, uint32 nAccessFlags, ProcessHandle_t *pHandle );
#ifdef PLATFORM_POSIX
PLATFORM_INTERFACE ProcessHandle_t ThreadWrapProcessId( uint32 nProcessId, uint32 nAccessFlags );
#endif
PLATFORM_INTERFACE bool ThreadCloseProcess( ProcessHandle_t hProcess );
PLATFORM_INTERFACE ProcessHandle_t ThreadGetCurrentProcessHandle();
PLATFORM_INTERFACE uint32 ThreadGetCurrentProcessId();
PLATFORM_INTERFACE uint32 ThreadGetProcessIdFromHandle( ProcessHandle_t hProcess );
PLATFORM_INTERFACE bool ThreadIsProcessIdActive( uint32 nProcessId );
PLATFORM_INTERFACE bool ThreadIsProcessActive( ProcessHandle_t hProcess, int32 *pExitCode );
PLATFORM_INTERFACE bool ThreadWaitForProcessExit( ProcessHandle_t hProcess, uint32 nMillis, int32 *pExitCode );
PLATFORM_INTERFACE bool ThreadGetProcessExitCode( ProcessHandle_t hProcess, int32 *pExitCode );
PLATFORM_INTERFACE bool ThreadTerminateProcessCode( ProcessHandle_t hProcess, int32 nExitCode );
PLATFORM_INTERFACE ThreadProcessPriority_t ThreadGetProcessPriority( ProcessHandle_t hProcess );
PLATFORM_INTERFACE bool ThreadSetProcessPriority( ProcessHandle_t hProcess, ThreadProcessPriority_t ePriority );
// Reads up to nSize bytes at pAddress of the process into pBuffer, returns the number of bytes read
PLATFORM_INTERFACE int ThreadReadProcessMemory( ProcessHandle_t hProcess, const void *pAddress, void *pBuffer, int nSize );
PLATFORM_INTERFACE uint32 ThreadShellExecute( const char *pchApplicationName, const char *pchCommandLine, const char *pchCurrentDirectory );
PLATFORM_INTERFACE uint32 ThreadShellExecuteEx( const char *pchVerb, const char *pchFile, const char *pchParameters, const char *pchCurrentDirectory );
PLATFORM_INTERFACE void ThreadStartFuzzer();


//-----------------------------------------------------------------------------
// Waiting on several objects
//-----------------------------------------------------------------------------
class CThreadMultiWaitObject;
class CThreadMultiWaitEvent;
#ifdef PLATFORM_WINDOWS
typedef void *ThreadMultiWaitItem_t;
#else
#include <poll.h>
typedef pollfd ThreadMultiWaitItem_t;
#endif

struct ThreadMultiWaitExtra_t
{
	uint32 nFlags; // CThreadMultiWaitObject type: 1 for a semaphore, 2 for an auto-reset event, 4 for a manual-reset event
#ifdef PLATFORM_OSX
	CThreadMultiWaitObject *pObject; // Auto-reset events clear their signalled state under its mutex
#endif
};

struct ThreadMultiWaitObjectSet_t
{
	ThreadMultiWaitItem_t *m_pItems;
	ThreadMultiWaitExtra_t *m_pExtra;
	int m_nItems;
#if defined( PLATFORM_POSIX ) && !defined( PLATFORM_OSX )
	int m_nEpoll;
#endif
};

PLATFORM_INTERFACE ThreadMultiWaitObjectSet_t *ThreadMultiWaitObjectSetCreate( CThreadMultiWaitObject **ppObjects, int nObjects );

// A set of the native wait items, with the flags of each one in pExtra
PLATFORM_OVERLOAD ThreadMultiWaitObjectSet_t *ThreadMultiWaitObjectSetCreate( const ThreadMultiWaitItem_t *pItems, const ThreadMultiWaitExtra_t *pExtra, int nItems );
PLATFORM_INTERFACE void ThreadMultiWaitObjectSetDestroy( ThreadMultiWaitObjectSet_t *pSet );
PLATFORM_INTERFACE int ThreadMultiWaitObjectSetGetCount( ThreadMultiWaitObjectSet_t *pSet );
// Waits on pSet, or on the nItems of pItems and pExtra when pSet is NULL; pSignalled receives a bit per signalled object, all of them with bWaitAll
PLATFORM_INTERFACE int ThreadMultiWait( ThreadMultiWaitObjectSet_t *pSet, ThreadMultiWaitItem_t *pItems, ThreadMultiWaitExtra_t *pExtra, int nItems, bool bWaitAll, uint64 *pSignalled, uint32 nMillis );


//-----------------------------------------------------------------------------

PLATFORM_OVERLOAD void ThreadAtomicNotifyOne( const uint32 *addr );
PLATFORM_OVERLOAD void ThreadAtomicNotifyN( const uint32 *addr, uint32 n );
PLATFORM_OVERLOAD void ThreadAtomicNotifyAll( const uint32 *addr );
PLATFORM_OVERLOAD void ThreadAtomicWait( volatile uint32 *addr, uint32 value );

PLATFORM_INTERFACE void ThreadSleep(unsigned duration = 0);
PLATFORM_INTERFACE void ThreadNanoSleep(unsigned ns);
PLATFORM_INTERFACE void ThreadSpin( unsigned int nCycles );
PLATFORM_INTERFACE void ThreadYield();
PLATFORM_INTERFACE ThreadId_t ThreadGetCurrentId();
PLATFORM_INTERFACE ThreadHandle_t ThreadGetCurrentHandle();
PLATFORM_INTERFACE int ThreadGetPriority( ThreadHandle_t hThread = NULL );
PLATFORM_INTERFACE bool ThreadSetPriority( ThreadHandle_t hThread, int priority );
inline		 bool ThreadSetPriority( int priority ) { return ThreadSetPriority( NULL, priority ); }
PLATFORM_INTERFACE uint16 GetPerfSortedLogicalProcessors( uint16 *pSortIndices, uint16 nSortIndices );
PLATFORM_INTERFACE void ThreadSetPerfSetAffinity( uint8 nPerfSet );
PLATFORM_INTERFACE void Plat_EnableHighQoSExecutionSpeed();
PLATFORM_INTERFACE bool ThreadInMainThread();
PLATFORM_INTERFACE void DeclareCurrentThreadIsMainThread();
PLATFORM_INTERFACE ThreadId_t ThreadGetMainThreadId();
PLATFORM_INTERFACE bool ThreadInSecondaryMainThread();
PLATFORM_INTERFACE void DeclareCurrentThreadIsSecondaryMainThread();

// True in the main thread or in the secondary main thread
PLATFORM_INTERFACE bool ThreadInPrimaryOrSecondaryMainThread();
PLATFORM_INTERFACE void ClearSecondaryMainThread();

// NOTE: ThreadedLoadLibraryFunc_t needs to return the sleep time in milliseconds or TT_INFINITE
typedef int (*ThreadedLoadLibraryFunc_t)(); 
PLATFORM_INTERFACE void SetThreadedLoadLibraryFunc( ThreadedLoadLibraryFunc_t func );
PLATFORM_INTERFACE ThreadedLoadLibraryFunc_t GetThreadedLoadLibraryFunc();

#if defined( PLATFORM_WINDOWS )
DLL_IMPORT unsigned long STDCALL GetCurrentThreadId();
#define ThreadGetCurrentId GetCurrentThreadId
#endif

inline void ThreadPause()
{
#if defined( COMPILER_CLANG ) || defined( COMPILER_GCC )
	__asm __volatile( "pause" );
#elif defined ( COMPILER_MSVC64 )
	_mm_pause();
#elif defined( COMPILER_MSVC32 )
	__asm pause;
#elif defined( COMPILER_MSVCX360 )
	YieldProcessor(); 
	__asm { or r0,r0,r0 } 
	YieldProcessor(); 
	__asm { or r1,r1,r1 } 
#else
#error "implement me"
#endif
}

PLATFORM_INTERFACE bool ThreadJoin( ThreadHandle_t, unsigned timeout = TT_INFINITE );

PLATFORM_INTERFACE void ThreadSetDebugNameS2( ThreadHandle_t hThread, const char *pszName );
inline		 void ThreadSetDebugName( const char *pszName ) { ThreadSetDebugNameS2( NULL, pszName ); }
PLATFORM_INTERFACE bool ThreadGetDebugNameForThreadHandle( ThreadHandle_t hThread, char *pszNameOut, size_t nNameOutSize );
PLATFORM_INTERFACE bool ThreadGetDebugNameForThreadId( ThreadId_t nThreadId, char *pszNameOut, size_t nNameOutSize );

typedef volatile int32 ThreadInitOnce_t;
PLATFORM_INTERFACE bool ThreadInitOnceCall_ShouldRun( ThreadInitOnce_t *pMarker );
PLATFORM_INTERFACE void ThreadInitOnceCall_Finish( ThreadInitOnce_t *pMarker );

PLATFORM_INTERFACE void ThreadSetAffinity( ThreadHandle_t hThread, int nAffinityMask );


//-----------------------------------------------------------------------------
//
// Interlock methods. These perform very fast atomic thread
// safe operations. These are especially relevant in a multi-core setting.
//
//-----------------------------------------------------------------------------

#ifdef PLATFORM_WINDOWS
#define NOINLINE
#elif defined(POSIX)
#define NOINLINE __attribute__ ((noinline))
#endif

#ifndef _X360
#define ThreadMemoryBarrier() ((void)0)
#else
#define ThreadMemoryBarrier() __lwsync()
#endif

#if defined( _LINUX ) || defined( _OSX )
#define USE_INTRINSIC_INTERLOCKED
// linux implementation
inline int32 ThreadInterlockedIncrement( int32 volatile *p )
{
	Assert( (size_t)p % 4 == 0 );
	return __sync_fetch_and_add( p, 1 ) + 1;
}

inline int32 ThreadInterlockedDecrement( int32 volatile *p )
{
	Assert( (size_t)p % 4 == 0 ); 
	return __sync_fetch_and_add( p, -1 ) - 1;
}

inline int32 ThreadInterlockedExchange( int32 volatile *p, int32 value )
{
	Assert( (size_t)p % 4 == 0 );
	int32 nRet;

	// Note: The LOCK instruction prefix is assumed on the XCHG instruction and GCC gets very confused on the Mac when we use it.
	__asm __volatile(
		"xchgl %2,(%1)"
		: "=r" (nRet)
		: "r" (p), "0" (value)
		: "memory");
	return nRet;
}

inline int32 ThreadInterlockedExchangeAdd( int32 volatile *p, int32 value )
{
	Assert( (size_t)p % 4 == 0 ); 
	return __sync_fetch_and_add( p, value );
}
inline int32 ThreadInterlockedCompareExchange( int32 volatile *p, int32 value, int32 comperand )
{
	Assert( (size_t)p % 4 == 0 ); 
	return __sync_val_compare_and_swap( p, comperand, value );
}


inline bool ThreadInterlockedAssignIf( int32 volatile *p, int32 value, int32 comperand )
{
	Assert( (size_t)p % 4 == 0 );
	return __sync_bool_compare_and_swap( p, comperand, value );
}

#elif ( defined( COMPILER_MSVC ) && ( _MSC_VER >= 1310 ) )
// windows 32 implemnetation using compiler intrinsics
#define USE_INTRINSIC_INTERLOCKED

extern "C"
{
	long __cdecl _InterlockedIncrement(volatile long*);
	long __cdecl _InterlockedDecrement(volatile long*);
	long __cdecl _InterlockedExchange(volatile long*, long);
	long __cdecl _InterlockedExchangeAdd(volatile long*, long);
	long __cdecl _InterlockedCompareExchange(volatile long*, long, long);
}

#pragma intrinsic( _InterlockedCompareExchange )
#pragma intrinsic( _InterlockedDecrement )
#pragma intrinsic( _InterlockedExchange )
#pragma intrinsic( _InterlockedExchangeAdd ) 
#pragma intrinsic( _InterlockedIncrement )

inline int32 ThreadInterlockedIncrement( int32 volatile *p )										{ Assert( (size_t)p % 4 == 0 ); return _InterlockedIncrement( (volatile long*)p ); }
inline int32 ThreadInterlockedDecrement( int32 volatile *p )										{ Assert( (size_t)p % 4 == 0 ); return _InterlockedDecrement( (volatile long*)p ); }
inline int32 ThreadInterlockedExchange( int32 volatile *p, int32 value )							{ Assert( (size_t)p % 4 == 0 ); return _InterlockedExchange( (volatile long*)p, value ); }
inline int32 ThreadInterlockedExchangeAdd( int32 volatile *p, int32 value )							{ Assert( (size_t)p % 4 == 0 ); return _InterlockedExchangeAdd( (volatile long*)p, value ); }
inline int32 ThreadInterlockedCompareExchange( int32 volatile *p, int32 value, int32 comperand )	{ Assert( (size_t)p % 4 == 0 ); return _InterlockedCompareExchange( (volatile long*)p, value, comperand ); }
inline bool ThreadInterlockedAssignIf( int32 volatile *p, int32 value, int32 comperand )			{ Assert( (size_t)p % 4 == 0 ); return ( _InterlockedCompareExchange( (volatile long*)p, value, comperand ) == comperand ); }
#else
// non 32-bit windows and 360 implementation
PLATFORM_INTERFACE int32 ThreadInterlockedIncrement( int32 volatile * ) NOINLINE;
PLATFORM_INTERFACE int32 ThreadInterlockedDecrement( int32 volatile * ) NOINLINE;
PLATFORM_INTERFACE int32 ThreadInterlockedExchange( int32 volatile *, int32 value ) NOINLINE;
PLATFORM_INTERFACE int32 ThreadInterlockedExchangeAdd( int32 volatile *, int32 value ) NOINLINE;
PLATFORM_INTERFACE int32 ThreadInterlockedCompareExchange( int32 volatile *, int32 value, int32 comperand ) NOINLINE;
PLATFORM_INTERFACE bool ThreadInterlockedAssignIf( int32 volatile *, int32 value, int32 comperand ) NOINLINE;
#endif


// tier0 exports none of the 64-bit and pointer interlocked functions, so they are intrinsics
#if defined( USE_INTRINSIC_INTERLOCKED ) && defined( PLATFORM_64BITS )
#ifdef COMPILER_MSVC
inline int64 ThreadInterlockedCompareExchange64( int64 volatile *p, int64 value, int64 comperand )	{ Assert( (size_t)p % 8 == 0 ); return _InterlockedCompareExchange64( p, value, comperand ); }
inline int64 ThreadInterlockedExchange64( int64 volatile *p, int64 value )							{ Assert( (size_t)p % 8 == 0 ); return _InterlockedExchange64( p, value ); }
inline int64 ThreadInterlockedIncrement64( int64 volatile *p )										{ Assert( (size_t)p % 8 == 0 ); return _InterlockedIncrement64( p ); }
inline int64 ThreadInterlockedDecrement64( int64 volatile *p )										{ Assert( (size_t)p % 8 == 0 ); return _InterlockedDecrement64( p ); }
inline int64 ThreadInterlockedExchangeAdd64( int64 volatile *p, int64 value )						{ Assert( (size_t)p % 8 == 0 ); return _InterlockedExchangeAdd64( p, value ); }
#else
inline int64 ThreadInterlockedCompareExchange64( int64 volatile *p, int64 value, int64 comperand )	{ Assert( (size_t)p % 8 == 0 ); return __sync_val_compare_and_swap( p, comperand, value ); }
inline int64 ThreadInterlockedExchange64( int64 volatile *p, int64 value )							{ Assert( (size_t)p % 8 == 0 ); return __atomic_exchange_n( p, value, __ATOMIC_SEQ_CST ); }
inline int64 ThreadInterlockedIncrement64( int64 volatile *p )										{ Assert( (size_t)p % 8 == 0 ); return __sync_add_and_fetch( p, 1 ); }
inline int64 ThreadInterlockedDecrement64( int64 volatile *p )										{ Assert( (size_t)p % 8 == 0 ); return __sync_sub_and_fetch( p, 1 ); }
inline int64 ThreadInterlockedExchangeAdd64( int64 volatile *p, int64 value )						{ Assert( (size_t)p % 8 == 0 ); return __sync_fetch_and_add( p, value ); }
#endif
inline bool ThreadInterlockedAssignIf64( volatile int64 *pDest, int64 value, int64 comperand )		{ return ThreadInterlockedCompareExchange64( pDest, value, comperand ) == comperand; }

inline void *ThreadInterlockedExchangePointer( void * volatile *p, void *value )							{ return (void *)ThreadInterlockedExchange64( reinterpret_cast<int64 volatile *>(p), reinterpret_cast<int64>(value) ); }
inline void *ThreadInterlockedCompareExchangePointer( void * volatile *p, void *value, void *comperand )	{ return (void *)ThreadInterlockedCompareExchange64( reinterpret_cast<int64 volatile *>(p), reinterpret_cast<int64>(value), reinterpret_cast<int64>(comperand) ); }
inline bool ThreadInterlockedAssignPointerIf( void * volatile *p, void *value, void *comperand )			{ return ThreadInterlockedAssignIf64( reinterpret_cast<int64 volatile *>(p), reinterpret_cast<int64>(value), reinterpret_cast<int64>(comperand) ); }
#else
PLATFORM_INTERFACE int64 ThreadInterlockedCompareExchange64( int64 volatile *, int64 value, int64 comperand ) NOINLINE;
PLATFORM_INTERFACE bool ThreadInterlockedAssignIf64( volatile int64 *pDest, int64 value, int64 comperand ) NOINLINE;

PLATFORM_INTERFACE int64 ThreadInterlockedExchange64( int64 volatile *, int64 value ) NOINLINE;
PLATFORM_INTERFACE int64 ThreadInterlockedIncrement64( int64 volatile * ) NOINLINE;
PLATFORM_INTERFACE int64 ThreadInterlockedDecrement64( int64 volatile * ) NOINLINE;
PLATFORM_INTERFACE int64 ThreadInterlockedExchangeAdd64( int64 volatile *, int64 value ) NOINLINE;
#endif

#if defined( USE_INTRINSIC_INTERLOCKED ) && !defined( PLATFORM_64BITS )
#define TIPTR()
inline void *ThreadInterlockedExchangePointer( void * volatile *p, void *value )							{ return (void *)( ( intp )ThreadInterlockedExchange( reinterpret_cast<intp volatile *>(p), reinterpret_cast<intp>(value) ) ); }
inline void *ThreadInterlockedCompareExchangePointer( void * volatile *p, void *value, void *comperand )	{ return (void *)( ( intp )ThreadInterlockedCompareExchange( reinterpret_cast<intp volatile *>(p), reinterpret_cast<intp>(value), reinterpret_cast<intp>(comperand) ) ); }
inline bool ThreadInterlockedAssignPointerIf( void * volatile *p, void *value, void *comperand )			{ return ( ThreadInterlockedCompareExchange( reinterpret_cast<intp volatile *>(p), reinterpret_cast<intp>(value), reinterpret_cast<intp>(comperand) ) == reinterpret_cast<intp>(comperand) ); }
#elif !defined( USE_INTRINSIC_INTERLOCKED )
PLATFORM_INTERFACE void *ThreadInterlockedExchangePointer( void * volatile *, void *value ) NOINLINE;
PLATFORM_INTERFACE void *ThreadInterlockedCompareExchangePointer( void * volatile *, void *value, void *comperand ) NOINLINE;
PLATFORM_INTERFACE bool ThreadInterlockedAssignPointerIf( void * volatile *, void *value, void *comperand ) NOINLINE;
#endif


inline unsigned ThreadInterlockedExchangeSubtract( int32 volatile *p, int32 value )	{ return ThreadInterlockedExchangeAdd( (int32 volatile *)p, -value ); }

inline void const *ThreadInterlockedExchangePointerToConst( void const * volatile *p, void const *value )							{ return ThreadInterlockedExchangePointer( const_cast < void * volatile * > ( p ), const_cast < void * > ( value ) );  }
inline void const *ThreadInterlockedCompareExchangePointerToConst( void const * volatile *p, void const *value, void const *comperand )	{ return ThreadInterlockedCompareExchangePointer( const_cast < void * volatile * > ( p ), const_cast < void * > ( value ), const_cast < void * > ( comperand ) ); }
inline bool ThreadInterlockedAssignPointerToConstIf( void const * volatile *p, void const *value, void const *comperand )			{ return ThreadInterlockedAssignPointerIf( const_cast < void * volatile * > ( p ), const_cast < void * > ( value ), const_cast < void * > ( comperand ) ); }



inline unsigned ThreadInterlockedExchangeSubtract( uint32 volatile *p, uint32 value )					{ return ThreadInterlockedExchangeAdd( (int32 volatile *)p, value ); }
inline unsigned ThreadInterlockedIncrement( uint32 volatile *p )										{ return ThreadInterlockedIncrement( (int32 volatile *)p ); }
inline unsigned ThreadInterlockedDecrement( uint32 volatile *p )										{ return ThreadInterlockedDecrement( (int32 volatile *)p ); }
inline unsigned ThreadInterlockedExchange( uint32 volatile *p, uint32 value )							{ return ThreadInterlockedExchange( (int32 volatile *)p, value ); }
inline unsigned ThreadInterlockedExchangeAdd( uint32 volatile *p, uint32 value )						{ return ThreadInterlockedExchangeAdd( (int32 volatile *)p, value ); }
inline unsigned ThreadInterlockedCompareExchange( uint32 volatile *p, uint32 value, uint32 comperand )	{ return ThreadInterlockedCompareExchange( (int32 volatile *)p, value, comperand ); }
inline bool ThreadInterlockedAssignIf( uint32 volatile *p, uint32 value, uint32 comperand )				{ return ThreadInterlockedAssignIf( (int32 volatile *)p, value, comperand ); }

//inline int ThreadInterlockedExchangeSubtract( int volatile *p, int value )	{ return ThreadInterlockedExchangeAdd( (int32 volatile *)p, value ); }
//inline int ThreadInterlockedIncrement( int volatile *p )	{ return ThreadInterlockedIncrement( (int32 volatile *)p ); }
//inline int ThreadInterlockedDecrement( int volatile *p )	{ return ThreadInterlockedDecrement( (int32 volatile *)p ); }
//inline int ThreadInterlockedExchange( int volatile *p, int value )	{ return ThreadInterlockedExchange( (int32 volatile *)p, value ); }
//inline int ThreadInterlockedExchangeAdd( int volatile *p, int value )	{ return ThreadInterlockedExchangeAdd( (int32 volatile *)p, value ); }
//inline int ThreadInterlockedCompareExchange( int volatile *p, int value, int comperand )	{ return ThreadInterlockedCompareExchange( (int32 volatile *)p, value, comperand ); }
//inline bool ThreadInterlockedAssignIf( int volatile *p, int value, int comperand )	{ return ThreadInterlockedAssignIf( (int32 volatile *)p, value, comperand ); }

#if defined( PLATFORM_64BITS )
#if defined (PLATFORM_WINDOWS) 
typedef __m128i int128;
inline int128 int128_zero()	{ return _mm_setzero_si128(); }

#pragma intrinsic( _InterlockedCompareExchange128 )

inline bool ThreadInterlockedAssignIf128( volatile int128 *pDest, const int128 &value, const int128 &comperand )
{
	Assert( (size_t)pDest % 16 == 0 );

	volatile int64 *pDest64 = ( volatile int64 * )pDest;
	int64 *pValue64 = ( int64 * )&value;
	int64 *pComperand64 = ( int64 * )&comperand;
	
	int64 local_comperand[2] = { pComperand64[0], pComperand64[1] };

	return _InterlockedCompareExchange128( pDest64, pValue64[1], pValue64[0], local_comperand ) == 1;
}
#else
typedef __int128_t int128;
#define int128_zero() 0

inline bool ThreadInterlockedAssignIf128( volatile int128 *pDest, const int128 &value, const int128 &comperand )
{
	Assert( (size_t)pDest % 16 == 0 );
	
	int128 local_comperand = comperand;
	
	return __sync_bool_compare_and_swap( pDest, local_comperand, value );
}
#endif
#endif

//-----------------------------------------------------------------------------
// Access to VTune thread profiling
//-----------------------------------------------------------------------------
#if defined(PLATFORM_WINDOWS) && defined(THREAD_PROFILER)
PLATFORM_INTERFACE void ThreadNotifySyncPrepare(void *p);
PLATFORM_INTERFACE void ThreadNotifySyncCancel(void *p);
PLATFORM_INTERFACE void ThreadNotifySyncAcquired(void *p);
PLATFORM_INTERFACE void ThreadNotifySyncReleasing(void *p);
#else
#define ThreadNotifySyncPrepare(p)		((void)0)
#define ThreadNotifySyncCancel(p)		((void)0)
#define ThreadNotifySyncAcquired(p)		((void)0)
#define ThreadNotifySyncReleasing(p)	((void)0)
#endif

//-----------------------------------------------------------------------------
// Encapsulation of a thread local datum (needed because THREAD_LOCAL doesn't
// work in a DLL loaded with LoadLibrary()
//-----------------------------------------------------------------------------

#ifndef NO_THREAD_LOCAL

#ifdef _LINUX
// linux totally supports compiler thread locals, even across dll's.
#define PLAT_COMPILER_SUPPORTED_THREADLOCALS 1
#define CTHREADLOCALINTEGER( typ ) __thread int
#define CTHREADLOCALINT __thread int
#define CTHREADLOCALPTR( typ ) __thread typ *
#define CTHREADLOCAL( typ ) __thread typ
#define GETLOCAL( x ) ( x )
#endif

#if defined( PLATFORM_WINDOWS ) || defined( OSX )
#ifndef __AFXTLS_H__ // not compatible with some Windows headers

#define CTHREADLOCALINT GenericThreadLocals::CThreadLocalInt<int>
#define CTHREADLOCALINTEGER( typ ) GenericThreadLocals::CThreadLocalInt<typ>
#define CTHREADLOCALPTR( typ ) GenericThreadLocals::CThreadLocalPtr<typ>
#define CTHREADLOCAL( typ ) GenericThreadLocals::CThreadLocal<typ>
#define GETLOCAL( x ) ( x.Get() )


namespace GenericThreadLocals
{
	// a (not so efficient) implementation of thread locals for compilers without full support (i.e. visual c).
	// don't use this explicity - instead, use the CTHREADxxx macros above.

	class PLATFORM_CLASS CThreadLocalBase
	{
public:
		CThreadLocalBase();
		~CThreadLocalBase();

		void * Get() const;
		void   Set(void *);

private:
#if defined(POSIX)
		pthread_key_t m_index;
#else
		uint32 m_index;
#endif
	};

	//---------------------------------------------------------

	template <class T>
	class CThreadLocal : public CThreadLocalBase
	{
	public:
		CThreadLocal()
		{
#ifdef PLATFORM_64BITS
			COMPILE_TIME_ASSERT( sizeof(T) <= sizeof(void *) );
#else
			COMPILE_TIME_ASSERT( sizeof(T) == sizeof(void *) );
#endif
		}

		void operator=( T i ) { Set( i ); }

		T Get() const
		{
#ifdef PLATFORM_64BITS
			void *pData = CThreadLocalBase::Get();
			return *reinterpret_cast<T*>( &pData );
#else
	#ifdef COMPILER_MSVC
		#pragma warning ( disable : 4311 )
	#endif
			return reinterpret_cast<T>( CThreadLocalBase::Get() );
	#ifdef COMPILER_MSVC
		#pragma warning ( default : 4311 )
	#endif
#endif
		}

		void Set(T val)
		{
#ifdef PLATFORM_64BITS
			void* pData = 0;
			*reinterpret_cast<T*>( &pData ) = val;
			CThreadLocalBase::Set( pData );
#else
	#ifdef COMPILER_MSVC
		#pragma warning ( disable : 4312 )
	#endif
			CThreadLocalBase::Set( reinterpret_cast<void *>(val) );
	#ifdef COMPILER_MSVC
		#pragma warning ( default : 4312 )
	#endif
#endif
		}
	};


	//---------------------------------------------------------

	template <class T = int32>
	class CThreadLocalInt : public CThreadLocal<T>
	{
	public:
		operator const T() const { return this->Get(); }
		int	operator=( T i ) { Set( i ); return i; }

		T operator++()					{ T i = this->Get(); this->Set( ++i ); return i; }
		T operator++(int)				{ T i = this->Get(); this->Set( i + 1 ); return i; }

		T operator--()					{ T i = this->Get(); this->Set( --i ); return i; }
		T operator--(int)				{ T i = this->Get(); this->Set( i - 1 ); return i; }

		inline CThreadLocalInt(  ) { }
		inline CThreadLocalInt( const T &initialvalue )
		{
			this->Set( initialvalue );
		}
	};


	//---------------------------------------------------------

	template <class T>
	class CThreadLocalPtr : private CThreadLocalBase
	{
	public:
		CThreadLocalPtr() {}

		operator const void *() const          					{ return (const T *)Get(); }
		operator void *()                      					{ return (T *)Get(); }

		operator const T *() const							    { return (const T *)Get(); }
		operator const T *()          							{ return (const T *)Get(); }
		operator T *()											{ return (T *)Get(); }

		T *			operator=( T *p )							{ Set( p ); return p; }

		bool        operator !() const							{ return (!Get()); }
		bool        operator!=( int i ) const					{ AssertMsg( i == 0, "Only NULL allowed on integer compare" ); return (Get() != NULL); }
		bool        operator==( int i ) const					{ AssertMsg( i == 0, "Only NULL allowed on integer compare" ); return (Get() == NULL); }
		bool		operator==( const void *p ) const			{ return (Get() == p); }
		bool		operator!=( const void *p ) const			{ return (Get() != p); }
		bool		operator==( const T *p ) const				{ return operator==((const void*)p); }
		bool		operator!=( const T *p ) const				{ return operator!=((const void*)p); }

		T *  		operator->()								{ return (T *)Get(); }
		T &  		operator *()								{ return *((T *)Get()); }

		const T *   operator->() const							{ return (const T *)Get(); }
		const T &   operator *() const							{ return *((const T *)Get()); }

		const T &	operator[]( int i ) const					{ return *((const T *)Get() + i); }
		T &			operator[]( int i )							{ return *((T *)Get() + i); }

	private:
		// Disallowed operations
		CThreadLocalPtr( T *pFrom );
		CThreadLocalPtr( const CThreadLocalPtr<T> &from );
		T **operator &();
		T * const *operator &() const;
		void operator=( const CThreadLocalPtr<T> &from );
		bool operator==( const CThreadLocalPtr<T> &p ) const;
		bool operator!=( const CThreadLocalPtr<T> &p ) const;
	};
}

#ifdef _OSX
PLATFORM_INTERFACE GenericThreadLocals::CThreadLocalInt<int> g_nThreadID;
#else // _OSX
#ifndef TIER0_DLL_EXPORT
__declspec( dllimport ) GenericThreadLocals::CThreadLocalInt<int> g_nThreadID;
#endif
#endif // _OSX

#endif /// afx32
#endif //_PLATFORM_WINDOWS

#endif // NO_THREAD_LOCAL

//-----------------------------------------------------------------------------
//
// A super-fast thread-safe integer A simple class encapsulating the notion of an 
// atomic integer used across threads that uses the built in and faster 
// "interlocked" functionality rather than a full-blown mutex. Useful for simple 
// things like reference counts, etc.
//
//-----------------------------------------------------------------------------

template <typename T>
class CInterlockedIntT
{
public:
	CInterlockedIntT() : m_value( 0 ) 				{ COMPILE_TIME_ASSERT( sizeof(T) == sizeof(int32) ); }
	CInterlockedIntT( T value ) : m_value( value ) 	{}

	T operator()( void ) const      { return m_value; }
	operator T() const				{ return m_value; }

	bool operator!() const			{ return ( m_value == 0 ); }
	bool operator==( T rhs ) const	{ return ( m_value == rhs ); }
	bool operator!=( T rhs ) const	{ return ( m_value != rhs ); }

	T operator++()					{ return (T)ThreadInterlockedIncrement( (int32 *)&m_value ); }
	T operator++(int)				{ return operator++() - 1; }

	T operator--()					{ return (T)ThreadInterlockedDecrement( (int32 *)&m_value ); }
	T operator--(int)				{ return operator--() + 1; }

	bool AssignIf( T conditionValue, T newValue )	{ return ThreadInterlockedAssignIf( (int32 *)&m_value, (int32)newValue, (int32)conditionValue ); }

	T operator=( T newValue )		{ ThreadInterlockedExchange((int32 *)&m_value, newValue); return m_value; }

	// Atomic add is like += except it returns the previous value as its return value
	T AtomicAdd( T add )			{ return (T)ThreadInterlockedExchangeAdd( (int32 *)&m_value, (int32)add ); }
	T CompareExchange( T exchange, T comperand )	{ return (T)ThreadInterlockedCompareExchange( (int32 *)&m_value, (int32)exchange, (int32)comperand ); }

	void operator+=( T add )		{ ThreadInterlockedExchangeAdd( (int32 *)&m_value, (int32)add ); }
	void operator-=( T subtract )	{ operator+=( -subtract ); }
	void operator*=( T multiplier )	{ 
		T original, result; 
		do 
		{ 
			original = m_value; 
			result = original * multiplier; 
		} while ( !AssignIf( original, result ) );
	}
	void operator/=( T divisor )	{ 
		T original, result; 
		do 
		{ 
			original = m_value; 
			result = original / divisor;
		} while ( !AssignIf( original, result ) );
	}

	T operator+( T rhs ) const		{ return m_value + rhs; }
	T operator-( T rhs ) const		{ return m_value - rhs; }

private:
	volatile T m_value;
};

typedef CInterlockedIntT<int> CInterlockedInt;
typedef CInterlockedIntT<unsigned> CInterlockedUInt;

//-----------------------------------------------------------------------------

template <typename T>
class CInterlockedPtr
{
public:
	CInterlockedPtr() : m_value( 0 ) 				{}
	CInterlockedPtr( T *value ) : m_value( value ) 	{}

	operator T *() const			{ return m_value; }

	bool operator!() const			{ return ( m_value == 0 ); }
	bool operator==( T *rhs ) const	{ return ( m_value == rhs ); }
	bool operator!=( T *rhs ) const	{ return ( m_value != rhs ); }

#ifdef X64BITS
	T* operator++() { return ((T*)ThreadInterlockedExchangeAdd64((int64*)&m_value, sizeof(T))) + 1; }
	T* operator++(int) { return (T*)ThreadInterlockedExchangeAdd64((int64*)&m_value, sizeof(T)); }

	T* operator--() { return ((T*)ThreadInterlockedExchangeAdd64((int64*)&m_value, -sizeof(T))) - 1; }
	T* operator--(int) { return (T*)ThreadInterlockedExchangeAdd64((int64*)&m_value, -sizeof(T)); }

	bool AssignIf(T* conditionValue, T* newValue) { return ThreadInterlockedAssignPointerToConstIf((void const**)&m_value, (void const*)newValue, (void const*)conditionValue); }

	T* operator=(T* newValue) { ThreadInterlockedExchangePointerToConst((void const**)&m_value, (void const*)newValue); return newValue; }

	void operator+=(int add) { ThreadInterlockedExchangeAdd64((int64*)&m_value, add * sizeof(T)); }
	void operator-=(int subtract) { operator+=(-subtract); }

	// Atomic add is like += except it returns the previous value as its return value
	T* AtomicAdd(int add) { return (T*)ThreadInterlockedExchangeAdd64((int64*)&m_value, add * sizeof(T)); }
#else
	T *operator++()					{ return ((T *)ThreadInterlockedExchangeAdd( (int32 *)&m_value, sizeof(T) )) + 1; }
	T *operator++(int)				{ return (T *)ThreadInterlockedExchangeAdd( (int32 *)&m_value, sizeof(T) ); }

	T *operator--()					{ return ((T *)ThreadInterlockedExchangeAdd( (int32 *)&m_value, -sizeof(T) )) - 1; }
	T *operator--(int)				{ return (T *)ThreadInterlockedExchangeAdd( (int32 *)&m_value, -sizeof(T) ); }

	bool AssignIf( T *conditionValue, T *newValue )	{ return ThreadInterlockedAssignPointerToConstIf( (void const **) &m_value, (void const *) newValue, (void const *) conditionValue ); }

	T *operator=( T *newValue )		{ ThreadInterlockedExchangePointerToConst( (void const **) &m_value, (void const *) newValue ); return newValue; }

	void operator+=( int add )		{ ThreadInterlockedExchangeAdd( (int32 *)&m_value, add * sizeof(T) ); }
	void operator-=( int subtract )	{ operator+=( -subtract ); }

	// Atomic add is like += except it returns the previous value as its return value
	T *AtomicAdd( int add ) { return ( T * ) ThreadInterlockedExchangeAdd( (int32 *)&m_value, add * sizeof(T) ); }
#endif

	T *operator+( int rhs ) const		{ return m_value + rhs; }
	T *operator-( int rhs ) const		{ return m_value - rhs; }
	T *operator+( unsigned rhs ) const	{ return m_value + rhs; }
	T *operator-( unsigned rhs ) const	{ return m_value - rhs; }
	size_t operator-( T *p ) const		{ return m_value - p; }
	size_t operator-( const CInterlockedPtr<T> &p ) const	{ return m_value - p.m_value; }

private:
	T * volatile m_value;
};



//-----------------------------------------------------------------------------
//
// Platform independent for critical sections management
//
//-----------------------------------------------------------------------------

class CAtomicMutex
{
public:
	enum class State
	{
		NONE = 0,
		Acquiring = (1 << 0),
		Acquired = (1 << 1),

		MASK = (1 << 2) - 1
	};

	CAtomicMutex( uint16 spin_iters = 200 ) :
		m_LockCount( 0 ),
		m_AcquireSpinIterations( spin_iters ),
		m_CurrentOwnerID( 0 )
	{}

	//------------------------------------------------------
	// Mutex acquisition/release. Const intentionally defeated.
	//------------------------------------------------------
	void Lock( const char *pFileName = NULL, int nLine = -1 );
	void Lock( const char *pFileName = NULL, int nLine = -1 ) const { (const_cast<CAtomicMutex *>(this))->Lock( pFileName, nLine ); }
	void Unlock( const char *pFileName = NULL, int nLine = -1 );
	void Unlock( const char *pFileName = NULL, int nLine = -1 ) const { (const_cast<CAtomicMutex *>(this))->Unlock( pFileName, nLine ); }

	bool AssertOwnedByCurrentThread();
	void SetTrace( bool ) {}

private:
	DLL_CLASS_IMPORT void AcquireLock( ThreadId_t threadid );

private:
	// Uses CAtomicMutex::State enum
	CInterlockedUInt m_State;
	uint16		m_LockCount;
	uint16		m_AcquireSpinIterations;
	ThreadId_t	m_CurrentOwnerID;
};

typedef CAtomicMutex CThreadSpinMutex;
typedef CThreadSpinMutex CThreadFastMutex;

//-----------------------------------------------------------------------------
// Counting semaphore on ThreadAtomicWait
//-----------------------------------------------------------------------------
class CAtomicSemaphore
{
public:
	CAtomicSemaphore( uint32 nInitialCount = 0 ) : m_nCount( nInitialCount ), m_nWaiters( 0 ) {}

	// Takes one count, spinning a while before it sleeps until one is released
	void Wait()
	{
		uint32 nCount = m_nCount;

		for ( ;; )
		{
			if ( nCount == 0 )
			{
				for ( uint32 nSpin = 0; nSpin < 64 && nCount == 0; nSpin++ )
				{
					ThreadSpin( 1u << MIN( nSpin, 8u ) );
					nCount = m_nCount;
				}

				while ( nCount == 0 )
				{
					DoWait();
					nCount = m_nCount;
				}
			}

			const uint32 nPrevCount = ThreadInterlockedCompareExchange( &m_nCount, nCount - 1, nCount );

			if ( nPrevCount == nCount )
				return;

			nCount = nPrevCount;
		}
	}

	// Takes one count if one is available
	bool TryWait()
	{
		uint32 nCount = m_nCount;

		while ( nCount > 0 )
		{
			const uint32 nPrevCount = ThreadInterlockedCompareExchange( &m_nCount, nCount - 1, nCount );

			if ( nPrevCount == nCount )
				return true;

			nCount = nPrevCount;
		}

		return false;
	}

	// Adds nCount counts and wakes up to as many waiting threads
	void Release( uint32 nCount = 1 )
	{
		ThreadInterlockedExchangeAdd( &m_nCount, nCount );

		const uint32 nWaiters = m_nWaiters;

		if ( nWaiters != 0 )
			ThreadAtomicNotifyN( const_cast< const uint32 * >( &m_nCount ), MIN( nWaiters, nCount ) );
	}

	uint32 GetCount() const { return m_nCount; }

private:
	// Waits on m_nCount while it is zero, counting the thread in m_nWaiters
	DLL_CLASS_IMPORT void DoWait();

	volatile uint32 m_nCount;
	volatile uint32 m_nWaiters;

	CAtomicSemaphore( const CAtomicSemaphore & ) = delete;
	CAtomicSemaphore &operator=( const CAtomicSemaphore & ) = delete;
};

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------

class CThreadNullMutex
{
public:
	CThreadNullMutex( const char* pDebugName = NULL ) {}

	static void Lock( const char *pFileName = NULL, int nLine = -1 )	{}
	static void Unlock( const char *pFileName = NULL, int nLine = -1 )	{}

	static bool TryLock( const char *pFileName = NULL, int nLine = -1 )	{ return true; }
	static bool AssertOwnedByCurrentThread()							{ return true; }
	static void SetTrace( bool b )	{}

	static ThreadId_t GetOwnerId()	{ return 0;	}
	static int	GetDepth()			{ return 0; }
};

//-----------------------------------------------------------------------------
//
// A mutex decorator class used to control the use of a mutex, to make it
// less expensive when not multithreading
//
//-----------------------------------------------------------------------------

template <class BaseClass, bool *pCondition>
class CThreadConditionalMutex : public BaseClass
{
public:
	void Lock()				{ if ( *pCondition ) BaseClass::Lock(); }
	void Lock() const 		{ if ( *pCondition ) BaseClass::Lock(); }
	void Unlock()			{ if ( *pCondition ) BaseClass::Unlock(); }
	void Unlock() const		{ if ( *pCondition ) BaseClass::Unlock(); }

	bool TryLock()			{ if ( *pCondition ) return BaseClass::TryLock(); else return true; }
	bool TryLock() const 	{ if ( *pCondition ) return BaseClass::TryLock(); else return true; }
	bool AssertOwnedByCurrentThread() { if ( *pCondition ) return BaseClass::AssertOwnedByCurrentThread(); else return true; }
	void SetTrace( bool b ) { if ( *pCondition ) BaseClass::SetTrace( b ); }
};

//-----------------------------------------------------------------------------
// Mutex decorator that blows up if another thread enters
//-----------------------------------------------------------------------------

template <class BaseClass>
class CThreadTerminalMutex : public BaseClass
{
public:
	bool TryLock()			{ if ( !BaseClass::TryLock() ) { DebuggerBreak(); return false; } return true; }
	bool TryLock() const 	{ if ( !BaseClass::TryLock() ) { DebuggerBreak(); return false; } return true; }
	void Lock()				{ if ( !TryLock() ) BaseClass::Lock(); }
	void Lock() const 		{ if ( !TryLock() ) BaseClass::Lock(); }

};

//-----------------------------------------------------------------------------
//
// Class to Lock a critical section, and unlock it automatically
// when the lock goes out of scope
//
//-----------------------------------------------------------------------------

template <class MUTEX_TYPE = CAtomicMutex>
class CAutoLockT
{
public:
	FORCEINLINE CAutoLockT( MUTEX_TYPE &lock, const char *pFileName = nullptr, int nLine = 0 )
		: m_lock(lock)
	{
		m_lock.Lock( pFileName, nLine );
	}

	FORCEINLINE CAutoLockT( const MUTEX_TYPE &lock, const char *pFileName = nullptr, int nLine = 0 )
		: m_lock(const_cast<MUTEX_TYPE &>(lock))
	{
		m_lock.Lock( pFileName, nLine );
	}

	FORCEINLINE ~CAutoLockT()
	{
		m_lock.Unlock();
	}

private:
	MUTEX_TYPE &m_lock;

	// Disallow copying
	CAutoLockT( const CAutoLockT<MUTEX_TYPE> & );
	CAutoLockT &operator=( const CAutoLockT<MUTEX_TYPE> & );
};

typedef CAutoLockT<CAtomicMutex> CAutoLock;

//---------------------------------------------------------

template <int size>	struct CAutoLockTypeDeducer {};
template <> struct CAutoLockTypeDeducer<sizeof(CAtomicMutex)> {	typedef CAtomicMutex Type_t; };
template <> struct CAutoLockTypeDeducer<sizeof(CThreadNullMutex)> {	typedef CThreadNullMutex Type_t; };

#define AUTO_LOCK_( type, mutex ) \
	CAutoLockT< type > UNIQUE_ID( static_cast<const type &>( mutex ), __FILE__, __LINE__ )

template<typename T> T strip_cv_quals_for_mutex(T&);
template<typename T> T strip_cv_quals_for_mutex(const T&);
template<typename T> T strip_cv_quals_for_mutex(volatile T&);
template<typename T> T strip_cv_quals_for_mutex(const volatile T&);

#if defined( COMPILER_CLANG ) || defined( COMPILER_GCC )
#define AUTO_LOCK( mutex ) \
	AUTO_LOCK_( typename CAutoLockTypeDeducer<sizeof(mutex)>::Type_t, mutex )
#else
#define AUTO_LOCK( mutex ) \
	AUTO_LOCK_( CAutoLockTypeDeducer<sizeof(mutex)>::Type_t, mutex )
#endif


#define AUTO_LOCK_FM( mutex ) \
	AUTO_LOCK_( CThreadFastMutex, mutex )

#define LOCAL_THREAD_LOCK_( tag ) \
	; \
	static CThreadFastMutex autoMutex_## tag; \
	AUTO_LOCK( autoMutex_## tag )

#define LOCAL_THREAD_LOCK() \
	LOCAL_THREAD_LOCK_(_)

//-----------------------------------------------------------------------------
//
// Base class for event, semaphore and mutex objects.
//
//-----------------------------------------------------------------------------

#define TW_TIMEOUT	0xFFFF
#define TW_FAILED	0xFFFE

class PLATFORM_CLASS CThreadSyncObject
{
public:
	~CThreadSyncObject();

	//-----------------------------------------------------
	// Query if object is useful
	//-----------------------------------------------------
#ifdef PLATFORM_POSIX
	bool operator!() const;
#endif

	//-----------------------------------------------------
	// Access handle
	//-----------------------------------------------------
#ifdef PLATFORM_WINDOWS
	bool IsValid() const { return m_hSyncObject != NULL; }
	operator HANDLE() { return GetHandle(); }
	const HANDLE GetHandle() const { return m_hSyncObject; }
#endif // defined(PLATFORM_WINDOWS)

	//-----------------------------------------------------
	// Wait for a signal from the object
	//-----------------------------------------------------
	bool Wait( uint32 dwTimeout = TT_INFINITE );

#ifdef PLATFORM_POSIX
	bool Wait_NoDiagnostics( uint32 dwTimeout );
	virtual bool WaitImpl( uint32 dwTimeout );
	bool AcquireSemaphoreInternal( uint value );
	void CloseSemaphoreInternal( int semid, bool bUnlink, const char* pszName );
	bool CreateAnonymousSyncObjectInternal( int nInitialValue, bool bManualReset );
	bool CreateSemaphoreInternal( const char* pszName, int nInitialValue, bool bManualReset, bool* pbAlreadyExists );
	bool EnsureSemaphoreClearedInternal( int semid );
	bool EnsureSemaphorePostedInternal( int semid );
	bool IsSemaphoreOrphanedInternal( int semid, int semnum );
	bool OpenSemaphoreInternal( const char* pszName, bool bFailIfNotExist );
	bool ReleaseSemaphoreInternal( int semid, int nReleaseCount );
	void ResetThreadSyncObjectInternal ( );
	void SaveNameToFile( const char* pszName );
	void SignalThreadSyncObjectInternal ( );
#endif // defined(PLATFORM_POSIX)

	//-----------------------------------------------------
	// Wait for a signal from any of the specified objects.
	//
	// Returns the index of the object that signaled the event
	// or THREADSYNC_TIMEOUT if the timeout was hit before the wait condition was met.
	//
	// Returns TW_FAILED if an incoming object is invalid.
	//
	// If bWaitAll=true, then it'll return 0 if all the objects were set.
	//-----------------------------------------------------
	static uint32 WaitForMultiple( int nObjects, CThreadSyncObject **ppObjects, bool bWaitAll, uint32 dwTimeout = TT_INFINITE );
	
	// This builds a list of pointers and calls straight through to the other WaitForMultiple.
	static uint32 WaitForMultiple( int nObjects, CThreadSyncObject *ppObjects, bool bWaitAll, uint32 dwTimeout = TT_INFINITE );

protected:
	CThreadSyncObject();
	void AssertUseable();

#ifdef PLATFORM_WINDOWS
	HANDLE m_hSyncObject;
	bool m_bInitalized;
#elif defined(POSIX)
	const char *m_pszName; // 8
	int m_Semaphore; // 16
	bool m_bSemOwner;
	bool m_bSemAutoUndo;
	pthread_mutex_t	m_Mutex; // 24
	pthread_cond_t	m_Condition; // 64
	bool m_bInitalized; // 112
	bool m_bManualReset; // 113
	int32 m_cSet;
	int m_nBroadcastCount;
#else
#error "Implement me"
#endif

private:
	CThreadSyncObject( const CThreadSyncObject & );
	CThreadSyncObject &operator=( const CThreadSyncObject & );
};

//-----------------------------------------------------------------------------
//
// CThreadSemaphore
//
//-----------------------------------------------------------------------------

class PLATFORM_CLASS CThreadSemaphore : public CThreadSyncObject
{
public:
	CThreadSemaphore( int32 initialValue = 0, int32 maxValue = 1, const char *pszName = nullptr, bool bCreate = false );
	~CThreadSemaphore();

	//-----------------------------------------------------
	// Increases the count of the semaphore object by a specified
	// amount.  Wait() decreases the count by one on return.
	//-----------------------------------------------------
	bool Release( int32 releaseCount = 1, int32 * pPreviousCount = NULL );

#if defined(PLATFORM_POSIX)
	//-----------------------------------------------------
	// Wait implementation for a signal from the object
	//-----------------------------------------------------
	bool WaitImpl( uint32 dwTimeout ) override;
#endif

private:
	CThreadSemaphore( const CThreadSemaphore & );
	CThreadSemaphore &operator=( const CThreadSemaphore & );
};

//-----------------------------------------------------------------------------
// Named mutex shared between processes
//-----------------------------------------------------------------------------
class CThreadCrossProcessMutex : public CThreadSyncObject
{
public:
	// bAllowEveryone gives every user access to the mutex
	DLL_CLASS_IMPORT CThreadCrossProcessMutex( const char *pszName, bool bAllowEveryone );
#ifdef PLATFORM_WINDOWS
	DLL_CLASS_IMPORT ~CThreadCrossProcessMutex();

	DLL_CLASS_IMPORT bool TryLock( const char *pFileName, int nLine, uint32 nTimeout );
	DLL_CLASS_IMPORT void Unlock( const char *pFileName, int nLine );
	DLL_CLASS_IMPORT bool AssertOwnedByCurrentThread() const;
	DLL_CLASS_IMPORT void SetTrace( bool bTrace );
#endif

	DLL_CLASS_IMPORT bool Release();
};


//-----------------------------------------------------------------------------
//
// Wrapper for unnamed event objects
//
//-----------------------------------------------------------------------------

#if defined( PLATFORM_WINDOWS )

//-----------------------------------------------------------------------------
//
// A mutex suitable for out-of-process, multi-processor usage
//
//-----------------------------------------------------------------------------

class PLATFORM_CLASS CThreadFullMutex : public CThreadSyncObject
{
public:
	CThreadFullMutex( bool bEstablishInitialOwnership = false, const char * pszName = NULL );

	//-----------------------------------------------------
	// Release ownership of the mutex
	//-----------------------------------------------------
	bool Release();

	// To match regular CAtomicMutex:
	void Lock()							{ Wait(); }
	void Lock( unsigned timeout )		{ Wait( timeout ); }
	void Unlock()						{ Release(); }
	bool AssertOwnedByCurrentThread()	{ return true; }
	void SetTrace( bool )				{}

private:
	CThreadFullMutex( const CThreadFullMutex & );
	CThreadFullMutex &operator=( const CThreadFullMutex & );
};
#endif

enum NamedEventResult_t
{
	TT_EventDoesntExist	= 0,
	TT_EventNotSignaled,
	TT_EventSignaled
};

class PLATFORM_CLASS CThreadEvent : public CThreadSyncObject
{
public:
	CThreadEvent( bool fManualReset = false );
	CThreadEvent( const char *name, bool initialState = false, bool bManualReset = false );
#ifdef PLATFORM_WINDOWS
	static NamedEventResult_t CheckNamedEvent( const char *name, uint32 dwTimeout = 0 );
#endif
#ifdef WIN32
	CThreadEvent( HANDLE hHandle );
#endif
	//-----------------------------------------------------
	// Set the state to signaled
	//-----------------------------------------------------
	bool Set();

	//-----------------------------------------------------
	// Set the state to nonsignaled
	//-----------------------------------------------------
	bool Reset();

	//-----------------------------------------------------
	// Check if the event is signaled
	//-----------------------------------------------------
	bool Check();

	// See CThreadSyncObject for definitions of these functions.
	static uint32 WaitForMultiple( int nObjects, CThreadEvent **ppObjects, bool bWaitAll, uint32 dwTimeout = TT_INFINITE );
	static uint32 WaitForMultiple( int nObjects, CThreadEvent *ppObjects, bool bWaitAll, uint32 dwTimeout = TT_INFINITE );

private:
	CThreadEvent( const CThreadEvent & );
	CThreadEvent &operator=( const CThreadEvent & );
};

// Hard-wired manual event for use in array declarations
class CThreadManualEvent : public CThreadEvent
{
public:
	CThreadManualEvent()
	 :	CThreadEvent( true )
	{
	}
};

//-----------------------------------------------------------------------------
// Objects that ThreadMultiWait can wait on
//-----------------------------------------------------------------------------
class CThreadMultiWaitObject
{
public:
	DLL_CLASS_IMPORT CThreadMultiWaitObject();
	DLL_CLASS_IMPORT ~CThreadMultiWaitObject();

	DLL_CLASS_IMPORT void Signal();

	// Waits for the object alone, consuming the signal like ThreadMultiWait does
	bool Wait( uint32 nTimeout = TT_INFINITE )
	{
		ThreadMultiWaitItem_t item;
		ThreadMultiWaitExtra_t extra;
		GetWaitItem( &item, &extra );

		uint64 nSignalled;
		return ThreadMultiWait( nullptr, &item, &extra, 1, false, &nSignalled, nTimeout ) > 0;
	}

	bool Check() { return Wait( 0 ); }

	// The item and the extra data that ThreadMultiWait takes for the object
	void GetWaitItem( ThreadMultiWaitItem_t *pItem, ThreadMultiWaitExtra_t *pExtra ) const
	{
#ifdef PLATFORM_WINDOWS
		*pItem = m_hEventFd;
#else
		pItem->fd = m_hEventFd;
		pItem->events = POLLIN;
		pItem->revents = 0;
#endif
		pExtra->nFlags = m_nExtraFlags;
#ifdef PLATFORM_OSX
		pExtra->pObject = const_cast< CThreadMultiWaitObject * >( this );
#endif
	}

	uint32 GetExtraFlags() const { return m_nExtraFlags; }

protected:
#ifdef PLATFORM_WINDOWS
	HANDLE m_hEventFd;
#else
	int m_hEventFd;
#endif
#ifdef PLATFORM_OSX
	int m_nWriteFd; // The other end of the pipe that m_hEventFd reads, signalled while it holds a byte
#endif
	uint32 m_nExtraFlags; // ThreadMultiWaitExtra_t flags: 1 for semaphores, 2 for auto-reset events, 4 for manual-reset events
#ifdef PLATFORM_OSX
	bool m_bSignalled;
	bool m_bHasMutex; // Only events have the mutex
	pthread_mutex_t m_Mutex;
#endif

private:
	CThreadMultiWaitObject( const CThreadMultiWaitObject & ) = delete;
	CThreadMultiWaitObject &operator=( const CThreadMultiWaitObject & ) = delete;
};

class CThreadMultiWaitEvent : public CThreadMultiWaitObject
{
public:
	DLL_CLASS_IMPORT CThreadMultiWaitEvent( bool bManualReset = false );
	DLL_CLASS_IMPORT ~CThreadMultiWaitEvent();

	DLL_CLASS_IMPORT void Set();
	DLL_CLASS_IMPORT void Reset();
};

class CThreadMultiWaitSemaphore : public CThreadMultiWaitObject
{
public:
	DLL_CLASS_IMPORT CThreadMultiWaitSemaphore();
	DLL_CLASS_IMPORT ~CThreadMultiWaitSemaphore();

	DLL_CLASS_IMPORT void Release();
};

// Takes an array of events, which are multi-wait objects at the same address
inline ThreadMultiWaitObjectSet_t *ThreadMultiWaitObjectSetCreate( CThreadMultiWaitEvent **ppEvents, int nEvents )
{
	return ThreadMultiWaitObjectSetCreate( reinterpret_cast< CThreadMultiWaitObject ** >( ppEvents ), nEvents );
}


//-----------------------------------------------------------------------------
// Blocks the arriving threads until the expected count arrives
//-----------------------------------------------------------------------------
class CThreadBarrier
{
public:
	DLL_CLASS_IMPORT CThreadBarrier( uint32 nExpected );
	DLL_CLASS_IMPORT ~CThreadBarrier();

	DLL_CLASS_IMPORT void ArriveAndWait();

private:
	void *m_pImpl; // Counters, a mutex and a condition variable allocated by tier0

	CThreadBarrier( const CThreadBarrier & ) = delete;
	CThreadBarrier &operator=( const CThreadBarrier & ) = delete;
};


//-----------------------------------------------------------------------------
// Lets background work pause at safe points while the foreground needs it
//-----------------------------------------------------------------------------
class CThreadBackgroundWorkGate
{
public:
	DLL_CLASS_IMPORT CThreadBackgroundWorkGate();
	DLL_CLASS_IMPORT ~CThreadBackgroundWorkGate();

	DLL_CLASS_IMPORT void PauseWork();
	DLL_CLASS_IMPORT void ResumeWork();
	DLL_CLASS_IMPORT bool PauseRequested() const;

	DLL_CLASS_IMPORT void OnBeginBackgroundWork();
	DLL_CLASS_IMPORT void OnEndBackgroundWork();

private:
	void *m_pImpl; // State, a mutex and a condition variable allocated by tier0

	CThreadBackgroundWorkGate( const CThreadBackgroundWorkGate & ) = delete;
	CThreadBackgroundWorkGate &operator=( const CThreadBackgroundWorkGate & ) = delete;
};


//-----------------------------------------------------------------------------
// Per thread record tier0 keeps for every thread it knows about
//-----------------------------------------------------------------------------
class CThreadLocalScope
{
public:
	class CScopeCreatedOrDestroyedHandler
	{
	public:
		typedef void (*FN_OnScopeChange)( CThreadLocalScope *pScope, bool bCreated );

		DLL_CLASS_IMPORT CScopeCreatedOrDestroyedHandler( FN_OnScopeChange pHandler );
		DLL_CLASS_IMPORT ~CScopeCreatedOrDestroyedHandler();

	private:
		FN_OnScopeChange m_pHandler;
		CScopeCreatedOrDestroyedHandler *m_pNext;

		CScopeCreatedOrDestroyedHandler( const CScopeCreatedOrDestroyedHandler & ) = delete;
		CScopeCreatedOrDestroyedHandler &operator=( const CScopeCreatedOrDestroyedHandler & ) = delete;
	};

	DLL_CLASS_IMPORT CThreadLocalScope( ThreadHandle_t hThreadHandle, bool bDeleteOnThreadDetach );
	DLL_CLASS_IMPORT ~CThreadLocalScope();

	DLL_CLASS_IMPORT bool IsLocalThreadScope() const;

	DLL_CLASS_IMPORT static CAtomicMutex &GetScopeListMutex();
	DLL_CLASS_IMPORT static CThreadLocalScope *FindScopeForThreadHandle( ThreadHandle_t hThreadHandle );
	DLL_CLASS_IMPORT static CThreadLocalScope *FindScopeForThreadId( ThreadId_t nThreadId );

	const char *GetThreadName() const { return m_szThreadName; }
	DLL_CLASS_IMPORT void SetThreadName( const char *pBuffer );

protected:
	CThreadLocalScope *m_pAllScopesListNext;
	ThreadHandle_t m_hOwningThread;
	bool m_bInScopeList; // Set once the scope is linked into the list of all scopes
#ifdef PLATFORM_WINDOWS
	uint32 m_nThreadId;
#endif
	char m_szThreadName[ 32 ];
	int m_nInternalFlags;

private:
	CThreadLocalScope( const CThreadLocalScope & ) = delete;
	CThreadLocalScope &operator=( const CThreadLocalScope & ) = delete;
};


//-----------------------------------------------------------------------------
//
// CThreadRWLock
//
//-----------------------------------------------------------------------------

class PLATFORM_CLASS CThreadRWLock
{
public:
	CThreadRWLock();

	void LockForRead( const char *pFileName = NULL, int nLine = -1 );
	void UnlockRead( const char *pFileName = NULL, int nLine = -1 );
	void LockForWrite( const char *pFileName = NULL, int nLine = -1 );
	void UnlockWrite( const char *pFileName = NULL, int nLine = -1 );

	void LockForRead( const char *pFileName = NULL, int nLine = -1 ) const { const_cast<CThreadRWLock *>(this)->LockForRead( pFileName, nLine ); }
	void UnlockRead( const char *pFileName = NULL, int nLine = -1 ) const { const_cast<CThreadRWLock *>(this)->UnlockRead( pFileName, nLine ); }
	void LockForWrite( const char *pFileName = NULL, int nLine = -1 ) const { const_cast<CThreadRWLock *>(this)->LockForWrite( pFileName, nLine ); }
	void UnlockWrite( const char *pFileName = NULL, int nLine = -1 ) const { const_cast<CThreadRWLock *>(this)->UnlockWrite( pFileName, nLine ); }

private:
	void WaitForRead();

	CThreadFastMutex m_mutex;
	CThreadEvent m_CanWrite;
	CThreadEvent m_CanRead;

	int m_nWriters;
	int m_nActiveReaders;
	int m_nPendingReaders;
};

//-----------------------------------------------------------------------------
//
// CThreadSpinRWLock
//
//-----------------------------------------------------------------------------

class CThreadSpinRWLock
{
public:
	bool TryLockForWrite( const char *pFileName = NULL, int nLine = -1 ) { return m_mutex.try_lock(); }
	bool TryLockForRead( const char *pFileName = NULL, int nLine = -1 ) { return m_mutex.try_lock_shared(); }

	void LockForRead( const char *pFileName = NULL, int nLine = -1 ) { m_mutex.lock_shared(); }
	void UnlockRead( const char *pFileName = NULL, int nLine = -1 ) { m_mutex.unlock_shared(); }
	void LockForWrite( const char *pFileName = NULL, int nLine = -1 ) { m_mutex.lock(); }
	void UnlockWrite( const char *pFileName = NULL, int nLine = -1 ) { m_mutex.unlock(); }

	bool TryLockForWrite( const char *pFileName = NULL, int nLine = -1 ) const { return const_cast<CThreadSpinRWLock *>(this)->TryLockForWrite( pFileName, nLine ); }
	bool TryLockForRead( const char *pFileName = NULL, int nLine = -1 ) const { return const_cast<CThreadSpinRWLock *>(this)->TryLockForRead( pFileName, nLine ); }

	void LockForRead( const char *pFileName = NULL, int nLine = -1 ) const { const_cast<CThreadSpinRWLock *>(this)->LockForRead( pFileName, nLine ); }
	void UnlockRead( const char *pFileName = NULL, int nLine = -1 ) const { const_cast<CThreadSpinRWLock *>(this)->UnlockRead( pFileName, nLine ); }
	void LockForWrite( const char *pFileName = NULL, int nLine = -1 ) const { const_cast<CThreadSpinRWLock *>(this)->LockForWrite( pFileName, nLine ); }
	void UnlockWrite( const char *pFileName = NULL, int nLine = -1 ) const { const_cast<CThreadSpinRWLock *>(this)->UnlockWrite( pFileName, nLine ); }

private:
	std::shared_mutex m_mutex;
};

enum RWLockTranstionResult_t
{
	// Another writer had the lock in between
	RWLTR_STATE_INVALIDATED = 0,
	RWLTR_STATE_REMAINED_VALID = 1,
};

class PLATFORM_CLASS CThreadRWLock_FastRead
{
public:
	enum InterlockBits_t : uint32
	{
		READ_LOCK_MASK = 0x3FF,
		WAITING_READ_LOCK_INCREMENT = 0x400,
		WAITING_READ_LOCK_MASK = 0xFFC00,
		WRITE_LOCK_INCREMENT = 0x100000,
		WRITE_LOCK_MASK = 0x7FF00000,
		READ_LOCK_ACCOUNTING_BIT = 0x80000000
	};

	enum WriteLockTransition_t
	{
		TRANSITION_NONE,
		TRANSITION_TO_READ,
		TRANSITION_TO_WRITE
	};

	CThreadRWLock_FastRead();

	void UnlockWrite(const char* pFileName = nullptr, int nLine = -1);
	RWLockTranstionResult_t UnlockRead_LockForWrite(const char* pFileName, int nLine = -1, WriteLockTransition_t transition = TRANSITION_TO_READ);
	RWLockTranstionResult_t UnlockWrite_LockForRead(const char* pFileName = nullptr, int nLine = -1);
	bool TryUnlockRead_LockForWrite(const char* pFileName, int nLine, bool bForce, WriteLockTransition_t transition = TRANSITION_TO_WRITE);
	bool TryUnlockWrite_LockForRead(const char* pFileName = nullptr, int nLine = -1);
	void HaveWriteLock_BlockReadsNow(bool bBlock);
	void HaveWriteLock_UnblockReads();

protected:
	bool EncounteredComplexReadLockOperation(uint mode, bool bSomething, const char* pFileName, int nLine = -1);
	void UnlockReadAccounting(const char* pFileName, int nLine, uint mode);

private:
	CInterlockedUInt m_Interlock;
	CInterlockedInt m_ReadlockAccounting;
	CThreadSemaphore m_Semaphore;
	CThreadEvent m_WriteLockUnblocked;
	CThreadEvent m_ReadLockUnblocked;

	ThreadId_t m_WritingThreadID;
	int m_nWritingThreadLockCount;
};

//-----------------------------------------------------------------------------
//
// A thread wrapper similar to a Java thread.
//
//-----------------------------------------------------------------------------

enum ThreadPriorityEnum_t
{
	TP_PRIORITY_DEFAULT = 0,
	TP_PRIORITY_NORMAL = 0,
	TP_PRIORITY_HIGH = 1,
	TP_PRIORITY_LOW = -1,
	TP_PRIORITY_HIGHEST = 2,
	TP_PRIORITY_LOWEST = -2,
};

enum CThreadClassKind_t : int;

class PLATFORM_CLASS CThread
{
public:
	CThread();
	virtual ~CThread();

	//-----------------------------------------------------

	const char *GetName();
	void SetName( const char * );

	//-----------------------------------------------------
	// Functions for the other threads
	//-----------------------------------------------------

	// Start thread running  - error if already running
	virtual bool Start( unsigned nBytesStack = 0, ThreadPriorityEnum_t nPriority = TP_PRIORITY_DEFAULT );

	// Returns true if thread has been created and hasn't yet exited
	bool IsAlive();

	// This method causes the current thread to wait until this thread
	// is no longer alive.
	bool Join( unsigned timeout = TT_INFINITE );

	// Access the thread handle directly
	ThreadHandle_t GetThreadHandle();

#ifdef PLATFORM_WINDOWS
	uint GetThreadID() const { return m_threadId; }
#endif

	//-----------------------------------------------------

	int GetResult();

	//-----------------------------------------------------
	// Functions for both this, and maybe, and other threads
	//-----------------------------------------------------

	// Forcibly, abnormally, but relatively cleanly stop the thread
	void Stop( int exitCode = 0 );

#ifdef PLATFORM_WINDOWS
	// Get the priority
	ThreadPriorityEnum_t GetPriority() const;

	// Set the priority
	bool SetPriority( ThreadPriorityEnum_t );
#endif

	// Suspend a thread
	unsigned Suspend();

	// Resume a suspended thread
	unsigned Resume();

#ifdef PLATFORM_WINDOWS
	bool IsSuspended();
#endif

	CThreadClassKind_t GetClassKind() const { return m_ClassKind; }

	// Force hard-termination of thread.  Used for critical failures.
	bool Terminate( int exitCode = 0 );

	//-----------------------------------------------------
	// Global methods
	//-----------------------------------------------------

	// Get the Thread object that represents the current thread, if any.
	// Can return NULL if the current thread was not created using
	// CThread
	static CThread *GetCurrentCThread();

	// Offer a context switch. Under Win32, equivalent to Sleep(0)
#ifdef Yield
#undef Yield
#endif
	static void Yield() { ThreadYield(); }

	// This method causes the current thread to yield and not to be
	// scheduled for further execution until a certain amount of real
	// time has elapsed, more or less. Duration is in milliseconds
	static void Sleep( unsigned duration );

	// Returns the class kind of the CThread that runs the calling thread
	static CThreadClassKind_t GetCurrentCThreadClassKind();

protected:

	// Thread will run this function on startup, must be supplied by
	// derived class, performs the intended action of the thread.
	virtual int Run() = 0;

	// Called when the thread exits
	virtual void OnExit();

	// Allow for custom start waiting
	virtual bool WaitForCreateComplete( CThreadEvent *pEvent );

	virtual bool IsThreadRunning();

	// Thread initially runs this. param is actually 'this'. function
	// just gets this and calls ThreadProc
	static uintp ThreadProc( void *pv );

	// The thread handle for the const methods
	ThreadHandle_t GetThreadHandle() const
	{
#ifdef PLATFORM_WINDOWS
		return ( ThreadHandle_t )m_hThread;
#else
		return ( ThreadHandle_t )m_threadId;
#endif
	}

	CAtomicMutex m_Lock;
#ifdef PLATFORM_POSIX
	CThreadEvent m_ExitEvent;	// Set right before the thread's function exits.
#endif

private:
	enum Flags
	{
		SUPPORT_STOP_PROTOCOL = 1 << 0
	};

	// The argument of ThreadProc, allocated by Start
	struct ThreadInit_t
	{
		CThread *     pThread;
		ThreadPriorityEnum_t priority;
	};

	// make copy constructor and assignment operator inaccessible
	CThread( const CThread & );
	CThread &operator=( const CThread & );

#ifdef PLATFORM_WINDOWS
	HANDLE 	m_hThread;
	ThreadId_t m_threadId;
#elif defined(PLATFORM_POSIX)
	pthread_t m_threadId;
#endif
	int		m_result;
	bool	m_bAlive;
	char	m_szName[32];
	unsigned m_flags;
	CThreadClassKind_t m_ClassKind;
	CThreadManualEvent m_NotSuspendedEvent;
};

//-----------------------------------------------------------------------------
// Simple thread class encompasses the notion of a worker thread, handing
// synchronized communication.
//-----------------------------------------------------------------------------

// These are internal reserved error results from a call attempt
enum WTCallResult_t
{
	WTCR_FAIL			= -1,
	WTCR_TIMEOUT		= -2,
	WTCR_THREAD_GONE	= -3,
};

class PLATFORM_CLASS CWorkerThread : public CThread
{
public:
	CWorkerThread();

	//-----------------------------------------------------
	//
	// Inter-thread communication
	//
	// Calls in either direction take place on the same "channel."
	// Seperate functions are specified to make identities obvious
	//
	//-----------------------------------------------------

	// Master: Signal the thread, and block for a response
	int CallWorker( unsigned, unsigned timeout = TT_INFINITE, bool fBoostWorkerPriorityToMaster = true );

	// Worker: Signal the thread, and block for a response
	int CallMaster( unsigned, unsigned timeout = TT_INFINITE );

	// Wait for the next request
	bool WaitForCall( unsigned dwTimeout, unsigned *pResult = NULL );
	bool WaitForCall( unsigned *pResult = NULL );

	// Is there a request?
	bool PeekCall( unsigned *pParam = NULL );

	// Reply to the request
	void Reply( unsigned );

	// Wait for a reply in the case when CallWorker() with timeout != TT_INFINITE
	int WaitForReply( unsigned timeout = TT_INFINITE );

	// If you want to do WaitForMultipleObjects you'll need to include
	// this handle in your wait list or you won't be responsive
	CThreadEvent& GetCallHandle();	// (returns m_EventSend)

	// Find out what the request was
	unsigned GetCallParam() const;

	// Boost the worker thread to the master thread, if worker thread is lesser, return old priority
	int BoostPriority();

protected:
	int Call( unsigned, unsigned timeout, bool fBoost );

private:
	CWorkerThread( const CWorkerThread & );
	CWorkerThread &operator=( const CWorkerThread & );

	CThreadEvent	m_EventSend;
	CThreadEvent	m_EventComplete;

	unsigned        m_Param;
	int				m_ReturnVal;
};


// a unidirectional message queue. A queue of type T. Not especially high speed since each message
// is malloced/freed. Note that if your message class has destructors/constructors, they MUST be
// thread safe!
template<class T> class CMessageQueue
{
	CThreadEvent SignalEvent;								// signals presence of data
	CAtomicMutex QueueAccessMutex;

	// the parts protected by the mutex
	struct MsgNode
	{
		MsgNode *Next;
		T Data;
	};

	MsgNode *Head;
	MsgNode *Tail;

public:
	CMessageQueue( void )
	{
		Head = Tail = NULL;
	}

	// check for a message. not 100% reliable - someone could grab the message first
	bool MessageWaiting( void ) 
	{
		return ( Head != NULL );
	}

	void WaitMessage( T *pMsg )
	{
		for(;;)
		{
			while( ! MessageWaiting() )
				SignalEvent.Wait();
			QueueAccessMutex.Lock( __FILE__, __LINE__ );
			if (! Head )
			{
				// multiple readers could make this null
				QueueAccessMutex.Unlock( __FILE__, __LINE__ );
				continue;
			}
			*( pMsg ) = Head->Data;
			MsgNode *remove_this = Head;
			Head = Head->Next;
			if (! Head)										// if empty, fix tail ptr
				Tail = NULL;
			QueueAccessMutex.Unlock( __FILE__, __LINE__ );
			delete remove_this;
			break;
		}
	}

	void QueueMessage( T const &Msg)
	{
		MsgNode *new1=new MsgNode;
		new1->Data=Msg;
		new1->Next=NULL;
		QueueAccessMutex.Lock( __FILE__, __LINE__ );
		if ( Tail )
		{
			Tail->Next=new1;
			Tail = new1;
		}
		else
		{
			Head = new1;
			Tail = new1;
		}
		SignalEvent.Set();
		QueueAccessMutex.Unlock( __FILE__, __LINE__ );
	}
};


//-----------------------------------------------------------------------------
//
// CAtomicMutex. Inlining to reduce overhead and to allow client code
// to decide debug status (tracing)
//
//-----------------------------------------------------------------------------

inline void CAtomicMutex::Lock( const char *pFileName, int nLine )
{
	ThreadId_t thisThreadID = ThreadGetCurrentId();

	if(m_CurrentOwnerID == thisThreadID)
	{
		m_LockCount++;
	}
	else if(m_State || m_State.CompareExchange( (uint32)State::MASK, 0 ))
	{
		AcquireLock( thisThreadID );
	}
	else
	{
		m_LockCount = 1;
		m_CurrentOwnerID = thisThreadID;
	}
}

//---------------------------------------------------------

inline void CAtomicMutex::Unlock( const char *pFileName, int nLine )
{
	m_LockCount--;
	if(m_LockCount == 0)
	{
		m_CurrentOwnerID = 0;
		if(m_State.AtomicAdd(~(uint32)State::MASK + 1) != (uint32)State::MASK)
			ThreadAtomicNotifyOne( (uint32 *)&m_State );
	}
}

//---------------------------------------------------------

inline bool CAtomicMutex::AssertOwnedByCurrentThread()
{
	return ThreadGetCurrentId() == m_CurrentOwnerID;
}

//-----------------------------------------------------------------------------
//
// CThreadRWLock inline functions
//
//-----------------------------------------------------------------------------

inline CThreadRWLock::CThreadRWLock()
:	m_CanRead( true ),
	m_nWriters( 0 ),
	m_nActiveReaders( 0 ),
	m_nPendingReaders( 0 )
{
}

inline void CThreadRWLock::LockForRead( const char *pFileName, int nLine )
{
	m_mutex.Lock( pFileName, nLine );
	if ( m_nWriters)
	{
		WaitForRead();
	}
	m_nActiveReaders++;
	m_mutex.Unlock( pFileName, nLine );
}

inline void CThreadRWLock::UnlockRead( const char *pFileName, int nLine )
{
	m_mutex.Lock( pFileName, nLine );
	m_nActiveReaders--;
	if ( m_nActiveReaders == 0 && m_nWriters != 0 )
	{
		m_CanWrite.Set();
	}
	m_mutex.Unlock( pFileName, nLine );
}

// read data from a memory address
template<class T> FORCEINLINE T ReadVolatileMemory( T const *pPtr )
{
	volatile const T * pVolatilePtr = ( volatile const T * ) pPtr;
	return *pVolatilePtr;
}

//-----------------------------------------------------------------------------

#ifdef _LINUX
DLL_GLOBAL_IMPORT __thread int g_nThreadID;
#endif

#if defined( PLATFORM_WINDOWS )
#pragma warning(pop)
#endif

enum CopiedLockState_t : int32
{
	CLS_NOCOPY = 0,
	CLS_UNLOCKED = 1,
	CLS_LOCKED_BY_COPYING_THREAD = 2,
};

template < class MUTEX, CopiedLockState_t L = CLS_UNLOCKED >
class CCopyableLock : public MUTEX
{
	typedef MUTEX BaseClass;

public:
	// ...
};

#endif // THREADTOOLS_H
