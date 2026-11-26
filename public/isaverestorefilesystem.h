//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The file system of saved games.
//
//=============================================================================//

#ifndef ISAVERESTOREFILESYSTEM_H
#define ISAVERESTOREFILESYSTEM_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "filesystem.h"

//-----------------------------------------------------------------------------
// The file system of saved games (CSaveRestoreFileSystemPassthrough passes everything to the file system). No
// virtual destructor.
//-----------------------------------------------------------------------------
abstract_class ISaveRestoreFileSystem
{
public:
	virtual FileHandle_t Open( const char *pFullName, const char *pOptions, const char *pathID ) = 0;
	virtual void Close( FileHandle_t hFile ) = 0;
	virtual int Read( void *pOutput, int size, FileHandle_t hFile ) = 0;
	virtual int Write( const void *pInput, int size, FileHandle_t hFile ) = 0;
	virtual void Seek( FileHandle_t hFile, int pos, FileSystemSeek_t method ) = 0;
	virtual unsigned int Tell( FileHandle_t hFile ) = 0;
	virtual unsigned int Size( FileHandle_t hFile ) = 0;
	virtual unsigned int Size( const char *pFileName, const char *pPathID ) = 0;
	virtual bool FileExists( const char *pFileName, const char *pPathID ) = 0;
	virtual bool RenameFile( const char *pOldPath, const char *pNewPath, const char *pathID ) = 0;
	virtual void RemoveFile( const char *pRelativePath, const char *pathID ) = 0;
	virtual void AsyncFinishAllWrites() = 0;
	virtual void AsyncRelease( FSAsyncControl_t hControl ) = 0;
	virtual FSAsyncStatus_t AsyncWrite( const char *pFileName, const void *pSrc, int nSrcBytes, bool bFreeMemory, bool bAppend, FSAsyncControl_t *pControl ) = 0;
	virtual FSAsyncStatus_t AsyncFinish( FSAsyncControl_t hControl, bool wait ) = 0;
	virtual FSAsyncStatus_t AsyncAppend( const char *pFileName, const void *pSrc, int nSrcBytes, bool bFreeMemory, FSAsyncControl_t *pControl ) = 0;
	virtual FSAsyncStatus_t AsyncAppendFile( const char *pDestFileName, const char *pSrcFileName, FSAsyncControl_t *pControl ) = 0;
	virtual void DirectoryCopy( const char *pPath, const char *pDestFileName, bool bIsXSave, const char *pPathID ) = 0;
};

#endif // ISAVERESTOREFILESYSTEM_H
