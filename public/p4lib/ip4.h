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
#include "tier0/utlvector.h"
#include "tier0/utlstring.h"
#include "appframework/iappsystem.h"


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
	CUtlSymbol m_sName;			// file name
	CUtlSymbol m_sPath;			// residing folder
	CUtlSymbol m_sDepotFile;	// the name in the depot
	CUtlSymbol m_sClientFile;	// the name on the client in Perforce syntax
	CUtlSymbol m_sLocalFile;	// the name on the client in local syntax
	int m_iHeadRevision;		// head revision number
	int m_iHaveRevision;		// the revision the clientspec has synced locally
	bool m_bOpenedByOther;		// opened by another user
	bool m_bDir;				// directory
	bool m_bDeleted;			// deleted
	P4FileState_t m_eOpenState;	// current change state
	int m_iChangelist;			// changelist current opened in
};


//-----------------------------------------------------------------------------
// Purpose: a single revision of a file ( 56 bytes in CS2: callers read the
// submitting user as a string at offset 32 )
//-----------------------------------------------------------------------------
struct P4Revision_t
{
	int m_iChange;		// changelist number
	int m_nYear, m_nMonth, m_nDay;
	int m_nHour, m_nMinute, m_nSecond;

	CUtlSymbolLarge m_sUser;	// submitting user
	CUtlSymbolLarge m_sClient;	// submitting client
	CUtlString m_Description;
};


//-----------------------------------------------------------------------------
// Purpose: a single clientspec
//-----------------------------------------------------------------------------
struct P4Client_t
{
	CUtlSymbol m_sName;
	CUtlSymbol m_sUser;
	CUtlSymbol m_sHost;			// machine name this client is on
	CUtlSymbol m_sLocalRoot;	// local path
};


//-----------------------------------------------------------------------------
// Purpose: Interface to accessing P4 commands
//
// CP4 forwards every call to a backend: CP4Dummy ( type 0 ), CP4Mimic ( type 1,
// plain file operations ) or the Perforce backend of p4lib_perforce.dll ( type 2,
// GetCP4ImplInterface ), which CS2 does not ship. Names come from the Source 1
// IP4 where the slot position and return type match, from the callers in
// resourcecompiler.dll / toolframework2.dll ( CP4File ) and from CP4Mimic; slots
// marked "presumed" have no implementation or caller in CS2 that shows what they
// do, see the tools report.
// nOpenFlags / nChangelist: callers pass 0 or 1 and -1 ( default changelist ).
//-----------------------------------------------------------------------------
#define P4_MAX_INPUT_BUFFER_SIZE	16384		// descriptions should be limited to this size!

