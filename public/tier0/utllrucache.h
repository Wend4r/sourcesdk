#ifndef UTLLRUCACHE_H
#define UTLLRUCACHE_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "tier0/dbg.h"
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

	// Registers the cache under pszCacheName, which is kept rather than copied; the derived cache
	// sets its size and the statistics
	CUtlLRUCacheBase( const char *pszCacheName );

	// Called by the statics above for the caches of the name
	virtual void SetCacheSize( int nSize ) = 0;

	// Removes every entry, Purge also frees the memory; the name of Purge is ours
	virtual void Flush() = 0;
	virtual void Purge() = 0;

	// Removes the cache from the registry
	virtual ~CUtlLRUCacheBase();

	CUtlLRUCacheBase( const CUtlLRUCacheBase & ) = delete;
	CUtlLRUCacheBase &operator=( const CUtlLRUCacheBase & ) = delete;

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

inline CUtlLRUCacheBase::CUtlLRUCacheBase( const char *pszCacheName )
{
	CUtlLRUCacheRegistry *pRegistry = GetLRUCacheRegistry();

	FOR_EACH_VEC( *pRegistry, i )
	{
		if ( pRegistry->Element( i ).m_pCache == this )
		{
			Warning( "Attempt to re-add CUtlLRUCache cache %s to registry!\n", pszCacheName );
			return;
		}
	}

	LRUCacheRegistryEntry_t entry = { pszCacheName, this };
	pRegistry->AddToTail( entry );
}

inline CUtlLRUCacheBase::~CUtlLRUCacheBase()
{
	CUtlLRUCacheRegistry *pRegistry = GetLRUCacheRegistry();

	FOR_EACH_VEC( *pRegistry, i )
	{
		if ( pRegistry->Element( i ).m_pCache == this )
		{
			pRegistry->Remove( i );
			return;
		}
	}

	Warning( "Attempt to remove unknown CUtlLRUCache cache from registry!\n" );
}

#endif // UTLLRUCACHE_H
