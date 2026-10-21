#include "common/assert.h"
#include "common/macros.h"

#include <tier0/platform.h>
#include <tier0/bufferstring.h>
#include <tier0/characterset.h>
#include <tier0/exprevaluator.h>
#include <tier0/filterstring.h>
#include <tier0/formatoutputstream.h>
#include <tier0/gridstringprinter.h>
#include <tier0/identifyfailuremessages.h>
#include <tier0/utlbuffer.h>
#include <tier0/utlstablesymboltable.h>
#include <tier0/utlstring.h>
#include <tier0/utlstringtoken.h>

#include <cstring>
#include <cwchar>

REGISTER_NAMED_TEST( "StringToken.MakeMatchesTheInlineHash", StringToken_MakeMatchesTheInlineHash )
{
	TEST_EQ( CUtlStringToken::Make( "Hello" ).GetHashCode(), MakeStringToken( "Hello" ) );

	// Tokens ignore case.
	TEST_EQ( CUtlStringToken::Make( "HELLO" ).GetHashCode(), CUtlStringToken::Make( "hello" ).GetHashCode() );
}

REGISTER_NAMED_TEST( "StringAndToken.PointsAtTheStringWithItsToken", StringAndToken_PointsAtTheStringWithItsToken )
{
	const char szName[] = "Entity42";
	CUtlStringAndToken name( szName );

	// The string is not copied.
	TEST_EQ( name.Get(), static_cast< const char * >( szName ) );
	TEST_EQ( name.GetToken().GetHashCode(), MakeStringToken( "entity42" ) );
	TEST_TRUE( name.IsEqual_FastCaseInsensitive( "ENTITY42" ) );

	CUtlStringAndToken empty;
	TEST_NULL( empty.Get() );
}

REGISTER_NAMED_TEST( "StringAndToken.StorageKeepsItsOwnCopy", StringAndToken_StorageKeepsItsOwnCopy )
{
	char szName[] = "Entity42";
	CUtlStringAndTokenWithStorage name( szName );

	// Changing the source does not change the stored copy.
	szName[ 0 ] = 'X';
	TEST_EQ( strcmp( name.Get(), "Entity42" ), 0 );

	// A copy points at its own storage.
	CUtlStringAndTokenWithStorage copy( name );
	TEST_NE( copy.Get(), name.Get() );
	TEST_EQ( strcmp( copy.Get(), "Entity42" ), 0 );
	TEST_EQ( copy.GetToken().GetHashCode(), name.GetToken().GetHashCode() );

	copy.Set( "Other" );
	TEST_EQ( strcmp( copy.Get(), "Other" ), 0 );
	TEST_EQ( copy.GetToken().GetHashCode(), MakeStringToken( "other" ) );
}

REGISTER_NAMED_TEST( "StringAndToken.FormatKeepsTheTextAndItsToken", StringAndToken_FormatKeepsTheTextAndItsToken )
{
	CUtlStringAndTokenWithStorage name;
	name.Format( "Entity%d", 42 );

	TEST_EQ( strcmp( name.Get(), "Entity42" ), 0 );
	TEST_EQ( name.GetToken().GetHashCode(), MakeStringToken( "entity42" ) );

	TEST_TRUE( name.IsEqual_CaseSensitive( "Entity42" ) );
	TEST_FALSE( name.IsEqual_CaseSensitive( "ENTITY42" ) );
	TEST_TRUE( name.IsEqual_FastCaseInsensitive( "ENTITY42" ) );
	TEST_FALSE( name.IsEqual_FastCaseInsensitive( "Entity4" ) );
}

REGISTER_NAMED_TEST( "BufferString.SliceTakesARangeOfChars", BufferString_SliceTakesARangeOfChars )
{
	CBufferString text( "Hello, world" );

	TEST_EQ( strcmp( text.Slice( 0, 5 ).Get(), "Hello" ), 0 );

	// Without an end the slice runs to the end of the string.
	TEST_EQ( strcmp( text.Slice( 7 ).Get(), "world" ), 0 );

	// A negative start counts from the end.
	TEST_EQ( strcmp( text.Slice( -5 ).Get(), "world" ), 0 );
}

