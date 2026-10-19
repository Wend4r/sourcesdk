#include "common/assert.h"
#include "common/macros.h"

#include <tier0/platform.h>
#include <tier0/fileio.h>
#include <tier0/bufferstring.h>

#include <cstdio>
#include <cstring>
#include <string>

// The tests use POSIX paths and calls
#if !defined( _WIN32 )

#include <stdlib.h>
#include <unistd.h>

REGISTER_NAMED_TEST( "PathString.SetAndWide", PathString_SetAndWide )
{
	// The path is made absolute and loses its double slashes.
	CPathString path( "/tmp//sourcesdk_path_string_test" );
	TEST_EQ( strcmp( path.GetUTF8Path(), "/tmp/sourcesdk_path_string_test" ), 0 );

	// The wide form starts with \\?\ on every platform.
	const wchar_t *pwszPath = path.GetWCharPathPrePended();
	TEST_TRUE( pwszPath != NULL );
	TEST_TRUE( wcsncmp( pwszPath, L"\\\\?\\", 4 ) == 0 );

	path.Set( "/tmp", "file.txt" );
	TEST_EQ( strcmp( path.GetUTF8Path(), "/tmp/file.txt" ), 0 );
}

REGISTER_NAMED_TEST( "DirIterator.ListsFiles", DirIterator_ListsFiles )
{
	char szDir[] = "/tmp/sourcesdk_dir_iterator_XXXXXX";
	TEST_TRUE( mkdtemp( szDir ) != NULL );

	const char *pszNames[] = { "a.txt", "b.txt", "c.bin" };

	for ( const char *pszName : pszNames )
	{
		std::string strFile = std::string( szDir ) + "/" + pszName;
		FILE *pFile = fopen( strFile.c_str(), "wb" );
		TEST_TRUE( pFile != NULL );
		fputs( pszName, pFile );
		fclose( pFile );
	}

	// More than 1 in the flags fills the file info.
	CDirIterator iterator( szDir, "*.txt", 2 );
	int nFiles = 0;

	while ( iterator.NextFile() )
	{
		const char *pszName = iterator.CurrentFileName();

		if ( strcmp( pszName, "." ) == 0 || strcmp( pszName, ".." ) == 0 )
			continue;

		TEST_TRUE( strcmp( pszName, "a.txt" ) == 0 || strcmp( pszName, "b.txt" ) == 0 );
		TEST_FALSE( iterator.BCurrentIsDir() );
		TEST_EQ( iterator.CurrentFileLength(), ( int64 )5 );

		std::string strFull = std::string( szDir ) + "/" + pszName;
		TEST_EQ( strcmp( iterator.CurrentFileFullPath(), strFull.c_str() ), 0 );
		nFiles++;
	}

	TEST_EQ( nFiles, 2 );
	iterator.Stop();

	for ( const char *pszName : pszNames )
	{
		unlink( ( std::string( szDir ) + "/" + pszName ).c_str() );
	}

	rmdir( szDir );
}

REGISTER_NAMED_TEST( "DirWatcher.WatchesADirectory", DirWatcher_WatchesADirectory )
{
	char szDir[] = "/tmp/sourcesdk_dir_watcher_XXXXXX";
	TEST_TRUE( mkdtemp( szDir ) != NULL );

	CDirWatcher watcher;
	TEST_FALSE( watcher.SetDirToWatch( NULL, CDirWatcher::CHANGE_NAME, false, NULL, 0 ) );
	TEST_TRUE( watcher.SetDirToWatch( szDir, CDirWatcher::CHANGE_NAME | CDirWatcher::CHANGE_LAST_WRITE, true, NULL, 0 ) );

	// Nothing changed yet.
	CBufferString file;
	TEST_FALSE( watcher.GetChangedFile( &file ) );
	TEST_EQ( watcher.GetChangedFiles( NULL, NULL ), ( uint32 )0 );

	watcher.Stop();
	rmdir( szDir );
}

#endif // !_WIN32
