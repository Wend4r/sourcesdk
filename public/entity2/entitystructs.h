#ifndef ENTITY2_ENTITYSTRUCTS_H
#define ENTITY2_ENTITYSTRUCTS_H

#ifdef _WIN32
#pragma once
#endif

class CEntityKeyValues;
class IEntityPrecacheConfiguration;
class IEntityResourceManifest;

struct CEntityPrecacheContext
{
	const CEntityKeyValues* m_pKeyValues;
	IEntityPrecacheConfiguration* m_pConfig;
	IEntityResourceManifest* m_pManifest;
};

#endif // ENTITY2_ENTITYSTRUCTS_H