REGISTER_NAMED_TEST( "BufferString.ReplaceCharWithString", BufferString_ReplaceCharWithString )
{
	CBufferString list( "a,b,c" );

	TEST_EQ( list.Replace( ',', " and " ), 2 );
	TEST_EQ( strcmp( list.Get(), "a and b and c" ), 0 );
}

REGISTER_NAMED_TEST( "UtlString.RemoveFromEnd", UtlString_RemoveFromEnd )
{
	CUtlString fileName( "readme.txt" );

	TEST_EQ( strcmp( fileName.RemoveFromEnd( ".txt" ).Get(), "readme" ), 0 );
	TEST_EQ( strcmp( fileName.RemoveFromEnd( ".TXT", false ).Get(), "readme" ), 0 );

	// A different ending leaves the string as it is.
	TEST_EQ( strcmp( fileName.RemoveFromEnd( ".md" ).Get(), "readme.txt" ), 0 );
}

REGISTER_NAMED_TEST( "FilterString.MatchesTermsAlternativesAndExclusions", FilterString_MatchesTermsAlternativesAndExclusions )
{
	// "fox" but not "dog", or "cat".
	CFilterString filter( "fox -dog|cat", FILTER_STRING_OP_CONTAINS, FILTER_STRING_CASE_INSENSITIVE );

	TEST_TRUE( filter.DoesMatchString( "quick fox" ) );
	TEST_TRUE( filter.DoesMatchString( "foxes" ) );
	TEST_FALSE( filter.DoesMatchString( "fox and dog" ) );
	TEST_TRUE( filter.DoesMatchString( "Cat" ) );
	TEST_FALSE( filter.DoesMatchString( "bird" ) );
}

REGISTER_NAMED_TEST( "FilterString.EmptyFilterMatchesEverything", FilterString_EmptyFilterMatchesEverything )
{
	CFilterString filter( "  ", FILTER_STRING_OP_CONTAINS, FILTER_STRING_CASE_INSENSITIVE );

	// The filter is trimmed, an empty one matches any string.
	TEST_TRUE( filter.IsEmpty() );
	TEST_TRUE( filter.DoesMatchString( "anything" ) );

	filter.SetString( "fox", FILTER_STRING_OP_CONTAINS, FILTER_STRING_CASE_INSENSITIVE );
	TEST_FALSE( filter.IsEmpty() );
	TEST_EQ( strcmp( filter.GetString(), "fox" ), 0 );
}

REGISTER_NAMED_TEST( "FilterString.CaseSensitiveMode", FilterString_CaseSensitiveMode )
{
	CFilterString filter( "cat", FILTER_STRING_OP_CONTAINS, FILTER_STRING_CASE_SENSITIVE );

	TEST_TRUE( filter.DoesMatchString( "cat" ) );
	TEST_FALSE( filter.DoesMatchString( "Cat" ) );

	// SetString replaces the filter.
	filter.SetString( "Cat", FILTER_STRING_OP_CONTAINS, FILTER_STRING_CASE_SENSITIVE );
	TEST_TRUE( filter.DoesMatchString( "Cat" ) );
	TEST_FALSE( filter.DoesMatchString( "cat" ) );
}

REGISTER_NAMED_TEST( "FilterString.EqualsAndNegations", FilterString_EqualsAndNegations )
{
	// Equality compares the whole filter, without splitting it into terms.
	CFilterString equals( "fox -dog", FILTER_STRING_OP_EQUALS, FILTER_STRING_CASE_INSENSITIVE );
	TEST_TRUE( equals.DoesMatchString( "FOX -DOG" ) );
	TEST_FALSE( equals.DoesMatchString( "fox" ) );

	CFilterString notEquals( "fox", FILTER_STRING_OP_NOT_EQUALS, FILTER_STRING_CASE_INSENSITIVE );
	TEST_FALSE( notEquals.DoesMatchString( "Fox" ) );
	TEST_TRUE( notEquals.DoesMatchString( "foxes" ) );

	CFilterString notContains( "fox -dog|cat", FILTER_STRING_OP_NOT_CONTAINS, FILTER_STRING_CASE_INSENSITIVE );
	TEST_FALSE( notContains.DoesMatchString( "quick fox" ) );
	TEST_TRUE( notContains.DoesMatchString( "fox and dog" ) );
	TEST_TRUE( notContains.DoesMatchString( "bird" ) );
}

