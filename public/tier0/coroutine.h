//====== Copyright 1996-2004, Valve Corporation, All rights reserved. =======
//
// Purpose: setjmp/longjmp based cooperative multitasking system, exported by tier0
//
//=============================================================================

#ifndef TIER0_COROUTINE_H
#define TIER0_COROUTINE_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"

// coroutine callback
#ifdef POSIX
typedef void (*CoroutineFunc_t )(void *);
#else
typedef void (__cdecl *CoroutineFunc_t )(void *);
#endif

// handle to a coroutine, an index into the coroutines of the thread
typedef int32 HCoroutine;

// creates a new coroutine
// no coroutine code is executed until Coroutine_Continue() is called
PLATFORM_INTERFACE HCoroutine Coroutine_Create( CoroutineFunc_t pFunc, void *pvParam );

// continues the specified coroutine
// returns true if the coroutine is still running, false otherwise
PLATFORM_INTERFACE bool Coroutine_Continue( HCoroutine hCoroutine, const char *pchName = NULL );

// cancels a currently running coroutine
PLATFORM_INTERFACE void Coroutine_Cancel( HCoroutine hCoroutine );

// called from the coroutine to return control to the main thread
PLATFORM_INTERFACE void Coroutine_YieldToMain();

// returns true if the code is currently running inside of a coroutine
PLATFORM_INTERFACE bool Coroutine_IsActive();

#ifdef PLATFORM_LINUX
// Assembly of the POSIX coroutines: pushes the non-volatile registers on the current stack,
// stores where they begin and end in *ppStackHigh and *ppStackLow, runs pfnExec( pvParam ) and
// then pfnFinish, which must not return
PLATFORM_INTERFACE void Coroutine_Launch_ASM( byte **ppStackHigh, byte **ppStackLow, void (*pfnExec)( void * ), void *pvParam, void (*pfnFinish)() );

// Stores rbx, rbp, r12, r13, r14 and r15 in that order
PLATFORM_INTERFACE void SaveNonVolatileRegs( uint64 *pRegs );
#endif

#endif // TIER0_COROUTINE_H
