#include "common/assert.h"
#include "common/macros.h"

#include <tier0/bufferstring.h>
#include <tier0/utlstring.h>

#include <cstdlib>
#include <cstring>

REGISTER_NAMED_TEST( "CBufferString.ConstructionAndConcat", CBufferString_ConstructionAndConcat )
{
	// Cover the plain construction paths and the most common append helpers.
	CBufferString empty;

	TEST_TRUE( empty.IsEmpty() );
	TEST_TRUE( empty.IsStackAllocated() );
	TEST_EQ( empty.GetType(), CBufferString::BS_TYPE_STACK );

	CUtlString utlString( "utl" );
	CBufferString fromCUtl( utlString );
	CBufferString literal = "literal"_bs;

	TEST_TRUE( fromCUtl == "utl" );
	TEST_TRUE( literal == "literal" );

	CBufferString buffer;

	buffer += "alpha";
	buffer += '-';
	buffer += "beta";
	buffer += 42;
	buffer += 1.5;

	TEST_TRUE( buffer.StartsWith( "alpha" ) );
	TEST_TRUE( buffer.EndsWith( "1.5" ) );
	TEST_TRUE( buffer.EndsWith_FastCaseInsensitive( "1.5" ) );
	TEST_TRUE( buffer.StartsWith_FastCaseInsensitive( "ALPHA" ) );

	const char *joinParts[] = { ":", "gamma" };
	const int joinLens[] = { 1, 5 };

	buffer.AppendConcat( 2, joinParts, joinLens );
	buffer.AppendRepeat( '!', 2 );
	TEST_TRUE( buffer == "alpha-beta421.5:gamma!!" );
}

REGISTER_NAMED_TEST( "CBufferString.InsertReplaceAndFormat", CBufferString_InsertReplaceAndFormat )
{
	// Exercise the mutation APIs in isolation so one step does not hide another.
	CBufferString buffer( "hello world" );

	buffer.Insert( 5, "," );
	TEST_TRUE( buffer == "hello, world" );

	buffer.ReplaceAt( 6, 1, " brave" );
	TEST_TRUE( buffer.Length() > 6 );

	buffer.ReplaceAt( 12, " " );
	TEST_TRUE( buffer.Length() > 6 );

	buffer.RemoveAt( 5, 1 );
	TEST_TRUE( buffer.Length() > 5 );

	buffer.Set( "hello brave world" );
	buffer.Replace( ' ', '_' );
	TEST_TRUE( buffer == "hello_brave_world" );

	buffer.Set( "hello_brave_world" );
	buffer.Replace( "_brave_", "-" );
	TEST_TRUE( buffer == "hello-world" );

	buffer.Set( "hello-world" );
	buffer.ReverseChars( 6, 5 );
	TEST_TRUE( buffer == "hello-dlrow" );

	buffer.Format( "%s:%d", "value", 17 );
	TEST_TRUE( buffer == "value:17" );

	buffer.AppendFormat( "-%s", "tail" );
	TEST_TRUE( buffer == "value:17-tail" );

	TEST_TRUE( buffer.StrFormat( "%s:%d", "fmt", 9 ) == buffer.Get() );
	TEST_TRUE( buffer == "fmt:9" );

	TEST_TRUE( buffer.StrAppendFormat( "-%s", "append" ) == buffer.Get() );
	TEST_TRUE( buffer == "fmt:9-append" );
}

REGISTER_NAMED_TEST( "CBufferString.CaseTrimAndLength", CBufferString_CaseTrimAndLength )
{
	// Check casing, trimming and manual length changes on the same string object.
	CBufferString buffer( "  AbC Def  " );

	buffer.TrimHead();
	TEST_TRUE( buffer == "AbC Def  " );

	buffer.TrimTail();
	TEST_TRUE( buffer == "AbC Def" );

	buffer.ToLower();
	TEST_TRUE( buffer == "abc def" );

	buffer.ToUpper( 4 );
	TEST_TRUE( buffer == "abc DEF" );

	buffer.ToLowerFast( 4 );
	TEST_TRUE( buffer == "abc def" );

	buffer.RemoveWhitespace();
	TEST_TRUE( buffer == "abcdef" );

	buffer.Set( "prefix-suffix" );
	buffer.TruncateAt( 6 );
	TEST_TRUE( buffer == "prefix" );

	char *pLength = buffer.SetLength( 3 );

	TEST_NOT_NULL( pLength );
	buffer[0] = 'a';
	buffer[1] = 'b';
	buffer[2] = 'c';
	TEST_TRUE( buffer == "abc" );
}

REGISTER_NAMED_TEST( "CBufferString.CapacityAndOwnership", CBufferString_CapacityAndOwnership )
{
	// Validate allocation growth and the ownership transfer style helpers.
	CBufferString buffer;
	char *pOut = nullptr;

	TEST_TRUE( buffer.EnsureCapacity( 64, &pOut ) >= 64 );
	TEST_NOT_NULL( pOut );
	TEST_TRUE( buffer.AllocatedSize() >= 64 );

	buffer.Set( "owned-value" );
	TEST_TRUE( buffer.EnsureAddedCapacity( 16, &pOut ) >= 16 );
	TEST_NOT_NULL( pOut );

	char *pLength = buffer.EnsureLength( 20 );

	TEST_NOT_NULL( pLength );
	TEST_EQ( buffer.Length(), 20 );

	buffer.Set( "owned-value" );
	char *pOwned = buffer.EnsureOwnedAllocation( CBufferString::BS_AO_HEAP_ALLOCATION );

	TEST_NOT_NULL( pOwned );

	char *pRelinquished = buffer.Relinquish( CBufferString::BS_AO_FREE_HEAP_ALLOCATION );

	TEST_NOT_NULL( pRelinquished );
	TEST_EQ( V_strcmp( pRelinquished, "owned-value" ), 0 );

	buffer.Set( "preserve-me" );
	buffer.Purge( 32 );
	TEST_TRUE( buffer.IsEmpty() );
	TEST_TRUE( buffer.AllocatedSize() >= 32 || buffer.IsAllocationEmpty() );
}

