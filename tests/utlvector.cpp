#include "common/assert.h"
#include "common/macros.h"

#include <tier1/utlvector.h>

#include <utility>

static int __cdecl CompareIntsAscending( const int *pLeft, const int *pRight )
{
	if ( *pLeft < *pRight )
	{
		return -1;
	}

	if ( *pLeft > *pRight )
	{
		return 1;
	}

	return 0;
}

struct VectorTrackedValue_t
{
	static int s_nAlive;

	int m_nValue;

	VectorTrackedValue_t( int nValue = 0 ) : m_nValue( nValue ) { ++s_nAlive; }
	VectorTrackedValue_t( const VectorTrackedValue_t &other ) : m_nValue( other.m_nValue ) { ++s_nAlive; }
	VectorTrackedValue_t( VectorTrackedValue_t &&other ) noexcept : m_nValue( other.m_nValue ) { ++s_nAlive; other.m_nValue = -1; }
	~VectorTrackedValue_t() { --s_nAlive; }

	VectorTrackedValue_t &operator=( const VectorTrackedValue_t &other )
	{
		m_nValue = other.m_nValue;
		return *this;
	}

	VectorTrackedValue_t &operator=( VectorTrackedValue_t &&other ) noexcept
	{
		m_nValue = other.m_nValue;
		other.m_nValue = -1;
		return *this;
	}

	bool operator==( const VectorTrackedValue_t &other ) const { return m_nValue == other.m_nValue; }
};

int VectorTrackedValue_t::s_nAlive = 0;

struct VectorDeleteTracked_t
{
	static int s_nDeleted;
	~VectorDeleteTracked_t() { ++s_nDeleted; }
};

int VectorDeleteTracked_t::s_nDeleted = 0;

REGISTER_NAMED_TEST( "CUtlVector.Empty", CUtlVector_Empty )
{
	// Empty vectors should report zero count and reject out-of-range indices.
	CUtlVector< int > vec;

	TEST_EQ( vec.Count(), 0 );
	TEST_TRUE( vec.IsEmpty() );
	TEST_FALSE( vec.IsValidIndex( 0 ) );
}

REGISTER_NAMED_TEST( "CUtlVector.InsertRemove", CUtlVector_InsertRemove )
{
	// Insert and remove operations should preserve the expected element order.
	CUtlVector< int > vec;

	TEST_EQ( vec.AddToTail( 2 ), 0 );
	TEST_EQ( vec.AddToHead( 1 ), 0 );
	TEST_EQ( vec.InsertAfter( 1, 3 ), 2 );
	TEST_EQ( vec.InsertBefore( 1, 4 ), 1 );

	TEST_EQ( vec.Count(), 4 );
	TEST_EQ( vec[0], 1 );
	TEST_EQ( vec[1], 4 );
	TEST_EQ( vec[2], 2 );
	TEST_EQ( vec[3], 3 );
	TEST_TRUE( vec.IsValidIndex( 3 ) );
	TEST_FALSE( vec.IsValidIndex( 4 ) );

	vec.Remove( 1 );
	TEST_EQ( vec.Count(), 3 );
	TEST_EQ( vec[0], 1 );
	TEST_EQ( vec[1], 2 );
	TEST_EQ( vec[2], 3 );

	vec.RemoveAll();
	TEST_EQ( vec.Count(), 0 );
}

REGISTER_NAMED_TEST( "CUtlVector.Iteration", CUtlVector_Iteration )
{
	// Range iteration should visit each element exactly once in order.
	CUtlVector< int > vec;

	vec.AddToTail( 1 );
	vec.AddToTail( 2 );
	vec.AddToTail( 3 );

	int nSum = 0;

	for ( int nValue : vec )
	{
		nSum += nValue;
	}

	TEST_EQ( nSum, 6 );
}

REGISTER_NAMED_TEST( "CUtlVector.CopyMove", CUtlVector_CopyMove )
{
	// Copy and move construction should keep the visible payload intact.
	CUtlVector< int > vec;

	vec.AddToTail( 7 );
	vec.AddToTail( 9 );

	CUtlVector< int > copy( vec );

	TEST_EQ( copy.Count(), 2 );
	TEST_EQ( copy[0], 7 );
	TEST_EQ( copy[1], 9 );

	CUtlVector< int > moved( Move( vec ) );

	TEST_EQ( moved.Count(), 2 );
	TEST_EQ( moved[0], 7 );
	TEST_EQ( moved[1], 9 );
}

