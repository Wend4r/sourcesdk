#ifndef RESOURCESYSTEM_RESOURCEMANIFESTHELPERS_H
#define RESOURCESYSTEM_RESOURCEMANIFESTHELPERS_H

#ifdef _WIN32
#pragma once
#endif

#include <initializer_list>

class CStrongHandleVoid;

struct ResourceManifestEntry_t
{
	const char *m_pResourceName;
	bool m_bResourceNameIsManifestName;
	CStrongHandleVoid *m_pHandle;
	ResourceManifestEntry_t *m_pNextInstanceOfThisEntry;
	int m_nHandleReferences;
};

typedef std::initializer_list< std::initializer_list< ResourceManifestEntry_t > > ManifestEntryGroupList_t;

struct ResourceManifestDesc_t
{
	const char *m_pszManifestName;
	const char *m_pszManifestGroup;
	ManifestEntryGroupList_t *m_pEntryGroups;
	const char *m_pszFileName;
	int m_nLine;
	bool m_bRegistered;
	bool m_bDisallowRegistration;
};

#endif // RESOURCESYSTEM_RESOURCEMANIFESTHELPERS_H