REGISTER_NAMED_TEST( "FilterString.ComparesNumbers", FilterString_ComparesNumbers )
{
	CFilterString less( "10", FILTER_STRING_OP_LESS, FILTER_STRING_CASE_INSENSITIVE );
	TEST_TRUE( less.DoesMatchString( "5" ) );
	TEST_FALSE( less.DoesMatchString( "10" ) );

	CFilterString greater( "10", FILTER_STRING_OP_GREATER, FILTER_STRING_CASE_INSENSITIVE );
	TEST_TRUE( greater.DoesMatchString( "15" ) );
	TEST_FALSE( greater.DoesMatchString( "10" ) );

	CFilterString lessOrEqual( "10", FILTER_STRING_OP_LESS_EQUAL, FILTER_STRING_CASE_INSENSITIVE );
	TEST_TRUE( lessOrEqual.DoesMatchString( "10" ) );
	TEST_FALSE( lessOrEqual.DoesMatchString( "15" ) );

	CFilterString greaterOrEqual( "10", FILTER_STRING_OP_GREATER_EQUAL, FILTER_STRING_CASE_INSENSITIVE );
	TEST_TRUE( greaterOrEqual.DoesMatchString( "10" ) );
	TEST_FALSE( greaterOrEqual.DoesMatchString( "5" ) );
}

REGISTER_NAMED_TEST( "StableSymbolTable.SymbolsKeepTheirIds", StableSymbolTable_SymbolsKeepTheirIds )
{
	CStableSymbolTable table;
	bool bIsNew = false;

	const int nAlpha = table.GetId( "Alpha", &bIsNew );
	TEST_EQ( nAlpha, 0 );
	TEST_TRUE( bIsNew );

	// The lookup ignores case and returns the existing id.
	TEST_EQ( table.GetId( "ALPHA", &bIsNew ), nAlpha );
	TEST_FALSE( bIsNew );

	TEST_EQ( table.GetId( "Beta", &bIsNew ), 1 );
	TEST_TRUE( bIsNew );

	TEST_EQ( table.Count(), 2 );
	TEST_EQ( strcmp( table.GetSymbol( nAlpha ), "Alpha" ), 0 );
	TEST_EQ( strcmp( table.GetAllSymbols()[ 1 ], "Beta" ), 0 );

	// The ids run from 0 to Count() - 1.
	const char *pszOrder[] = { "Alpha", "Beta" };

	FOR_EACH_STABLE_SYMBOL( table, nId )
	{
		TEST_EQ( strcmp( table.GetSymbol( nId ), pszOrder[ nId ] ), 0 );
	}

	int nLastId = -1;

	FOR_EACH_STABLE_SYMBOL_BACK( table, nId )
	{
		nLastId = nId;
	}

	TEST_EQ( nLastId, 0 );

	// Insert places a symbol at a chosen id, growing the table up to it.
	table.Insert( 5, "Gamma" );
	TEST_EQ( table.Count(), 6 );
	TEST_EQ( table.GetId( "gamma", &bIsNew ), 5 );
	TEST_FALSE( bIsNew );
}

// Exposes the hash and equality of the symbol table for the tests.
class CTestStableSymbolTable : public CStableSymbolTable
{
public:
	using CStableSymbolTable::Hash_t;
	using CStableSymbolTable::Eq_t;

	// Finds the id of a symbol with the same hash and equality that tier0 uses in GetId
	int Find( const char *pSymbol ) const
	{
		UtlHashHandle_t hSymbol = m_SymbolToId.Find( pSymbol );
		return hSymbol != m_SymbolToId.InvalidHandle() ? m_SymbolToId[ hSymbol ] : -1;
	}
};