REGISTER_NAMED_TEST( "CUtlVector.NonTrivialLifetime", CUtlVector_NonTrivialLifetime )
{
	// Non-trivial element lifetimes should balance after vector destruction.
	TEST_EQ( VectorTrackedValue_t::s_nAlive, 0 );

	{
		CUtlVector< VectorTrackedValue_t > vec;

		vec.AddToTail( VectorTrackedValue_t( 10 ) );
		vec.AddToTail( VectorTrackedValue_t( 20 ) );
		TEST_EQ( vec.Count(), 2 );
		TEST_EQ( vec[0].m_nValue, 10 );
		TEST_EQ( vec[1].m_nValue, 20 );

		vec.Remove( 0 );
		TEST_EQ( vec.Count(), 1 );
		TEST_EQ( vec[0].m_nValue, 20 );
	}

	TEST_EQ( VectorTrackedValue_t::s_nAlive, 0 );
}

REGISTER_NAMED_TEST( "CUtlVector.FindAndFastRemove", CUtlVector_FindAndFastRemove )
{
	// Lookup helpers and fast removal should leave only the expected survivors.
	CUtlVector< int > vec;

	vec.AddToTail( 4 );
	vec.AddToTail( 5 );
	vec.AddToTail( 6 );

	TEST_EQ( vec.Find( 5 ), 1 );
	TEST_EQ( vec.Find( 9 ), vec.InvalidIndex() );
	TEST_TRUE( vec.HasElement( 6 ) );
	TEST_FALSE( vec.HasElement( 7 ) );
	TEST_TRUE( vec.FindAndRemove( 5 ) );
	TEST_FALSE( vec.FindAndRemove( 5 ) );

	vec.FastRemove( 0 );
	TEST_EQ( vec.Count(), 1 );
	TEST_EQ( vec[0], 6 );

	vec.AddToTail( 7 );
	TEST_TRUE( vec.FindAndFastRemove( 6 ) );
	TEST_EQ( vec.Count(), 1 );
	TEST_EQ( vec[0], 7 );
}

REGISTER_NAMED_TEST( "CUtlVector.CountFillCopySwap", CUtlVector_CountFillCopySwap )
{
	// Count, fill, copy and swap helpers should keep the backing vector coherent.
	CUtlVector< int > vec;

	vec.SetCountNonDestructively( 3 );
	vec.FillWithValue( 11 );
	TEST_EQ( vec.Count(), 3 );
	TEST_EQ( vec[0], 11 );
	TEST_EQ( vec[2], 11 );

	const int values[] = { 3, 1, 2 };
	vec.CopyArray( values, 3 );
	TEST_EQ( vec[0], 3 );
	TEST_EQ( vec[1], 1 );
	TEST_EQ( vec[2], 2 );

	CUtlVector< int > other;

	other.AddToTail( 9 );
	other.AddToTail( 8 );
	vec.Swap( other );

	TEST_EQ( vec.Count(), 2 );
	TEST_EQ( vec[0], 9 );
	TEST_EQ( other.Count(), 3 );
	TEST_EQ( other[0], 3 );
}

REGISTER_NAMED_TEST( "CUtlVector.SortAndAddVector", CUtlVector_SortAndAddVector )
{
	// Sorting and vector appends should keep elements ordered and contiguous.
	CUtlVector< int > vec;

	vec.AddToTail( 3 );
	vec.AddToTail( 1 );
	vec.AddToTail( 2 );

	vec.Sort( &CompareIntsAscending );
	TEST_EQ( vec[0], 1 );
	TEST_EQ( vec[1], 2 );
	TEST_EQ( vec[2], 3 );

	CUtlVector< int > appended;

	appended.AddToTail( 4 );
	appended.AddToTail( 5 );

	const int nBase = vec.AddVectorToTail( appended );

	TEST_EQ( nBase, 3 );
	TEST_EQ( vec.Count(), 5 );
	TEST_EQ( vec[3], 4 );
	TEST_EQ( vec[4], 5 );
}

REGISTER_NAMED_TEST( "CUtlVector.PurgeAndDeleteElements", CUtlVector_PurgeAndDeleteElements )
{
	// Purge-and-delete should destroy heap-owned elements and empty the vector.
	VectorDeleteTracked_t::s_nDeleted = 0;

	CUtlVector< VectorDeleteTracked_t * > vec;

	vec.AddToTail( new VectorDeleteTracked_t() );
	vec.AddToTail( new VectorDeleteTracked_t() );

	vec.PurgeAndDeleteElements();

	TEST_EQ( VectorDeleteTracked_t::s_nDeleted, 2 );
	TEST_EQ( vec.Count(), 0 );
}
