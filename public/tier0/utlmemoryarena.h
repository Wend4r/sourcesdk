#ifndef UTLMEMORYARENA_H
#define UTLMEMORYARENA_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "tier0/logging.h"

//-----------------------------------------------------------------------------
// Allocator over one block of reserved virtual memory
//-----------------------------------------------------------------------------
class CUtlMemoryArena
{
public:
	// bAllowLarge reserves the memory with PLAT_VIRTUAL_ALLOW_LARGE_RESERVATION
	DLL_CLASS_IMPORT CUtlMemoryArena( uint32 nSize, bool bAllowLarge );
	DLL_CLASS_IMPORT ~CUtlMemoryArena();

	DLL_CLASS_IMPORT byte *Alloc( uint32 nSize );
	DLL_CLASS_IMPORT uint32 Free( byte *pMem );
	DLL_CLASS_IMPORT uint32 MaxAvailableAllocSize();

	DLL_CLASS_IMPORT const byte *MemoryBegin() const;
	DLL_CLASS_IMPORT const byte *MemoryEnd() const;

	DLL_CLASS_IMPORT void DumpBlockList( LoggingChannelID_t channelID );

private:
	// Block header in front of every allocation; the links are offsets from the start of the memory
	struct mem_block_t
	{
		uint32 m_nBytesSize;
		uint32 m_bInUse;
		uint32 m_nPrevOffset;
		uint32 m_nNextOffset;
	};

	mem_block_t *m_pBlockList;
	mem_block_t *m_pAllocStart;
	byte *m_pMemory;
	uint32 m_nAllocationSize;
	uint32 m_nAllocStatsId;

	CUtlMemoryArena( const CUtlMemoryArena & ) = delete;
	CUtlMemoryArena &operator=( const CUtlMemoryArena & ) = delete;
};

#endif // UTLMEMORYARENA_H
