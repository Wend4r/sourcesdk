#include "common/assert.h"
#include "common/macros.h"

#include <tier0/platform.h>
#include <tier0/fileio.h>
#include <tier0/bufferstring.h>
#include <tier0/memalloc.h>
#include <tier0/utlbuffer.h>
#include <tier0/utlhashtable.h>
#include <tier0/utlstring.h>

#include <cstdio>
#include <cstring>
#include <string>

// The tests use POSIX paths and calls
#if !defined( _WIN32 )

#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>
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

REGISTER_NAMED_TEST( "PathString.PooledBuffer", PathString_PooledBuffer )
{
	// tier0 allocates the blocks with four bytes for every character of the longest path.
	TEST_EQ( sizeof( CPathString::Buffer_t ), static_cast< size_t >( CPathString::k_nPathStringBufferBytes ) );

	// A block from the pool takes the whole size and goes back to it.
	CPathString::Buffer_t *pBuffer = CPathString::NewPathStringBuffer();
	TEST_TRUE( pBuffer != NULL );
	char *pszText = reinterpret_cast< char * >( pBuffer->m_data );
	memset( pszText, 'a', CPathString::k_nUTF8PathStringBufferChars - 1 );
	pszText[ CPathString::k_nUTF8PathStringBufferChars - 1 ] = '\0';
	TEST_EQ( strlen( pszText ), static_cast< size_t >( CPathString::k_nUTF8PathStringBufferChars - 1 ) );
	CPathString::DeletePathStringBuffer( pBuffer );

	// A path longer than the SDK's MAX_PATH fits.
	std::string strLong = "/tmp/" + std::string( 1000, 'p' );
	CPathString path( strLong.c_str() );
	TEST_EQ( strcmp( path.GetUTF8Path(), strLong.c_str() ), 0 );
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
		TEST_EQ( iterator.CurrentFileLength(), ( uint64 )5 );

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

	// The macOS tier0 reads pBuffer before it looks for changes.
	CUtlBuffer buffer;
	TEST_EQ( watcher.GetChangedFiles( &buffer, NULL ), ( uint32 )0 );

	watcher.Stop();
	rmdir( szDir );
}

// A temporary directory with one file of known content, removed by the destructor
class CFioTestDir
{
public:
	CFioTestDir()
	{
		strcpy( m_szDir, "/tmp/sourcesdk_fio_XXXXXX" );
		m_bValid = mkdtemp( m_szDir ) != NULL;
		m_strFile = std::string( m_szDir ) + "/file.txt";

		FILE *pFile = fopen( m_strFile.c_str(), "wb" );
		m_bValid = m_bValid && pFile != NULL;

		if ( pFile )
		{
			fputs( "0123456789", pFile );
			fclose( pFile );
		}
	}

	~CFioTestDir()
	{
		std::string strCommand = std::string( "rm -rf " ) + m_szDir;
		( void )!system( strCommand.c_str() );
	}

	std::string Path( const char *pszName ) const { return std::string( m_szDir ) + "/" + pszName; }

	char m_szDir[ 64 ];
	std::string m_strFile;
	bool m_bValid;
};

REGISTER_NAMED_TEST( "FioCreateFile.CreateWriteReadClose", FioCreateFile_CreateWriteReadClose )
{
	CFioTestDir dir;
	TEST_TRUE( dir.m_bValid );
	const std::string strFile = dir.Path( "new.bin" );

	OSFILEHANDLE hFile;
	TEST_EQ( FioCreateFile( strFile.c_str(), k_EFioReadWrite, k_EFioShareReadWrite, k_EFioCreateNew, k_EFioNoModifiers, &hFile ), 0 );

	// A second k_EFioCreateNew fails with the error of the platform.
	OSFILEHANDLE hSecond;
	TEST_NE( FioCreateFile( strFile.c_str(), k_EFioReadWrite, k_EFioShareReadWrite, k_EFioCreateNew, k_EFioNoModifiers, &hSecond ), 0 );

	int32 nWritten = 0;
	TEST_EQ( FioWrite( hFile, "abcdef", 6, &nWritten ), 0 );
	TEST_EQ( nWritten, 6 );
	TEST_EQ( FioFlush( hFile ), 0 );
	TEST_EQ( FioClose( hFile ), 0 );

	// k_EFioOpenExisting fails without a file.
	TEST_NE( FioCreateFile( dir.Path( "missing" ).c_str(), k_EFioRead, k_EFioShareRead, k_EFioOpenExisting, k_EFioNoModifiers, &hFile ), 0 );
}

