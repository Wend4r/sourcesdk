#include "common/assert.h"
#include "common/macros.h"

#include <tier1/utlleanvector.h>
#include <tier0/utlblockvector.h>
#include <tier0/utlstring.h>

template class CUtlBlockVector< CUtlString >;
template class CUtlBlockVector< CUtlString, unsigned short >;

REGISTER_NAMED_TEST( "CUtlBlockVector.StableAddresses", CUtlBlockVector_StableAddresses )
{
	// Elements live in fixed size pages, so growing never moves them.
	CUtlBlockVector< int > vec( 4 );

	vec.AddToTail( 0 );
	int *pFirst = &vec[ 0 ];

	for ( int i = 1; i < 100; i++ )
	{
		TEST_EQ( vec.AddToTail( i ), i );
	}

	TEST_EQ( vec.Count(), 100 );
	TEST_TRUE( vec.NumAllocated() >= 100 );
	TEST_EQ( &vec[ 0 ], pFirst );

	for ( int i = 0; i < 100; i++ )
	{
		TEST_EQ( vec[ i ], i );
	}

	int nSum = 0;

	for ( int nValue : vec )
	{
		nSum += nValue;
	}

	TEST_EQ( nSum, 4950 );
}

REGISTER_NAMED_TEST( "CUtlBlockVector.RemoveAndFind", CUtlBlockVector_RemoveAndFind )
{
	// Removal keeps or swaps the order, Find and the count follow it.
	CUtlBlockVector< CUtlString > vec;

	vec.AddToTail( "a" );
	vec.AddToTail( "b" );
	vec.AddToTail( "c" );
	vec.AddToTail( "d" );

	TEST_EQ( vec.Find( "c" ), 2 );
	TEST_TRUE( vec.FindAndRemove( "b" ) );
	TEST_EQ( vec.Count(), 3 );
	TEST_EQ( V_strcmp( vec[ 1 ].Get(), "c" ), 0 );

	vec.FastRemove( 0 );
	TEST_EQ( vec.Count(), 2 );
	TEST_EQ( V_strcmp( vec[ 0 ].Get(), "d" ), 0 );
	TEST_EQ( vec.Find( "a" ), vec.InvalidIndex() );

	vec.SetCount( 5 );
	TEST_EQ( vec.Count(), 5 );
	TEST_TRUE( vec[ 4 ].IsEmpty() );

	vec.RemoveAll();
	TEST_TRUE( vec.IsEmpty() );
	TEST_TRUE( vec.NumAllocated() > 0 );

	vec.Purge();
	TEST_EQ( vec.NumAllocated(), 0 );
}

REGISTER_NAMED_TEST( "CUtlBlockVector.Swap", CUtlBlockVector_Swap )
{
	// Swap exchanges the elements and the pages they live in.
	CUtlBlockVector< int > first, second;

	first.AddToTail( 1 );
	first.AddToTail( 2 );
	second.AddToTail( 3 );

	int *pFirst = &first[ 0 ];

	first.Swap( second );
	TEST_EQ( first.Count(), 1 );
	TEST_EQ( second.Count(), 2 );
	TEST_EQ( first[ 0 ], 3 );
	TEST_EQ( &second[ 0 ], pFirst );
	TEST_EQ( second[ 1 ], 2 );
}

REGISTER_NAMED_TEST( "CUtlBlockVector.ShortIndex", CUtlBlockVector_ShortIndex )
{
	// A short index caps the capacity at its signed maximum.
	CUtlBlockVector< int, unsigned short > vec( 16 );

	for ( int i = 0; i < 1000; i++ )
	{
		vec.AddToTail( i );
	}

	TEST_EQ( vec.Count(), 1000 );
	TEST_EQ( vec[ ( unsigned short )999 ], 999 );
	TEST_TRUE( vec.NumAllocated() <= 32767 );
}
