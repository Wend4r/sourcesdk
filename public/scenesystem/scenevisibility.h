#ifndef SCENEVISIBILITY_H
#define SCENEVISIBILITY_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "mathlib/aabb.h"
#include "mathlib/obb.h"
#include "mathlib/vectorws.h"
#include "tier0/bitvec.h"

class CFrustum;
class CVoxelVisibility;
struct vis_info_t;

struct VisClusterWord_t
{
	uint16 m_nCount;
	uint16 m_nWord;
	uint32 m_nBits;
};

//-----------------------------------------------------------------------------
// Purpose: The potentially visible set of a spawn group, from IVEngineServer2::GetPVSForSpawnGroup().
// Positions are in world space unless a method says otherwise.
// A vis_info_t with a cluster count of 0 sees every cluster.
//-----------------------------------------------------------------------------
abstract_class IPVS
{
public:
	virtual int GetClustersForOrigin( uint32 *pClusters, int nMaxClusters, const VectorWS &vecOrigin ) = 0;
	virtual int GetClustersForBounds( uint32 *pClusters, int nMaxClusters, const VectorWS &vecMins, const VectorWS &vecMaxs, bool bIsStatic ) = 0;
	virtual int GetClustersForOrientedBounds( uint32 *pClusters, int nMaxClusters, const OBB_t &orientedBox, bool bIsStatic ) = 0;
	virtual int GetClustersForFrustum( uint32 *pClusters, int nMaxClusters, const CFrustum *pFrustum, bool bIsStatic ) = 0;
	virtual int GetVisibleEverywhereClusterList( uint32 *pClusters, int nListMax ) = 0;
	virtual int FilterClustersInRadius( uint32 *pClusters, int nClusters, const VectorWS &vecOrigin, float flMaxDistance ) = 0;

	virtual int GetClusterCount() = 0;
	virtual int GetAllClusterBounds( AABBWS_t *pBounds, int nMaxBounds ) = 0;
	virtual int GetClusterForPosition( const VectorWS &vecOrigin ) = 0;

	virtual const VectorWS *GetVisDebugPosition() = 0;
	virtual bool HasTwoBaseClusters() = 0;

	virtual bool IsInPVS( const VectorWS &vecOrigin, const vis_info_t *pVisInfo ) = 0;
	virtual bool IsAbsBoxInPVS( const VectorWS &vecMins, const VectorWS &vecMaxs, const vis_info_t *pVisInfo ) = 0;
	virtual bool GetClusterBounds( int nCluster, VectorWS *pMins, VectorWS *pMaxs ) = 0;
	virtual bool IsSkyVisibleFromPosition( const VectorWS &vecOrigin ) = 0;
	virtual bool IsSunVisibleFromPosition( const VectorWS &vecOrigin ) = 0;

	virtual void GetVisForCluster( int nCluster, vis_info_t *pVisInfo ) = 0;
	// AMNOTE: nClusters is the bit count, not the number of uint32 words
	virtual void GetPVSForClusters( const uint32 *pClusterBits, int nClusters, vis_info_t *pVisInfo ) = 0;

	virtual bool IsSunVisibleInPVS( const vis_info_t *pVisInfo ) = 0;

	virtual void ResetPVS( vis_info_t *pVisInfo ) = 0;
	// AMNOTE: Takes spawn-group-local coordinates (world position minus GetWorldOffset())
	virtual void AddOriginToPVS( const Vector &vecLocalOrigin, vis_info_t *pVisInfo ) = 0;
	// AMNOTE: Takes spawn-group-local coordinates
	virtual void GetPrecisePVS( vis_info_t *pVisInfo, const Vector &vecLocalOrigin ) = 0;
	virtual void GetSunlightPVS( vis_info_t *pVisInfo ) = 0;
	virtual void GetOrthoPVS( vis_info_t *pVisInfo ) = 0;

	virtual int GetClusterBitsInBox( CBitVec<4096> *pClusters, const VectorWS &vecMins, const VectorWS &vecMaxs, bool bIsStatic ) = 0;

	// AMNOTE: Takes spawn-group-local coordinates (world bounds minus GetWorldOffset())
	virtual bool IsBoxInVisBounds( const AABB_t &box ) = 0;
	virtual bool CheckBoxInCluster( const AABBWS_t &box, int nCluster ) = 0;

	virtual CVoxelVisibility *GetVoxelVisibility() = 0;

	virtual VectorWS GetWorldOffset() = 0;
	virtual void SnapBoxToVisCells( AABBWS_t *pBox, int nExtraCells ) = 0;

	virtual int GetClusterWordsInBox( VisClusterWord_t *pEntries, int nMaxEntries, const VectorWS &vecMins, const VectorWS &vecMaxs, bool bIsStatic ) = 0;
};

#endif // SCENEVISIBILITY_H
