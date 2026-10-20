//========= Copyright 1996-2008, Valve Corporation, All rights reserved. ============//
//
// Purpose: A collection of utility classes to simplify file I/O, and
//			as much as possible contain portability problems. Here avoiding 
//			including windows.h.
//
//=============================================================================

#ifndef FILEIO_H
#define FILEIO_H

#if defined (_WIN32)
typedef __time64_t time64_t;
#else
#include <sys/types.h>
#include <sys/stat.h>
typedef int64_t time64_t;
#if !defined( _PS3 )
#include <signal.h>
#include <dirent.h>
#include <limits.h>
#endif // _PS3
#endif

#include "tier0/platform.h"
#include "tier0/t0constants.h"
#include "tier0/utlstring.h"
#include "tier0/utllinkedlist.h"
#include "tier0/utlhashtable.h"

class CBufferString;
class CUtlBuffer;
template < class T, typename I, size_t N > class CUtlVectorMemory_Growable;
template < class T, typename I, class A > class CUtlVector;

//-----------------------------------------------------------------------------
// Absolute path with fixed slashes and no double slashes, in buffers from a pool
// of tier0, and its wide form with \\?\ in front for unicode aware file calls
//-----------------------------------------------------------------------------
class CPathString
{
public:
	// Bytes of a pooled block: four per character of the longest path of the platform
#ifdef PLATFORM_WINDOWS
	static const int k_nPathStringBufferBytes = 128 * 1024;
#elif defined( PLATFORM_OSX )
	static const int k_nPathStringBufferBytes = 4 * 1024;
#else
	static const int k_nPathStringBufferBytes = 16 * 1024;
#endif
	static const int k_nUTF8PathStringBufferChars = k_nPathStringBufferBytes;
	static const int k_nWCharPathStringBufferChars = k_nPathStringBufferBytes / sizeof( wchar_t );

	// One pooled block that holds the text of a path, the UTF-8 or the wide one
	struct Buffer_t
	{
		uint8 m_data[ k_nPathStringBufferBytes ];
	};

	CPathString() : m_pchUTF8Path( NULL ), m_pwchWideCharPathPrepended( NULL ) {}
	explicit CPathString( const char *pchUTF8Path ) : m_pchUTF8Path( NULL ), m_pwchWideCharPathPrepended( NULL ) { Set( pchUTF8Path ); }
	CPathString( const char *pchDirectory, const char *pchFileName ) : m_pchUTF8Path( NULL ), m_pwchWideCharPathPrepended( NULL ) { Set( pchDirectory, pchFileName ); }
	DLL_CLASS_IMPORT ~CPathString();

	DLL_CLASS_IMPORT void Set( const char *pchUTF8Path );

	// Composes the path from a directory and a file name
	DLL_CLASS_IMPORT void Set( const char *pchDirectory, const char *pchFileName );

	const char *GetUTF8Path() const { return m_pchUTF8Path; }

	// The path after \\?\ or \\?\UNC\, UTF-16 on Windows and UTF-32 elsewhere, null when there is no path
	const wchar_t *GetWCharPathPrePended()
	{
		PopulateWCharPath();
		return m_pwchWideCharPathPrepended;
	}

	DLL_CLASS_IMPORT static Buffer_t *NewPathStringBuffer();
	DLL_CLASS_IMPORT static void DeletePathStringBuffer( Buffer_t *pBuffer );

	static char *NewUTF8PathStringBuffer() { return reinterpret_cast< char * >( NewPathStringBuffer() ); }
	static void DeleteUTF8PathStringBuffer( char *pBuffer ) { DeletePathStringBuffer( reinterpret_cast< Buffer_t * >( pBuffer ) ); }
	static wchar_t *NewWCharPathStringBuffer() { return reinterpret_cast< wchar_t * >( NewPathStringBuffer() ); }
	static void DeleteWCharPathStringBuffer( wchar_t *pBuffer ) { DeletePathStringBuffer( reinterpret_cast< Buffer_t * >( pBuffer ) ); }

protected:
	DLL_CLASS_IMPORT void PopulateWCharPath();
	DLL_CLASS_IMPORT void DeleteWCharPath();