REGISTER_NAMED_TEST( "CFioFile.OwnsTheHandle", CFioFile_OwnsTheHandle )
{
	CFioTestDir dir;
	CFioFile file;
	TEST_FALSE( file.IsOpen() );
	TEST_EQ( file.Close(), 0 );

	// A failed open leaves the file closed.
	TEST_NE( file.Open( dir.Path( "missing" ).c_str() ), 0 );
	TEST_FALSE( file.IsOpen() );
	TEST_TRUE( file.GetHandle() == INVALID_OSFILEHANDLE );

	TEST_EQ( file.Open( dir.m_strFile.c_str() ), 0 );
	TEST_TRUE( file.IsOpen() );

	// The detached handle is the caller's to close.
	OSFILEHANDLE hFile = file.Detach();
	TEST_FALSE( file.IsOpen() );
	TEST_EQ( FioClose( hFile ), 0 );

	// The destructor closes the handle, a second open closes the first one.
	{
		CFioFile scoped;
		TEST_EQ( scoped.Open( dir.m_strFile.c_str() ), 0 );
		TEST_EQ( scoped.Open( dir.m_strFile.c_str() ), 0 );
	}
}

REGISTER_NAMED_TEST( "FioRead.Int32", FioRead_Int32 )
{
	CFioTestDir dir;
	CFioFile file;
	TEST_EQ( file.Open( dir.m_strFile.c_str() ), 0 );

	char szBuffer[ 16 ] = {};
	int32 nRead = 0;
	TEST_EQ( file.Read( szBuffer, ( int32 )4, &nRead ), 0 );
	TEST_EQ( nRead, 4 );
	TEST_EQ( memcmp( szBuffer, "0123", 4 ), 0 );

	// The handle moved on.
	TEST_EQ( file.Read( szBuffer, ( int32 )16, &nRead ), 0 );
	TEST_EQ( nRead, 6 );
	TEST_EQ( memcmp( szBuffer, "456789", 6 ), 0 );
}

REGISTER_NAMED_TEST( "FioRead.SizeT", FioRead_SizeT )
{
	CFioTestDir dir;
	CFioFile file;
	TEST_EQ( file.Open( dir.m_strFile.c_str() ), 0 );

	char szBuffer[ 16 ] = {};
	size_t nRead = 0;
	TEST_EQ( file.Read( szBuffer, sizeof( szBuffer ), &nRead ), 0 );
	TEST_EQ( nRead, ( size_t )10 );
	TEST_EQ( memcmp( szBuffer, "0123456789", 10 ), 0 );
}

REGISTER_NAMED_TEST( "FioReadAt.ReadsAtOffset", FioReadAt_ReadsAtOffset )
{
	CFioTestDir dir;
	CFioFile file;
	TEST_EQ( file.Open( dir.m_strFile.c_str() ), 0 );

	char szBuffer[ 4 ] = {};
	int32 nRead = 0;
	TEST_EQ( file.ReadAt( 7, szBuffer, 3, &nRead ), 0 );
	TEST_EQ( nRead, 3 );
	TEST_EQ( memcmp( szBuffer, "789", 3 ), 0 );
}

REGISTER_NAMED_TEST( "FioWrite.SizeT", FioWrite_SizeT )
{
	CFioTestDir dir;
	CFioFile file;
	TEST_EQ( file.Open( dir.m_strFile.c_str(), k_EFioWrite, k_EFioShareNone, k_EFioCreateAlways ), 0 );

	size_t nWritten = 0;
	TEST_EQ( file.Write( "xyz", ( size_t )3, &nWritten ), 0 );
	TEST_EQ( nWritten, ( size_t )3 );
	TEST_EQ( file.Close(), 0 );

	// k_EFioCreateAlways truncated the old content.
	uint32 nCRC = 0;
	uint64 nBytes = 0;
	TEST_EQ( FioGetFileCRC32( dir.m_strFile.c_str(), &nCRC, 4096, &nBytes ), 0 );
	TEST_EQ( nBytes, ( uint64 )3 );
}

REGISTER_NAMED_TEST( "FioWriteAt.WritesAtOffset", FioWriteAt_WritesAtOffset )
{
	CFioTestDir dir;
	CFioFile file;
	TEST_EQ( file.Open( dir.m_strFile.c_str(), k_EFioReadWrite, k_EFioShareNone ), 0 );

	int32 nWritten = 0;
	TEST_EQ( file.WriteAt( 2, "AB", 2, &nWritten ), 0 );
	TEST_EQ( nWritten, 2 );

	char szBuffer[ 10 ] = {};
	int32 nRead = 0;
	TEST_EQ( file.ReadAt( 0, szBuffer, 10, &nRead ), 0 );
	TEST_EQ( memcmp( szBuffer, "01AB456789", 10 ), 0 );
}

