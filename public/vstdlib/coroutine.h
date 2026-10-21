//====== Copyright 1996-2004, Valve Corporation, All rights reserved. =======
//
// Purpose: setjmp/longjmp based cooperative multitasking system
//
//=============================================================================

#ifndef COROUTINE_H
#define COROUTINE_H
#pragma once

#include "vstdlib/vstdlib.h"

// enable this to do coroutine tracing
// this will tell coroutine API users to set coroutine names
// #define COROUTINE_TRACE

//-----------------------------------------------------------------------------
// Purpose: handles running coroutines
//			setjmp/longjmp based cooperative multitasking system
//-----------------------------------------------------------------------------

// The functions that tier0 still exports
#include "tier0/coroutine.h"

// 'load' a coroutine only to debug it - immediately breaks into debugger
// when continued, pops back to the prior coroutine
VSTDLIB_INTERFACE void Coroutine_DebugBreak( HCoroutine hCoroutine );

// Load a coroutine and generate an assert.  Used to get a minidump of a job
VSTDLIB_INTERFACE void Coroutine_DebugAssert( HCoroutine hCoroutine, const char *pchMsg );

// returns a handle the currently active coroutine
VSTDLIB_INTERFACE HCoroutine Coroutine_GetCurrentlyActive();

// call when a thread is quiting to release any per-thread memory
VSTDLIB_INTERFACE void Coroutine_ReleaseThreadMemory();

// runs a self-test of the coroutine system
VSTDLIB_INTERFACE bool Coroutine_Test();

// memory validation 
VSTDLIB_INTERFACE void Coroutine_ValidateGlobals( class CValidator &validator );

// for debugging purposes - returns stack depth of current coroutine
VSTDLIB_INTERFACE size_t Coroutine_GetStackDepth();



#endif // COROUTINE_H
