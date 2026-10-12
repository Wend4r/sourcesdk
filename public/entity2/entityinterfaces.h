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
	int m_Unk1;
	uint16 m_SpawnOrder;
	uint8 m_Depth;
	uint8 m_OriginalSpawnOrder;
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
