#include "common/assert.h"
#include "common/macros.h"

#include <tier0/utlstring.h>
#include <tier1/utlhashtable.h>

REGISTER_NAMED_TEST( "CUtlHashtable.InsertFindRemove", CUtlHashtable_InsertFindRemove )
{
	// Hashtable inserts should expose keys, values and duplicate detection.
	CUtlHashtable< int, int > table;

	bool bDidInsert = false;
	const UtlHashHandle_t hOne = table.Insert( 1, 10, &bDidInsert );

	TEST_TRUE( bDidInsert );
	TEST_TRUE( table.IsValidHandle( hOne ) );
	TEST_EQ( table.Count(), 1 );
	TEST_EQ( table.Key( hOne ), 1 );
	TEST_EQ( table.Element( hOne ), 10 );

	const UtlHashHandle_t hOneAgain = table.Insert( 1, 20, &bDidInsert );

	TEST_FALSE( bDidInsert );
	TEST_EQ( hOneAgain, hOne );
	TEST_EQ( table.Element( hOneAgain ), 10 );

	const UtlHashHandle_t hTwo = table.Insert( 2, 20, &bDidInsert );

	TEST_TRUE( bDidInsert );
	TEST_EQ( table.Count(), 2 );
	TEST_EQ( table.Find( 2 ), hTwo );
	TEST_TRUE( table.HasElement( 1 ) );

	TEST_TRUE( table.Remove( 1 ) );
	TEST_FALSE( table.HasElement( 1 ) );
	TEST_EQ( table.Count(), 1 );

	table.RemoveAll();
	TEST_EQ( table.Count(), 0 );
}

REGISTER_NAMED_TEST( "CUtlHashtable.GetSwapCompactPurge", CUtlHashtable_GetSwapCompactPurge )
{
	// Lookup helpers, swaps and purge should leave the table in a coherent state.
	CUtlHashtable< int, int > table;

	table.Insert( 10, 100 );
	table.Insert( 20, 200 );

	TEST_EQ( table.Get( 10, -1 ), 100 );
	TEST_EQ( table.Get( 30, -1 ), -1 );
	TEST_NOT_NULL( table.GetPtr( 20 ) );
	TEST_NULL( table.GetPtr( 30 ) );

	UtlHashHandle_t hNext = table.RemoveAndAdvance( table.FirstHandle() );

	TEST_TRUE( hNext == table.InvalidHandle() || table.IsValidHandle( hNext ) );

	CUtlHashtable< int, int > other;

	other.Insert( 99, 999 );
	table.Swap( other );
	TEST_TRUE( table.HasElement( 99 ) );
	TEST_FALSE( other.HasElement( 99 ) );

	table.Compact( true );
	table.Purge();
	TEST_EQ( table.Count(), 0 );
}

// Value that records assignments made onto memory that holds no constructed instance
static const uint64 s_nAliveMagic = 0x5EED0A11CE0FFEE5ull;
static int s_nAssignsToUnconstructed = 0;

class CAssignTrackedValue
{
public:
	CAssignTrackedValue() : m_nMagic( s_nAliveMagic ), m_nValue( 0 ) {}
	CAssignTrackedValue( int nValue ) : m_nMagic( s_nAliveMagic ), m_nValue( nValue ) {}
	CAssignTrackedValue( const CAssignTrackedValue &other ) : m_nMagic( s_nAliveMagic ), m_nValue( other.m_nValue ) {}
	~CAssignTrackedValue() { m_nMagic = 0; }

	CAssignTrackedValue &operator=( const CAssignTrackedValue &other )
	{
		if ( m_nMagic != s_nAliveMagic )
			s_nAssignsToUnconstructed++;

		m_nMagic = s_nAliveMagic;
		m_nValue = other.m_nValue;
		return *this;
	}

	uint64 m_nMagic;
	int m_nValue;
};

