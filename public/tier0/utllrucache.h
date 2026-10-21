#ifndef UTLLRUCACHE_H
#define UTLLRUCACHE_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "tier0/utlvector.h"

//-----------------------------------------------------------------------------
// Controls the registered LRU caches by name, a null name affects every cache
// where the member allows it
//-----------------------------------------------------------------------------
class CUtlLRUCacheBase
{
public:
	DLL_CLASS_IMPORT static void SetCacheSize( const char *pszCacheName, int nSize );
	DLL_CLASS_IMPORT static void Flush( const char *pszCacheName );
	DLL_CLASS_IMPORT static void ResetCacheStats( const char *pszCacheName );
	DLL_CLASS_IMPORT static void SpewStats();

	// Called by the statics above for the caches of the name
	virtual void SetCacheSize( int nSize ) = 0;
	virtual void Flush() = 0;

protected:
	int m_nCacheSize;

	// The statistics, ResetCacheStats() zeroes the counters and restarts m_nStatsStartTime
	int m_nRequests;
	time_t m_nStatsStartTime; // Plat_GetTime()
	int m_nHits;
	int m_nEvictions;
};

// An entry of the registry the statics of CUtlLRUCacheBase search by name
struct LRUCacheRegistryEntry_t
{
	const char *m_pszName;
	CUtlLRUCacheBase *m_pCache;
};

typedef CUtlVector< LRUCacheRegistryEntry_t > CUtlLRUCacheRegistry;

PLATFORM_INTERFACE CUtlLRUCacheRegistry *GetLRUCacheRegistry();

//-----------------------------------------------------------------------------
// The registry entry of a cache for the lifetime of the object
//-----------------------------------------------------------------------------
class CUtlLRUCacheRegistration
{
public:
	CUtlLRUCacheRegistration( const char *pszCacheName, CUtlLRUCacheBase *pCache ) : m_pszName( pszCacheName ), m_pCache( pCache )
	{
		LRUCacheRegistryEntry_t entry = { pszCacheName, pCache };
		GetLRUCacheRegistry()->AddToTail( entry );
	}

	~CUtlLRUCacheRegistration()
	{
		CUtlLRUCacheRegistry *pRegistry = GetLRUCacheRegistry();

		for ( int i = pRegistry->Count(); --i >= 0; )
		{
			const LRUCacheRegistryEntry_t &entry = pRegistry->Element( i );

			// The same cache can be registered under several names
			if ( entry.m_pCache == m_pCache && entry.m_pszName == m_pszName )
			{
				pRegistry->Remove( i );
				break;
			}
		}
	}

	CUtlLRUCacheRegistration( const CUtlLRUCacheRegistration & ) = delete;
	CUtlLRUCacheRegistration &operator=( const CUtlLRUCacheRegistration & ) = delete;

private:
	const char *m_pszName;
	CUtlLRUCacheBase *m_pCache;
};

#endif // UTLLRUCACHE_H
