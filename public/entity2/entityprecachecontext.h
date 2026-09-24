#ifndef ENTITYPRECACHECONTEXT_H
#define ENTITYPRECACHECONTEXT_H

#if _WIN32
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

#endif // ENTITYPRECACHECONTEXT_H
