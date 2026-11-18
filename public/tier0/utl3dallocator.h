#ifndef UTL3DALLOCATOR_H
#define UTL3DALLOCATOR_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "tier0/basetypes.h"
#include "tier0/utlvector.h"
#include "tier0/utlintrusivelist.h"
#include "mathlib/intvector3d.h"

//-----------------------------------------------------------------------------
// Allocates boxes from a 3D volume, keeps the free regions bucketed by log2 of their volume
//-----------------------------------------------------------------------------
class CUtl3DAllocator
{
public:
	DLL_CLASS_IMPORT void Reset( int nWidth, int nHeight, int nDepth );
	// Takes the free block with the least waste; with bUseClosestBlock and no block large enough,
	// the one closest in size, and pResult is cut to it
	DLL_CLASS_IMPORT bool Allocate3D( int nWidth, int nHeight, int nDepth, bool bUseClosestBlock, Rect3D_t *pResult );

	int TotalFree() const { return m_nTotalFree; }

private:
	struct FreeRegion_t
	{
		FreeRegion_t *m_pNext;
		FreeRegion_t *m_pPrev;
		Rect3D_t m_rect;
	};

	CUtlIntrusiveDList< FreeRegion_t > m_freeRegions[ 32 ];
	int m_nTotalFree;
};

//-----------------------------------------------------------------------------
// Allocates boxes from a 3D volume split as a tree of octants
//-----------------------------------------------------------------------------
class CUtl3DAllocator2
{
public:
	DLL_CLASS_IMPORT CUtl3DAllocator2();

	DLL_CLASS_IMPORT void Reset( int nWidth, int nHeight, int nDepth );
	DLL_CLASS_IMPORT bool Allocate3D( int nWidth, int nHeight, int nDepth, Rect3D_t *pResult );
	DLL_CLASS_IMPORT void Free3D( const Rect3D_t &rect );

private:
	CUtl3DAllocator2( const CUtl3DAllocator2 & ) = delete;

	struct Node3D_t
	{
		IntVector3D m_vSplitPoint;
		int m_ChildNodes[ 8 ];
	};

	CUtlVectorFixedGrowable< Node3D_t, 64 > m_Nodes;
	CUtlVectorFixedGrowable< int, 64 > m_FreeNodes;
	IntVector3D m_vSize;
};

#endif // UTL3DALLOCATOR_H
