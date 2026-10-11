#include "common/assert.h"
#include "common/macros.h"

#include <tier1/utlhandletable.h>

REGISTER_NAMED_TEST( "CUtlHandleTable.AddGetRemove", CUtlHandleTable_AddGetRemove )
{
	// Handles should round-trip their index and serial number.
	CUtlHandleTable< int, 16 > table;
	int nFirst = 1;
	int nSecond = 2;

	UtlHandle_t hFirst = table.AddHandle();
	UtlHandle_t hSecond = table.AddHandle();
	table.SetHandle( hFirst, &nFirst );
	table.SetHandle( hSecond, &nSecond );

	TEST_NE( hFirst, hSecond );
	TEST_EQ( table.GetHandleCount(), 2u );
	TEST_EQ( table.GetValidHandleCount(), 2u );
	TEST_TRUE( table.IsHandleValid( hFirst ) );
	TEST_EQ( table.GetHandle( hFirst ), &nFirst );
	TEST_EQ( table.GetHandle( hSecond ), &nSecond );
	TEST_EQ( table.GetIndexFromHandle( hFirst ), 0 );
	TEST_EQ( table.GetIndexFromHandle( hSecond ), 1 );
	TEST_EQ( table.GetHandleFromIndex( 1 ), hSecond );

	table.MarkHandleInvalid( hFirst );
	TEST_FALSE( table.IsHandleValid( hFirst ) );
	TEST_NULL( table.GetHandle( hFirst ) );
	TEST_EQ( table.GetHandle( hFirst, false ), &nFirst );
	TEST_EQ( table.GetValidHandleCount(), 1u );

	table.MarkHandleValid( hFirst );
	TEST_TRUE( table.IsHandleValid( hFirst ) );

	// A handle with the same index but another serial number must be rejected.
	UtlHandle_t hStale = hFirst ^ ( 1u << 16 );
	TEST_EQ( table.GetIndexFromHandle( hStale ), 0 );
	TEST_FALSE( table.IsHandleValid( hStale ) );
	TEST_NULL( table.GetHandle( hStale ) );
}
