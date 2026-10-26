#ifndef SAVERESTOREFILEDATA_H
#define SAVERESTOREFILEDATA_H

#pragma once

#include "tier0/logging.h"
#include "tier0/utlmap.h"
#include "tier0/utlvector.h"

class CUtlBuffer;

// Named in-memory files of a save ("save/<map>.hl1"), handed to a loading spawn group
abstract_class ISaveRestoreFileData
{
public:
	// Copies the data into a new file, or replaces the data of an existing one
	virtual void AddFile( const char *pszFileName, const void *pData, int nSize ) = 0;

	// Copy the data of the file into the output; false when there is no such file
	virtual bool GetFile( const char *pszFileName, CUtlVector< uint8 > &data ) const = 0;
	virtual bool GetFile( const char *pszFileName, CUtlBuffer &buf ) const = 0;

	virtual void RemoveAllFiles() = 0;
	virtual int GetFileCount() const = 0;

	// Logs "save file %d: %32.32s %d bytes" per file
	virtual void PrintFiles( LoggingChannelID_t channelID ) const = 0;

	virtual bool HasFile( const char *pszFileName ) const = 0;

	virtual ~ISaveRestoreFileData() {}
};

class CSaveRestoreFileData : public ISaveRestoreFileData
{
public:
	virtual void AddFile( const char *pszFileName, const void *pData, int nSize ) override;
	virtual bool GetFile( const char *pszFileName, CUtlVector< uint8 > &data ) const override;
	virtual bool GetFile( const char *pszFileName, CUtlBuffer &buf ) const override;
	virtual void RemoveAllFiles() override;
	virtual int GetFileCount() const override;
	virtual void PrintFiles( LoggingChannelID_t channelID ) const override;
	virtual bool HasFile( const char *pszFileName ) const override;
	virtual ~CSaveRestoreFileData();

	// File name (owned copy) to its data
	CUtlOrderedMap< const char *, CUtlVector< uint8 > *, CDefFastCaselessStringLess > m_Files;
};

#endif // SAVERESTOREFILEDATA_H