	// Empties the path, keeping its buffer
	DLL_CLASS_IMPORT void Reset();

private:
	char *m_pchUTF8Path;
	wchar_t *m_pwchWideCharPathPrepended;

	CPathString( const CPathString & ) = delete;
	CPathString &operator=( const CPathString & ) = delete;
};

//-----------------------------------------------------------------------------
// File I/O of tier0 on the native handles; the functions return 0 or the error code
// of the platform (GetLastError, errno)
//-----------------------------------------------------------------------------
enum EFioAccessFlags : uint32
{
	k_EFioRead = 1,
	k_EFioWrite = 2,
	k_EFioReadWrite = 3,
};

enum EFioShareFlags : uint32
{
	k_EFioShareNone = 0,
	k_EFioShareRead = 1,
	k_EFioShareWrite = 2,
	k_EFioShareReadWrite = 3,
	k_EFioShareDelete = 4,
	k_EFioShareReadDelete = 5,
	k_EFioShareWriteDelete = 6,
	k_EFioShareReadWriteDelete = 7,
};

enum EFioCreateMode : uint32
{
	k_EFioOpenExisting = 0,
	k_EFioOpenAlways = 1,
	k_EFioCreateNew = 2,
	k_EFioCreateAlways = 3,
};

// Modifiers of FioCreateFile
enum EFioModifiers : uint32
{
	k_EFioNoModifiers = 0,
	k_EFioAsync = 1,
	k_EFioNoBuffering = 2,
	k_EFioNoAccessTime = 4,
	k_EFioSequentialScan = 8,
	k_EFioTemporary = 16,
	k_EFioDeleteOnClose = 32,
};

enum EFioSeekMode : uint32
{
	k_EFioSeekSet = 0,
	k_EFioSeekCurrent = 1,
	k_EFioSeekEnd = 2,
};

// What FioGetBaseFileInfo fills in, FioBaseFileInfo_t::nValidInfo tells what it did
enum EFioMetadataFlags : uint32
{
	k_EFioMetadataName = 1,
	k_EFioMetadataLength = 2,
	k_EFioMetadataFileFlags = 4,
	k_EFioMetadataCreateTime = 8,
	k_EFioMetadataWriteTime = 16,
	k_EFioMetadataFollowSymbolicLinks = 32,
	k_EFioMetadataDoesNotExist = 64,
	k_EFioStandardMetadata = 31,
};

enum EFioFileInfoFlags : uint32
{
	k_EFioFileInfoFile = 1,
	k_EFioFileInfoDirectory = 2,

	// A symbolic link or another reparse point
	k_EFioFileInfoSymbolicLink = 4,
	k_EFioFileInfoReadOnly = 8,
};

// Time of the platform's file system, FioFileTimeToTimeT converts it
typedef uint64 FioFileTime_t;

//-----------------------------------------------------------------------------
// Size, times and kind of a file
//-----------------------------------------------------------------------------
struct FioBaseFileInfo_t
{
	uint64 nLength;
	FioFileTime_t nCreateTime;
	FioFileTime_t nWriteTime;

	// EFioMetadataFlags of the members that were filled in
	uint nValidInfo;

	// EFioFileInfoFlags
	uint nFileFlags;
};

// Space of the disk of a path, in bytes
struct FioDiskInfo_t
{
	uint64 nTotalBytes;
	uint64 nFreeBytesAvailableToCaller;
	uint64 nTotalFreeBytes;
};

PLATFORM_OVERLOAD int FioCreateFile( const char *pFileName, EFioAccessFlags eAccess, EFioShareFlags eShare, EFioCreateMode eMode, uint nModifiers, OSFILEHANDLE *pHandle );
PLATFORM_OVERLOAD int FioCreatePipe( OSFILEHANDLE *pRead, OSFILEHANDLE *pWrite, int32 nBufferBytesHint );
PLATFORM_OVERLOAD int FioDuplicate( OSFILEHANDLE Handle, bool bInherit, OSFILEHANDLE *pDuplicate );
PLATFORM_OVERLOAD int FioClose( OSFILEHANDLE Handle );
PLATFORM_OVERLOAD int FioCancelIo( OSFILEHANDLE Handle );
PLATFORM_OVERLOAD int FioFlush( OSFILEHANDLE Handle );