REGISTER_NAMED_TEST( "StableSymbolTable.HashAndEqualityIgnoreCase", StableSymbolTable_HashAndEqualityIgnoreCase )
{
	CTestStableSymbolTable::Hash_t hash;
	CTestStableSymbolTable::Eq_t eq;

	TEST_EQ( hash( "Alpha" ), hash( "ALPHA" ) );
	TEST_EQ( hash( "Alpha" ), static_cast< uint >( MakeStringToken( "alpha" ) ) );
	TEST_NE( hash( "Alpha" ), hash( "Beta" ) );

	TEST_TRUE( eq( "Alpha", "aLPHA" ) );
	TEST_FALSE( eq( "Alpha", "Alpha2" ) );

	// Lookups through the table find what tier0 added.
	CTestStableSymbolTable table;
	bool bIsNew = false;
	const int nBeta = table.GetId( "Beta", &bIsNew );

	TEST_EQ( table.Find( "BETA" ), nBeta );
	TEST_EQ( table.Find( "Gamma" ), -1 );
}

REGISTER_NAMED_TEST( "UtlBuffer.AccessPutAndAccessGetMoveTheCursors", UtlBuffer_AccessPutAndAccessGetMoveTheCursors )
{
	CUtlBuffer buffer;

	void *pPut = buffer.AccessPut( 8 );
	TEST_NOT_NULL( pPut );
	TEST_EQ( buffer.TellPut(), 8 );
	memcpy( pPut, "ABCDEFGH", 8 );

	const char *pGet = static_cast< const char * >( buffer.AccessGet( 3 ) );
	TEST_NOT_NULL( pGet );
	TEST_EQ( memcmp( pGet, "ABC", 3 ), 0 );
	TEST_EQ( buffer.TellGet(), 3 );

	// Reading past what was written fails.
	TEST_NULL( buffer.AccessGet( 100 ) );
}

REGISTER_NAMED_TEST( "UtlBuffer.ParseTokenIntoBufferString", UtlBuffer_ParseTokenIntoBufferString )
{
	CUtlBuffer text( 0, 0, CUtlBuffer::TEXT_BUFFER );
	text.PutString( "  first second" );

	characterset_t breaks;
	CharacterSetBuild( &breaks, "" );

	// Leading whitespace is skipped, the token ends at the next space.
	CBufferString token;
	TEST_EQ( text.ParseToken( &breaks, &token ), 5 );
	TEST_EQ( strcmp( token.Get(), "first" ), 0 );

	CUtlBuffer tagged( 0, 0, CUtlBuffer::TEXT_BUFFER );
	tagged.PutString( "<name>rest" );

	CBufferString tag;
	TEST_TRUE( tagged.ParseToken( "<", ">", &tag ) );
	TEST_EQ( strcmp( tag.Get(), "name" ), 0 );
}

REGISTER_NAMED_TEST( "FormatOutputStream.CharBufferStopsWhenFull", FormatOutputStream_CharBufferStopsWhenFull )
{
	char szBuffer[ 8 ];
	CCharBufferFormatOutputStream stream( szBuffer, sizeof( szBuffer ) );

	stream.PutS( "abc" );
	TEST_FALSE( stream.IsFull() );
	TEST_EQ( stream.RemainingCapacity(), 4 );

	// The rest is cut off, one char stays free for the terminator.
	stream.PutS( "defghijk" );
	stream.Terminate();

	TEST_TRUE( stream.IsFull() );
	TEST_EQ( stream.GetCharsWritten(), 7 );
	TEST_EQ( strcmp( szBuffer, "abcdefg" ), 0 );
}

REGISTER_NAMED_TEST( "FormatOutputStream.BufferStringAppends", FormatOutputStream_BufferStringAppends )
{
	CBufferString text;
	CBufferStringFormatOutputStream stream( text );

	stream.PutS( "hello " );
	stream.PutRepeatedChar( '*', 3 );
	stream.PutC( '!' );

	TEST_EQ( strcmp( text.Get(), "hello ***!" ), 0 );
	TEST_EQ( stream.GetCharsWritten(), 10 );
}

