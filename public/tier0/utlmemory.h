#ifndef TIER0_UTLMEMORY_H
#define TIER0_UTLMEMORY_H

#ifdef COMPILER_MSVC
#pragma once
#endif

#include "platform.h"

PLATFORM_INTERFACE int		UtlMemory_CalcNewAllocationCount( int nAllocationCount, int nGrowSize, int nNewSize, int nBytesItem );
PLATFORM_INTERFACE void*	UtlMemory_Alloc( void* pMem, bool bRealloc, int nNewSize, int nOldSize );
PLATFORM_INTERFACE void		UtlMemory_FailedAllocation( int nTotalElements, int nNewElements );

#endif // TIER0_UTLMEMORY_H
