#ifndef SPAWNGROUPDESC_H
#define SPAWNGROUPDESC_H

#pragma once

#include "entity2/entityidentity.h"
#include "mathlib/mathlib.h"
#include "resourcefile/resourcetype.h"
#include "tier0/utlstring.h"
#include "worldrenderer/icomputeworldorigin.h"

class CCompressedResourceManifest;

struct SpawnGroupDesc_t
{
public:
	SpawnGroupDesc_t()
	 :  m_pWorldOffsetCallback(NULL),
	    m_sWorldGroupname("default"),
	    m_hOwner(0),
	    m_iPriorityLoader(-2),
	    m_manifestLoadPriority(RESOURCE_MANIFEST_LOAD_PRIORITY_DEFAULT),
	    m_flTimeoutInterval(0.0f),
	    m_bCreateClientEntitiesOnLaterConnectingClients(false),
	    m_bDontSpawnEntities(false),
	    m_bBlockUntilLoaded(false),
	    m_bLoadStreamingData(true),
	    m_bCreateNewSceneWorld(false),
	    m_bManualCompletion(false),
	    m_bSetActivePostLoad(false),
	    m_bIsNotPointPrefab(true),
	    m_bLevelTransition(false),
	    m_bReservedFlag1(false),
	    m_bReservedFlag2(false)
	{
		SetIdentityMatrix(m_vecWorldOffset);
	}

	CUtlString m_sWorldName;
	CUtlString m_sWorldMountName;
	CUtlString m_sEntityLumpName;
	CUtlString m_sEntityFilterName;
	CUtlString m_sDescriptiveName;
	CUtlString m_sParentNameFixup;
	CUtlString m_sLocalNameFixup;
	IComputeWorldOriginCallback *m_pWorldOffsetCallback;
	matrix3x4a_t m_vecWorldOffset;
	CUtlString m_sWorldGroupname;
	CUtlString m_sSaveFileName;
	SpawnGroupHandle_t m_hOwner;
	int m_iPriorityLoader;
	ResourceManifestLoadPriority_t m_manifestLoadPriority;
	float m_flTimeoutInterval;

	bool m_bCreateClientEntitiesOnLaterConnectingClients;
	bool m_bDontSpawnEntities;
	bool m_bBlockUntilLoaded;
	bool m_bLoadStreamingData;
	bool m_bCreateNewSceneWorld;
	bool m_bManualCompletion;
	bool m_bSetActivePostLoad;
	// Named after the earlier note that spawn groups created by point_prefab clear it; true by
	// default. ISpawnGroup::IsNotPointPrefab returns it; engine2 itself never reads it.
	bool m_bIsNotPointPrefab;
	bool m_bLevelTransition;
	// Named by what is known: kept by the spawn group (ISpawnGroup::GetReservedFlag1 returns the
	// first) and copied back by GetSpawnGroupDesc; engine2 never reads them otherwise.
	bool m_bReservedFlag1;
	bool m_bReservedFlag2;
};

struct SpawnGroupDescReceive_t
{
public:
	bool m_bIsInitialSpawnGroup;
	bool m_bCreateClientOnlyEntities;
	bool m_bIsSynchronousSpawn;
	CCompressedResourceManifest *m_pCompressedResource;
	int32 m_nTickCount;
	bool m_bManifestInComplete;
	SpawnGroupHandle_t m_hSpawnGroupParentHandle;
	WorldGroupId_t m_hWorldGroupId;
};

#endif // SPAWNGROUPDESC_H