REGISTER_NAMED_TEST( "FormatStringElement.ParsesOneConversion", FormatStringElement_ParsesOneConversion )
{
	// ParseElement takes the text after the '%' and returns what follows the conversion.
	CFormatStringElement element;
	const char *pszRest = element.ParseElement( "-8.3f tail" );

	TEST_EQ( strcmp( element, "%-8.3f" ), 0 );
	TEST_EQ( strcmp( pszRest, " tail" ), 0 );

	// EnforceTypeAndSize swaps a conversion type that is not allowed for the first allowed one.
	CFormatStringElement number( "%5d" );
	number.EnforceTypeAndSize( "s", "" );
	TEST_EQ( strcmp( number, "%5s" ), 0 );

	// The element holds up to 11 chars of the conversion.
	CFormatStringElement longElement( "%-123456789.3f" );
	TEST_EQ( strcmp( longElement, "%-123456789" ), 0 );
}

REGISTER_NAMED_TEST( "GridStringPrinter.LinesUpColumns", GridStringPrinter_LinesUpColumns )
{
	CGridStringPrinter grid;
	grid.SetColumnHeader( 0, "Name" );
	grid.SetColumnHeader( 1, "Count" );

	grid.SetString( 0, 0, "apples" );
	grid.SetString( 0, 1, "3" );

	// DelimitedPrint adds a row, split at the delimiter.
	grid.DelimitedPrint( ',', "%s,%d", "pears", 12 );

	TEST_EQ( grid.NumRows(), 2 );
	TEST_EQ( grid.NumColumns(), 2 );
	TEST_EQ( strcmp( grid.GetString( 1, 0 ).Get(), "pears" ), 0 );
	TEST_EQ( strcmp( grid.GetColumnHeader( 1 ).Get(), "Count" ), 0 );

	// The rows and columns can be walked in both directions.
	CUtlString names;

	FOR_EACH_GRID_ROW( grid, nRow )
	{
		names.Append( grid.GetString( nRow, 0 ).Get() );
	}

	FOR_EACH_GRID_ROW_BACK( grid, nRow )
	{
		names.Append( grid.GetString( nRow, 0 ).Get() );
	}

	TEST_EQ( strcmp( names.Get(), "applespearspearsapples" ), 0 );

	CUtlString headers;

	FOR_EACH_GRID_COLUMN( grid, nColumn )
	{
		headers.Append( grid.GetColumnHeader( nColumn ).Get() );
	}

	FOR_EACH_GRID_COLUMN_BACK( grid, nColumn )
	{
		headers.Append( grid.GetColumnHeader( nColumn ).Get() );
	}

	TEST_EQ( strcmp( headers.Get(), "NameCountCountName" ), 0 );

	// Every column is as wide as its longest value.
	CUtlVector< CUtlString > lines;
	grid.GetLines( lines, true );

	TEST_EQ( lines.Count(), 4 );
	TEST_EQ( strcmp( lines[ 0 ].Get(), "   Name | Count " ), 0 );
	TEST_EQ( strcmp( lines[ 1 ].Get(), " ------ | ----- " ), 0 );
	TEST_EQ( strcmp( lines[ 2 ].Get(), " apples |     3 " ), 0 );
	TEST_EQ( strcmp( lines[ 3 ].Get(), "  pears |    12 " ), 0 );
}