REGISTER_NAMED_TEST( "FioSeek.SetCurrentEnd", FioSeek_SetCurrentEnd )
{
	CFioTestDir dir;
	CFioFile file;
	TEST_EQ( file.Open( dir.m_strFile.c_str() ), 0 );

	uint64 nPosition = 0;
	TEST_EQ( file.Seek( 3, k_EFioSeekSet, &nPosition ), 0 );
	TEST_EQ( nPosition, ( uint64 )3 );
	TEST_EQ( file.Seek( 2, k_EFioSeekCurrent, &nPosition ), 0 );
	TEST_EQ( nPosition, ( uint64 )5 );
	TEST_EQ( file.Seek( -1, k_EFioSeekEnd, &nPosition ), 0 );
	TEST_EQ( nPosition, ( uint64 )9 );

	char c = 0;
	int32 nRead = 0;
	TEST_EQ( file.Read( &c, ( int32 )1, &nRead ), 0 );
	TEST_EQ( c, '9' );
}

REGISTER_NAMED_TEST( "FioGetSize.FileLength", FioGetSize_FileLength )
{
	CFioTestDir dir;
	CFioFile file;
	TEST_EQ( file.Open( dir.m_strFile.c_str() ), 0 );

	uint64 nBytes = 0;
	TEST_EQ( file.GetSize( &nBytes ), 0 );
	TEST_EQ( nBytes, ( uint64 )10 );
}

REGISTER_NAMED_TEST( "FioSetSize.GrowsAndShrinks", FioSetSize_GrowsAndShrinks )
{
	CFioTestDir dir;
	CFioFile file;
	TEST_EQ( file.Open( dir.m_strFile.c_str(), k_EFioReadWrite, k_EFioShareNone ), 0 );

	uint64 nBytes = 0;
	TEST_EQ( file.SetSize( 32 ), 0 );
	TEST_EQ( file.GetSize( &nBytes ), 0 );
	TEST_EQ( nBytes, ( uint64 )32 );

	TEST_EQ( file.SetSize( 4 ), 0 );
	TEST_EQ( file.GetSize( &nBytes ), 0 );
	TEST_EQ( nBytes, ( uint64 )4 );
}

REGISTER_NAMED_TEST( "FioTruncateToSeekPosition.CutsAtPosition", FioTruncateToSeekPosition_CutsAtPosition )
{
	CFioTestDir dir;
	CFioFile file;
	TEST_EQ( file.Open( dir.m_strFile.c_str(), k_EFioReadWrite, k_EFioShareNone ), 0 );

	uint64 nPosition = 0;
	TEST_EQ( file.Seek( 6, k_EFioSeekSet, &nPosition ), 0 );
	TEST_EQ( file.TruncateToSeekPosition(), 0 );

	uint64 nBytes = 0;
	TEST_EQ( file.GetSize( &nBytes ), 0 );
	TEST_EQ( nBytes, ( uint64 )6 );
}

REGISTER_NAMED_TEST( "FioCreatePipe.WriteThenRead", FioCreatePipe_WriteThenRead )
{
	OSFILEHANDLE hRead, hWrite;
	TEST_EQ( FioCreatePipe( &hRead, &hWrite, 4096 ), 0 );

	int32 nWritten = 0;
	TEST_EQ( FioWrite( hWrite, "pipe", 4, &nWritten ), 0 );
	TEST_EQ( nWritten, 4 );

	char szBuffer[ 8 ] = {};
	int32 nRead = 0;
	TEST_EQ( FioRead( hRead, szBuffer, ( int32 )4, &nRead ), 0 );
	TEST_EQ( nRead, 4 );
	TEST_EQ( memcmp( szBuffer, "pipe", 4 ), 0 );

	TEST_EQ( FioClose( hWrite ), 0 );
	TEST_EQ( FioClose( hRead ), 0 );
}

REGISTER_NAMED_TEST( "FioDuplicate.SameFile", FioDuplicate_SameFile )
{
	CFioTestDir dir;
	OSFILEHANDLE hFile, hDuplicate;
	TEST_EQ( FioCreateFile( dir.m_strFile.c_str(), k_EFioRead, k_EFioShareRead, k_EFioOpenExisting, k_EFioNoModifiers, &hFile ), 0 );
	TEST_EQ( FioDuplicate( hFile, false, &hDuplicate ), 0 );
	TEST_NE( hDuplicate, hFile );
	TEST_EQ( FioIsSameFileIdentity( hFile, hDuplicate ), 0 );

	// The duplicate outlives the original.
	FioClose( hFile );
	uint64 nBytes = 0;
	TEST_EQ( FioGetSize( hDuplicate, &nBytes ), 0 );
	TEST_EQ( nBytes, ( uint64 )10 );
	FioClose( hDuplicate );
}