abstract_class IP4 : public IAppSystem
{
public:
	virtual bool IsConnectedToServer() = 0;

	virtual int GetImplementationType() = 0;

	// nType: 0 dummy, 1 mimic, 2 perforce
	virtual bool SetImplementationType( int nType ) = 0;

	virtual P4Client_t &GetActiveClient() = 0;
	virtual void SetActiveClient( const char *pClientName ) = 0;
	virtual void RefreshActiveClient() = 0;

	// Presumed: the Source 1 path translator at this position
	virtual void GetDepotFilePath( char *pDepotFilePath, const char *pFileSpec, int nSize ) = 0;

	virtual CUtlVector< P4File_t > &GetFileList( const char *pPath, bool bRecursive ) = 0;

	// Presumed: the Source 1 opened-file queries at these positions
	virtual void GetOpenedFileList( CUtlVector< P4File_t > &fileList, bool bDefaultChangeOnly ) = 0;
	virtual void GetOpenedFileListInDirectory( const char *pRootDirectory, CUtlVector< P4File_t > &fileList ) = 0;
	virtual void GetOpenedFileListInPath( const char *pPathID, CUtlVector< P4File_t > &fileList ) = 0;

	// Retrieves revision history for a file or directory
	virtual CUtlVector< P4Revision_t > &GetRevisionList( const char *pPath, bool bIsDir ) = 0;

	// Returns a list of clientspecs
	virtual CUtlVector< P4Client_t > &GetClientList() = 0;

	// A third persistent container returned by reference ( empty in the dummy and mimic )
	virtual void *GetPersistentList() = 0;

	// resourcecompiler passes its -v switch
	virtual void SetVerbose( bool bVerbose ) = 0;

	// File manipulation
	virtual bool OpenFileForAdd( const char *pFullPath, int nOpenFlags, int nChangelist ) = 0;
	virtual bool OpenFileForEdit( const char *pFullPath, int nOpenFlags, int nChangelist ) = 0;

	// Named by what CP4Mimic does: 1 opened for edit ( file exists ), 0 for add ( it does not ), -1 failed
	virtual int OpenFileForEditOrAdd( const char *pFullPath, int nOpenFlags, int nChangelist ) = 0;

	// CP4File calls it only for files that are in perforce
	virtual bool OpenFileForDelete( const char *pFullPath ) = 0;

	// Named by what CP4Mimic does: copies pSrcPath to the new file pDstPath
	virtual bool BranchFile( const char *pSrcPath, const char *pDstPath, int nOpenFlags, int nChangelist ) = 0;

	// No caller or implementation in CS2 shows what it does ( true in the dummy and mimic )
	virtual bool PerformFileOperation() = 0;

	// Callers pass -1 to sync to the head revision
	virtual bool SyncFile( const char *pFullPath, int nRevision = -1 ) = 0;

	// Presumed: the Source 1 submit / revert at these positions
	virtual bool SubmitFile() = 0;
	virtual bool RevertFile() = 0;

	// Sets the changelist description files get opened under
	virtual void SetOpenFileChangeList( const char *pChangeListName ) = 0;

	// File checkin/checkout for multiple files ( add / delete / submit / revert presumed from Source 1 )
	virtual bool OpenFilesForAdd( int nCount, const char **ppFullPathList ) = 0;
	virtual bool OpenFilesForEdit( int nCount, const char **ppFullPathList ) = 0;
	virtual bool OpenFilesForDelete( int nCount, const char **ppFullPathList ) = 0;
	virtual bool SubmitFiles( int nCount, const char **ppFullPathList, const char *pDescription ) = 0;
	virtual bool RevertFiles( int nCount, const char **ppFullPathList ) = 0;

	// resourcecompiler calls it after SetOpenFileChangeList unless -norevert is given
	virtual bool RevertUnchangedFiles() = 0;

	// Presumed: the single-file form of RevertUnchangedFiles ( CP4File slot 8 )
	virtual bool RevertFileIfUnchanged( const char *pFullPath ) = 0;

	virtual bool IsFileInPerforce( const char *pFullPath ) = 0;

	// No caller or implementation in CS2 shows what these do ( false in the dummy )
	virtual bool QueryFileFlag() = 0;
	virtual bool QueryFileFlag2() = 0;

	// Presumed: the Source 1 file state and depot root at these positions ( 0 in the dummy )
	virtual P4FileState_t GetFileState() = 0;
	virtual const char *GetDepotRoot() = 0;

	// Gets a string for a symbol; MSVC orders the overloads in reverse ( the CUtlSymbolLarge one comes first )
	virtual const char *String( CUtlSymbol s ) const = 0;
	virtual const char *String( CUtlSymbolLarge s ) = 0;

	virtual bool GetClientSpecForFile( const char *pFullPath, char *pClientSpec, int nMaxLen ) = 0;
	virtual bool GetClientSpecForDirectory( const char *pFullPathDir, char *pClientSpec, int nMaxLen ) = 0;
	virtual bool GetClientSpecForPath( const char *pPathId, char *pClientSpec, int nMaxLen ) = 0;

	// Presumed: the Source 1 file info query, then queries that are false in the dummy
	virtual bool GetFileInfo() = 0;
	virtual bool QueryClientSpecFlag() = 0;
	virtual bool QueryClientSpecFlag2() = 0;
	virtual bool QueryClientSpecFlag3() = 0;

	// Presumed: the Source 1 last error / changelist file list at these positions
	virtual const char *GetLastError() = 0;
	virtual void GetFileListInChangelist() = 0;

	// Presumed: a pointer getter ( NULL in the dummy ) and a query ( false in the dummy )
	virtual void *GetFileListUsingClientSpec() = 0;
	virtual bool QueryChangeListFlag() = 0;

	// Named by how resourcecompiler uses it ( CP4File slot 6 ): called before OpenFileForEdit when the
	// compile context asks for it; CP4Mimic makes the file writable
	virtual bool PrepareFileForEdit( const char *pFullPath, int nMode, int nOpenFlags, int nChangelist ) = 0;

	// No caller or implementation in CS2 shows what it does ( false in the dummy )
	virtual bool QueryChangeListFlag2() = 0;

	// Named by what CP4Mimic does: copy pSrcPath over pDstPath ( and make it writable ), plain copy
	virtual bool CopyFileForEdit( const char *pSrcPath, const char *pDstPath ) = 0;
	virtual bool CopyLocalFile( const char *pSrcPath, const char *pDstPath ) = 0;

	// No caller or implementation in CS2 shows what it does ( empty in the dummy )
	virtual void ResetClientState() = 0;

	// Named by how CP4 uses it: switched off around the GetClientSpecFor* queries
	virtual void SetStateRefreshEnabled( bool bEnabled ) = 0;

	// Presumed: a persistent object returned by reference ( static in the dummy and mimic )
	virtual void *GetPersistentObject() = 0;

	// toolframework2 reads the "tools_keepalive" counter to keep the connection alive
	virtual int GetCounterValue( const char *pCounterName ) = 0;

	// No caller or implementation in CS2 shows what these do; named by what the dummy backend returns
	// ( 0 / false / nothing )
	virtual int GetServerValue() = 0;
	virtual bool QueryServerFlag() = 0;
	virtual bool QueryClientFlag() = 0;
	virtual void ResetServerState() = 0;
	virtual bool QueryOperationFlag() = 0;

	// Returns a 24-byte CUtlVector by value through pResult ( the dummy returns it empty )
	virtual void *GetResultVector( void *pResult ) = 0;

	// No caller or implementation in CS2 shows what these do; named by what the dummy backend returns
	virtual bool QueryResultFlag() = 0;
	virtual void *GetServerObject() = 0;

	// Takes a 16-byte object by value ( a copy passed by pointer )
	virtual bool SetServerObject( void *pValue ) = 0;

	virtual ~IP4() {}
};

DECLARE_TIER2_INTERFACE( IP4, p4 );

#endif // IP4_H