REGISTER_NAMED_TEST( "CUtlHashtable.RelocateIntoUnconstructedSlots", CUtlHashtable_RelocateIntoUnconstructedSlots )
{
	// Growing, bumping chained entries and refilling chain roots move entries into free slots,
	// which hold no constructed instance and must not be assigned to.
	s_nAssignsToUnconstructed = 0;

	{
		CUtlHashtable< int, CAssignTrackedValue > table;

		for ( int i = 0; i < 1000; i++ )
			table.Insert( i, CAssignTrackedValue( i * 3 ) );

		for ( int i = 0; i < 1000; i += 3 )
			TEST_TRUE( table.Remove( i ) );

		for ( int i = 1000; i < 1500; i++ )
			table.Insert( i, CAssignTrackedValue( i * 3 ) );

		for ( int i = 0; i < 1500; i++ )
		{
			const UtlHashHandle_t h = table.Find( i );

			if ( i < 1000 && i % 3 == 0 )
			{
				TEST_EQ( h, table.InvalidHandle() );
				continue;
			}

			TEST_TRUE( table.IsValidHandle( h ) );
			TEST_EQ( table.Element( h ).m_nMagic, s_nAliveMagic );
			TEST_EQ( table.Element( h ).m_nValue, i * 3 );
		}
	}

	TEST_EQ( s_nAssignsToUnconstructed, 0 );
}

// Registry of constructed CLiveTrackedValue instances, by address
static const void *s_pLiveInstances[ 4096 ];
static int s_nLiveInstances = 0;
static int s_nLifetimeErrors = 0;

static int FindLiveInstance( const void *pInstance )
{
	for ( int i = 0; i < s_nLiveInstances; i++ )
	{
		if ( s_pLiveInstances[ i ] == pInstance )
			return i;
	}

	return -1;
}

static void AddLiveInstance( const void *pInstance )
{
	if ( FindLiveInstance( pInstance ) != -1 || s_nLiveInstances == ARRAYSIZE( s_pLiveInstances ) )
	{
		s_nLifetimeErrors++;
		return;
	}

	s_pLiveInstances[ s_nLiveInstances++ ] = pInstance;
}

static void RemoveLiveInstance( const void *pInstance )
{
	int nIndex = FindLiveInstance( pInstance );

	if ( nIndex == -1 )
	{
		s_nLifetimeErrors++;
		return;
	}

	s_pLiveInstances[ nIndex ] = s_pLiveInstances[ --s_nLiveInstances ];
}

static void CheckLiveInstance( const void *pInstance )
{
	if ( FindLiveInstance( pInstance ) == -1 )
		s_nLifetimeErrors++;
}

// Not trivially relocatable: it points to itself and is tracked by address
class CLiveTrackedValue
{
public:
	CLiveTrackedValue( int nValue = 0 ) : m_pSelf( this ), m_nValue( nValue ) { AddLiveInstance( this ); }
	CLiveTrackedValue( const CLiveTrackedValue &other ) : m_pSelf( this ), m_nValue( other.m_nValue ) { CheckLiveInstance( &other ); AddLiveInstance( this ); }
	CLiveTrackedValue( CLiveTrackedValue &&other ) : m_pSelf( this ), m_nValue( other.m_nValue ) { CheckLiveInstance( &other ); other.m_nValue = -1; AddLiveInstance( this ); }
	~CLiveTrackedValue() { RemoveLiveInstance( this ); m_pSelf = nullptr; }

	CLiveTrackedValue &operator=( const CLiveTrackedValue &other )
	{
		CheckLiveInstance( this );
		CheckLiveInstance( &other );
		m_nValue = other.m_nValue;
		return *this;
	}

	bool IsSelf() const { return m_pSelf == this; }

	CLiveTrackedValue *m_pSelf;
	int m_nValue;
};

typedef CUtlHashtable< int, CLiveTrackedValue, DefaultHashFunctor< int >, DefaultEqualFunctor< int >, CUtlLeanVector< CUtlHashtableEntry< int, CLiveTrackedValue > >, false > LiveTrackedHashtable_t;