REGISTER_NAMED_TEST( "IdentifyFailureMessages.FindsAFatalError", IdentifyFailureMessages_FindsAFatalError )
{
	CIdentifyFailureMessages scanner;
	const char *pszEnd = nullptr;
	const char szLog[] = "loading map\n12(Main):  FATAL ERROR: Out of memory\n";

	// Ordinary lines are skipped, the log prefix in front of a line is ignored.
	TEST_EQ( scanner.ProcessText( szLog, -1, &pszEnd ), CIdentifyFailureMessages::STATE_FATAL_ERROR_FOUND );
	TEST_TRUE( scanner.HasFoundFailure() );
	TEST_EQ( strcmp( scanner.GetMessage(), "Out of memory" ), 0 );
	TEST_EQ( pszEnd, szLog + sizeof( szLog ) - 1 );

	// The next text starts a new search.
	TEST_EQ( scanner.ProcessText( "plain line\n", -1, &pszEnd ), CIdentifyFailureMessages::STATE_LINE_START );
	TEST_FALSE( scanner.HasFoundFailure() );
	TEST_EQ( strcmp( scanner.GetMessage(), "" ), 0 );
}

REGISTER_NAMED_TEST( "IdentifyFailureMessages.CollectsAFailedAssertion", IdentifyFailureMessages_CollectsAFailedAssertion )
{
	CIdentifyFailureMessages scanner;
	const char *pszEnd = nullptr;

	TEST_EQ( scanner.ProcessText( "game/server/player.cpp (123) : Assertion Failed in function Think: pPlayer\n", -1, &pszEnd ), CIdentifyFailureMessages::STATE_ASSERTION );
	TEST_EQ( strcmp( scanner.GetFile(), "game/server/player.cpp" ), 0 );
	TEST_EQ( scanner.GetLine(), 123 );

	// The line after the assertion completes it.
	TEST_EQ( scanner.ProcessText( "callstack\n", -1, &pszEnd ), CIdentifyFailureMessages::STATE_ASSERTION_FOUND );
	TEST_EQ( strcmp( scanner.GetMessage(), "Assertion Failed in function Think: pPlayer\ncallstack\n" ), 0 );
}

REGISTER_NAMED_TEST( "IdentifyFailureMessages.CollectsAFailedAssertMsg", IdentifyFailureMessages_CollectsAFailedAssertMsg )
{
	CIdentifyFailureMessages scanner;
	const char *pszEnd = nullptr;

	TEST_EQ( scanner.ProcessText( "game/server/player.cpp (77) : AssertMsg Failed in function Spawn\n", -1, &pszEnd ), CIdentifyFailureMessages::STATE_ASSERTMSG );
	TEST_EQ( scanner.ProcessText( "Bad spawn point\n", -1, &pszEnd ), CIdentifyFailureMessages::STATE_ASSERTMSG_TEXT );
	TEST_EQ( scanner.ProcessText( "callstack\n", -1, &pszEnd ), CIdentifyFailureMessages::STATE_ASSERTION_FOUND );

	TEST_EQ( strcmp( scanner.GetMessage(), "AssertMsg Failed in function Spawn\nBad spawn point\ncallstack\n" ), 0 );
	TEST_EQ( scanner.GetLine(), 77 );
}

REGISTER_NAMED_TEST( "IdentifyFailureMessages.EmptyText", IdentifyFailureMessages_EmptyText )
{
	CIdentifyFailureMessages scanner;
	const char *pszEnd = nullptr;

	TEST_EQ( scanner.ProcessText( "", -1, &pszEnd ), CIdentifyFailureMessages::STATE_NO_TEXT );
	TEST_FALSE( scanner.HasFoundFailure() );
}

REGISTER_NAMED_TEST( "GridStringPrinter.WrapsAtSpaces", GridStringPrinter_WrapsAtSpaces )
{
	// A wrapped column cuts the value at its maximum length, or after the last space before it.
	const char *pszExpected[ 2 ][ 3 ] =
	{
		{ " alpha beta ", " gamma delt ", "          a " },
		{ "     alpha  ", " betagamma  ", "      delta " },
	};

	for ( int nWrapAtSpaces = 0; nWrapAtSpaces < 2; nWrapAtSpaces++ )
	{
		CGridStringPrinter grid;
		grid.SetColumnHeader( 0, "Words" );
		grid.SetMaxStringLengthAllColumns( 10 );
		grid.SetWordWrapColumn( 0, true, nWrapAtSpaces != 0 );
		grid.SetString( 0, 0, "alpha betagamma delta" );

		CUtlVector< CUtlString > lines;
		grid.GetLines( lines, false );

		TEST_EQ( lines.Count(), 3 );

		for ( int i = 0; i < 3; i++ )
		{
			TEST_EQ( strcmp( lines[ i ].Get(), pszExpected[ nWrapAtSpaces ][ i ] ), 0 );
		}
	}
}

