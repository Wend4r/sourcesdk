#ifndef FILESYSTEM_IFILESYSTEM_H
#define FILESYSTEM_IFILESYSTEM_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/threadtools.h"
#include "tier2/tier2.h"

struct FileFindHandle_t
{
	IFileSystem *m_pFileSystem;
	intp m_nFindDataIndex;
};

typedef int WaitForResourcesHandle_t;

enum FileSystemSeek_t
{
	FILESYSTEM_SEEK_HEAD	= SEEK_SET,
	FILESYSTEM_SEEK_CURRENT = SEEK_CUR,
	FILESYSTEM_SEEK_TAIL	= SEEK_END,
};

enum FileWarningLevel_t
{
	// A problem!
	FILESYSTEM_WARNING = -1,

	// Don't print anything
	FILESYSTEM_WARNING_QUIET = 0,

	// On shutdown, report names of files left unclosed
	FILESYSTEM_WARNING_REPORTUNCLOSED,

	// Report number of times a file was opened, closed
	FILESYSTEM_WARNING_REPORTUSAGE,

	// Report all open/close events to console ( !slow! )
	FILESYSTEM_WARNING_REPORTALLACCESSES,

	// Report all open/close/read events to the console ( !slower! )
	FILESYSTEM_WARNING_REPORTALLACCESSES_READ,

	// Report all open/close/read/write events to the console ( !slower! )
	FILESYSTEM_WARNING_REPORTALLACCESSES_READWRITE,

	// Report all open/close/read/write events and all async I/O file events to the console ( !slower(est)! )
	FILESYSTEM_WARNING_REPORTALLACCESSES_ASYNC,

};

// search path filtering
enum PathTypeFilter_t
{
	FILTER_NONE        = 0,	// no filtering, all search path types match
	FILTER_CULLPACK    = 1,	// pack based search paths are culled (maps and zips)
	FILTER_CULLNONPACK = 2,	// non-pack based search paths are culled
	FILTER_CULLLOCALIZED,
	FILTER_CULLLOCALIZED_ANY,
};

enum SearchPathAdd_t
{
	PATH_ADD_TO_HEAD,			// First path searched
	PATH_ADD_TO_TAIL,			// Last path searched
	PATH_ADD_TO_TAIL_ATINDEX,	// First path searched
};

enum SearchPathPriority_t
{
	SEARCH_PATH_PRIORITY_DEFAULT     = 0,
	SEARCH_PATH_PRIORITY_LOOSE_FILES = 1,
	SEARCH_PATH_PRIORITY_VPK         = 2,
};

enum GetSearchPathTypes_t
{
	GET_SEARCH_PATH_NO_PACK_FILES   = 0,
	GET_SEARCH_PATH_NO_AUTO_MOUNTED = 1,
	GET_SEARCH_PATH_ALL             = 2,
};

struct FileSystemStatistics
{
	CInterlockedUInt	nReads,		
						nWrites,		
						nBytesRead,
						nBytesWritten,
						nSeeks;
};

#endif // FILESYSTEM_IFILESYSTEM_H