REGISTER_NAMED_TEST( "FioCancelIo.NothingPending", FioCancelIo_NothingPending )
{
	CFioTestDir dir;
	CFioFile file;
	TEST_EQ( file.Open( dir.m_strFile.c_str() ), 0 );
	TEST_EQ( FioCancelIo( file.GetHandle() ), 0 );
}

REGISTER_NAMED_TEST( "FioClose.InvalidHandle", FioClose_InvalidHandle )
{
	// The error of the platform.
	TEST_NE( FioClose( INVALID_OSFILEHANDLE ), 0 );
}

REGISTER_NAMED_TEST( "FioFlush.WrittenFile", FioFlush_WrittenFile )
{
	CFioTestDir dir;
	CFioFile file;
	TEST_EQ( file.Open( dir.m_strFile.c_str(), k_EFioWrite, k_EFioShareNone ), 0 );
	int32 nWritten = 0;
	TEST_EQ( file.Write( "z", 1, &nWritten ), 0 );
	TEST_EQ( file.Flush(), 0 );
}

REGISTER_NAMED_TEST( "FioGetFileIdentity.HandleAndName", FioGetFileIdentity_HandleAndName )
{
	CFioTestDir dir;
	CFioFile file;
	TEST_EQ( file.Open( dir.m_strFile.c_str() ), 0 );

	uint64 nDiskByHandle = 0, nFileByHandle = 0;
	TEST_EQ( FioGetFileIdentity( file.GetHandle(), &nDiskByHandle, &nFileByHandle ), 0 );

	uint64 nDiskByName = 0, nFileByName = 0;
	TEST_EQ( FioGetFileIdentity( dir.m_strFile.c_str(), &nDiskByName, &nFileByName ), 0 );
	TEST_EQ( nDiskByHandle, nDiskByName );
	TEST_EQ( nFileByHandle, nFileByName );

	TEST_NE( FioGetFileIdentity( dir.Path( "missing" ).c_str(), &nDiskByName, &nFileByName ), 0 );
}

REGISTER_NAMED_TEST( "FioIsSameFileIdentity.Names", FioIsSameFileIdentity_Names )
{
	CFioTestDir dir;
	TEST_EQ( FioCopyFile( dir.m_strFile.c_str(), dir.Path( "copy.txt" ).c_str(), true ), 0 );

	// Two spellings of one file are the same, a copy is not.
	const std::string strDotted = std::string( dir.m_szDir ) + "/./file.txt";
	TEST_EQ( FioIsSameFileIdentity( dir.m_strFile.c_str(), strDotted.c_str() ), 0 );
	TEST_EQ( FioIsSameFileIdentity( dir.m_strFile.c_str(), dir.Path( "copy.txt" ).c_str() ), 17 );
}

REGISTER_NAMED_TEST( "FioIsSameFileIdentity.Handles", FioIsSameFileIdentity_Handles )
{
	CFioTestDir dir;
	TEST_EQ( FioCopyFile( dir.m_strFile.c_str(), dir.Path( "copy.txt" ).c_str(), true ), 0 );

	OSFILEHANDLE hFirst, hSecond, hCopy;
	TEST_EQ( FioCreateFile( dir.m_strFile.c_str(), k_EFioRead, k_EFioShareRead, k_EFioOpenExisting, k_EFioNoModifiers, &hFirst ), 0 );
	TEST_EQ( FioCreateFile( dir.m_strFile.c_str(), k_EFioRead, k_EFioShareRead, k_EFioOpenExisting, k_EFioNoModifiers, &hSecond ), 0 );
	TEST_EQ( FioCreateFile( dir.Path( "copy.txt" ).c_str(), k_EFioRead, k_EFioShareRead, k_EFioOpenExisting, k_EFioNoModifiers, &hCopy ), 0 );

	TEST_EQ( FioIsSameFileIdentity( hFirst, hSecond ), 0 );
	TEST_EQ( FioIsSameFileIdentity( hFirst, hCopy ), 17 );

	FioClose( hFirst );
	FioClose( hSecond );
	FioClose( hCopy );
}

