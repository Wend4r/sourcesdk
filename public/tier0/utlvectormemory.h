#ifndef TIER0_UTLMEMORY_H
#define TIER0_UTLMEMORY_H

#ifdef COMPILER_MSVC
#pragma once
#endif

#include "platform.h"

PLATFORM_INTERFACE int		UtlVectorMemory_CalcNewAllocationCount( int nAllocationCount, int nGrowSize, int nNewSize, int nBytesItem );
PLATFORM_INTERFACE void*	UtlVectorMemory_Alloc( void* pMem, bool bRealloc, int nNewSize, int nOldSize );
PLATFORM_INTERFACE void*	UtlVectorMemory_AllocAligned( void* pMem, bool bRealloc, int nNewSize, int nOldSize, size_t nAlign = 0 );
PLATFORM_INTERFACE void		UtlVectorMemory_FailedAllocation( int nTotalElements, int nNewElements );

#endif // TIER0_UTLMEMORY_H
