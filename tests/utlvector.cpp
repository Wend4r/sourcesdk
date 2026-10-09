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

REGISTER_NAMED_TEST( "CUtlVector.MoveStealsBuffer", CUtlVector_MoveStealsBuffer )
{
	// Moving a heap allocated vector should take over its buffer and leave the source empty.
	CUtlVector< int > vec;

	vec.AddToTail( 1 );
	vec.AddToTail( 2 );
	vec.AddToTail( 3 );

	const int *pBuffer = vec.Base();

	CUtlVector< int > moved( Move( vec ) );

	TEST_TRUE( moved.Base() == pBuffer );
	TEST_EQ( moved.Count(), 3 );
	TEST_EQ( moved[ 2 ], 3 );
	TEST_EQ( vec.Count(), 0 );
	TEST_TRUE( vec.Base() == nullptr );

	CUtlVector< int > assigned;

	assigned.AddToTail( 9 );
	assigned = Move( moved );

	TEST_TRUE( assigned.Base() == pBuffer );
	TEST_EQ( assigned.Count(), 3 );
	TEST_EQ( assigned[ 0 ], 1 );
	TEST_EQ( moved.Count(), 0 );

	CUtlVector< int > &self = assigned;

	assigned = Move( self );

	TEST_TRUE( assigned.Base() == pBuffer );
	TEST_EQ( assigned.Count(), 3 );

	vec.AddToTail( 4 );
	TEST_EQ( vec.Count(), 1 );
	TEST_EQ( vec[ 0 ], 4 );
}

REGISTER_NAMED_TEST( "CUtlVector.MoveNonTrivial", CUtlVector_MoveNonTrivial )
{
	// Moving non-trivial elements should neither leak nor double destroy them.
	TEST_EQ( VectorTrackedValue_t::s_nAlive, 0 );

	{
		CUtlVector< VectorTrackedValue_t > vec;

		vec.AddToTail( VectorTrackedValue_t( 10 ) );
		vec.AddToTail( VectorTrackedValue_t( 20 ) );

		const VectorTrackedValue_t *pBuffer = vec.Base();

		CUtlVector< VectorTrackedValue_t > moved( Move( vec ) );

		TEST_TRUE( moved.Base() == pBuffer );
		TEST_EQ( moved.Count(), 2 );
		TEST_EQ( moved[ 1 ].m_nValue, 20 );
		TEST_EQ( vec.Count(), 0 );
		TEST_EQ( VectorTrackedValue_t::s_nAlive, 2 );

		CUtlVector< VectorTrackedValue_t > assigned;

		assigned.AddToTail( VectorTrackedValue_t( 30 ) );
		assigned = Move( moved );

		TEST_EQ( assigned.Count(), 2 );
		TEST_EQ( assigned[ 0 ].m_nValue, 10 );
		TEST_EQ( moved.Count(), 0 );
		TEST_EQ( VectorTrackedValue_t::s_nAlive, 2 );
	}

	TEST_EQ( VectorTrackedValue_t::s_nAlive, 0 );

	{
		// Conservative memory can't swap, so the elements are moved one by one.
		CUtlVectorConservative< VectorTrackedValue_t > vec;

		vec.AddToTail( VectorTrackedValue_t( 1 ) );
		vec.AddToTail( VectorTrackedValue_t( 2 ) );

		CUtlVectorConservative< VectorTrackedValue_t > moved;

		moved.AddToTail( VectorTrackedValue_t( 3 ) );
		moved = Move( vec );

		TEST_EQ( moved.Count(), 2 );
		TEST_EQ( moved[ 0 ].m_nValue, 1 );
		TEST_EQ( moved[ 1 ].m_nValue, 2 );
		TEST_EQ( vec.Count(), 0 );
		TEST_EQ( VectorTrackedValue_t::s_nAlive, 2 );
	}

	TEST_EQ( VectorTrackedValue_t::s_nAlive, 0 );
}

