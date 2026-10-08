#ifndef MANIFESTREGISTRAR_H
#define MANIFESTREGISTRAR_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"

struct ResourceManifestDesc_t;

//-----------------------------------------------------------------------------
// Per-module list of resource manifests, exposed through GetResourceManifests()
//-----------------------------------------------------------------------------
class CManifestRegistrar
{
public:
	typedef ResourceManifestDesc_t *(*GetDescFn_t)();

	CManifestRegistrar( ResourceManifestDesc_t *pDesc );

	// Resolves the lazy getter on first access
	ResourceManifestDesc_t *GetDesc();

	static void RegisterAll();
	static void UnregisterManifests( ResourceManifestDesc_t **pManifests, int nManifests );
	static void UnregisterAll();

	static int GetCount();
	static CManifestRegistrar *GetFirst();

	CManifestRegistrar *m_pNext;
	ResourceManifestDesc_t *m_pDesc;
	GetDescFn_t m_pfnGetDesc;
};

#endif // MANIFESTREGISTRAR_H
