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
#include "tier1/utlvector.h"
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
// Purpose: a single revision of a file
//-----------------------------------------------------------------------------
struct P4Revision_t
{
	int m_iChange;		// changelist number
	int m_nYear, m_nMonth, m_nDay;
	int m_nHour, m_nMinute, m_nSecond;

	CUtlSymbol m_sUser;		// submitting user
	CUtlSymbol m_sClient;	// submitting client 
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
	virtual void unk017() = 0;

	virtual CUtlVector< P4File_t > &GetFileList( const char *pPath, bool bUnknown ) = 0;
	virtual void unk019() = 0;
	virtual void unk020() = 0;
	virtual void unk021() = 0;

	// Return persistent containers by reference
	virtual void *unk022() = 0;
	virtual void *unk023() = 0;
	virtual void *unk024() = 0;

	virtual void unk025( bool bUnknown ) = 0;

	// Callers pass ( path, 0 or 1, -1 )
	virtual bool OpenFileForAdd( const char *pFullPath, int nUnknown1, int nUnknown2 ) = 0;
	virtual bool OpenFileForEdit( const char *pFullPath, int nUnknown1, int nUnknown2 ) = 0;

	virtual int unk028( const char *pFullPath, int nUnknown1, int nUnknown2 ) = 0;

	// Only called for files that are in perforce
	virtual bool unk029( const char *pFullPath ) = 0;

	// The local backend copies pSrcPath to pDstPath
	virtual bool unk030( const char *pSrcPath, const char *pDstPath, int nUnknown1, int nUnknown2 ) = 0;
	virtual bool unk031() = 0;

	// Callers pass -1 to sync to the head revision
	virtual bool SyncFile( const char *pFullPath, int nRevision = -1 ) = 0;
	virtual bool unk033() = 0;
	virtual bool unk034() = 0;

	// Sets the changelist description files get opened under
	virtual void SetOpenFileChangeList( const char *pChangeListName ) = 0;

	virtual bool Unk_OpenFilesForAdd( void *p ) = 0;
	virtual bool OpenFilesForEdit( int nCount, const char **ppFullPathList ) = 0;
	virtual bool Unk_OpenFilesForDelete( void *p ) = 0;
	virtual bool unk039() = 0;
	virtual bool unk040() = 0;
	virtual bool unk041() = 0;
	virtual bool unk042( const char *pFullPath ) = 0;

	virtual bool IsFileInPerforce( const char *pFullPath ) = 0;
	virtual bool unk044() = 0;
	virtual bool unk045() = 0;
	virtual void *unk046() = 0;
	virtual void *unk047() = 0;

	// Takes an 8-byte symbol by value
	virtual const char *Unk_String( void *p ) = 0;
	virtual const char *String( CUtlSymbol s ) const = 0;

	virtual bool GetClientSpecForFile( const char *pFullPath, char *pClientSpec, int nMaxLen ) = 0;
	virtual bool GetClientSpecForDirectory( const char *pFullPathDir, char *pClientSpec, int nMaxLen ) = 0;
	virtual bool GetClientSpecForPath( const char *pPathId, char *pClientSpec, int nMaxLen ) = 0;

	virtual bool unk053() = 0;
	virtual bool unk054() = 0;
	virtual bool unk055() = 0;
	virtual bool unk056() = 0;
	virtual void *unk057() = 0;
	virtual void unk058() = 0;
	virtual void *unk059() = 0;
	virtual bool unk060() = 0;

	virtual bool unk061( const char *pFullPath, int nUnknown1, int nUnknown2, int nUnknown3 ) = 0;
	virtual bool unk062() = 0;

	virtual bool unk063( const char *pSrcPath, const char *pDstPath ) = 0;
	virtual bool unk064( const char *pSrcPath, const char *pDstPath ) = 0;

	virtual void unk065() = 0;
	virtual void unk066( bool bUnknown ) = 0;
	virtual void *unk067() = 0;
	virtual void *unk068() = 0;
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

#endif // IP4_H
