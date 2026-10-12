#include "common/assert.h"
#include "common/macros.h"

#include <tier0/utlsymbollarge.h>

REGISTER_NAMED_TEST( "CUtlSymbolTableLarge.AddFind", CUtlSymbolTableLarge_AddFind )
{
	// Symbol tables should intern equal strings into the same symbol.
	CUtlSymbolTableLarge symbols;

	CUtlSymbolLarge sym = symbols.AddString( "weapon_ak47" );

	TEST_EQ( V_strcmp( sym.String(), "weapon_ak47" ), 0 );
	TEST_EQ( symbols.AddString( "weapon_ak47" ).String(), sym.String() );
	TEST_EQ( symbols.FindString( "weapon_ak47" ).String(), sym.String() );
	TEST_EQ( symbols.GetNumStrings(), 1 );
}

REGISTER_NAMED_TEST( "CUtlSymbolTableLargeMT.AddFind", CUtlSymbolTableLargeMT_AddFind )
{
	// Thread-safe symbol tables should construct their mutex and intern strings.
	CUtlSymbolTableLargeMT symbols;

	CUtlSymbolLarge sym = symbols.AddString( "weapon_m4a1" );

	TEST_EQ( V_strcmp( sym.String(), "weapon_m4a1" ), 0 );
	TEST_EQ( symbols.AddString( "weapon_m4a1" ).String(), sym.String() );
	TEST_EQ( symbols.GetNumStrings(), 1 );
}

// Lookups go through the table's alternate key, inserts through the symbol id key
template < typename Table, bool bCaseInsensitive >
static void TestSymbolTableLarge()
{
	Table table;

	TEST_EQ( table.Find( "missing" ), UTL_INVAL_SYMBOL_LARGE );
	TEST_FALSE( table.FindString( "missing" ).IsValid() );

	UtlSymLargeId_t first = table.Add( "Alpha" );

	TEST_NE( first, UTL_INVAL_SYMBOL_LARGE );
	TEST_EQ( V_strcmp( table.String( first ), "Alpha" ), 0 );
	TEST_EQ( table.GetNumStrings(), 1 );

	TEST_EQ( table.Add( "Alpha" ), first );
	TEST_EQ( table.Find( "Alpha" ), first );
	TEST_EQ( table.Find( "ALPHA" ) == first, bCaseInsensitive );
	TEST_EQ( table.Add( "alpha" ) == first, bCaseInsensitive );
	TEST_EQ( table.GetNumStrings(), bCaseInsensitive ? 1 : 2 );

	// By length, the prefix is a different string from the full one
	TEST_EQ( table.Add( "Alphabet", 5 ), first );
	UtlSymLargeId_t alphabet = table.Add( "Alphabet" );
	TEST_NE( alphabet, first );
	TEST_EQ( table.Find( "Alphabet", 5 ), first );
	TEST_EQ( table.Find( "Alphabet" ), alphabet );

	CUtlSymbolLarge sym = table.AddString( "Beta" );
	TEST_EQ( V_strcmp( sym.String(), "Beta" ), 0 );
	TEST_TRUE( table.FindString( "Beta" ) == sym );

	// Enough strings of odd lengths to rehash the table several times and leave the hashes unaligned
	const int nCount = 1000;
	static UtlSymLargeId_t ids[ nCount ];
	char buf[ 64 ];
	int nUnalignedHashes = 0;

	for ( int i = 0; i < nCount; i++ )
	{
		int nLength = V_snprintf( buf, sizeof( buf ), "symbol_%d%.*s", i, i % 7, "xxxxxxx" );
		int nNumStrings = table.GetNumStrings();

		ids[ i ] = table.Add( buf, nLength );
		TEST_NE( ids[ i ], UTL_INVAL_SYMBOL_LARGE );
		TEST_EQ( table.GetNumStrings(), nNumStrings + 1 );

		if ( ( ( uintp )table.String( ids[ i ] ) - sizeof( LargeSymbolTableHashDecoration_t ) ) % alignof( LargeSymbolTableHashDecoration_t ) != 0 )
			nUnalignedHashes++;
	}

	TEST_NE( nUnalignedHashes, 0 );

	for ( int i = 0; i < nCount; i++ )
	{
		int nLength = V_snprintf( buf, sizeof( buf ), "symbol_%d%.*s", i, i % 7, "xxxxxxx" );

		TEST_EQ( table.Find( buf ), ids[ i ] );
		TEST_EQ( V_strcmp( table.String( ids[ i ] ), buf ), 0 );
		TEST_EQ( table.Hash( ids[ i ] ), CUtlSymbolLarge::Hash< bCaseInsensitive >( buf, nLength ) );
		TEST_EQ( table.Add( buf ), ids[ i ] );
	}

	TEST_EQ( table.Find( "Alpha" ), first );

	table.RemoveAll();
	TEST_EQ( table.Find( "Alpha" ), UTL_INVAL_SYMBOL_LARGE );
	TEST_EQ( table.Find( "symbol_1x" ), UTL_INVAL_SYMBOL_LARGE );

	first = table.Add( "Alpha" );
	TEST_NE( first, UTL_INVAL_SYMBOL_LARGE );
	TEST_EQ( table.Find( "Alpha" ), first );

	table.Purge();
	TEST_EQ( table.Find( "Alpha" ), UTL_INVAL_SYMBOL_LARGE );
}

REGISTER_NAMED_TEST( "CUtlSymbolTableLarge.ManySymbols", CUtlSymbolTableLarge_ManySymbols )
{
	TestSymbolTableLarge< CUtlSymbolTableLarge, false >();
	TestSymbolTableLarge< CUtlSymbolTableLarge_CI, true >();
	TestSymbolTableLarge< CUtlSymbolTableLargeMT, false >();
	TestSymbolTableLarge< CUtlSymbolTableLargeMT_CI, true >();
}
