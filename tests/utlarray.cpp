#include "common/assert.h"
#include "common/macros.h"

#include <mathlib/mathlib.h>
#include <tier1/utlarray.h>

static int __cdecl CompareArrayIntsAscending( const int *pLeft, const int *pRight )
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

REGISTER_NAMED_TEST( "CUtlArray.FixedCountAndAccess", CUtlArray_FixedCountAndAccess )
{
	// Fixed-size arrays should report stable count, allocation and element access.
	CUtlArray< int, 4 > array;

	array.FillWithValue( 7 );

	TEST_EQ( array.Count(), 4 );
	TEST_EQ( array.NumAllocated(), 4 );
	TEST_TRUE( array.IsValidIndex( 3 ) );
	TEST_FALSE( array.IsValidIndex( 4 ) );
	TEST_EQ( array[0], 7 );
	TEST_EQ( array[3], 7 );
}

REGISTER_NAMED_TEST( "CUtlArray.CopyFindSwap", CUtlArray_CopyFindSwap )
{
	// Copy helpers and swaps should preserve the expected element layout.
	const int values[] = { 1, 2, 3 };

	CUtlArray< int, 4 > array;

	array.CopyArray( values, 3 );
	array[3] = 4;

	TEST_EQ( array.Find( 3 ), 2 );
	TEST_TRUE( array.HasElement( 4 ) );
	TEST_FALSE( array.HasElement( 5 ) );

	CUtlArray< int, 4 > other;

	other.FillWithValue( 9 );
	array.Swap( other );

	TEST_EQ( array[0], 9 );
	TEST_EQ( other[0], 1 );
	TEST_EQ( other[3], 4 );
}

REGISTER_NAMED_TEST( "CUtlArray.AssignmentAndSort", CUtlArray_AssignmentAndSort )
{
	// Copy construction, assignment and sorting should all keep values reachable.
	CUtlArray< int, 4 > array;

	array[0] = 4;
	array[1] = 2;
	array[2] = 3;
	array[3] = 1;

	CUtlArray< int, 4 > copy( array );
	CUtlArray< int, 4 > assigned;

	assigned = array;

	copy.Sort( &CompareArrayIntsAscending );

	TEST_EQ( copy[0], 1 );
	TEST_EQ( copy[3], 4 );
	TEST_EQ( assigned[0], 4 );
	TEST_EQ( assigned.InvalidIndex(), -1 );
}

REGISTER_NAMED_TEST( "CUtlArray.MoveConstructAssign", CUtlArray_MoveConstructAssign )
{
	// Move construction and move assignment should transfer the stored values.
	CUtlArray< int, 4 > array;

	array[0] = 8;
	array[1] = 6;
	array[2] = 4;
	array[3] = 2;

	CUtlArray< int, 4 > moved( Move( array ) );

	TEST_EQ( moved[0], 8 );
	TEST_EQ( moved[3], 2 );

	CUtlArray< int, 4 > assigned;

	assigned = Move( moved );
	TEST_EQ( assigned[1], 6 );
	TEST_EQ( assigned[2], 4 );
}