PLATFORM_OVERLOAD int FioRead( OSFILEHANDLE Handle, void *pBuffer, int32 nBytes, int32 *pBytesRead );
PLATFORM_OVERLOAD int FioRead( OSFILEHANDLE Handle, void *pBuffer, size_t nBytes, size_t *pBytesRead );
PLATFORM_OVERLOAD int FioReadAt( OSFILEHANDLE Handle, uint64 nFilePos, void *pBuffer, int32 nBytes, int32 *pBytesRead );
PLATFORM_OVERLOAD int FioWrite( OSFILEHANDLE Handle, const void *pBuffer, int32 nBytes, int32 *pBytesWritten );
PLATFORM_OVERLOAD int FioWrite( OSFILEHANDLE Handle, const void *pBuffer, size_t nBytes, size_t *pBytesWritten );
PLATFORM_OVERLOAD int FioWriteAt( OSFILEHANDLE Handle, uint64 nFilePos, const void *pBuffer, int32 nBytes, int32 *pBytesWritten );

PLATFORM_OVERLOAD int FioSeek( OSFILEHANDLE Handle, int64 nOffset, EFioSeekMode eHow, uint64 *pNewPosition );
PLATFORM_OVERLOAD int FioGetSize( OSFILEHANDLE Handle, uint64 *pBytes );
PLATFORM_OVERLOAD int FioSetSize( OSFILEHANDLE Handle, uint64 nBytes );
PLATFORM_OVERLOAD int FioTruncateToSeekPosition( OSFILEHANDLE Handle );

// Identity of the disk and the file, equal for every path of one file
PLATFORM_OVERLOAD int FioGetFileIdentity( OSFILEHANDLE Handle, uint64 *pDiskId, uint64 *pFileId );
PLATFORM_OVERLOAD int FioGetFileIdentity( const char *pFileName, uint64 *pDiskId, uint64 *pFileId );

// 0 for the same file, ERROR_NOT_SAME_DEVICE (17) for different files, otherwise an error code
PLATFORM_OVERLOAD int FioIsSameFileIdentity( OSFILEHANDLE Handle1, OSFILEHANDLE Handle2 );
PLATFORM_OVERLOAD int FioIsSameFileIdentity( const char *pFileName1, const char *pFileName2 );

// nMetaFlags is a combination of EFioMetadataFlags
PLATFORM_OVERLOAD int FioGetBaseFileInfo( const char *pFileName, FioBaseFileInfo_t *pInfo, uint nMetaFlags );

// Puts the k_EFioFileInfoFile, k_EFioFileInfoDirectory and k_EFioFileInfoSymbolicLink bits of the file in pFileType
PLATFORM_INTERFACE int FioGetFileType( const char *pFileName, uint nMetaFlags, int *pFileType );

PLATFORM_OVERLOAD int FioGetDiskInfo( const char *pDiskPath, FioDiskInfo_t *pInfo );
PLATFORM_OVERLOAD int FioGetSectorSize( const char *pDiskPath, int *pSectorSize );
PLATFORM_OVERLOAD int64 FioFileTimeToTimeT( FioFileTime_t nFioTime );

// True for . and ..
PLATFORM_OVERLOAD bool FioIsDotDir( const char *pDir );

PLATFORM_OVERLOAD int FioReadFileToBuffer( const char *pFileName, CUtlBuffer *pBuffer, int32 nMaxBytes );

// The content is allocated with g_pMemAlloc
PLATFORM_OVERLOAD int FioReadFileToMemory( const char *pFileName, void **pContent, int32 *pContentBytes, bool bZeroTerminate, int32 nMaxBytes );
PLATFORM_OVERLOAD int FioReadFileToMemory( const char *pFileName, void **pContent, size_t *pContentBytes, bool bZeroTerminate, size_t nMaxBytes );

