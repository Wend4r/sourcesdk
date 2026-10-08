#include "iresourcesystem.h"
#include "resourcefile/manifestregistrar.h"
#include "resourcefile/resourcetype.h"

static CManifestRegistrar *s_pFirst = NULL;
static int s_nManifestCount = 0;

CManifestRegistrar::CManifestRegistrar( ResourceManifestDesc_t *pDesc ) :
	m_pNext( s_pFirst ),
	m_pDesc( pDesc ),
	m_pfnGetDesc( NULL )
{
	s_pFirst = this;
	s_nManifestCount++;
}

ResourceManifestDesc_t *CManifestRegistrar::GetDesc()
{
	if ( !m_pDesc && m_pfnGetDesc )
	{
		m_pDesc = m_pfnGetDesc();
		m_pfnGetDesc = NULL;
	}

	return m_pDesc;
}

void CManifestRegistrar::RegisterAll()
{
	for ( CManifestRegistrar *pCur = s_pFirst; pCur; pCur = pCur->m_pNext )
	{
		ResourceManifestDesc_t *pDesc = pCur->GetDesc();

		if ( !pDesc->m_bRegistered && !pDesc->m_bDisallowRegistration )
		{
			pDesc->m_bRegistered = true;
			g_pResourceManifestRegistry->RegisterManifest( pDesc );
		}
	}
}

void CManifestRegistrar::UnregisterManifests( ResourceManifestDesc_t **pManifests, int nManifests )
{
	for ( int i = 0; i < nManifests; i++ )
	{
		ResourceManifestDesc_t *pDesc = pManifests[i];

		if ( pDesc->m_bRegistered )
		{
			g_pResourceManifestRegistry->UnregisterManifest( pDesc );
			pDesc->m_bRegistered = false;
		}
	}
}

void CManifestRegistrar::UnregisterAll()
{
	for ( CManifestRegistrar *pCur = s_pFirst; pCur; pCur = pCur->m_pNext )
	{
		// Unresolved descriptors were never registered
		ResourceManifestDesc_t *pDesc = pCur->m_pDesc;

		if ( pDesc && pDesc->m_bRegistered )
		{
			g_pResourceManifestRegistry->UnregisterManifest( pDesc );
			pDesc->m_bRegistered = false;
		}
	}
}

int CManifestRegistrar::GetCount()
{
	return s_nManifestCount;
}

CManifestRegistrar *CManifestRegistrar::GetFirst()
{
	return s_pFirst;
}
