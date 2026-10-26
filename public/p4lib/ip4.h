//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======
//
// Purpose: 
//
//=============================================================================

#ifndef IP4_H
#define IP4_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/utlsymbol.h"
#include "tier0/utlsymbollarge.h"
#include "tier0/bufferstring.h"
#include "tier0/utlvector.h"
#include "tier0/utlstring.h"
#include "appframework/iappsystem.h"
#include "tier0/threadtools.h"


//-----------------------------------------------------------------------------
// Current perforce file state
//-----------------------------------------------------------------------------
enum P4FileState_t
{
	P4FILE_UNOPENED = 0,
	P4FILE_OPENED_FOR_ADD,
	P4FILE_OPENED_FOR_EDIT,
	P4FILE_OPENED_FOR_DELETE,
	P4FILE_OPENED_FOR_INTEGRATE,
};


//-----------------------------------------------------------------------------
// Purpose: definition of a file
//-----------------------------------------------------------------------------
struct P4File_t
{
	CUtlSymbolLarge m_sName;			// file name
	CUtlSymbolLarge m_sPath;			// residing folder
	CUtlSymbolLarge m_sDepotFile;		// the name in the depot
	CUtlSymbolLarge m_sClientFile;		// the name on the client in Perforce syntax
	CUtlSymbolLarge m_sLocalFile;		// the name on the client in local syntax
	int m_iHeadRevision;				// head revision number
	int m_iHaveRevision;				// the revision the clientspec has synced locally

	// 7 by default
	int m_nUnk030;

	bool m_bOpenedByOther;				// opened by another user
	bool m_bDir;						// directory
	bool m_bDeleted;					// deleted at the head revision
	bool m_bUnk037;
	P4FileState_t m_eOpenState;			// current change state
	int m_iChangelist;					// changelist current opened in

	// Users that have the file opened, see m_bOpenedByOther
	int m_nOtherUserCount;
	CUtlSymbolLarge m_sOtherUsers[ 8 ];

	uint64 m_nUnk088;
};


//-----------------------------------------------------------------------------
// Purpose: a single revision of a file
//-----------------------------------------------------------------------------
struct P4Revision_t
{
	int m_iChange;		// changelist number
	int m_nYear, m_nMonth, m_nDay;
	int m_nHour, m_nMinute, m_nSecond;

	CUtlSymbolLarge m_sUser;	// submitting user
	CUtlSymbolLarge m_sClient;	// submitting client 
	CUtlSymbolLarge m_Description;
};


//-----------------------------------------------------------------------------
// Purpose: a single clientspec
//-----------------------------------------------------------------------------
struct P4Client_t
{
	CUtlSymbolLarge m_sName;
	CUtlSymbolLarge m_sUser;
	CUtlSymbolLarge m_sHost;		// machine name this client is on
	CUtlSymbolLarge m_sLocalRoot;	// local path
};


//-----------------------------------------------------------------------------
// Purpose: a pending changelist
//-----------------------------------------------------------------------------
struct P4Changelist_t
{
	int m_iChange;					// changelist number
	uint8 m_pad004[ 18 ];
	CUtlSymbol m_sDescription;
};


//-----------------------------------------------------------------------------
// Perforce file type of a file opened for add or edit
//-----------------------------------------------------------------------------
enum PerforceFileType_t : int
{
	// What every caller passes; no shipped backend reads the type
	PERFORCE_FILE_TYPE_DEFAULT = 0,
};


//-----------------------------------------------------------------------------
// The backend IP4 forwards to
//-----------------------------------------------------------------------------
enum P4ImplementationType_t
{
	// CP4Dummy
	P4_IMPLEMENTATION_DUMMY = 0,

	// CP4Mimic, which works on the local files without Perforce
	P4_IMPLEMENTATION_MIMIC = 1,

	// The p4lib_perforce implementation, CP4Dummy without it
	P4_IMPLEMENTATION_PERFORCE = 2,
};


//-----------------------------------------------------------------------------
// Result of IP4::TryOpenFileForEdit
//-----------------------------------------------------------------------------
enum P4TryOpenResult_t : int
{
	P4_TRY_OPEN_FAILED = -1,

	// The file is not in Perforce yet; callers add it later
	P4_TRY_OPEN_NOT_IN_PERFORCE = 0,

	P4_TRY_OPEN_OPENED = 1,

	// The file has to be reopened, see IP4::ReopenFile
	P4_TRY_OPEN_NEEDS_REOPEN = 2,
};


//-----------------------------------------------------------------------------
// Called with the Perforce command ("add", "integ", "copy", ...) and the files it ran on
//-----------------------------------------------------------------------------
typedef void ( *P4CommandCallbackFn_t )( const char *pCommand, int nFileCount, const char **ppFileList );


