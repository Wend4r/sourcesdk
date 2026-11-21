//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Loaders of the game's .vdata files: they keep the vdata resources,
//          listen for their reloads and turn each entry into an object of the
//          vdata class (CDecalGroupVData, CExplosionTypeData, CNoiseStreamData).
//
//=============================================================================//

#ifndef VDATAFILEMANAGER_H
#define VDATAFILEMANAGER_H
#ifdef _WIN32
#pragma once
#endif

#include "resourcesystem/ivdataresourcelistener.h"

#include <tier0/globalsymbol.h>
#include <tier0/utlhashtable.h>
#include <tier0/utlmap.h>
#include <tier0/utlvector.h>

class KeyValues3;

// Named by what it holds: the description of the entry being loaded that the manager passes to
// CreateVData / OnVDataCreated (a polymorphic stack object: entry name, its symbol, its index,
// the file name). Its layout is not declared.
class VDataInitInfo_t;

//-----------------------------------------------------------------------------
// 40 bytes on both platforms. The class names come from RTTI; no source names the members or the
// virtual methods, they are named by what the code does. The table holds no destructor on either
// platform.
//-----------------------------------------------------------------------------
class CBaseVDataFileManager : public IVDataResourceListener
{
public:
	// Calls Rebuild
	virtual void OnVDataReloaded( ResourceHandle_t hResource ) override;

	// Unregisters the manager from every file, releases the files and empties m_VDataFiles (the
	// typed managers also empty their entries)
	virtual void RemoveAllFiles();
	// Empty in every manager of the server and the client
	virtual void EmptyHook();
	// Builds the entries out of every file (pure here); called by AddVDataFile and on reloads
	virtual void Rebuild() = 0;

	// RemoveAllFiles, then registers the manager as a listener of the file, keeps a reference to it
	// in m_VDataFiles and calls Rebuild (non-virtual)
	void AddVDataFile( ResourceHandle_t hFile );

public:
	// The vdata files this manager loaded (each holds a reference to its resource)
	CUtlVector< ResourceHandle_t > m_VDataFiles;
	// Incremented by every Rebuild
	int m_nRebuildCount;
};

//-----------------------------------------------------------------------------
// 120 bytes on both platforms: the entries of the files by name. Rebuild gives each new entry
// name the next index, creates
// the object with CreateVData and keeps it when OnVDataCreated accepts it.
//-----------------------------------------------------------------------------
template < class T >
class CBaseTypesafeVDataFileManager : public CBaseVDataFileManager
{
public:
	// Also empties m_EntryIndices and m_Entries
	virtual void RemoveAllFiles() override;
	virtual void Rebuild() override;

	// Called with every object CreateVData made; an object it rejects is deleted. Returns true here
	virtual bool OnVDataCreated( T *pData, const VDataInitInfo_t &info );
	// Makes the object of one entry out of its KeyValues3 (pure here)
	virtual T *CreateVData( const KeyValues3 *pKV, const VDataInitInfo_t &info ) = 0;

public:
	// The index of each entry by its name
	CUtlHashtable< CGlobalSymbol, int > m_EntryIndices;
	// The objects by index
	CUtlMap< int, T *, int > m_Entries;
	// The index the next new entry name gets
	int m_nNextEntryIndex;
};

//-----------------------------------------------------------------------------
// Loads the objects with the schema (KV3 transfer of the vdata class); same size as its base.
//-----------------------------------------------------------------------------
template < class T >
class CVDataFileManager : public CBaseTypesafeVDataFileManager< T >
{
public:
	virtual T *CreateVData( const KeyValues3 *pKV, const VDataInitInfo_t &info ) override;

	// Called when the transfer of an entry fails, with the entry name and the error text; empty here
	virtual void OnVDataLoadError( const char *pszEntryName, const KeyValues3 *pKV, const char *pszError ) {}
};

#endif // VDATAFILEMANAGER_H
