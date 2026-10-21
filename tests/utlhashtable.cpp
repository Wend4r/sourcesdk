#include "common/assert.h"
#include "common/macros.h"

#include <tier0/utlstring.h>
#include <tier0/utlhashtable.h>

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

// Exposes the slot count, which is not part of the public interface
template < typename KeyT, typename ValueT >
class CSlotCountHashtable : public CUtlHashtable< KeyT, ValueT >
{
public:
	using CUtlHashtable< KeyT, ValueT >::CUtlHashtable;

	int SlotCount() const { return this->m_nTableSize; }
};

REGISTER_NAMED_TEST( "CUtlHashtable.GrowAtLoadFactor", CUtlHashtable_GrowAtLoadFactor )
{
	// The table grows only once an insert would take it past a load of 3/4.
	CSlotCountHashtable< int, int > table( 8 );

	TEST_EQ( table.SlotCount(), 0 );

	for ( int i = 0; i < 6; i++ )
		table.Insert( i, i );

	TEST_EQ( table.SlotCount(), 8 );

	table.Insert( 6, 6 );
	TEST_EQ( table.SlotCount(), 16 );

	for ( int i = 7; i < 12; i++ )
		table.Insert( i, i );

	TEST_EQ( table.SlotCount(), 16 );

	table.Insert( 12, 12 );
	TEST_EQ( table.SlotCount(), 32 );

	for ( int i = 13; i < 1000; i++ )
		table.Insert( i, i );

	TEST_EQ( table.Count(), 1000 );
	TEST_EQ( table.SlotCount(), 2048 );

	for ( int i = 0; i < 1000; i++ )
		TEST_EQ( table.Get( i, -1 ), i );
}

REGISTER_NAMED_TEST( "CUtlHashtable.SetExternalBuffer", CUtlHashtable_SetExternalBuffer )
{
	// A fixed external buffer is used as the table storage, in place.
	static CUtlHashtableEntry< int, int > entries[ 16 ];
	CUtlHashtable< int, int > table( entries, 16 );

	for ( int i = 0; i < 12; i++ )
		table.Insert( i, i * 2 );

	TEST_EQ( table.Count(), 12 );

	for ( int i = 0; i < 12; i++ )
	{
		const UtlHashHandle_t h = table.Find( i );

		TEST_TRUE( table.IsValidHandle( h ) );
		TEST_EQ( table.Element( h ), i * 2 );
		TEST_EQ( &table.Element( h ), &entries[ h ].Raw()->m_value );
	}

	TEST_TRUE( table.Remove( 4 ) );
	TEST_FALSE( table.HasElement( 4 ) );
	TEST_EQ( table.Count(), 11 );

	// A growable raw byte buffer moves to owned memory once it is outgrown
	alignas( CUtlHashtableEntry< int, int > ) static byte rawBuffer[ 8 * sizeof( CUtlHashtableEntry< int, int > ) + 3 ];
	CUtlHashtable< int, int > growable;

	growable.SetExternalBuffer( rawBuffer, sizeof( rawBuffer ), false, true );
	growable.Insert( 1, 1 );
	TEST_EQ( &growable.Element( growable.Find( 1 ) ), &( ( CUtlHashtableEntry< int, int > * )rawBuffer )[ growable.Find( 1 ) ].Raw()->m_value );

	for ( int i = 2; i < 100; i++ )
		growable.Insert( i, i );

	TEST_EQ( growable.Count(), 99 );

	for ( int i = 1; i < 100; i++ )
		TEST_EQ( growable.Get( i, -1 ), i );

#if _DEBUG
	// Checks against a clone kept in an external buffer
	growable.DbgCheckIntegrity();
#endif

	growable.Purge();
	TEST_EQ( growable.Count(), 0 );
}

// Allocator that tracks the blocks it handed out, and refuses to free any other block
static void *s_pCountedAllocations[ 64 ];
static int s_nCountedAllocations = 0;
static int s_nForeignFrees = 0;

static void TrackAllocation( void *pMemory )
{
	if ( pMemory && s_nCountedAllocations < ARRAYSIZE( s_pCountedAllocations ) )
		s_pCountedAllocations[ s_nCountedAllocations++ ] = pMemory;
}