REGISTER_NAMED_TEST( "FioGetBaseFileInfo.FileAndDirectory", FioGetBaseFileInfo_FileAndDirectory )
{
	CFioTestDir dir;
	FioBaseFileInfo_t info;
	memset( &info, 0, sizeof( info ) );

	TEST_EQ( FioGetBaseFileInfo( dir.m_strFile.c_str(), &info, k_EFioStandardMetadata ), 0 );
	TEST_EQ( info.nLength, ( uint64 )10 );
	TEST_TRUE( ( info.nValidInfo & k_EFioMetadataLength ) != 0 );
	TEST_TRUE( ( info.nFileFlags & k_EFioFileInfoFile ) != 0 );
	TEST_FALSE( ( info.nFileFlags & k_EFioFileInfoDirectory ) != 0 );
	TEST_TRUE( info.nWriteTime != 0 );

	memset( &info, 0, sizeof( info ) );
	TEST_EQ( FioGetBaseFileInfo( dir.m_szDir, &info, k_EFioStandardMetadata ), 0 );
	TEST_TRUE( ( info.nFileFlags & k_EFioFileInfoDirectory ) != 0 );

	TEST_NE( FioGetBaseFileInfo( dir.Path( "missing" ).c_str(), &info, k_EFioStandardMetadata ), 0 );
}

REGISTER_NAMED_TEST( "FioGetFileType.Kinds", FioGetFileType_Kinds )
{
	CFioTestDir dir;
	TEST_EQ( FioCreateSymbolicLink( dir.Path( "link" ).c_str(), dir.m_strFile.c_str(), false ), 0 );

	int nType = 0;
	TEST_EQ( FioGetFileType( dir.m_strFile.c_str(), 0, &nType ), 0 );
	TEST_EQ( nType, ( int )k_EFioFileInfoFile );

	TEST_EQ( FioGetFileType( dir.m_szDir, 0, &nType ), 0 );
	TEST_EQ( nType, ( int )k_EFioFileInfoDirectory );

	// Without k_EFioMetadataFollowSymbolicLinks the link itself is seen.
	TEST_EQ( FioGetFileType( dir.Path( "link" ).c_str(), 0, &nType ), 0 );
	TEST_TRUE( ( nType & k_EFioFileInfoSymbolicLink ) != 0 );
}

REGISTER_NAMED_TEST( "FioGetDiskInfo.TempDirectory", FioGetDiskInfo_TempDirectory )
{
	FioDiskInfo_t info;
	memset( &info, 0, sizeof( info ) );
	TEST_EQ( FioGetDiskInfo( "/tmp", &info ), 0 );
	TEST_TRUE( info.nTotalBytes > 0 );
#ifndef PLATFORM_OSX
	// The macOS tier0 counts the free blocks in f_bsize and the total in f_frsize, so the free
	// bytes can exceed the total
	TEST_TRUE( info.nTotalFreeBytes <= info.nTotalBytes );
#endif
	TEST_TRUE( info.nFreeBytesAvailableToCaller <= info.nTotalFreeBytes );
}

REGISTER_NAMED_TEST( "FioGetSectorSize.TempDirectory", FioGetSectorSize_TempDirectory )
{
	int nSectorSize = 0;
	TEST_EQ( FioGetSectorSize( "/tmp", &nSectorSize ), 0 );
	TEST_TRUE( nSectorSize > 0 );
	TEST_EQ( nSectorSize & ( nSectorSize - 1 ), 0 );
}

REGISTER_NAMED_TEST( "FioFileTimeToTimeT.WriteTime", FioFileTimeToTimeT_WriteTime )
{
	CFioTestDir dir;
	FioBaseFileInfo_t info;
	TEST_EQ( FioGetBaseFileInfo( dir.m_strFile.c_str(), &info, k_EFioMetadataWriteTime ), 0 );

	// The file was written in the last minute.
	const int64 nTime = FioFileTimeToTimeT( info.nWriteTime );
	const int64 nNow = ( int64 )time( NULL );
	TEST_TRUE( nTime <= nNow + 1 );
	TEST_TRUE( nTime >= nNow - 60 );
}

REGISTER_NAMED_TEST( "FioIsDotDir.DotsOnly", FioIsDotDir_DotsOnly )
{
	TEST_TRUE( FioIsDotDir( "." ) );
	TEST_TRUE( FioIsDotDir( ".." ) );
	TEST_FALSE( FioIsDotDir( "..." ) );
	TEST_FALSE( FioIsDotDir( ".git" ) );
	TEST_FALSE( FioIsDotDir( "a" ) );
}

