#include "common/assert.h"
#include "common/macros.h"

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