//-----------------------------------------------------------------------------
// File CRC cache the asset system reads before computing the CRC of a file and fills after
//-----------------------------------------------------------------------------
abstract_class IP4FileCRCCache
{
public:
	virtual ~IP4FileCRCCache() {}

	// Returns false when the cache has no CRC for the file
	virtual bool GetFileCRC( const char *pFullPath, uint32 *pCRC ) = 0;
	virtual void SetFileCRC( const char *pFullPath, uint32 nCRC ) = 0;
};


//-----------------------------------------------------------------------------
// Purpose: Interface to accessing P4 commands
//-----------------------------------------------------------------------------
#define P4_MAX_INPUT_BUFFER_SIZE	16384		// descriptions should be limited to this size!

abstract_class IP4 : public IAppSystem
{
public:
	// False with the dummy and local backends; callers skip every Perforce operation then
	virtual bool IsAvailable() = 0;

	virtual P4ImplementationType_t GetImplementationType() = 0;
	virtual bool SetImplementationType( P4ImplementationType_t nType ) = 0;

	// name of the current clientspec
	virtual P4Client_t &GetActiveClient() = 0;

	// changes the current client
	virtual void SetActiveClient( const char *pClientName ) = 0;

	// Refreshes the current client from p4 settings
	virtual void RefreshActiveClient() = 0;

	// The path the asset browser passes to "p4v.exe -p4vc history" for the file
	virtual void GetHistoryPath( CBufferString *pResult, const char *pFullPath ) = 0;

	// retrieves the list of files in a path
	virtual CUtlVector< P4File_t > &GetFileList( const char *pPath, bool bUnknown ) = 0;

	// returns the list of files opened for edit/integrate/delete (MSVC places the second overload first)
	virtual void GetOpenedFileList( CUtlVector< P4File_t > &fileList, bool bDefaultChangeOnly ) = 0;
	virtual void GetOpenedFileList( const char *pRootDirectory, CUtlVector< P4File_t > &fileList ) = 0;
	virtual void GetOpenedFileListInPath( const char *pPathID, CUtlVector< P4File_t > &fileList ) = 0;

	// retrieves revision history for a file or directory, at most nMaxRevisions entries
	virtual CUtlVector< P4Revision_t > &GetRevisionList( const char *pPath, bool bIsDir, int nMaxRevisions, bool bUnknown ) = 0;

	// returns a list of clientspecs
	virtual CUtlVector< P4Client_t > &GetClientList() = 0;

	// returns the pending changelists of the active client
	virtual CUtlVector< P4Changelist_t > &GetPendingChangelists() = 0;

	// Verbose output of the Perforce operations
	virtual void SetVerbose( bool bVerbose ) = 0;

	// file manipulation; callers pass -1 for nUnknown
	virtual bool OpenFileForAdd( const char *pFullPath, PerforceFileType_t eFileType, int nUnknown ) = 0;
	virtual bool OpenFileForEdit( const char *pFullPath, PerforceFileType_t eFileType, int nUnknown ) = 0;

	// Opens an existing file for edit
	virtual P4TryOpenResult_t TryOpenFileForEdit( const char *pFullPath, PerforceFileType_t eFileType, int nUnknown ) = 0;

	// Only called for files that are in perforce
	virtual bool OpenFileForDelete( const char *pFullPath ) = 0;

	// The local backend copies pSrcPath to pDstPath
	virtual bool OpenFileForMove( const char *pSrcPath, const char *pDstPath, PerforceFileType_t eFileType, int nUnknown ) = 0;

	// default revision is to sync to the head revision
	virtual bool SyncFile( const char *pFullPath, int nRevision = -1 ) = 0;

	// Syncs to "pFullPath@pRevisionSpec" (MSVC places this overload first)
	virtual bool SyncFile( const char *pFullPath, const char *pRevisionSpec ) = 0;

	// submit/revert
	virtual bool SubmitFile( const char *pFullPath, const char *pDescription ) = 0;
	virtual bool RevertFile( const char *pFullPath ) = 0;

	// sets the name of the changelist to open files under, NULL for "Default" changelist
	virtual void SetOpenFileChangeList( const char *pChangeListName ) = 0;

	// file checkin/checkout for multiple files
	virtual bool OpenFilesForAdd( int nCount, const char **ppFullPathList ) = 0;
	virtual bool OpenFilesForEdit( int nCount, const char **ppFullPathList ) = 0;
	virtual bool OpenFilesForDelete( int nCount, const char **ppFullPathList ) = 0;

	// submit/revert for multiple files
	virtual bool SubmitFiles( int nCount, const char **ppFullPathList, const char *pDescription ) = 0;
	virtual bool RevertFiles( int nCount, const char **ppFullPathList ) = 0;

	// Reverts the unchanged files of the changelist set by SetOpenFileChangeList
	virtual bool RevertUnchangedFiles() = 0;

	// Reverts the file if it is opened but unchanged
	virtual bool RevertUnchangedFile( const char *pFullPath ) = 0;

	// Is this file in perforce?
	virtual bool IsFileInPerforce( const char *pFullPath ) = 0;
	virtual bool unk044() = 0;
	virtual bool unk045() = 0;

	// Get the perforce file state
	virtual P4FileState_t GetFileState( const char *pFullPath ) = 0;
	virtual void *unk047() = 0;

	// Gets a string for a symbol
	virtual const char *String( CUtlSymbol s ) const = 0;

	// Placed before the overload above on MSVC. Passed by value in the engine, where the type is not
	// trivially copyable; the reference keeps that calling convention with the SDK type
	virtual const char *String( const CUtlSymbolLarge &s ) const = 0;

	// Return the clientspec and the server port to use for the file, directory or filesystem path ID
	virtual bool GetClientSpecForFile( const char *pFullPath, CBufferString *pClientSpec, CBufferString *pPort ) = 0;
	virtual bool GetClientSpecForDirectory( const char *pFullPathDir, CBufferString *pClientSpec, CBufferString *pPort ) = 0;
	virtual bool GetClientSpecForPath( const char *pPathId, CBufferString *pClientSpec, CBufferString *pPort ) = 0;

	// have we connected? if not, nothing works
	virtual bool IsConnectedToServer( bool bRetry = true ) = 0;
	virtual bool unk054() = 0;
	virtual bool unk055() = 0;

	// Returns file information for a single file
	virtual bool GetFileInfo( const char *pFullPath, P4File_t *pFileInfo ) = 0;

	// retrieves the last error from the last op (which is likely to span multiple lines)
	virtual const char *GetLastError() = 0;

	virtual void GetFileListInChangelist( unsigned int nChangeListNumber, CUtlVector< P4File_t > &fileList ) = 0;

	// Creates a pending changelist and returns its number, negative on failure
	virtual int CreateChangelist( const char *pDescription ) = 0;
	virtual bool SetChangelistDescription( int nChangelist, const char *pDescription ) = 0;

	// Moves an opened file to the changelist (the default one for nullptr); the local backend opens it for edit
	virtual bool ReopenFile( const char *pFullPath, int *pChangelist, PerforceFileType_t eFileType, int nUnknown ) = 0;

	// True while working disconnected
	virtual bool IsWorkingDisconnected() = 0;

	// Opens pDstPath for integrate from pSrcPath; the local backend copies the file and makes the copy writable
	virtual bool IntegrateFile( const char *pSrcPath, const char *pDstPath ) = 0;

	// Opens pDstPath for copy from pSrcPath; the local backend copies the file
	virtual bool CopyFile( const char *pSrcPath, const char *pDstPath ) = 0;

	// Called for every Perforce command that is run
	virtual void SetCommandCallback( P4CommandCallbackFn_t pfnCallback ) = 0;

	// CP4 turns it off before and back on after each GetClientSpecFor* query
	virtual void EnableCommandCallback( bool bEnable ) = 0;

	// Returns an 8-byte static object in the dummy and local backends
	virtual void *unk067() = 0;

	virtual int GetCounterValue( const char *pCounterName ) = 0;
	virtual void *unk069() = 0;
	virtual bool unk070() = 0;
	virtual bool unk071() = 0;
	virtual void unk072() = 0;
	virtual bool unk073() = 0;

	// Returns an empty vector in the dummy and local backends; the element type is unknown
	virtual CUtlVector< void * > unk074() = 0;
	virtual bool unk075() = 0;

	// The caller owns the returned cache and deletes it; nullptr without a Perforce backend
	virtual IP4FileCRCCache *CreateFileCRCCache() = 0;

	// Takes a 16-byte object by value
	virtual bool unk077( void *p ) = 0;

	virtual ~IP4() {}
};

DECLARE_TIER2_INTERFACE( IP4, p4 );

class CP4 : public CBaseAppSystem< IP4 >
{
public:
	// Export of p4lib_perforce, called to create the Perforce implementation
	IP4 *( *m_pfnGetCP4ImplInterface )();

	// Implementation the calls are forwarded to: Perforce, CP4Dummy or CP4Mimic
	IP4 *m_pImplementation;

	// Set by Init when p4lib_perforce exports GetCP4ImplInterface; SetImplementationType
	// falls back to CP4Dummy without it
	bool m_bPerforceAvailable;

	// p4lib_perforce, loaded unless running on a customer machine or disabled by the command line
	PlatModule_t m_hPerforceModule;

	// Thread that ran Init
	ThreadId_t m_nInitThreadId;
};

// Used when Perforce is not available
class CP4Dummy : public CBaseAppSystem< IP4 >
{
};

// Works on the local files, without Perforce
class CP4Mimic : public CBaseAppSystem< IP4 >
{
};


#endif // IP4_H
