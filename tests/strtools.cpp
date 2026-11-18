#include "common/assert.h"
#include "common/macros.h"

#include <tier0/platform.h>
#include <tier0/strtools.h>
#include <tier0/tier0_strtools.h>
#include <tier0/bufferstring.h>

REGISTER_NAMED_TEST( "V_MakeAbsolutePath.StartingDir", V_MakeAbsolutePath_StartingDir )
{
	// A relative path is joined to the starting directory.
	char szPath[ MAX_PATH ];

	TEST_TRUE( V_MakeAbsolutePath( szPath, sizeof( szPath ), "Sub/File.txt", "/Base/Dir" ) );
	TEST_NOT_NULL( V_strstr( szPath, "File.txt" ) );

#ifndef _WIN32
	TEST_EQ( V_strcmp( szPath, "/Base/Dir/Sub/File.txt" ), 0 );

	TEST_TRUE( V_MakeAbsolutePath( szPath, sizeof( szPath ), "/Abs/Path", "/Base" ) );
	TEST_EQ( V_strcmp( szPath, "/Abs/Path" ), 0 );
#endif
}

REGISTER_NAMED_TEST( "V_MakeAbsolutePath.Lowercase", V_MakeAbsolutePath_Lowercase )
{
	// bLowercase lowers the whole result, including the starting directory.
	char szPath[ MAX_PATH ];
	char szLowercase[ MAX_PATH ];

	TEST_TRUE( V_MakeAbsolutePath( szPath, sizeof( szPath ), "Sub/File.txt", "/Base/Dir", false ) );
	TEST_TRUE( V_MakeAbsolutePath( szLowercase, sizeof( szLowercase ), "Sub/File.txt", "/Base/Dir", true ) );

	TEST_TRUE( V_strcmp( szPath, szLowercase ) != 0 );

	V_strlower( szPath );
	TEST_EQ( V_strcmp( szPath, szLowercase ), 0 );

#ifndef _WIN32
	TEST_EQ( V_strcmp( szLowercase, "/base/dir/sub/file.txt" ), 0 );
#endif
}

REGISTER_NAMED_TEST( "V_MakeAbsolutePath.Truncates", V_MakeAbsolutePath_Truncates )
{
	// A short output buffer receives a null terminated prefix of the path.
	char szPath[ 8 ];

	V_MakeAbsolutePath( szPath, sizeof( szPath ), "Sub/File.txt", "/Base/Dir" );
	TEST_EQ( V_strlen( szPath ), 7 );

#ifndef _WIN32
	TEST_EQ( V_strcmp( szPath, "/Base/D" ), 0 );
#endif
}

REGISTER_NAMED_TEST( "StrReplaceChar.ReplacesEveryChar", StrReplaceChar_ReplacesEveryChar )
{
	char szPath[] = "a/b/c/";
	StrReplaceChar( szPath, '/', '\\' );
	TEST_EQ( V_strcmp( szPath, "a\\b\\c\\" ), 0 );

	// Nothing else changes.
	char szText[] = "abc";
	StrReplaceChar( szText, 'x', 'y' );
	TEST_EQ( V_strcmp( szText, "abc" ), 0 );
}

REGISTER_NAMED_TEST( "LengthOfMatchingPaths.SharedDirectories", LengthOfMatchingPaths_SharedDirectories )
{
	// Up to and including the last slash both paths share.
	TEST_EQ( LengthOfMatchingPaths( "a/b/c.txt", "a/b/d.txt" ), 4 );
	TEST_EQ( LengthOfMatchingPaths( "dir/sub/f", "dir/subx/f" ), 4 );

	// Case and the kind of slash do not matter, a whole match counts every character.
	TEST_EQ( LengthOfMatchingPaths( "A\\B\\c", "a/b/c" ), 5 );
	TEST_EQ( LengthOfMatchingPaths( "x/y", "x/y" ), 3 );

	// No shared directory.
	TEST_EQ( LengthOfMatchingPaths( "abc", "abd" ), 0 );
}

REGISTER_NAMED_TEST( "nexttoken.SplitsOnSeparator", nexttoken_SplitsOnSeparator )
{
	char szToken[ 64 ];

	const char *pszRest = nexttoken( szToken, "one,two", ',' );
	TEST_EQ( V_strcmp( szToken, "one" ), 0 );
	TEST_NOT_NULL( pszRest );
	TEST_EQ( V_strcmp( pszRest, "two" ), 0 );

	// The last token leaves an empty rest.
	pszRest = nexttoken( szToken, pszRest, ',' );
	TEST_EQ( V_strcmp( szToken, "two" ), 0 );
	TEST_NOT_NULL( pszRest );
	TEST_EQ( *pszRest, '\0' );

	// An empty string gives an empty token and null.
	pszRest = nexttoken( szToken, pszRest, ',' );
	TEST_EQ( szToken[ 0 ], '\0' );
	TEST_NULL( pszRest );
}

REGISTER_NAMED_TEST( "V_XMLDecode.Entities", V_XMLDecode_Entities )
{
	CBufferString out;
	const char *pszResult = V_XMLDecode( "a&lt;b&gt;&amp;c&quot;&apos;", &out, -1, 0 );
	TEST_EQ( V_strcmp( out.Get(), "a<b>&c\"'" ), 0 );
	TEST_TRUE( pszResult == out.Get() );

	// Bit 0 of the flags appends.
	V_XMLDecode( "Z", &out, -1, 1 );
	TEST_EQ( V_strcmp( out.Get(), "a<b>&c\"'Z" ), 0 );

	// A length takes only that many characters.
	V_XMLDecode( "abcdef", &out, 3, 0 );
	TEST_EQ( V_strcmp( out.Get(), "abc" ), 0 );
}

REGISTER_NAMED_TEST( "V_ValidatePathConcat.RejectsRootedSuffixes", V_ValidatePathConcat_RejectsRootedSuffixes )
{
	TEST_TRUE( V_ValidatePathConcat( "/base", "sub/file", nullptr, true ) );

	// An absolute or drive path cannot follow a prefix.
	TEST_FALSE( V_ValidatePathConcat( "/base", "/abs", nullptr, true ) );

	// Without a prefix or a suffix there is nothing to check.
	TEST_TRUE( V_ValidatePathConcat( nullptr, "/abs", nullptr, true ) );
	TEST_TRUE( V_ValidatePathConcat( "base", "", nullptr, true ) );
}

REGISTER_NAMED_TEST( "V_TryAndMakeFriendlyNameFromCppName.MemberNames", V_TryAndMakeFriendlyNameFromCppName_MemberNames )
{
	struct Case_t
	{
		const char *m_pszCppName;
		const char *m_pszFriendlyName;
	};

	const Case_t cases[] =
	{
		{ "m_flMaxSpeed", "Max Speed" },
		{ "m_nCount", "Count" },
		{ "m_bEnabled", "Enabled" },
		{ "m_pszName", "Name" },
		{ "m_vecOrigin", "Origin" },
		{ "m_iHealth", "Health" },
		{ "SomeValue", "Some Value" },
	};

	for ( const Case_t &c : cases )
	{
		CBufferString name;
		TEST_TRUE( V_TryAndMakeFriendlyNameFromCppName( c.m_pszCppName, nullptr, &name ) );
		TEST_EQ( V_strcmp( name.Get(), c.m_pszFriendlyName ), 0 );
	}

	// A name without words fails and leaves the output empty.
	CBufferString name;
	TEST_FALSE( V_TryAndMakeFriendlyNameFromCppName( "lowercase", nullptr, &name ) );
	TEST_TRUE( name.IsEmpty() );
}