static bool UntrackAllocation( void *pMemory )
{
	for ( int i = 0; i < s_nCountedAllocations; i++ )
	{
		if ( s_pCountedAllocations[ i ] == pMemory )
		{
			s_pCountedAllocations[ i ] = s_pCountedAllocations[ --s_nCountedAllocations ];
			return true;
		}
	}

	return false;
}

class CCountingAllocator
{
public:
	template < typename T, typename I = int >
	static T *Alloc( I nCount, I &nAdjustedCount )
	{
		T *pNew = CMemAllocAllocator::Alloc< T, I >( nCount, nAdjustedCount );
		TrackAllocation( pNew );
		return pNew;
	}

	template < typename T, typename I = int >
	static T *Realloc( T *pMemory, I nCount, I &nAdjustedCount )
	{
		if ( pMemory && !UntrackAllocation( pMemory ) )
		{
			s_nForeignFrees++;
			return Alloc< T, I >( nCount, nAdjustedCount );
		}

		T *pNew = CMemAllocAllocator::Realloc< T, I >( pMemory, nCount, nAdjustedCount );
		TrackAllocation( pNew );
		return pNew;
	}

	static void Free( void *pMemory )
	{
		if ( !pMemory )
			return;

		if ( !UntrackAllocation( pMemory ) )
		{
			s_nForeignFrees++;
			return;
		}

		CMemAllocAllocator::Free( pMemory );
	}
};

typedef CUtlHashtableEntry< int, int > IntEntry_t;
typedef CUtlHashtable< int, int, DefaultHashFunctor< int >, DefaultEqualFunctor< int >, CUtlLeanVector< IntEntry_t, int, CCountingAllocator > > CountedHashtable_t;

static void FillCountedHashtable( CountedHashtable_t &table, int nCount )
{
	for ( int i = 0; i < nCount; i++ )
		table.Insert( i, i * 5 );

	TEST_EQ( table.Count(), nCount );

	for ( int i = 0; i < nCount; i++ )
		TEST_EQ( table.Get( i, -1 ), i * 5 );
}

REGISTER_NAMED_TEST( "CUtlHashtable.ResizeFreesOwnedTable", CUtlHashtable_ResizeFreesOwnedTable )
{
	// After growing several times only the current table allocation may remain.
	s_nCountedAllocations = 0;
	s_nForeignFrees = 0;

	{
		CountedHashtable_t table;

		FillCountedHashtable( table, 1000 );
		TEST_EQ( s_nCountedAllocations, 1 );

		table.Compact( true );
		TEST_EQ( s_nCountedAllocations, 1 );

		table.Purge();
		TEST_EQ( s_nCountedAllocations, 0 );

		FillCountedHashtable( table, 100 );
		TEST_EQ( s_nCountedAllocations, 1 );
	}

	TEST_EQ( s_nCountedAllocations, 0 );
	TEST_EQ( s_nForeignFrees, 0 );
}

REGISTER_NAMED_TEST( "CUtlHashtable.ResizeKeepsExternalBuffer", CUtlHashtable_ResizeKeepsExternalBuffer )
{
	// Growing out of an external buffer must leave it to its owner,
	// while a buffer handed over with bAssumeOwnership is freed like any other table.
	s_nCountedAllocations = 0;
	s_nForeignFrees = 0;

	{
		static IntEntry_t entries[ 8 ];
		CountedHashtable_t table;

		table.SetExternalBuffer( entries, 8, false, true );
		FillCountedHashtable( table, 100 );

		TEST_EQ( s_nForeignFrees, 0 );
		TEST_EQ( s_nCountedAllocations, 1 );
	}

	TEST_EQ( s_nCountedAllocations, 0 );

	{
		int nAllocated = 0;
		IntEntry_t *pEntries = CCountingAllocator::Alloc< IntEntry_t >( 8, nAllocated );
		CountedHashtable_t table;

		table.SetExternalBuffer( pEntries, 8, true, true );
		TEST_EQ( s_nCountedAllocations, 1 );

		FillCountedHashtable( table, 100 );
		TEST_EQ( s_nCountedAllocations, 1 );
	}

	TEST_EQ( s_nCountedAllocations, 0 );
	TEST_EQ( s_nForeignFrees, 0 );
}

