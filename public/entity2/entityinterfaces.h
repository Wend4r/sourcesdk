#ifndef ENTITY2_ENTITYINTERFACES_H
#define ENTITY2_ENTITYINTERFACES_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/threadtools.h"
#include "entityprecachecontext.h"
#include "entityio.h"

class CEntityKeyValues;
class IEntityPrecacheConfiguration;
class IEntityResourceManifestBuilder;

struct EntityNotification_t
{
	CEntityIdentity* m_pEntity;
};

struct EntitySpawnInfo_t : EntityNotification_t
{
	const CEntityKeyValues* m_pKeyValues;

	// Named by what it does: the index the save restore system gives an entity restored from a save file, -1 for an entity of the map
	int m_nSaveFileEntityIndex;

	// Position in the queue; the last sort key
	uint16 m_SpawnOrder;

	// Depth in the parent hierarchy given by "parentname", 1 for an entity without a parent; the first sort key
	uint8 m_Depth;

	// Named by what it does: copied from the entity class, a higher value spawns first among the entities of the same depth
	uint8 m_ClassSpawnOrder;
};

struct SecondaryPrecacheMemberCallback_t
{
	void (CEntityInstance::*pfnPrecache)(ResourceHandle_t hResource, const CEntityPrecacheContext* pContext);
};

struct CEntityResourceManifestLock
{
	IEntityResourceManifestBuilder* m_pBuilder;
	matrix3x4a_t m_vSpawnGroupOffset;
	SpawnGroupHandle_t m_hSpawnGroup;
	IEntityPrecacheConfiguration* m_pConfig;
	IEntityResourceManifest* m_pManifest;
	bool m_bIsLocked;
	bool m_bPrecacheEnable;
};

#endif // ENTITY2_ENTITYINTERFACES_H