PLATFORM_OVERLOAD int FioCopyFile( const char *pSourceFileName, const char *pDestFileName, bool bFailIfExists );
PLATFORM_OVERLOAD int FioCreateSymbolicLink( const char *pLinkName, const char *pTargetName, bool bDirectory );
PLATFORM_OVERLOAD int FioReadLink( const char *pLinkName, CBufferString *pTargetBuf, bool bFullPath );

// The CRC32 of the whole file, read in chunks of nChunkBytes
PLATFORM_OVERLOAD int FioGetFileCRC32( const char *pFileName, uint32 *pCRC32, int32 nChunkBytes, uint64 *pFileBytes );
PLATFORM_OVERLOAD int FioGetFileCRC32Buffer( const char *pFileName, uint32 *pCRC32, uint8 *pWorkBuffer, int32 nWorkBufferBytes, uint64 *pFileBytes );

// The path of pFile with the name as the directory spells it, null when there is no such file;
// bRequireUniqueMatch fails when the name matches more than one file
PLATFORM_OVERLOAD const char *FioFindFileInDirCaseInsensitive( const char *pFile, CBufferString *pFileNameOut, bool bRequireUniqueMatch );
PLATFORM_OVERLOAD bool FioGetActualFilenameCase( const char *pFileName, CBufferString *pActualFileName, bool bRequireUniqueMatch );

// Adds the absolute paths of the files under pDirectory, traversing it on the thread pool
PLATFORM_OVERLOAD bool FioEnumerateAbsPathsInDirRecursive( const char *pDirectory, CUtlHashtable< CUtlString > *pPaths );

//-----------------------------------------------------------------------------
// A native file handle closed by the destructor; the members return what the Fio functions do
//-----------------------------------------------------------------------------
class CFioFile
{
public:
	CFioFile() : m_hFile( INVALID_OSFILEHANDLE ) {}
	~CFioFile() { Close(); }

	CFioFile( const CFioFile & ) = delete;
	CFioFile &operator=( const CFioFile & ) = delete;

	int Open( const char *pFileName, EFioAccessFlags eAccess = k_EFioRead, EFioShareFlags eShare = k_EFioShareRead, EFioCreateMode eMode = k_EFioOpenExisting, uint nModifiers = k_EFioNoModifiers )
	{
		Close();

		int nError = FioCreateFile( pFileName, eAccess, eShare, eMode, nModifiers, &m_hFile );

		if ( nError != 0 )
			m_hFile = INVALID_OSFILEHANDLE;

		return nError;
	}

	int Close()
	{
		if ( !IsOpen() )
			return 0;

		int nError = FioClose( m_hFile );
		m_hFile = INVALID_OSFILEHANDLE;
		return nError;
	}

	bool IsOpen() const { return m_hFile != INVALID_OSFILEHANDLE; }
	OSFILEHANDLE GetHandle() const { return m_hFile; }

	// Gives the handle up without closing it
	OSFILEHANDLE Detach()
	{
		OSFILEHANDLE hFile = m_hFile;
		m_hFile = INVALID_OSFILEHANDLE;
		return hFile;
	}

	int Read( void *pBuffer, int32 nBytes, int32 *pBytesRead ) { return FioRead( m_hFile, pBuffer, nBytes, pBytesRead ); }
	int Read( void *pBuffer, size_t nBytes, size_t *pBytesRead ) { return FioRead( m_hFile, pBuffer, nBytes, pBytesRead ); }
	int ReadAt( uint64 nFilePos, void *pBuffer, int32 nBytes, int32 *pBytesRead ) { return FioReadAt( m_hFile, nFilePos, pBuffer, nBytes, pBytesRead ); }
	int Write( const void *pBuffer, int32 nBytes, int32 *pBytesWritten ) { return FioWrite( m_hFile, pBuffer, nBytes, pBytesWritten ); }
	int Write( const void *pBuffer, size_t nBytes, size_t *pBytesWritten ) { return FioWrite( m_hFile, pBuffer, nBytes, pBytesWritten ); }
	int WriteAt( uint64 nFilePos, const void *pBuffer, int32 nBytes, int32 *pBytesWritten ) { return FioWriteAt( m_hFile, nFilePos, pBuffer, nBytes, pBytesWritten ); }
	int Flush() { return FioFlush( m_hFile ); }