REGISTER_NAMED_TEST( "DefaultConditionalSymbolProc.PlatformSymbols", DefaultConditionalSymbolProc_PlatformSymbols )
{
	// The $ is optional and the case does not matter.
#if defined( _WIN32 )
	TEST_TRUE( DefaultConditionalSymbolProc( "$WIN32", nullptr ) );
	TEST_TRUE( DefaultConditionalSymbolProc( "windows", nullptr ) );
	TEST_FALSE( DefaultConditionalSymbolProc( "$LINUX", nullptr ) );
#elif defined( OSX )
	TEST_TRUE( DefaultConditionalSymbolProc( "$OSX", nullptr ) );
	TEST_FALSE( DefaultConditionalSymbolProc( "$WIN32", nullptr ) );
#else
	TEST_TRUE( DefaultConditionalSymbolProc( "$LINUX", nullptr ) );
	TEST_TRUE( DefaultConditionalSymbolProc( "posix", nullptr ) );
	TEST_FALSE( DefaultConditionalSymbolProc( "$WIN32", nullptr ) );
	TEST_FALSE( DefaultConditionalSymbolProc( "$OSX", nullptr ) );
#endif
}

REGISTER_NAMED_TEST( "DefaultConditionalErrorProc.Warns", DefaultConditionalErrorProc_Warns )
{
	// Only a warning, the call returns.
	DefaultConditionalErrorProc( "DefaultConditionalErrorProc test", nullptr );
}

REGISTER_NAMED_TEST( "DefaultConditionalSymbolProc.EvaluatorDefault", DefaultConditionalSymbolProc_EvaluatorDefault )
{
	// The procs fit the callbacks of the evaluator.
	CExpressionEvaluator evaluator;
	bool bResult = false;

#if defined( _WIN32 )
	TEST_TRUE( evaluator.Evaluate( bResult, "$WIN32 && !$OSX", DefaultConditionalSymbolProc, DefaultConditionalErrorProc ) );
#elif defined( OSX )
	TEST_TRUE( evaluator.Evaluate( bResult, "$OSX && !$WIN32", DefaultConditionalSymbolProc, DefaultConditionalErrorProc ) );
#else
	TEST_TRUE( evaluator.Evaluate( bResult, "$LINUX && !$WIN32", DefaultConditionalSymbolProc, DefaultConditionalErrorProc ) );
#endif

	TEST_TRUE( bResult );
}

REGISTER_NAMED_TEST( "FormatArgumentsToBuffer.PlainText", FormatArgumentsToBuffer_PlainText )
{
	// Without arguments the text is copied and %% becomes %.
	CBufferString text;
	CBufferStringFormatOutputStream stream( text );
	FormatArgumentsToBuffer( &stream, "plain 100%% text\nnext", nullptr, 0 );
	TEST_EQ( V_strcmp( text.Get(), "plain 100% text\nnext" ), 0 );
}

REGISTER_NAMED_TEST( "Formatting_AdjustIndentation.FourSpacesPerLevel", Formatting_AdjustIndentation_FourSpacesPerLevel )
{
	CBufferString text;
	CBufferStringFormatOutputStream stream( text );

	stream.OutputIndentation();
	TEST_TRUE( text.IsEmpty() );

	Formatting_AdjustIndentation( 2 );
	stream.OutputIndentation();
	TEST_EQ( V_strcmp( text.Get(), "        " ), 0 );

	// Back to no indentation.
	Formatting_AdjustIndentation( -2 );
	text.Clear();
	stream.OutputIndentation();
	TEST_TRUE( text.IsEmpty() );
}