REGISTER_NAMED_TEST( "FioReadFileToBuffer.WholeAndLimited", FioReadFileToBuffer_WholeAndLimited )
{
	CFioTestDir dir;
	CUtlBuffer buffer;
	TEST_EQ( FioReadFileToBuffer( dir.m_strFile.c_str(), &buffer, -1 ), 0 );
	TEST_EQ( buffer.TellPut(), 10 );
	TEST_EQ( memcmp( buffer.Base(), "0123456789", 10 ), 0 );

	// A file over the limit is EINVAL.
	TEST_EQ( FioReadFileToBuffer( dir.m_strFile.c_str(), &buffer, 5 ), 22 );

	TEST_NE( FioReadFileToBuffer( dir.Path( "missing" ).c_str(), &buffer, -1 ), 0 );
}

REGISTER_NAMED_TEST( "FioReadFileToMemory.Int32", FioReadFileToMemory_Int32 )
{
	CFioTestDir dir;
	void *pContent = NULL;
	int32 nBytes = 0;

	// The terminator is not counted.
	TEST_EQ( FioReadFileToMemory( dir.m_strFile.c_str(), &pContent, &nBytes, true, ( int32 )-1 ), 0 );
	TEST_NOT_NULL( pContent );
	TEST_EQ( nBytes, 10 );
	TEST_EQ( strcmp( static_cast< const char * >( pContent ), "0123456789" ), 0 );
	g_pMemAlloc->Free( pContent );
}

REGISTER_NAMED_TEST( "FioReadFileToMemory.SizeT", FioReadFileToMemory_SizeT )
{
	CFioTestDir dir;
	void *pContent = NULL;
	size_t nBytes = 0;
	TEST_EQ( FioReadFileToMemory( dir.m_strFile.c_str(), &pContent, &nBytes, false, ( size_t )-1 ), 0 );
	TEST_NOT_NULL( pContent );
	TEST_EQ( nBytes, ( size_t )10 );
	TEST_EQ( memcmp( pContent, "0123456789", 10 ), 0 );
	g_pMemAlloc->Free( pContent );
}

REGISTER_NAMED_TEST( "FioCopyFile.FailIfExists", FioCopyFile_FailIfExists )
{
	CFioTestDir dir;
	const std::string strCopy = dir.Path( "copy.txt" );
	TEST_EQ( FioCopyFile( dir.m_strFile.c_str(), strCopy.c_str(), true ), 0 );
	TEST_NE( FioCopyFile( dir.m_strFile.c_str(), strCopy.c_str(), true ), 0 );
	TEST_EQ( FioCopyFile( dir.m_strFile.c_str(), strCopy.c_str(), false ), 0 );

	uint32 nSource = 0, nCopy = 0;
	TEST_EQ( FioGetFileCRC32( dir.m_strFile.c_str(), &nSource, 4096, NULL ), 0 );
	TEST_EQ( FioGetFileCRC32( strCopy.c_str(), &nCopy, 4096, NULL ), 0 );
	TEST_EQ( nSource, nCopy );
}

REGISTER_NAMED_TEST( "FioCreateSymbolicLink.PointsAtTarget", FioCreateSymbolicLink_PointsAtTarget )
{
	CFioTestDir dir;
	const std::string strLink = dir.Path( "link" );
	TEST_EQ( FioCreateSymbolicLink( strLink.c_str(), dir.m_strFile.c_str(), false ), 0 );

	char szTarget[ 256 ] = {};
	TEST_TRUE( readlink( strLink.c_str(), szTarget, sizeof( szTarget ) - 1 ) > 0 );
	TEST_EQ( strcmp( szTarget, dir.m_strFile.c_str() ), 0 );
}

REGISTER_NAMED_TEST( "FioReadLink.Target", FioReadLink_Target )
{
	CFioTestDir dir;
	const std::string strLink = dir.Path( "link" );
	TEST_EQ( symlink( "file.txt", strLink.c_str() ), 0 );

	CBufferString target;
	TEST_EQ( FioReadLink( strLink.c_str(), &target, false ), 0 );
	TEST_EQ( strcmp( target.Get(), "file.txt" ), 0 );

	// The full path resolves the relative target.
	TEST_EQ( FioReadLink( strLink.c_str(), &target, true ), 0 );
	TEST_EQ( strcmp( target.Get(), dir.m_strFile.c_str() ), 0 );

	TEST_NE( FioReadLink( dir.m_strFile.c_str(), &target, false ), 0 );
}

REGISTER_NAMED_TEST( "FioGetFileCRC32.MatchesCRC32", FioGetFileCRC32_MatchesCRC32 )
{
	CFioTestDir dir;
	uint32 nCRC = 0;
	uint64 nBytes = 0;

	// The file is read in chunks of the given size.
	TEST_EQ( FioGetFileCRC32( dir.m_strFile.c_str(), &nCRC, 4, &nBytes ), 0 );
	TEST_EQ( nBytes, ( uint64 )10 );

	// The CRC32 of "0123456789".
	TEST_EQ( nCRC, ( uint32 )0xA684C7C6 );
}