	int Seek( int64 nOffset, EFioSeekMode eHow, uint64 *pNewPosition ) { return FioSeek( m_hFile, nOffset, eHow, pNewPosition ); }
	int GetSize( uint64 *pBytes ) const { return FioGetSize( m_hFile, pBytes ); }
	int SetSize( uint64 nBytes ) { return FioSetSize( m_hFile, nBytes ); }
	int TruncateToSeekPosition() { return FioTruncateToSeekPosition( m_hFile ); }

private:
	OSFILEHANDLE m_hFile;
};

//-----------------------------------------------------------------------------
// Iterates the files and directories that match a pattern, UTF-8 everywhere
//-----------------------------------------------------------------------------
class CDirIterator
{
public:
	DLL_CLASS_IMPORT CDirIterator();

	// nMetaFlags are the EFioMetadataFlags passed on to FioGetBaseFileInfo, more than
	// k_EFioMetadataName fills the file info of every file
	DLL_CLASS_IMPORT CDirIterator( const char *pchPath, const char *pchPattern, uint32 nMetaFlags );
	DLL_CLASS_IMPORT CDirIterator( const char *pchSearchPath, uint32 nMetaFlags );
	DLL_CLASS_IMPORT ~CDirIterator();

	// Moves to the next file, the first call keeps the one found by the constructor
	DLL_CLASS_IMPORT bool NextFile();
	bool BNextFile() { return NextFile(); }

	// Name of the current file, without the path
	const char *CurrentFileName() const
	{
#ifdef _WIN32
		return m_pFileName;
#else
		return m_pNames[ m_nCurName ]->d_name;
#endif
	}

	DLL_CLASS_IMPORT const char *CurrentFileFullPath();

	uint CurrentFileInfoMetaFlags() const { return m_CurInfo.nValidInfo; }
	const FioBaseFileInfo_t &CurrentFileInfo() const { return m_CurInfo; }
	uint64 CurrentFileLength() const { return m_CurInfo.nLength; }
	FioFileTime_t CurrentFileCreateTime() const { return m_CurInfo.nCreateTime; }
	FioFileTime_t CurrentFileWriteTime() const { return m_CurInfo.nWriteTime; }
	uint CurrentFileFlags() const { return m_CurInfo.nFileFlags; }
	bool CurrentIsDir() const { return ( m_CurInfo.nFileFlags & k_EFioFileInfoDirectory ) != 0; }
	bool CurrentIsDotDir() const { return FioIsDotDir( CurrentFileName() ); }
	bool CurrentIsReadOnly() const { return ( m_CurInfo.nFileFlags & k_EFioFileInfoReadOnly ) != 0; }
	bool CurrentIsSymbolicLink() const { return ( m_CurInfo.nFileFlags & k_EFioFileInfoSymbolicLink ) != 0; }
	bool BCurrentIsDir() const { return CurrentIsDir(); }

	DLL_CLASS_IMPORT void Stop();

protected:
	DLL_CLASS_IMPORT void Construct();
	DLL_CLASS_IMPORT bool Init( const char *pchPath, const char *pchPattern, const char *pchSearchPath, uint32 nMetaFlags );
	DLL_CLASS_IMPORT bool SplitPathAndPattern( const char *pchSearchPath, CUtlString *pPath, CUtlString *pPattern );

