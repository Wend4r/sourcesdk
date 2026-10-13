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
	bool m_bUnk035;
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
	CUtlSymbol m_sName;
	CUtlSymbol m_sUser;
	CUtlSymbol m_sHost;			// machine name this client is on
	CUtlSymbol m_sLocalRoot;	// local path
};


//-----------------------------------------------------------------------------
// Purpose: Interface to accessing P4 commands
//-----------------------------------------------------------------------------
#define P4_MAX_INPUT_BUFFER_SIZE	16384		// descriptions should be limited to this size!

abstract_class IP4 : public IAppSystem
{
public:
	virtual bool IsConnectedToServer() = 0;

	virtual int GetImplementationType() = 0;

	virtual bool SetImplementationType( int nType ) = 0;

	virtual P4Client_t &GetActiveClient() = 0;
	virtual void SetActiveClient( const char *pClientName ) = 0;
	virtual void RefreshActiveClient() = 0;

	// Translates the path; the result is shown with p4v history
	virtual void unk017( CBufferString *pResult, const char *pFullPath ) = 0;

	virtual CUtlVector< P4File_t > &GetFileList( const char *pPath, bool bUnknown ) = 0;

	// returns the list of files opened for edit/integrate/delete (MSVC places the second overload first)
	virtual void GetOpenedFileList( CUtlVector< P4File_t > &fileList, bool bDefaultChangeOnly ) = 0;
	virtual void GetOpenedFileList( const char *pRootDirectory, CUtlVector< P4File_t > &fileList ) = 0;
	virtual void GetOpenedFileListInPath( const char *pPathID, CUtlVector< P4File_t > &fileList ) = 0;

	// retrieves revision history for a file or directory
	// Callers pass 50 or a count for nUnknown1
	virtual CUtlVector< P4Revision_t > &GetRevisionList( const char *pPath, bool bIsDir, int nUnknown1, bool bUnknown2 ) = 0;

	// Return persistent containers by reference
	virtual void *unk023() = 0;

	// Container of 24-byte changelist entries: the number at +0, the description as a CUtlSymbol at +22
	virtual void *unk024() = 0;

	virtual void unk025( bool bUnknown ) = 0;

	// Callers pass ( path, 0 or 1, -1 )
	virtual bool OpenFileForAdd( const char *pFullPath, int nUnknown1, int nUnknown2 ) = 0;
	virtual bool OpenFileForEdit( const char *pFullPath, int nUnknown1, int nUnknown2 ) = 0;

	virtual int unk028( const char *pFullPath, int nUnknown1, int nUnknown2 ) = 0;

	// Only called for files that are in perforce
	virtual bool OpenFileForDelete( const char *pFullPath ) = 0;

	// Callers open a file for move with it; the local backend copies pSrcPath to pDstPath
	virtual bool unk030( const char *pSrcPath, const char *pDstPath, int nUnknown1, int nUnknown2 ) = 0;
	virtual bool unk031() = 0;

	// Callers pass -1 to sync to the head revision
	virtual bool SyncFile( const char *pFullPath, int nRevision = -1 ) = 0;

	// submit/revert
	virtual bool SubmitFile( const char *pFullPath, const char *pDescription ) = 0;
	virtual bool RevertFile( const char *pFullPath ) = 0;

	// Sets the changelist description files get opened under
	virtual void SetOpenFileChangeList( const char *pChangeListName ) = 0;

	// file checkin/checkout for multiple files
	virtual bool OpenFilesForAdd( int nCount, const char **ppFullPathList ) = 0;
	virtual bool OpenFilesForEdit( int nCount, const char **ppFullPathList ) = 0;
	virtual bool OpenFilesForDelete( int nCount, const char **ppFullPathList ) = 0;

	// submit/revert for multiple files
	virtual bool SubmitFiles( int nCount, const char **ppFullPathList, const char *pDescription ) = 0;
	virtual bool RevertFiles( int nCount, const char **ppFullPathList ) = 0;

	virtual bool unk041() = 0;

	// Used to revert the files that are unchanged
	virtual bool unk042( const char *pFullPath ) = 0;

	virtual bool IsFileInPerforce( const char *pFullPath ) = 0;
	virtual bool unk044() = 0;
	virtual bool unk045( const char *pDepotPath ) = 0;

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

	// False shows the server as disconnected
	virtual bool unk053() = 0;
	virtual bool unk054() = 0;
	virtual bool unk055() = 0;
	// Returns file information for a single file
	virtual bool GetFileInfo( const char *pFullPath, P4File_t *pFileInfo ) = 0;

	// Retrieves the last error from the last op (which is likely to span multiple lines)
	virtual const char *GetLastError() = 0;
	virtual void unk058() = 0;

	// Creates a changelist and returns its number, negative on failure
	virtual int unk059( const char *pDescription ) = 0;

	// Sets the description of the changelist
	virtual bool unk060( int nChangelist, const char *pDescription ) = 0;

	// Opens the file in the changelist; the local backend opens it for edit
	virtual bool unk061( const char *pFullPath, int *pChangelist, int nUnknown1, int nUnknown2 ) = 0;

	// True while working disconnected
	virtual bool unk062() = 0;

	virtual bool unk063( const char *pSrcPath, const char *pDstPath ) = 0;
	virtual bool unk064( const char *pSrcPath, const char *pDstPath ) = 0;

	virtual void unk065() = 0;
	virtual void unk066( bool bUnknown ) = 0;
	virtual void *unk067() = 0;

	virtual int GetCounterValue( const char *pCounterName ) = 0;
	virtual void *unk069() = 0;
	virtual bool unk070() = 0;
	virtual bool unk071() = 0;
	virtual void unk072() = 0;
	virtual bool unk073() = 0;

	// Returns a 24-byte vector by value through pResult
	virtual void unk074( void *pResult ) = 0;
	virtual bool unk075() = 0;
	virtual void *unk076() = 0;

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
	IP4 *m_pUnk010;

	bool m_bUnk018;
	PlatModule_t m_hUnk020;
	ThreadId_t m_nUnk028;
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
