#ifndef MAPSPAWNGROUP_H
#define MAPSPAWNGROUP_H

#pragma once

#include "ispawngroup.h"

class IPVS;

class CMapSpawnGroup
{
public:
	const char *GetWorldName() const { return m_pSpawnGroup->GetWorldName(); }
	const char *GetEntityLumpName() const { return m_pSpawnGroup->GetEntityLumpName(); }
	const char *GetEntityFilterName() const { return m_pSpawnGroup->GetEntityFilterName(); }
	SpawnGroupHandle_t GetSpawnGroupHandle() const { return m_pSpawnGroup->GetHandle(); }
	const matrix3x4a_t &GetWorldOffset() const { return m_pSpawnGroup->GetWorldOffset(); }
	const char *GetParentNameFixup() const { return m_pSpawnGroup->GetParentNameFixup(); }
	const char *GetLocalNameFixup() const { return m_pSpawnGroup->GetLocalNameFixup(); }
	SpawnGroupHandle_t GetOwnerSpawnGroup() const { return m_pSpawnGroup->GetOwnerSpawnGroup(); }
	ILoadingSpawnGroup *GetLoadingSpawnGroup() const { return m_pSpawnGroup->GetLoadingSpawnGroup(); }
	void SetLoadingSpawnGroup(ILoadingSpawnGroup *pLoading) { m_pSpawnGroup->SetLoadingSpawnGroup(pLoading); }
	int GetCreationTick() const { return m_pSpawnGroup->GetCreationTick(); }
	bool DontSpawnEntities() const { return m_pSpawnGroup->DontSpawnEntities(); }
	void GetSpawnGroupDesc(SpawnGroupDesc_t *pDesc) const { m_pSpawnGroup->GetSpawnGroupDesc(pDesc); }

public:
	ISpawnGroup *GetSpawnGroup() const { return m_pSpawnGroup; }
	IPVS *GetPVS() const { return m_pPVS; }

private:
	ISpawnGroup *m_pSpawnGroup;
	bool m_bSpawnGroupPrecacheDispatched;
	bool m_bSpawnGroupLoadDispatched;
	IPVS *m_pPVS;
	// CUtlVector<SpawnGroupConnectionInfo_t> m_Connections;
};

#endif // MAPSPAWNGROUP_H