REGISTER_NAMED_TEST( "CUtlVector.MoveInlineStorage", CUtlVector_MoveInlineStorage )
{
	// Inline storage must not be handed over, so its elements are moved instead.
	CUtlVectorFixedGrowable< int, 4 > vec;

	vec.AddToTail( 5 );
	vec.AddToTail( 6 );

	CUtlVectorFixedGrowable< int, 4 > moved( Move( vec ) );

	TEST_TRUE( moved.Base() != vec.Base() );
	TEST_EQ( moved.Count(), 2 );
	TEST_EQ( moved[ 0 ], 5 );
	TEST_EQ( moved[ 1 ], 6 );
	TEST_EQ( vec.Count(), 0 );

	vec.AddToTail( 7 );
	TEST_EQ( vec.Count(), 1 );
	TEST_EQ( vec[ 0 ], 7 );
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

static bool __cdecl IntLessDescending( const int &left, const int &right )
{
	return left > right;
}

static bool __cdecl IntLessModulo( const int &left, const int &right, void *pCtx )
{
	const int nModulo = *static_cast< const int * >( pCtx );

	return ( left % nModulo ) < ( right % nModulo );
}

REGISTER_NAMED_TEST( "CUtlVector.SortOverloads", CUtlVector_SortOverloads )
{
	// Every sort overload should order elements, including non-contiguous block storage.
	CUtlVector< int > vec;

	for ( int i = 0; i < 64; ++i )
	{
		vec.AddToTail( ( i * 37 ) % 64 );
	}

	vec.Sort();

	for ( int i = 0; i < 64; ++i )
	{
		TEST_EQ( vec[ i ], i );
	}

	vec.Sort( &IntLessDescending );
	TEST_EQ( vec[ 0 ], 63 );
	TEST_EQ( vec[ 63 ], 0 );

	int nModulo = 10;

	vec.Sort( &IntLessModulo, &nModulo );

	for ( int i = 1; i < 64; ++i )
	{
		TEST_TRUE( ( vec[ i - 1 ] % 10 ) <= ( vec[ i ] % 10 ) );
	}

	vec.SortPredicate( []( const int &left, const int &right ) { return left < right; } );
	TEST_EQ( vec[ 0 ], 0 );
	TEST_EQ( vec[ 63 ], 63 );

	CUtlBlockVector< int > blockVec;

	blockVec.AddToTail( 3 );
	blockVec.AddToTail( 1 );
	blockVec.AddToTail( 2 );
	blockVec.Sort( &CompareIntsAscending );

	TEST_EQ( blockVec[ 0 ], 1 );
	TEST_EQ( blockVec[ 1 ], 2 );
	TEST_EQ( blockVec[ 2 ], 3 );
}

template < typename TVector >
constexpr bool g_bVectorIterable = requires( TVector &vec, const TVector &constVec ) { vec.begin(); vec.end(); constVec.begin(); constVec.end(); };

REGISTER_NAMED_TEST( "CUtlVector.ContiguousIterators", CUtlVector_ContiguousIterators )
{
	// Pointer iterators are only offered when the elements live in one block.
	TEST_TRUE( g_bVectorIterable< CUtlVector< int > > );
	TEST_TRUE( ( g_bVectorIterable< CUtlVectorFixedGrowable< int, 4 > > ) );
	TEST_FALSE( g_bVectorIterable< CUtlBlockVector< int > > );
}

static bool __cdecl IntLessAscending( const int &left, const int &right )
{
	return left < right;
}

static bool __cdecl IntLessAscendingCtx( const int &left, const int &right, void * )
{
	return left < right;
}

REGISTER_NAMED_TEST( "CUtlVector.SortedHelpersAndFindMatch", CUtlVector_SortedHelpersAndFindMatch )
{
	// Sorted searches should find matches, the last equal element and keep inserts ordered.
	CUtlVector< int > vec;

	vec.SortedInsert( 30, &IntLessAscending );
	vec.SortedInsert( 10, &IntLessAscending );
	vec.SortedInsert( 20, &IntLessAscendingCtx, nullptr );
	vec.SortedInsert( 20, &IntLessAscending );
	vec.SortedInsert( 20, &IntLessAscending );

	TEST_EQ( vec.Count(), 5 );
	TEST_EQ( vec[ 0 ], 10 );
	TEST_EQ( vec[ 1 ], 20 );
	TEST_EQ( vec[ 3 ], 20 );
	TEST_EQ( vec[ 4 ], 30 );

	TEST_EQ( vec.SortedFind( 10, &IntLessAscending ), 0 );
	TEST_EQ( vec.SortedFind( 30, &IntLessAscendingCtx, nullptr ), 4 );
	TEST_EQ( vec.SortedFind( 25, &IntLessAscending ), vec.InvalidIndex() );

	TEST_EQ( vec.SortedFindLessOrEqual( 20, &IntLessAscending ), 3 );
	TEST_EQ( vec.SortedFindLessOrEqual( 20, &IntLessAscendingCtx, nullptr ), 3 );
	TEST_EQ( vec.SortedFindLessOrEqual( 25, &IntLessAscending ), 3 );
	TEST_EQ( vec.SortedFindLessOrEqual( 5, &IntLessAscending ), -1 );
	TEST_EQ( vec.SortedFindLessOrEqual( 35, &IntLessAscending ), 4 );
	TEST_EQ( vec.SortedFindLessOrEqual( 20, &IntLessAscending, 0, 1 ), 1 );

	TEST_EQ( vec.FindMatch( []( const int &nValue ) { return nValue > 15; } ), 1 );
	TEST_EQ( vec.FindMatch( []( const int &nValue ) { return nValue > 30; } ), vec.InvalidIndex() );
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

struct VectorCountingAllocator_t
{
	static int s_nReallocs;
	static int s_nFrees;

	template < typename T, typename I = int >
	static T *Realloc( T *pMem, I nCount, I &nAdjustedCount )
	{
		++s_nReallocs;
		return CMemAllocAllocator::Realloc< T, I >( pMem, nCount, nAdjustedCount );
	}

	static void Free( void *pMem )
	{
		++s_nFrees;
		CMemAllocAllocator::Free( pMem );
	}
};

int VectorCountingAllocator_t::s_nReallocs = 0;
int VectorCountingAllocator_t::s_nFrees = 0;

REGISTER_NAMED_TEST( "CUtlVector.RawAllocator", CUtlVector_RawAllocatorStorage )
{
	// Raw allocator vectors should route storage through the selected allocator.
	CUtlVector_RawAllocator< int > vecDefault;

	vecDefault.AddToTail( 1 );
	vecDefault.AddToTail( 2 );

	TEST_EQ( vecDefault.Count(), 2 );
	TEST_EQ( vecDefault[ 1 ], 2 );

	VectorCountingAllocator_t::s_nReallocs = 0;
	VectorCountingAllocator_t::s_nFrees = 0;

	{
		CUtlVector_RawAllocator< int, int, VectorCountingAllocator_t > vec;

		for ( int i = 0; i < 16; ++i )
		{
			vec.AddToTail( i );
		}

		TEST_EQ( vec.Count(), 16 );
		TEST_EQ( vec[ 15 ], 15 );
		TEST_TRUE( VectorCountingAllocator_t::s_nReallocs > 0 );
	}

	TEST_EQ( VectorCountingAllocator_t::s_nFrees, 1 );
}
