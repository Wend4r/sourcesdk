//====== Copyright c 1996-2007, Valve Corporation, All rights reserved. =======//
//
// Purpose: 
//
// $NoKeywords: $
//
//=============================================================================//

#ifndef P4HELPERS_H
#define P4HELPERS_H
#ifdef _WIN32
#pragma once
#endif


#include "tier0/utlstring.h"
#include "tier0/utlvector.h"
#include "tier0/smartptr.h"


enum PerforceFileType_t : int;
enum P4TryOpenResult_t : int;


//
// Class representing file operations, each forwarding to the IP4 method of the same purpose
//
class CP4File
{
public:
	CP4File( char const *szFilename, PerforceFileType_t eFileType, int nUnknown );
	virtual ~CP4File();

public:
	// Opens the file for edit
	virtual bool Edit( void );

	// See IP4::TryOpenFileForEdit
	virtual P4TryOpenResult_t TryOpenFileForEdit();

	// Opens the file for add
	virtual bool Add( void );

	// Opens the file for integrate from pSrcPath
	virtual bool IntegrateFile( const char *pSrcPath );

	// Moves the file to pDstPath
	virtual bool OpenFileForMove( const char *pDstPath );

	// Moves the opened file to the default changelist
	virtual bool ReopenFile();

	// Is the file in perforce?
	virtual bool IsFileInPerforce();

	virtual bool RevertUnchangedFile();

	// Opens the file for delete when it is in perforce
	virtual bool OpenFileForDelete();

	// Collects the submitting users of the last nMaxRevisions revisions, skipping the "script" user
	// unless bIncludeScriptUser is set; returns the resulting count, -1 without IP4
	virtual int GetRevisionUsers( CUtlVector< CUtlString > &users, int nMaxRevisions, bool bIncludeScriptUser );

protected:
	// The filename that this class instance represents
	CUtlString m_sFilename;

	// Passed to the IP4 open methods
	PerforceFileType_t m_eFileType;
	int m_nUnknown;
};

//
// An override of CP4File performing no Perforce interaction
//
class CP4File_Dummy : public CP4File
{
public:
	explicit CP4File_Dummy( char const *szFilename, PerforceFileType_t eFileType, int nUnknown ) : CP4File( szFilename, eFileType, nUnknown ) {}

public:
	virtual bool Edit( void ) { return true; }
	virtual bool Add( void ) { return true; }
	virtual bool IsFileInPerforce() { return false; }
};


//
// Class representing a factory for creating other helper objects
//
class CP4Factory
{
public:
	CP4Factory();
	~CP4Factory();

public:
	// Sets whether dummy objects are created by the factory.
	// Returns the old state of the dummy mode.
	bool SetDummyMode( bool bDummyMode );

public:
	// Sets the name of the changelist to open files under,
	// NULL for "Default" changelist.
	void SetOpenFileChangeList( const char *szChangeListName );

public:
	// Creates a file access object for the given filename.
	CP4File *AccessFile( char const *szFilename ) const;

protected:
	// Whether the factory is in the "dummy mode" and is creating dummy objects
	bool m_bDummyMode;
};

// Default p4 factory
extern CP4Factory *g_p4factory;


//
// CP4AutoEditFile - edits the file upon construction
//
class CP4AutoEditFile
{
public:
	explicit CP4AutoEditFile( char const *szFilename ) : m_spImpl( g_p4factory->AccessFile( szFilename ) ) { m_spImpl->Edit(); }

	CP4File * File() const { return m_spImpl.Get(); }

protected:
	CPlainAutoPtr< CP4File > m_spImpl;
};

//
// CP4AutoAddFile - adds the file upon construction
//
class CP4AutoAddFile
{
public:
	explicit CP4AutoAddFile( char const *szFilename ) : m_spImpl( g_p4factory->AccessFile( szFilename ) ) { m_spImpl->Add(); }

	CP4File * File() const { return m_spImpl.Get(); }

protected:
	CPlainAutoPtr< CP4File > m_spImpl;
};

//
// CP4AutoEditAddFile - edits the file upon construction / adds upon destruction
//
class CP4AutoEditAddFile
{
public:
	explicit CP4AutoEditAddFile( char const *szFilename ) : m_spImpl( g_p4factory->AccessFile( szFilename ) )
	{ 
		m_spImpl->Edit(); 
	}
	~CP4AutoEditAddFile( void ) { m_spImpl->Add(); }

	CP4File * File() const { return m_spImpl.Get(); }

protected:
	CPlainAutoPtr< CP4File > m_spImpl;
};


#endif // #ifndef P4HELPERS_H