REGISTER_NAMED_TEST( "FioGetFileCRC32Buffer.SmallWorkBuffer", FioGetFileCRC32Buffer_SmallWorkBuffer )
{
	CFioTestDir dir;
	uint8 workBuffer[ 3 ];
	uint32 nCRC = 0;
	uint64 nBytes = 0;
	TEST_EQ( FioGetFileCRC32Buffer( dir.m_strFile.c_str(), &nCRC, workBuffer, sizeof( workBuffer ), &nBytes ), 0 );
	TEST_EQ( nBytes, ( uint64 )10 );
	TEST_EQ( nCRC, ( uint32 )0xA684C7C6 );
}

REGISTER_NAMED_TEST( "FioFindFileInDirCaseInsensitive.Spelling", FioFindFileInDirCaseInsensitive_Spelling )
{
	CFioTestDir dir;
	const std::string strUpper = dir.Path( "FILE.TXT" );

	CBufferString found;
	const char *pszFound = FioFindFileInDirCaseInsensitive( strUpper.c_str(), &found, true );
	TEST_NOT_NULL( pszFound );
	TEST_EQ( strcmp( pszFound, dir.m_strFile.c_str() ), 0 );

	TEST_NULL( FioFindFileInDirCaseInsensitive( dir.Path( "missing" ).c_str(), &found, true ) );

	// Two spellings match with bRequireUniqueMatch; a case-insensitive file system, as on macOS,
	// keeps one file for both.
	FILE *pFile = fopen( dir.Path( "File.txt" ).c_str(), "wb" );
	TEST_NOT_NULL( pFile );
	fclose( pFile );

	if ( FioIsSameFileIdentity( dir.m_strFile.c_str(), dir.Path( "File.txt" ).c_str() ) != 0 )
		TEST_NULL( FioFindFileInDirCaseInsensitive( strUpper.c_str(), &found, true ) );

	TEST_NOT_NULL( FioFindFileInDirCaseInsensitive( strUpper.c_str(), &found, false ) );
}

REGISTER_NAMED_TEST( "FioGetActualFilenameCase.Spelling", FioGetActualFilenameCase_Spelling )
{
	CFioTestDir dir;
	CBufferString actual;
	TEST_TRUE( FioGetActualFilenameCase( dir.Path( "FILE.TXT" ).c_str(), &actual, true ) );
	TEST_EQ( strcmp( actual.Get(), dir.m_strFile.c_str() ), 0 );
	TEST_FALSE( FioGetActualFilenameCase( dir.Path( "missing" ).c_str(), &actual, true ) );
}

REGISTER_NAMED_TEST( "FioEnumerateAbsPathsInDirRecursive.Tree", FioEnumerateAbsPathsInDirRecursive_Tree )
{
	CFioTestDir dir;
	TEST_EQ( mkdir( dir.Path( "sub" ).c_str(), 0700 ), 0 );
	FILE *pFile = fopen( dir.Path( "sub/inner.txt" ).c_str(), "wb" );
	TEST_NOT_NULL( pFile );
	fclose( pFile );

	CUtlHashtable< CUtlString > paths;
	TEST_TRUE( FioEnumerateAbsPathsInDirRecursive( dir.m_szDir, &paths ) );
	TEST_TRUE( paths.HasElement( CUtlString( dir.m_strFile.c_str() ) ) );
	TEST_TRUE( paths.HasElement( CUtlString( dir.Path( "sub/inner.txt" ).c_str() ) ) );
}

REGISTER_NAMED_TEST( "FioBaseFileInfo_t.Layout", FioBaseFileInfo_t_Layout )
{
	TEST_EQ( sizeof( FioBaseFileInfo_t ), ( size_t )32 );
	TEST_EQ( offsetof( FioBaseFileInfo_t, nWriteTime ), ( size_t )16 );
	TEST_EQ( offsetof( FioBaseFileInfo_t, nFileFlags ), ( size_t )28 );
	TEST_EQ( sizeof( FioFileTime_t ), ( size_t )8 );
}

REGISTER_NAMED_TEST( "FioDiskInfo_t.Layout", FioDiskInfo_t_Layout )
{
	TEST_EQ( sizeof( FioDiskInfo_t ), ( size_t )24 );
	TEST_EQ( offsetof( FioDiskInfo_t, nTotalFreeBytes ), ( size_t )16 );
}

