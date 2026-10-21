#ifndef POOLALLOCATOR_H
#define POOLALLOCATOR_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "tier0/utlvector.h"

//-----------------------------------------------------------------------------
// Allocates ranges of an abstract pool of nPoolSize units, the memory itself is
// managed by the caller
//-----------------------------------------------------------------------------
class CPoolAllocator
{
public:
	struct Handle_t
	{
		uint32 m_nOffset;
		uint32 m_nSize; // 0 for a failed allocation
	};

	DLL_CLASS_IMPORT CPoolAllocator( uint32 nPoolSize );
	DLL_CLASS_IMPORT ~CPoolAllocator();

	DLL_CLASS_IMPORT Handle_t Allocate( uint32 nSize, uint32 nAlignment );
	DLL_CLASS_IMPORT bool Free( Handle_t hAllocation );
	DLL_CLASS_IMPORT void GrowPool( uint32 nNewPoolSize );

	DLL_CLASS_IMPORT uint32 ActivePoolHighWatermark() const;
	DLL_CLASS_IMPORT void Debug_GetAllocationsSnapshot( CUtlVector< Handle_t > *pAllocations ) const;

	uint32 GetPoolSize() const { return m_nPoolSize; }

private:
	uint32 m_nPoolSize;
	uint32 m_nAllocatedSize;
	uint32 m_nPeakAllocatedSize;

	// Trees of the free and the allocated ranges
	void *m_pImpl;

	CPoolAllocator( const CPoolAllocator & ) = delete;
	CPoolAllocator &operator=( const CPoolAllocator & ) = delete;
};

#endif // POOLALLOCATOR_H