REGISTER_NAMED_TEST( "CBufferString.PathHelpers", CBufferString_PathHelpers )
{
	// Path-oriented methods should normalize, split and shorten without losing text state.
	CBufferString buffer;

	buffer.ComposeFileName( "dir/sub", "file.txt", '/' );
	TEST_TRUE( buffer == "dir/sub/file.txt" );

	buffer.DefaultExtension( ".dat" );
	TEST_TRUE( buffer == "dir/sub/file.txt" );

	buffer.Set( "dir/sub/file" );
	buffer.DefaultExtension( ".dat" );
	TEST_TRUE( buffer == "dir/sub/file.dat" );

	buffer.ExtractFileExtension( "dir/sub/file.txt" );
	TEST_TRUE( buffer == "txt" );

	buffer.ExtractFilePath( "dir/sub/file.txt" );
	TEST_TRUE( buffer == "dir/sub" );

	buffer.ExtractFirstDir( "dir/sub/file.txt" );
	TEST_TRUE( buffer == "dir" );

	buffer.Set( "dir\\\\sub//file.txt" );
	buffer.FixSlashes( '/' );
	TEST_TRUE( buffer.EndsWith( "file.txt" ) );

	buffer.Set( "./dir/../sub/file.txt" );
	TEST_NOT_NULL( buffer.RemoveDotSlashes( '/' ) );
	TEST_TRUE( buffer == "sub/file.txt" );

	buffer.Set( "sub/path" );
	buffer.EnsureTrailingSlash( '/' );
	TEST_TRUE( buffer == "sub/path/" );

	buffer.Set( "root/dir/file.txt" );
	CBufferString removedDir;

	buffer.RemoveFirstDir( &removedDir );
	TEST_TRUE( removedDir == "root" );
	TEST_TRUE( buffer == "dir/file.txt" );

	buffer.Set( "root/dir/file.txt" );
	buffer.RemoveFilePath();
	TEST_TRUE( buffer == "file.txt" );

	buffer.EnsureTrailingSlash( '/' );
	buffer.StripTrailingSlash();
	TEST_TRUE( buffer == "file.txt" );

	buffer.Set( "root/dir/file.txt" );
	buffer.ShortenPath();
	TEST_TRUE( buffer == "root/dir" );
}

REGISTER_NAMED_TEST( "CBufferString.ConversionAndUtf8", CBufferString_ConversionAndUtf8 )
{
	// Unicode conversion helpers should preserve valid text and trim broken tails safely.
	CBufferString buffer;

	const wchar_t wideValue[] = L"Wide";

	buffer.ConvertIn( wideValue, 4 );
	TEST_TRUE( buffer == "Wide" );

	const char utf8Char[] = { 'A', ( char )0xD0, ( char )0x91, 'B', '\0' };

	buffer.Set( utf8Char );
	buffer.RemoveAtUTF8( 1, 1 );
	TEST_TRUE( buffer == "AB" );

	char utf8Tail[] = { 'A', ( char )0xD0, '\0' };

	buffer.Set( utf8Tail, 2 );
	TEST_TRUE( buffer.RemovePartialUTF8Tail() );
	TEST_TRUE( buffer == "A" );

	buffer.Set( utf8Char );
	buffer.RemoveTailUTF8( 1 );
	TEST_TRUE( buffer.StartsWith( "A" ) );
}

REGISTER_NAMED_TEST( "CBufferString.CopyMoveAndPointerAccess", CBufferString_CopyMoveAndPointerAccess )
{
	// Copy, move and direct buffer pointer access should keep visible string contents usable.
	CBufferString original( "payload" );

	CBufferString copy( original );

	TEST_TRUE( copy == "payload" );
	TEST_TRUE( original == copy );

	CBufferString assigned;

	assigned = original;
	TEST_TRUE( assigned == "payload" );

	CBufferString moved( Move( original ) );

	TEST_TRUE( moved == "payload" );

	CBufferString moveAssigned;

	moveAssigned = Move( moved );
	TEST_TRUE( moveAssigned == "payload" );

	CBufferStringN< 32 > fixed( "trim me" );
	CBufferStringN< 32 > fixedCopy( fixed );

	TEST_TRUE( fixedCopy == "trim me" );

	CBufferStringN< 32 > fixedMoved( Move( fixedCopy ) );

	TEST_TRUE( fixedMoved == "trim me" );

	char *pInsert = fixedMoved.GetInsertPtr( fixedMoved.Length(), 1 );

	TEST_NOT_NULL( pInsert );
	pInsert[0] = '!';
	pInsert[1] = '\0';
	TEST_TRUE( fixedMoved == "trim me!" );

	char *pReplace = fixedMoved.GetReplacePtr( 0, 4, 4 );

	TEST_NOT_NULL( pReplace );
	std::memcpy( pReplace, "keep", 4 );
	pReplace[4] = ' ';
	TEST_TRUE( fixedMoved.StartsWith( "keep" ) );
}

REGISTER_NAMED_TEST( "CBufferString.CUtlStringInterop", CBufferString_CUtlStringInterop )
{
	// Interop with CUtlString should work through the supported constructor and assignment paths.
	CUtlString source( "utl-string-value" );
	CBufferString fromCUtl( source );

	TEST_TRUE( fromCUtl == "utl-string-value" );

	CBufferString assigned;

	assigned = source;
	TEST_TRUE( assigned == "utl-string-value" );
}