REGISTER_NAMED_TEST( "EFioFlags.Values", EFioFlags_Values )
{
	TEST_EQ( ( int )k_EFioReadWrite, ( int )( k_EFioRead | k_EFioWrite ) );
	TEST_EQ( ( int )k_EFioShareReadWriteDelete, ( int )( k_EFioShareRead | k_EFioShareWrite | k_EFioShareDelete ) );
	TEST_EQ( ( int )k_EFioCreateAlways, 3 );
	TEST_EQ( ( int )k_EFioDeleteOnClose, 32 );
	TEST_EQ( ( int )k_EFioSeekEnd, 2 );
	TEST_EQ( ( int )k_EFioStandardMetadata, ( int )( k_EFioMetadataName | k_EFioMetadataLength | k_EFioMetadataFileFlags | k_EFioMetadataCreateTime | k_EFioMetadataWriteTime ) );
	TEST_EQ( ( int )k_EFioFileInfoReadOnly, 8 );
}

REGISTER_NAMED_TEST( "CPathString.WideAndUTF8Buffers", CPathString_WideAndUTF8Buffers )
{
	// The typed buffers are pool blocks.
	char *pszUTF8 = CPathString::NewUTF8PathStringBuffer();
	TEST_NOT_NULL( pszUTF8 );
	pszUTF8[ CPathString::k_nUTF8PathStringBufferChars - 1 ] = '\0';
	CPathString::DeleteUTF8PathStringBuffer( pszUTF8 );

	wchar_t *pwszWide = CPathString::NewWCharPathStringBuffer();
	TEST_NOT_NULL( pwszWide );
	pwszWide[ CPathString::k_nWCharPathStringBufferChars - 1 ] = L'\0';
	CPathString::DeleteWCharPathStringBuffer( pwszWide );

	TEST_EQ( ( size_t )CPathString::k_nWCharPathStringBufferChars * sizeof( wchar_t ), ( size_t )CPathString::k_nPathStringBufferBytes );
}

REGISTER_NAMED_TEST( "CPathString.DirectoryAndFileName", CPathString_DirectoryAndFileName )
{
	CPathString path( "/tmp", "a.txt" );
	TEST_EQ( strcmp( path.GetUTF8Path(), "/tmp/a.txt" ), 0 );

	// An empty path holds no buffers.
	CPathString empty;
	TEST_NULL( empty.GetUTF8Path() );
}

REGISTER_NAMED_TEST( "CDirIterator.CurrentFileInfo", CDirIterator_CurrentFileInfo )
{
	CFioTestDir dir;
	TEST_EQ( mkdir( dir.Path( "sub" ).c_str(), 0700 ), 0 );
	TEST_EQ( FioCreateSymbolicLink( dir.Path( "link" ).c_str(), dir.m_strFile.c_str(), false ), 0 );
	TEST_EQ( chmod( dir.m_strFile.c_str(), 0400 ), 0 );

	CDirIterator iterator( dir.m_szDir, "*", k_EFioStandardMetadata );
	int nSeen = 0;

	while ( iterator.BNextFile() )
	{
		const char *pszName = iterator.CurrentFileName();
		const FioBaseFileInfo_t &info = iterator.CurrentFileInfo();
		TEST_EQ( info.nFileFlags, iterator.CurrentFileFlags() );
		TEST_EQ( info.nValidInfo, iterator.CurrentFileInfoMetaFlags() );

		if ( iterator.CurrentIsDotDir() )
		{
			TEST_TRUE( FioIsDotDir( pszName ) );
			continue;
		}

		if ( strcmp( pszName, "file.txt" ) == 0 )
		{
			TEST_FALSE( iterator.CurrentIsDir() );
			TEST_TRUE( iterator.CurrentIsReadOnly() );
			TEST_EQ( iterator.CurrentFileLength(), ( uint64 )10 );
			TEST_TRUE( iterator.CurrentFileWriteTime() != 0 );
			TEST_TRUE( iterator.CurrentFileCreateTime() != 0 || ( iterator.CurrentFileInfoMetaFlags() & k_EFioMetadataCreateTime ) == 0 );
			nSeen++;
		}
		else if ( strcmp( pszName, "sub" ) == 0 )
		{
			TEST_TRUE( iterator.CurrentIsDir() );
			TEST_TRUE( iterator.BCurrentIsDir() );
			nSeen++;
		}
		else if ( strcmp( pszName, "link" ) == 0 )
		{
			TEST_TRUE( iterator.CurrentIsSymbolicLink() );
			nSeen++;
		}
	}

	TEST_EQ( nSeen, 3 );
	chmod( dir.m_strFile.c_str(), 0600 );
}

#endif // !_WIN32
