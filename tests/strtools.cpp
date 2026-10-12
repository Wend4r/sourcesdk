#include "common/assert.h"
#include "common/macros.h"

#include <tier0/platform.h>
#include <tier0/strtools.h>

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