static void CheckLiveTrackedTable( const LiveTrackedHashtable_t &table, int nCount )
{
	TEST_EQ( table.Count(), nCount );
	TEST_EQ( s_nLiveInstances, nCount );
	TEST_EQ( s_nLifetimeErrors, 0 );

	int nVisited = 0;

	FOR_EACH_HASHTABLE( table, it )
	{
		const CLiveTrackedValue &value = table.Element( it );

		TEST_TRUE( value.IsSelf() );
		TEST_NE( FindLiveInstance( &value ), -1 );
		TEST_EQ( value.m_nValue, table.Key( it ) * 7 );
		nVisited++;
	}

	TEST_EQ( nVisited, nCount );
}

REGISTER_NAMED_TEST( "CUtlHashtable.NonTriviallyRelocatable", CUtlHashtable_NonTriviallyRelocatable )
{
	// Without bTriviallyRelocatable every relocation must move construct into the free slot
	// and destruct the source, so each live instance is exactly one valid table entry.
	s_nLiveInstances = 0;
	s_nLifetimeErrors = 0;

	{
		LiveTrackedHashtable_t table;

		for ( int i = 0; i < 1000; i++ )
			table.Insert( i, CLiveTrackedValue( i * 7 ) );

		CheckLiveTrackedTable( table, 1000 );

		for ( int i = 0; i < 1000; i += 3 )
			TEST_TRUE( table.Remove( i ) );

		CheckLiveTrackedTable( table, 666 );

		for ( int i = 1000; i < 1500; i++ )
			table.Insert( i, CLiveTrackedValue( i * 7 ) );

		CheckLiveTrackedTable( table, 1166 );

		for ( UtlHashHandle_t it = table.FirstHandle(); it != table.InvalidHandle(); )
		{
			if ( table.Key( it ) % 2 == 0 )
				it = table.RemoveAndAdvance( it );
			else
				it = table.NextHandle( it );
		}

		CheckLiveTrackedTable( table, 583 );

		table.Compact( true );
		CheckLiveTrackedTable( table, 583 );

		for ( int i = 1; i < 1500; i += 2 )
		{
			if ( i < 1000 && i % 3 == 0 )
				continue;

			const CLiveTrackedValue *pValue = table.GetPtr( i );

			TEST_NOT_NULL( pValue );
			TEST_EQ( pValue->m_nValue, i * 7 );
		}
	}

	TEST_EQ( s_nLiveInstances, 0 );
	TEST_EQ( s_nLifetimeErrors, 0 );
}

REGISTER_NAMED_TEST( "CUtlHashtable.AlternateKeys", CUtlHashtable_AlternateKeys )
{
	// CUtlString keys can be looked up, inserted and removed through their const char* alternate type.
	CUtlHashtable< CUtlString, int > table;

	bool bDidInsert = false;
	const UtlHashHandle_t hAlpha = table.Insert( "alpha", 1, &bDidInsert );

	TEST_TRUE( bDidInsert );
	TEST_EQ( table.Insert( CUtlString( "alpha" ), 2, &bDidInsert ), hAlpha );
	TEST_FALSE( bDidInsert );
	TEST_EQ( table.Insert( "beta" ), table.Find( CUtlString( "beta" ) ) );

	const char *pszAlpha = "alpha";

	TEST_EQ( table.Find( pszAlpha ), hAlpha );
	TEST_TRUE( table.HasElement( "beta" ) );
	TEST_FALSE( table.HasElement( "gamma" ) );
	TEST_EQ( table.Get( "alpha", -1 ), 1 );
	TEST_EQ( table.Get( "gamma", -1 ), -1 );
	TEST_NOT_NULL( table.GetPtr( pszAlpha ) );
	TEST_NULL( table.GetPtr( "gamma" ) );

	TEST_TRUE( table.Remove( "alpha" ) );
	TEST_FALSE( table.Remove( pszAlpha ) );
	TEST_EQ( table.Count(), 1 );
}