	uint m_nMetaFlags;
	bool m_bNoFiles;
	bool m_bHaveFullPath;

public:
	// Lets Windows fetch larger blocks of the directory, read by the next Init
	bool m_bLargeFetch;

protected:
	CUtlString m_pBasePath;
	char *m_pFullPath;
	int m_nFullPathChars;
	FioBaseFileInfo_t m_CurInfo;
	bool m_bUsedFirstFile;

#ifdef _WIN32
	HANDLE m_hFind;
	char *m_pFileName;
	int m_nFileNameChars;
#else
	// From scandir
	struct dirent **m_pNames;
	int m_nNames;
	int m_nCurName;
#endif

	CDirIterator( const CDirIterator & ) = delete;
	CDirIterator &operator=( const CDirIterator & ) = delete;
};

//-----------------------------------------------------------------------------
// Purpose: Encapsulates buffered async writing to a large file (one that will require multiple write calls)
//			calling Close() or destructing this object will block until the file is completely written
//-----------------------------------------------------------------------------
class CFileWriter
{
public:

	// Possible seek types
	enum ESeekOrigin
	{
		k_ESeekSet,
		k_ESeekCur,
		k_ESeekEnd
	};

	CFileWriter( bool bAsync = false );
	virtual ~CFileWriter();

	bool BFileOpen();
	bool BSetFile( const char *pchFile, bool bAllowOpenExisting = false );
	bool Write( const void *pvData, uint32 cubData );
	int  Printf( char *pDest, int bufferLen, PRINTF_FORMAT_STRING char const *pFormat, ... );
	bool Seek( uint64 offset, ESeekOrigin eOrigin );
	void Flush();
	void Close();
	uint64 GetBytesWritten();

#ifdef _WIN32
	static void __stdcall ThreadedWriteFileCompletionFunc( unsigned long dwErrorCode, unsigned long dwBytesTransfered, struct _OVERLAPPED *pOverlapped );
#elif defined( _PS3 )
	// not implemented on PS3
#elif defined(POSIX)
	static void __stdcall ThreadedWriteFileCompletionFunc( sigval sigval );
#else
#error
#endif

	void Sleep( uint nMSec ); // system specific sleep call

private:
	HANDLE m_hFileDest;
	uint64 m_cubWritten;
	volatile int m_cubOutstanding;
	bool m_bAsync;
	bool m_bDefaultAsync;
	uint32 m_unThreadID; // main thread for this FileWriter. On this thread we support correct async IO
	
	// if the CFileWriter is called from any other thread, we block until the write is complete
	// this is not great but a good enough for log files and we didn't need a full blow IOCP manager for this.
	volatile int m_cPendingCallbacksFromOtherThreads; 

};

// data accessor
inline uint64 CFileWriter::GetBytesWritten()
{ 
	return m_cubWritten; 
}

#if !defined(_PS3)
//-----------------------------------------------------------------------------
// Watches a directory for file changes
//-----------------------------------------------------------------------------
class CDirWatcher
{
public:
	enum ChangeFlags_t
	{
		// File and directory names
		CHANGE_NAME = ( 1 << 0 ),
		CHANGE_SIZE = ( 1 << 1 ),
		CHANGE_ATTRIBUTES = ( 1 << 2 ),
		CHANGE_CREATION = ( 1 << 3 ),
		CHANGE_LAST_WRITE = ( 1 << 4 ),
	};

#if defined( PLATFORM_WINDOWS )
	// The thread that receives the change notifications, 0 for the calling one
	typedef uint32 MonitoringThreadInfo_t;
#elif defined( PLATFORM_OSX )
	typedef struct __CFRunLoop *MonitoringThreadInfo_t;
#else
	typedef void *MonitoringThreadInfo_t;
#endif

	DLL_CLASS_IMPORT CDirWatcher();
	~CDirWatcher() { Stop(); }

	// Only one directory can be watched at a time, nChangeFlags are ChangeFlags_t,
	// nBufferSize is the size of the notification buffer, 0 for 16 KB
	DLL_CLASS_IMPORT bool SetDirToWatch( const char *pchDir, uint32 nChangeFlags, bool bRecursive, MonitoringThreadInfo_t monitoringThreadInfo, size_t nBufferSize );

