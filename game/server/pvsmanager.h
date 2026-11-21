//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The PVS manager: per spawn group access to the engine's PVS.
//			Factory "PVSManager".
//
//=============================================================================//

#ifndef PVSMANAGER_H
#define PVSMANAGER_H
#ifdef _WIN32
#pragma once
#endif

#include "igamesystem.h"
#include "mathlib/vector.h"
#include "mathlib/aabb.h"
#include "tier0/utlvector.h"

class IPVS;
struct vis_info_t;

//-----------------------------------------------------------------------------
// Every method but the last takes the spawn group first (0 and -1 mean the active one), asks
// the engine for its IPVS
// and forwards to it.
//-----------------------------------------------------------------------------
abstract_class IPVSManager
{
public:
	virtual IPVS *GetPVS( SpawnGroupHandle_t hSpawnGroup ) = 0;
	// IPVS::GetClusterCount
	virtual int GetClusterCount( SpawnGroupHandle_t hSpawnGroup ) = 0;
	// IPVS::GetAllClusterBounds
	virtual int GetAllClusterBounds( SpawnGroupHandle_t hSpawnGroup, AABBWS_t *pBounds, int nMaxBounds ) = 0;
	// IPVS::GetClusterForPosition
	virtual int GetClusterForOrigin( SpawnGroupHandle_t hSpawnGroup, const Vector &vecOrigin ) = 0;
	// Stores the spawn group in pVisInfo, then IPVS::GetVisForCluster; false without a PVS
	virtual bool GetPVSForCluster( SpawnGroupHandle_t hSpawnGroup, int nCluster, vis_info_t *pVisInfo ) = 0;
	// IPVS::IsInPVS
	virtual bool CheckOriginInPVS( SpawnGroupHandle_t hSpawnGroup, const Vector &vecOrigin, const vis_info_t *pVisInfo ) = 0;
	// IPVS::IsAbsBoxInPVS
	virtual bool CheckBoxInPVS( SpawnGroupHandle_t hSpawnGroup, const Vector &vecMins, const Vector &vecMaxs, const vis_info_t *pVisInfo ) = 0;
	// Stores the spawn group in pVisInfo, then IPVS::ResetPVS
	virtual void ResetPVS( SpawnGroupHandle_t hSpawnGroup, vis_info_t *pVisInfo ) = 0;
	// IPVS::AddOriginToPVS
	virtual void AddOriginToPVS( SpawnGroupHandle_t hSpawnGroup, const Vector &vecOrigin, vis_info_t *pVisInfo ) = 0;
	// Forwards to the engine (IVEngineServer2)
	virtual void GetAllSpawnGroupsWithPVS( CUtlVector< SpawnGroupHandle_t > *pSpawnGroups, CUtlVector< IPVS * > *pPVS ) = 0;
};

//-----------------------------------------------------------------------------
// Reallocating game system, 32 bytes on both platforms (no members). libserver.so repeats
// the ten IPVSManager overrides at the end of the
// main table (slots 66..75).
//-----------------------------------------------------------------------------
class CPVSManager : public CAutoGameSystem, public IPVSManager
{
public:
	DECLARE_GAME_SYSTEM();

	// Marks the PVS state of every entity of one class as changed
	GS_EVENT( ActiveSpawnGroupChanged );

	// Forward to the game system factory
	virtual void SetGameSystemGlobalPtrs( void *pValue ) override;
	virtual bool DoesGameSystemReallocate() override;

	// IPVSManager
	virtual IPVS *GetPVS( SpawnGroupHandle_t hSpawnGroup ) override;
	virtual int GetClusterCount( SpawnGroupHandle_t hSpawnGroup ) override;
	virtual int GetAllClusterBounds( SpawnGroupHandle_t hSpawnGroup, AABBWS_t *pBounds, int nMaxBounds ) override;
	virtual int GetClusterForOrigin( SpawnGroupHandle_t hSpawnGroup, const Vector &vecOrigin ) override;
	virtual bool GetPVSForCluster( SpawnGroupHandle_t hSpawnGroup, int nCluster, vis_info_t *pVisInfo ) override;
	virtual bool CheckOriginInPVS( SpawnGroupHandle_t hSpawnGroup, const Vector &vecOrigin, const vis_info_t *pVisInfo ) override;
	virtual bool CheckBoxInPVS( SpawnGroupHandle_t hSpawnGroup, const Vector &vecMins, const Vector &vecMaxs, const vis_info_t *pVisInfo ) override;
	virtual void ResetPVS( SpawnGroupHandle_t hSpawnGroup, vis_info_t *pVisInfo ) override;
	virtual void AddOriginToPVS( SpawnGroupHandle_t hSpawnGroup, const Vector &vecOrigin, vis_info_t *pVisInfo ) override;
	virtual void GetAllSpawnGroupsWithPVS( CUtlVector< SpawnGroupHandle_t > *pSpawnGroups, CUtlVector< IPVS * > *pPVS ) override;
};

#endif // PVSMANAGER_H
