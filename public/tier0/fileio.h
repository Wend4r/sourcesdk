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
	// The longest path of the platform in characters
#ifdef PLATFORM_WINDOWS
	enum { MAX_PATH_CHARS = 32767 };
#else
	enum { MAX_PATH_CHARS = PATH_MAX };
#endif

	// One pooled block that holds the text of a path, four bytes per character
	// (128 KB on Windows, 16 KB on Linux, 4 KB on macOS)
	union ALIGN16 Buffer_t
	{
		// Next free block while the block is in the pool of tier0 (a TSLNodeBase_t)
		Buffer_t *m_pNext;

		char m_szUTF8Path[ MAX_PATH_CHARS * 4 ];

		// UTF-16 on Windows, UTF-32 elsewhere, after the \\?\ or \\?\UNC\ prefix
		wchar_t m_wszPath[ MAX_PATH_CHARS * 4 / sizeof( wchar_t ) ];
	} ALIGN16_POST;

	CPathString() : m_pUTF8Path( NULL ), m_pWCharPathPrePended( NULL ) {}
	explicit CPathString( const char *pchUTF8Path ) : m_pUTF8Path( NULL ), m_pWCharPathPrePended( NULL ) { Set( pchUTF8Path ); }
	DLL_CLASS_IMPORT ~CPathString();

	DLL_CLASS_IMPORT void Set( const char *pchUTF8Path );

	// Composes the path from a directory and a file name
	DLL_CLASS_IMPORT void Set( const char *pchDirectory, const char *pchFileName );

	const char *GetUTF8Path() const { return m_pUTF8Path ? m_pUTF8Path->m_szUTF8Path : NULL; }

	// Null when there is no path
	const wchar_t *GetWCharPathPrePended()
	{
		PopulateWCharPath();
		return m_pWCharPathPrePended ? m_pWCharPathPrePended->m_wszPath : NULL;
	}

	DLL_CLASS_IMPORT static Buffer_t *NewPathStringBuffer();
	DLL_CLASS_IMPORT static void DeletePathStringBuffer( Buffer_t *pBuffer );

protected:
	// Empties the path, keeping its buffer
	DLL_CLASS_IMPORT void Reset();

	DLL_CLASS_IMPORT void PopulateWCharPath();
	DLL_CLASS_IMPORT void DeleteWCharPath();

private:
	Buffer_t *m_pUTF8Path;
	Buffer_t *m_pWCharPathPrePended;

	CPathString( const CPathString & ) = delete;
	CPathString &operator=( const CPathString & ) = delete;
};

//-----------------------------------------------------------------------------
// Size, times and kind of a file
//-----------------------------------------------------------------------------
struct FioBaseFileInfo_t
{
	enum Flags_t
	{
		FILE_IS_FILE = ( 1 << 0 ),
		FILE_IS_DIRECTORY = ( 1 << 1 ),

		// A symbolic link or another reparse point
		FILE_IS_LINK = ( 1 << 2 ),
		FILE_IS_READ_ONLY = ( 1 << 3 ),
	};

	uint64 m_nSize;

	// File times, FioFileTimeToTimeT converts them
	uint64 m_nCreationTime;
	uint64 m_nLastWriteTime;

	// Bits of the members that were filled in
	uint32 m_nValidFields;
	uint32 m_nFlags;
};

//-----------------------------------------------------------------------------
// Iterates the files and directories that match a pattern, UTF-8 everywhere
//-----------------------------------------------------------------------------
class CDirIterator
{
public:
	DLL_CLASS_IMPORT CDirIterator();

	// nFlags are passed on to FioGetBaseFileInfo, more than 1 fills the file info of every file
	DLL_CLASS_IMPORT CDirIterator( const char *pchPath, const char *pchPattern, uint32 nFlags );
	DLL_CLASS_IMPORT CDirIterator( const char *pchSearchPath, uint32 nFlags );
	DLL_CLASS_IMPORT ~CDirIterator();

	// Moves to the next file, the first call keeps the one found by the constructor
	DLL_CLASS_IMPORT bool NextFile();

	// Name of the current file, without the path
	const char *CurrentFileName() const
	{
#ifdef _WIN32
		return m_pFileName->m_szUTF8Path;
#else
		return m_ppEntries[ m_nCurrentEntry ]->d_name;
#endif
	}

	DLL_CLASS_IMPORT const char *CurrentFileFullPath();

	const FioBaseFileInfo_t &CurrentFileInfo() const { return m_FileInfo; }
	int64 CurrentFileLength() const { return m_FileInfo.m_nSize; }
	bool BCurrentIsDir() const { return ( m_FileInfo.m_nFlags & FioBaseFileInfo_t::FILE_IS_DIRECTORY ) != 0; }
	bool BCurrentIsReadOnly() const { return ( m_FileInfo.m_nFlags & FioBaseFileInfo_t::FILE_IS_READ_ONLY ) != 0; }

	DLL_CLASS_IMPORT void Stop();

protected:
	DLL_CLASS_IMPORT void Construct();
	DLL_CLASS_IMPORT bool Init( const char *pchPath, const char *pchPattern, const char *pchSearchPath, uint32 nFlags );
	DLL_CLASS_IMPORT bool SplitPathAndPattern( const char *pchSearchPath, CUtlString *pPath, CUtlString *pPattern );

	uint32 m_nFlags;
	bool m_bNoFiles;
	bool m_bFullPathValid;

public:
	// Lets Windows fetch larger blocks of the directory, read by the next Init
	bool m_bLargeFetch;

protected:
	CUtlString m_strPath;
	CPathString::Buffer_t *m_pFullPath;
	int m_nFullPathSize;
	FioBaseFileInfo_t m_FileInfo;
	bool m_bUsedFirstFile;

#ifdef _WIN32
	HANDLE m_hFind;
	CPathString::Buffer_t *m_pFileName;
	int m_nFileNameSize;
#else
	// From scandir
	struct dirent **m_ppEntries;
	int m_nEntries;
	int m_nCurrentEntry;
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

	// Puts the changed files into pBuffer, or appends pointers into it to pFiles, returns their count
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
		virtual void unk000( const char *pszPath, void *pUnknown ) = 0;
		virtual void EnterDirectory( const char *pszDirectory ) = 0;
		virtual void LeaveDirectory() = 0;
		virtual void unk003( const char *pszPath ) {}
		virtual void unk004() {}
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

	DLL_CLASS_IMPORT bool AddWatch( const char *pszPath, Visitor *pVisitor, size_t nUnknown );
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
