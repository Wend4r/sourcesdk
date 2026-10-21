#include "common/assert.h"
#include "common/macros.h"

#include <tier0/platform.h>
#include <tier0/utllrucache.h>
#include <tier0/strtools.h>
#include <tier0/utlvector.h>

#include <cstddef>

// A cache that only counts the calls of the statics
class CTestLRUCache : public CUtlLRUCacheBase
{
public:
	CTestLRUCache( const char *pszCacheName = "sourcesdk_test_cache" ) : CUtlLRUCacheBase( pszCacheName )
	{
		m_nCacheSize = 0;
		m_nRequests = 10;
		m_nStatsStartTime = 0;
		m_nHits = 4;
		m_nEvictions = 2;
	}

	virtual void SetCacheSize( int nSize ) { m_nCacheSize = nSize; m_nSetSizeCalls++; }
	virtual void Flush() { m_nFlushCalls++; }
	virtual void Purge() { m_nPurgeCalls++; }

	int CacheSize() const { return m_nCacheSize; }
	int Requests() const { return m_nRequests; }
	int Hits() const { return m_nHits; }
	int Evictions() const { return m_nEvictions; }
	time_t StatsStartTime() const { return m_nStatsStartTime; }

	static size_t OffsetOfRequests() { return offsetof( CTestLRUCache, m_nRequests ); }
	static size_t OffsetOfStatsStartTime() { return offsetof( CTestLRUCache, m_nStatsStartTime ); }
	static size_t OffsetOfEvictions() { return offsetof( CTestLRUCache, m_nEvictions ); }

	using CUtlLRUCacheBase::m_nRequests;
	using CUtlLRUCacheBase::m_nHits;

	int m_nSetSizeCalls = 0;
	int m_nFlushCalls = 0;
	int m_nPurgeCalls = 0;
};

REGISTER_NAMED_TEST( "GetLRUCacheRegistry.Stable", GetLRUCacheRegistry_Stable )
{
	auto *pRegistry = GetLRUCacheRegistry();
	TEST_NOT_NULL( pRegistry );
	TEST_TRUE( GetLRUCacheRegistry() == pRegistry );
	TEST_TRUE( pRegistry->Count() >= 0 );
}

REGISTER_NAMED_TEST( "LRUCacheRegistryEntry_t.Layout", LRUCacheRegistryEntry_t_Layout )
{
	TEST_EQ( sizeof( LRUCacheRegistryEntry_t ), ( size_t )16 );
	TEST_EQ( offsetof( LRUCacheRegistryEntry_t, m_pCache ), ( size_t )8 );

	// The counters tier0 reads.
	TEST_EQ( CTestLRUCache::OffsetOfRequests(), ( size_t )12 );
	TEST_EQ( CTestLRUCache::OffsetOfStatsStartTime(), ( size_t )16 );
	TEST_EQ( CTestLRUCache::OffsetOfEvictions(), ( size_t )28 );
}

REGISTER_NAMED_TEST( "CUtlLRUCacheBase.RegistersItself", CUtlLRUCacheBase_RegistersItself )
{
	CUtlLRUCacheRegistry *pRegistry = GetLRUCacheRegistry();
	const int nCount = pRegistry->Count();

	{
		// A cache is in the registry for its lifetime.
		CTestLRUCache first( "sourcesdk_first_cache" ), second( "sourcesdk_second_cache" );
		TEST_EQ( pRegistry->Count(), nCount + 2 );
		TEST_TRUE( pRegistry->Tail().m_pCache == &second );
		TEST_EQ( V_strcmp( pRegistry->Tail().m_pszName, "sourcesdk_second_cache" ), 0 );
	}

	TEST_EQ( pRegistry->Count(), nCount );
}

REGISTER_NAMED_TEST( "CUtlLRUCacheBase.SetCacheSize", CUtlLRUCacheBase_SetCacheSize )
{
	CTestLRUCache cache;

	CUtlLRUCacheBase::SetCacheSize( "sourcesdk_test_cache", 42 );
	TEST_EQ( cache.m_nSetSizeCalls, 1 );
	TEST_EQ( cache.CacheSize(), 42 );

	// Another name or no name changes nothing.
	CUtlLRUCacheBase::SetCacheSize( "sourcesdk_other_cache", 7 );
	CUtlLRUCacheBase::SetCacheSize( nullptr, 7 );
	TEST_EQ( cache.m_nSetSizeCalls, 1 );
}

REGISTER_NAMED_TEST( "CUtlLRUCacheBase.Flush", CUtlLRUCacheBase_Flush )
{
	CTestLRUCache cache;

	CUtlLRUCacheBase::Flush( "sourcesdk_test_cache" );
	TEST_EQ( cache.m_nFlushCalls, 1 );
	CUtlLRUCacheBase::Flush( nullptr );
	TEST_EQ( cache.m_nFlushCalls, 1 );
}

REGISTER_NAMED_TEST( "CUtlLRUCacheBase.ResetCacheStats", CUtlLRUCacheBase_ResetCacheStats )
{
	CTestLRUCache cache;

	// The counters restart at the time of the reset.
	CUtlLRUCacheBase::ResetCacheStats( "sourcesdk_test_cache" );
	TEST_EQ( cache.Requests(), 0 );
	TEST_EQ( cache.Hits(), 0 );
	TEST_EQ( cache.Evictions(), 0 );
	TEST_TRUE( Plat_GetTime() - cache.StatsStartTime() <= 1 );

	// No name resets every cache.
	cache.m_nRequests = 5;
	CUtlLRUCacheBase::ResetCacheStats( nullptr );
	TEST_EQ( cache.Requests(), 0 );
}

REGISTER_NAMED_TEST( "CUtlLRUCacheBase.SpewStats", CUtlLRUCacheBase_SpewStats )
{
	CTestLRUCache cache;
	CUtlLRUCacheBase::ResetCacheStats( "sourcesdk_test_cache" );

	// Prints a table of every cache with requests.
	cache.m_nRequests = 3;
	cache.m_nHits = 1;
	CUtlLRUCacheBase::SpewStats();
}
