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

	// Every vertex at the nearest distance
	DLL_CLASS_IMPORT void Nearest( const Vec3D< float > &vPos, CUtlVectorFixedGrowable< const float *, 64 > &nearest ) const;

	DLL_CLASS_IMPORT void FindVertsInBox( CUtlVectorFixed< const float *, 64 > &verts, const Vec3D< float > &vMins, const Vec3D< float > &vMaxs, int nNode ) const;
	DLL_CLASS_IMPORT void FindVertsInBox( CUtlVectorFixedGrowable< const float *, 64 > &verts, const Vec3D< float > &vMins, const Vec3D< float > &vMaxs, int nNode ) const;
	DLL_CLASS_IMPORT void FindVertsInSphere( CUtlVectorFixedGrowable< const float *, 64 > &verts, const Vec3D< float > &vCenter, float flRadius, int nNode ) const;

	// The vertices at a distance from flInnerRadius to flOuterRadius
	DLL_CLASS_IMPORT void FindVertsInSphericalShell( CUtlVectorFixedGrowable< const float *, 64 > &verts, const Vec3D< float > &vCenter, float flInnerRadius, float flOuterRadius, int nNode ) const;

	DLL_CLASS_IMPORT void ForEachInBox( const Vec3D< float > &vMins, const Vec3D< float > &vMaxs, const std::function< void ( const float * ) > &fnCallback, int nNode ) const;
	DLL_CLASS_IMPORT void ForEachInSphere( const Vec3D< float > &vCenter, float flRadius, const std::function< void ( const float * ) > &fnCallback, int nNode ) const;

private:
	DLL_CLASS_IMPORT int BuildNode( int nFirstVert, int nVertCount );
	DLL_CLASS_IMPORT void NearestInternal( const Vec3D< float > &vPos, float &flBestDistSqr, const float **ppBest, int nNode ) const;
	DLL_CLASS_IMPORT void NearestInternal( const Vec3D< float > &vPos, float &flBestDistSqr, CUtlVectorFixedGrowable< const float *, 64 > &nearest, int nNode ) const;

	CUtlVector< CVertexKDNode > m_tree;
	CUtlBlockVector< const float * > m_vertexList;
};

#endif // VERTEXKDTREE_H