	// Takes the next changed file with the watched directory in front
	DLL_CLASS_IMPORT bool GetChangedFile( CBufferString *pFile );

	// Puts the changed files into pBuffer, or appends pointers into it to pFiles, returns their count;
	// pBuffer is required
	DLL_CLASS_IMPORT uint32 GetChangedFiles( CUtlBuffer *pBuffer, CUtlVector< char *, int, CUtlVectorMemory_Growable< char *, int, 0 > > *pFiles );

	DLL_CLASS_IMPORT void Stop();

private:
	DLL_CLASS_IMPORT void AddFileToChangeList( const char *pchFile );
#ifdef PLATFORM_WINDOWS
	DLL_CLASS_IMPORT bool PostDirWatch();
#endif

	CUtlString m_BaseDir;
	CUtlLinkedList< CUtlString, int > m_listChangedFiles;
	void *m_hFile;
	void *m_pOverlapped;
	void *m_pFileInfo;
	size_t m_nFileInfoSize;
	bool m_bRecursive;

	// FILE_NOTIFY_CHANGE_ bits made from the change flags
	uint32 m_nNotifyFilter;
#ifdef PLATFORM_OSX
	struct timespec m_modTime;
	void *m_WatcherStream;
#endif

	friend class CDirWatcherFriend;

	CDirWatcher( const CDirWatcher & ) = delete;
	CDirWatcher &operator=( const CDirWatcher & ) = delete;
};
#endif // _PS3


//-----------------------------------------------------------------------------
// Watches directory trees for file changes
//-----------------------------------------------------------------------------
class CDirTreeWatchManager
{
public:
	// Receives the files and directories of a traversal
	class Visitor
	{
	public:
		// A file (not a directory) of the current directory, with its info
		virtual void VisitFile( const char *pszFileName, const FioBaseFileInfo_t *pFileInfo ) = 0;
		virtual void EnterDirectory( const char *pszDirectory ) = 0;
		virtual void LeaveDirectory() = 0;

		// TraverseChangedFiles calls them around the changed files of every watched tree,
		// with the root path of the tree first
		virtual void BeginWatchedTree( const char *pszRootPath ) {}
		virtual void EndWatchedTree() {}
	};

#if defined( PLATFORM_WINDOWS )
	typedef uint32 MonitoringThreadInfo_t;
#elif defined( PLATFORM_OSX )
	typedef struct __CFRunLoop *MonitoringThreadInfo_t;
#else
	typedef void *MonitoringThreadInfo_t;
#endif

	DLL_CLASS_IMPORT CDirTreeWatchManager( uint32 nMetaFlags = 19 );
	DLL_CLASS_IMPORT ~CDirTreeWatchManager();

	// The watch of a directory is the one of CDirWatcher::SetDirToWatch with nBufferSize
	DLL_CLASS_IMPORT bool AddWatch( const char *pszPath, Visitor *pVisitor, size_t nBufferSize );
	DLL_CLASS_IMPORT bool GetChangedFile( CBufferString *pFile, FioBaseFileInfo_t *pFileInfo );
	DLL_CLASS_IMPORT void TraverseChangedFiles( Visitor *pVisitor );
	DLL_CLASS_IMPORT void SetMonitoringThreadInfo( MonitoringThreadInfo_t info );
	DLL_CLASS_IMPORT void Stop();

	DLL_CLASS_IMPORT static void CheckForPendingChanges();

private:
	struct Watcher_t;

	uint32 m_nMetaFlags;
	Watcher_t *m_pWatchers;
	MonitoringThreadInfo_t m_MonitoringThreadInfo;

	CDirTreeWatchManager( const CDirTreeWatchManager & ) = delete;
	CDirTreeWatchManager &operator=( const CDirTreeWatchManager & ) = delete;
};

bool CreateDirRecursive( const char *pchPathIn );
bool BFileExists( const char *pchFileNameIn );
bool BCreateDirectory( const char *path );
bool BRemoveDirectoryRecursive( const char *pchPath );

#endif // FILEIO_H
