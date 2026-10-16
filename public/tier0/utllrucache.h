#ifndef UTLLRUCACHE_H
#define UTLLRUCACHE_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"

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
};

#endif // UTLLRUCACHE_H