enum HashtableTestEnum_t
{
	HASHTABLE_TEST_ALPHA = 1,
	HASHTABLE_TEST_BETA = 2,
};

enum class HashtableTestEnumClass_t : uint64
{
	Alpha = 1ull << 40,
	Beta = 2ull << 40,
};

REGISTER_NAMED_TEST( "CUtlHashtable.EnumAndPairKeys", CUtlHashtable_EnumAndPairKeys )
{
	// Enums and std::pair keys should get default hash functors.
	TEST_EQ( DefaultHashFunctor< HashtableTestEnum_t >()( HASHTABLE_TEST_BETA ), DefaultHashFunctor< int >()( 2 ) );
	TEST_EQ( DefaultHashFunctor< const HashtableTestEnum_t >()( HASHTABLE_TEST_BETA ), DefaultHashFunctor< int >()( 2 ) );
	TEST_TRUE( DefaultHashFunctor< HashtableTestEnumClass_t >()( HashtableTestEnumClass_t::Alpha ) != DefaultHashFunctor< HashtableTestEnumClass_t >()( HashtableTestEnumClass_t::Beta ) );
	using PairHash_t = DefaultHashFunctor< std::pair< int, int > >;

	TEST_TRUE( PairHash_t()( std::make_pair( 1, 2 ) ) != PairHash_t()( std::make_pair( 2, 1 ) ) );

	CUtlHashtable< HashtableTestEnum_t, int > enumTable;

	enumTable.Insert( HASHTABLE_TEST_ALPHA, 10 );
	enumTable.Insert( HASHTABLE_TEST_BETA, 20 );

	TEST_EQ( enumTable.Element( enumTable.Find( HASHTABLE_TEST_BETA ) ), 20 );

	CUtlHashtable< std::pair< int, int >, int > pairTable;

	pairTable.Insert( std::make_pair( 1, 2 ), 12 );
	pairTable.Insert( std::make_pair( 2, 1 ), 21 );

	TEST_EQ( pairTable.Count(), 2 );
	TEST_EQ( pairTable.Element( pairTable.Find( std::make_pair( 2, 1 ) ) ), 21 );
	TEST_FALSE( pairTable.HasElement( std::make_pair( 3, 3 ) ) );
}

enum TestHashEnum_t : int
{
	TEST_HASH_ENUM_A = 1,
	TEST_HASH_ENUM_B = 7,
};

REGISTER_NAMED_TEST( "DefaultHashFunctor.EnumByUnderlyingValue", DefaultHashFunctor_EnumByUnderlyingValue )
{
	// An enum hashes as its underlying value.
	TEST_EQ( DefaultHashFunctor< TestHashEnum_t >()( TEST_HASH_ENUM_B ), DefaultHashFunctor< int >()( 7 ) );
	TEST_TRUE( DefaultHashFunctor< TestHashEnum_t >()( TEST_HASH_ENUM_A ) != DefaultHashFunctor< TestHashEnum_t >()( TEST_HASH_ENUM_B ) );

	// Tables of enums use it.
	CUtlHashtable< TestHashEnum_t > table;
	table.Insert( TEST_HASH_ENUM_A );
	TEST_TRUE( table.HasElement( TEST_HASH_ENUM_A ) );
	TEST_FALSE( table.HasElement( TEST_HASH_ENUM_B ) );
}

REGISTER_NAMED_TEST( "DefaultHashFunctor.CUtlStringKeys", DefaultHashFunctor_CUtlStringKeys )
{
	// The single template parameter of the engine, as in CUtlHashtable< CUtlString >.
	CUtlHashtable< CUtlString > table;
	table.Insert( CUtlString( "sourcesdk" ) );
	TEST_TRUE( table.HasElement( CUtlString( "sourcesdk" ) ) );
	TEST_EQ( DefaultHashFunctor< CUtlString >()( CUtlString( "abc" ) ), DefaultHashFunctor< CUtlString >()( CUtlString( "abc" ) ) );
}
