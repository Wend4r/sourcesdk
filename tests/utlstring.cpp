#include "common/assert.h"
#include "common/macros.h"

#include <tier0/utlstring.h>

#include <cstring>

REGISTER_NAMED_TEST( "CUtlString.Empty", CUtlString_Empty )
{
	// Empty strings should start null-safe and report zero visible length.
	CUtlString sString;

	TEST_TRUE( sString.IsEmpty() );
	TEST_EQ( sString.Length(), 0 );
	TEST_EQ( V_strcmp( sString.Get(), "" ), 0 );
}

REGISTER_NAMED_TEST( "CUtlString.CopyAssignCompare", CUtlString_CopyAssignCompare )
{
	// Copying, assignment and comparison should preserve visible text.
	CUtlString sString( "alpha" );
	CUtlString sCopy( sString );
	CUtlString sAssigned;

	sAssigned = "alpha";

	TEST_EQ( sString.Length(), 5 );
	TEST_TRUE( sString == sCopy );
	TEST_TRUE( sAssigned == sString );

	sAssigned = "beta";
	TEST_TRUE( sAssigned != sString );
}

REGISTER_NAMED_TEST( "CUtlString.ConcatClearNull", CUtlString_ConcatClearNull )
{
	// Concatenation, clear and null assignment should keep the string API stable.
	CUtlString sString( "alpha" );

	sString += "-";
	sString += "beta";

	TEST_EQ( V_strcmp( sString.Get(), "alpha-beta" ), 0 );

	sString.Clear();
	TEST_TRUE( sString.IsEmpty() );

	sString = nullptr;
	TEST_TRUE( sString.IsEmpty() );
}

REGISTER_NAMED_TEST( "CUtlString.SubstringReplaceTrimSwap", CUtlString_SubstringReplaceTrimSwap )
{
	// Substring, replace, trim and swap helpers should return the expected text slices.
	CUtlString sString( "  alpha/beta.txt  " );

	CUtlString sTrimmed( sString );

	sTrimmed.Trim();
	TEST_EQ( V_strcmp( sTrimmed.Get(), "alpha/beta.txt" ), 0 );

	TEST_EQ( V_strcmp( sTrimmed.Left( 5 ).Get(), "alpha" ), 0 );
	TEST_EQ( V_strcmp( sTrimmed.Right( 3 ).Get(), "txt" ), 0 );
	TEST_EQ( V_strcmp( sTrimmed.Slice( 6, 10 ).Get(), "beta" ), 0 );
	TEST_EQ( V_strcmp( sTrimmed.Replace( '/', '_' ).Get(), "alpha_beta.txt" ), 0 );

	CUtlString sOther( "other" );

	sTrimmed.Swap( sOther );
	TEST_EQ( V_strcmp( sTrimmed.Get(), "other" ), 0 );
	TEST_EQ( V_strcmp( sOther.Get(), "alpha/beta.txt" ), 0 );
}

REGISTER_NAMED_TEST( "CUtlString.MoveConstructAssign", CUtlString_MoveConstructAssign )
{
	// Move construction and move assignment should keep transferred text readable.
	CUtlString original( "moved-value" );

	CUtlString moved( Move( original ) );

	TEST_EQ( V_strcmp( moved.Get(), "moved-value" ), 0 );

	CUtlString assigned;

	assigned = Move( moved );
	TEST_EQ( V_strcmp( assigned.Get(), "moved-value" ), 0 );
}
