#ifndef VERTEXKDTREE_H
#define VERTEXKDTREE_H

#ifdef _WIN32
#pragma once
#endif

#include <functional>

#include "tier0/platform.h"
#include "tier0/utlvector.h"
#include "tier0/utlblockvector.h"
#include "mathlib/vec3d.h"

class CVertexKDNode
{
public:
	// Child nodes of a split node, the first vertex and the vertex count of a leaf
	int32 m_nChildren[ 2 ];

	// Split axis, 255 for a leaf
	int32 m_nAxis;
	float m_flSplit;
};

//-----------------------------------------------------------------------------
// KD tree over vertex positions, the vertices are not copied
//-----------------------------------------------------------------------------
class CVertexKDTree
{
public:
	// pVerts points at nVertCount positions, nStride floats apart
	DLL_CLASS_IMPORT void BuildMidpoint( const float *pVerts, int nVertCount, int nStride );
	DLL_CLASS_IMPORT void BuildMidpointFromVertexSubSet( const float *pVerts, int nStride, const int *pIndices, int nIndexCount );

	DLL_CLASS_IMPORT const float *Nearest( const Vec3D< float > &vPos ) const;

	DLL_CLASS_IMPORT void ForEachInBox( const Vec3D< float > &vMins, const Vec3D< float > &vMaxs, const std::function< void ( const float * ) > &fnCallback, int nNode ) const;
	DLL_CLASS_IMPORT void ForEachInSphere( const Vec3D< float > &vCenter, float flRadius, const std::function< void ( const float * ) > &fnCallback, int nNode ) const;

private:
	DLL_CLASS_IMPORT int BuildNode( int nFirstVert, int nVertCount );
	DLL_CLASS_IMPORT void NearestInternal( const Vec3D< float > &vPos, float &flBestDistSqr, const float **ppBest, int nNode ) const;

	CUtlVector< CVertexKDNode > m_tree;
	CUtlBlockVector< const float * > m_vertexList;
};

#endif // VERTEXKDTREE_H
