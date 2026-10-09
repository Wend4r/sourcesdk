#include "common/assert.h"
#include "common/macros.h"

#include <tier1/utlsymbollarge.h>

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
