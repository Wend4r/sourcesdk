//========= Copyright (c) 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: Memory allocation!
//
// $NoKeywords: $
//=============================================================================//

#ifndef TIER0_MEM_H
#define TIER0_MEM_H

#ifdef _WIN32
#pragma once
#endif

#include <stddef.h>


#include "tier0/platform.h"

#if !defined(STATIC_TIER0) && !defined(_STATIC_LINKED)

#ifdef TIER0_DLL_EXPORT
#  define MEM_INTERFACE DLL_EXPORT
#else
#  define MEM_INTERFACE DLL_IMPORT
#endif

#else // BUILD_AS_DLL

#define MEM_INTERFACE extern

#endif // BUILD_AS_DLL



//-----------------------------------------------------------------------------
// DLL-exported methods for particular kinds of memory
//-----------------------------------------------------------------------------
MEM_INTERFACE void *MemAllocScratch( int nMemSize );
MEM_INTERFACE void MemFreeScratch();

#if defined( POSIX )
MEM_INTERFACE void ZeroMemory( void *mem, size_t length );
#endif

//Only works with USE_MEM_DEBUG and memory allocation call stack tracking enabled.
MEM_INTERFACE int GetAllocationCallStack( void *mem, void **pCallStackOut, int iMaxEntriesOut );

MEM_INTERFACE void MemOutOfMemory( size_t nBytesAttempted );

//-----------------------------------------------------------------------------
// The bump allocator of the current thread: 256 KB from g_pMemAlloc that the
// allocations take from m_pNext; the names of the type and its members are ours
//-----------------------------------------------------------------------------
struct ThreadLocalStackAllocator_t
{
	uint8 *m_pNext;
	uint64 m_nBytesLeft;
	int m_nAllocations;
	uint8 *m_pBase;
	uint8 *m_pLimit;
};

PLATFORM_INTERFACE ThreadLocalStackAllocator_t *GetThreadLocalStackAllocator();

// Fills the memory with a debug pattern in debug builds of tier0, returns pMemory
PLATFORM_INTERFACE void *DebugBuild_InitFillMemory( void *pMemory );

//-----------------------------------------------------------------------------
// One step of TransferMemory, which copies m_nBytes at m_nSrcOffset of the source stream
// to m_nDstOffset of the destination stream for every element; the names are ours
//-----------------------------------------------------------------------------
struct TransferMemoryStep_t
{
	enum Op_t : uint8
	{
		OP_COPY = 0,
		OP_SKIP = 1,
		OP_END = 2,
	};

	uint8 m_nOp;
	uint8 m_nSrcStream;
	uint8 m_nDstStream;
	uint8 m_nPad;
	uint16 m_nSrcOffset;
	uint16 m_nDstOffset;
	uint32 m_nBytes;
	uint32 m_nPad2;
};

// Runs the steps up to OP_END for nCount elements; the streams advance by their strides
// pUnknown and nUnknown are not read by any build
PLATFORM_INTERFACE void TransferMemory( const TransferMemoryStep_t *pSteps, void *pUnknown, uint8 *const *ppSrcStreams, const int *pSrcStrides, int nUnknown, uint8 *const *ppDstStreams, const int *pDstStrides, int nCount );

#endif /* TIER0_MEM_H */
