#include "common/assert.h"
#include "common/macros.h"

#include <tier1/generichash.h>

#include <cstring>

REGISTER_NAMED_TEST( "GenericHash.ConstexprIntegers", GenericHash_ConstexprIntegers )
{
	// Integer hashes should be usable in constant expressions and match their runtime results.
	constexpr uint32 nHashInt = HashInt( 0x12345678 );
	constexpr uint32 nHashNegative = HashInt( -1 );
	constexpr uint32 nHashAlternate = HashIntAlternate( 0x12345678 );
	constexpr uint32 nHashConventional = HashIntConventional( 0x12345678 );
	constexpr unsigned nHashItem = HashItem< int >( 0x12345678 );

	volatile int nValue = 0x12345678;
	volatile int nNegative = -1;

	TEST_EQ( nHashInt, HashInt( nValue ) );
	TEST_EQ( nHashNegative, HashInt( nNegative ) );
	TEST_EQ( nHashAlternate, HashIntAlternate( ( uint32 )nValue ) );
	TEST_EQ( nHashConventional, HashIntConventional( nValue ) );
	TEST_EQ( nHashItem, nHashInt );
	TEST_EQ( HashItem< unsigned >( 0x12345678u ), nHashInt );
	TEST_TRUE( nHashNegative <= 0xffff );
}

REGISTER_NAMED_TEST( "GenericHash.ConstexprStrings", GenericHash_ConstexprStrings )
{
	// The string literal variants should fold at compile time and agree with the runtime hashes.
	constexpr uint32 nSeed = 0x3501A674;

	constexpr uint32 nLiteral = MurmurHash2( "weapon_ak47", nSeed );
	constexpr uint32 nLiteralTail1 = MurmurHash2( "abcde", nSeed );
	constexpr uint32 nLiteralTail2 = MurmurHash2( "abcdef", nSeed );
	constexpr uint32 nLiteralTail3 = MurmurHash2( "abcdefg", nSeed );
	constexpr uint32 nLower = MurmurHash2LowerCase( "Weapon_AK47", nSeed );
	constexpr uint32 nConventional = HashStringConventional( "weapon_ak47" );

	const char *pString = "weapon_ak47";
	const char *pMixedCase = "Weapon_AK47";

	TEST_EQ( nLiteral, MurmurHash2( pString, ( int )strlen( pString ), nSeed ) );
	TEST_EQ( nLiteralTail1, MurmurHash2( "abcde", 5, nSeed ) );
	TEST_EQ( nLiteralTail2, MurmurHash2( "abcdef", 6, nSeed ) );
	TEST_EQ( nLiteralTail3, MurmurHash2( "abcdefg", 7, nSeed ) );
	TEST_EQ( nLower, nLiteral );
	TEST_EQ( nLower, MurmurHash2LowerCase( pMixedCase, nSeed ) );
	TEST_EQ( nLower, MurmurHash2LowerCase( pMixedCase, ( int )strlen( pMixedCase ), nSeed ) );
	TEST_EQ( nLower, HashStringCaseless( pMixedCase ) );
	TEST_EQ( nLiteral, HashString( pString ) );
	TEST_EQ( nConventional, HashStringConventional( pString ) );

	// A partially filled buffer should hash like the string it holds, not its whole capacity.
	char szBuffer[ 64 ] = "Weapon_AK47";

	TEST_EQ( MurmurHash2LowerCase( szBuffer, nSeed ), nLower );
	TEST_EQ( MurmurHash2( szBuffer, nSeed ), MurmurHash2( "Weapon_AK47", nSeed ) );
}