REGISTER_NAMED_TEST( "FormatTo.BasicTypes", FormatTo_BasicTypes )
{
	char szBuffer[ 128 ];
	CCharBufferFormatOutputStream stream( szBuffer, sizeof( szBuffer ) );

	FormatTo( &stream, CFormatStringElement( "%d" ), 42 );
	FormatTo( &stream, CFormatStringElement( "%u" ), 7u );
	FormatTo( &stream, CFormatStringElement( "%s" ), true );
	FormatTo( &stream, CFormatStringElement( "%c" ), 'x' );
	FormatTo( &stream, CFormatStringElement( "%.2f" ), 1.5 );
	FormatTo( &stream, CFormatStringElement( "%s" ), "str" );
	FormatTo( &stream, CFormatStringElement( "%s" ), static_cast< const char * >( nullptr ) );
	FormatTo( &stream, CFormatStringElement( "%s" ), std::string_view( "view" ) );
	FormatTo( &stream, CFormatStringElement( "%s" ), CUtlString( "utl" ) );
	stream.Terminate();

	TEST_EQ( V_strcmp( szBuffer, "427truex1.50str*nullptr*viewutl" ), 0 );
}

REGISTER_NAMED_TEST( "V_snprintfNoSecurityDoNotUse.Truncates", V_snprintfNoSecurityDoNotUse_Truncates )
{
	char szBuffer[ 8 ];
	TEST_EQ( V_snprintfNoSecurityDoNotUse( szBuffer, sizeof( szBuffer ), "%s", "abcdefghijk" ), 7 );
	TEST_EQ( V_strcmp( szBuffer, "abcdefg" ), 0 );
}

static int TestVscprintf( const char *pszFormat, ... )
{
	va_list params;
	va_start( params, pszFormat );
	const int nLength = V_vscprintf( pszFormat, params );
	va_end( params );
	return nLength;
}

REGISTER_NAMED_TEST( "V_vscprintf.Length", V_vscprintf_Length )
{
	TEST_EQ( TestVscprintf( "%d-%s", 123, "abcd" ), 8 );
	TEST_EQ( TestVscprintf( "" ), 0 );
}

REGISTER_NAMED_TEST( "V_snwprintf_bytes.SizeInBytes", V_snwprintf_bytes_SizeInBytes )
{
	wchar_t szBuffer[ 8 ];
	TEST_EQ( V_snwprintf_bytes( szBuffer, sizeof( szBuffer ), L"%d", 1234 ), 4 );
	TEST_EQ( wcscmp( szBuffer, L"1234" ), 0 );
}

REGISTER_NAMED_TEST( "V_UnicodeConvertAndCaseStr.UTF16Output", V_UnicodeConvertAndCaseStr_UTF16Output )
{
	uchar16 szBuffer[ 16 ];
	uchar16 *pNewBuffer = nullptr;
	TEST_TRUE( V_UnicodeConvertAndCaseStr( "AbC", szBuffer, sizeof( szBuffer ), &pNewBuffer, 0 ) == szBuffer );
	TEST_NULL( pNewBuffer );
	TEST_EQ( ( int )szBuffer[ 0 ], ( int )'a' );
	TEST_EQ( ( int )szBuffer[ 2 ], ( int )'c' );
	TEST_EQ( ( int )szBuffer[ 3 ], 0 );
}

REGISTER_NAMED_TEST( "V_SanitizeTextForCommandLine.Quotes", V_SanitizeTextForCommandLine_Quotes )
{
	// Double quotes become single ones, bAppend keeps the text and nLength limits the input.
	CBufferString out;
	TEST_EQ( V_strcmp( V_SanitizeTextForCommandLine( "a\"b\"c", &out, false, -1 ), "a'b'c" ), 0 );
	TEST_EQ( V_strcmp( V_SanitizeTextForCommandLine( "xyz", &out, true, 2 ), "a'b'cxy" ), 0 );
	TEST_EQ( V_strcmp( V_SanitizeTextForCommandLine( "new", &out, false, -1 ), "new" ), 0 );
}
