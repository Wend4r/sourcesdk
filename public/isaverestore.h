//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
// $NoKeywords: $
//=============================================================================//

#ifndef ISAVERESTORE_H
#define ISAVERESTORE_H

#include "string_t.h"
#include "datamap.h"
#include "mathlib/vmatrix.h"
#include "tier0/refcount.h"

#if defined( _WIN32 )
#pragma once
#endif

#ifndef CLIENT_DLL
class SINGLE_INHERITANCE CBaseEntity;
#endif

class Vector;
class VMatrix;
struct edict_t;
template< class T > class CHandle;
typedef CHandle<CBaseEntity> EHANDLE;
struct matrix3x4_t;

class CSaveRestoreData;
class CGameSaveRestoreInfo;
class CSaveRestoreLoggingInfo;
class CEntityInstance;
class CEntityHandle;
class CEntityIndex;
class CBufferString;
class KeyValues3;
class CKV3MemberName;
class CKV3TransferSaveContext;
class CKV3TransferLoadContext;

class ISave;
class IRestore;

//-----------------------------------------------------------------------------

#pragma pack(push,1)

struct SaveRestoreRecordHeader_t
{
	unsigned short		size;
	unsigned short		symbol;
};


#pragma pack(pop)

//-----------------------------------------------------------------------------
//
// ISaveRestoreBlockHandler
//
//-----------------------------------------------------------------------------

const int MAX_BLOCK_NAME_LEN = 31;
const int SIZE_BLOCK_NAME_BUF = 31 + 1;

//-------------------------------------

abstract_class ISaveRestoreBlockHandler
{
public:
	virtual const char *GetBlockName() = 0;

	// Called immediately prior to save, generally used to set up any necessary tables.
	// pEntities: an int count followed by a pointer to that many 4-byte entity handles
	// (the entities of the save); the entity handler visits each of them.
	virtual void PreSave( CSaveRestoreData *pSaveData, const void *pEntities ) = 0;
	virtual void Save( ISave *pSave ) = 0;
	// Called after save to allow the writing out of any dictionaries/tables/indexes generated during save
	virtual void WriteSaveHeaders( ISave *pSave ) = 0;
	virtual void PostSave() = 0;

	virtual void PreRestore() = 0;
	// Called prior to Restore()
	virtual void ReadRestoreHeaders( IRestore *pRestore ) = 0;
	// nRestoreArg: a value the block set passes to every handler unchanged; its meaning is not established
	virtual void Restore( IRestore *pRestore, uint32 nRestoreArg, bool fCreatePlayers ) = 0;
	virtual void PostRestore() = 0;
};

//-------------------------------------

abstract_class ISaveRestoreBlockSet : public ISaveRestoreBlockHandler
{
public:
	virtual void AddBlockHandler( ISaveRestoreBlockHandler *pHandler ) = 0;
	virtual void RemoveBlockHandler( ISaveRestoreBlockHandler *pHandler ) = 0;
	// Opens the handler's block in pRestore and calls its Restore unless the block is missing
	virtual void CallBlockHandlerRestore( ISaveRestoreBlockHandler *pHandler, IRestore *pRestore, uint32 nRestoreArg, bool fCreatePlayers ) = 0;
};

extern ISaveRestoreBlockSet *g_pGameSaveRestoreBlockSet;

//-------------------------------------

abstract_class CDefSaveRestoreBlockHandler : public ISaveRestoreBlockHandler
{
	virtual const char *GetBlockName() = 0;

	virtual void PreSave( CSaveRestoreData *pSaveData, const void *pEntities ) {}
	virtual void Save( ISave *pSave ) {}
	virtual void WriteSaveHeaders( ISave *pSave ) {}
	virtual void PostSave() {}

	virtual void PreRestore() {}
	virtual void ReadRestoreHeaders( IRestore *pRestore ) {}
	virtual void Restore( IRestore *pRestore, uint32 nRestoreArg, bool fCreatePlayers ) {}
	virtual void PostRestore() {}
};

//-----------------------------------------------------------------------------
//
// ISave
//
// Builds the KeyValues3 tree of a save through a KV3 transfer context: blocks are
// named members of the current block and arrays hold one table per element
//
//-----------------------------------------------------------------------------

abstract_class ISave : public CMutableRefCounted< CMutableRefCountBase< CRefMT > >
{
public:
	// Starts a member of the current block and makes it the current block, bReuseExisting
	// takes a member that was already saved instead of failing with a double save
	virtual void StartBlock( const CKV3MemberName &name, bool bReuseExisting ) = 0;
	virtual void EndBlock() = 0;

	virtual CGameSaveRestoreInfo *GetGameSaveRestoreInfo() = 0;
	virtual CSaveRestoreLoggingInfo *GetSaveRestoreLogging() = 0;
	virtual CSaveRestoreData *GetSaveRestoreData() = 0;

	// The save file version (129) written after the file id
	virtual int GetSaveVersion() = 0;

	// The base tick of the game save restore info
	virtual int GetBaseTick() = 0;

	// KV3 transfers skip every field in skip fields mode, unless its save behavior is 2,
	// which no game field has; the game never turns it on
	virtual void SetSkipFieldsMode( bool bSkipFields ) = 0;
	virtual bool GetSkipFieldsMode() = 0;

	// The kind of the save: an autosave (also a dangerous one), a quicksave, or a save
	// that carries a level transition
	virtual bool IsAutosave() = 0;
	virtual bool IsQuicksave() = 0;
	virtual bool IsLevelTransition() = 0;

	// The entity being saved, SetCurrentEntity also matches it against the logging filter
	virtual CEntityInstance *GetCurrentEntity() = 0;
	virtual const CEntityInstance *GetCurrentEntity() const = 0;
	virtual void SetCurrentEntity( CEntityInstance *pEntity ) = 0;

	// Index of the resource in the resource list of the game save restore info, added when missing
	virtual int FindOrAddResource( const CBufferString &sName ) = 0;

	// Index of the entity in the entity table, -1 if it is not saved
	virtual int EntityIndex( const CEntityInstance *pEntity ) = 0;
	virtual int EntityIndex( CEntityHandle hEntity ) = 0;
	virtual int EntityIndexFromEntIndex( int nEntIndex ) = 0;

	// Rebase a time or a tick to the base of the game save restore info
	virtual float WriteTime( float flTime ) = 0;
	virtual int WriteTick( int nTick ) = 0;

	// True when every byte of the data is zero
	virtual bool DataEmpty( const char *pData, int nSize ) = 0;

	virtual KeyValues3 *GetCurrentBlock() = 0;

	// Makes the current block an array of nCount tables, if nCount is above 1
	virtual void SetArrayCount( int nCount ) = 0;

	// Like StartBlock, and makes the member an array of nCount tables if nCount is above 1 or bArray is set
	virtual void StartArrayBlock( const CKV3MemberName &name, int nCount, bool bArray ) = 0;

	// Make element nIndex of the current block the current block, TryStartArrayElement
	// only if the current block is an array
	virtual void StartArrayElement( int nIndex ) = 0;
	virtual bool TryStartArrayElement( int nIndex ) = 0;
	virtual void EndArrayElement() = 0;
	virtual void EndArrayBlock() = 0;

	// MarkBlock remembers the current block, IsAtMarkedBlock tells whether it is still current
	virtual KeyValues3 *MarkBlock() = 0;
	virtual bool IsAtMarkedBlock() = 0;

	virtual CKV3TransferSaveContext *GetTransferContext() = 0;
};

//-----------------------------------------------------------------------------
//
// IRestore
//
// Reads the KeyValues3 tree of a save through a KV3 transfer context, mirroring ISave
//
//-----------------------------------------------------------------------------

abstract_class IRestore : public CMutableRefCounted< CMutableRefCountBase< CRefMT > >
{
public:
	// Makes the member of the current block the current block, null when it is missing
	virtual void StartBlock( const CKV3MemberName &name ) = 0;

	// Like StartBlock unless the current block is an array, false when the member is missing
	virtual bool TryStartBlock( const CKV3MemberName &name ) = 0;
	virtual void EndBlock() = 0;

	// KV3 transfers skip global fields in global mode
	virtual void SetGlobalMode( int nGlobalMode ) = 0;
	virtual bool GetGlobalMode() = 0;

	virtual CGameSaveRestoreInfo *GetGameSaveRestoreInfo() = 0;
	virtual CSaveRestoreLoggingInfo *GetSaveRestoreLogging() = 0;
	virtual CSaveRestoreData *GetSaveRestoreData() = 0;

	// The save file version (129) read after the file id
	virtual int GetSaveVersion() = 0;

	// The same flag as ISave::SetSkipFieldsMode and ISave::GetSkipFieldsMode, not read on restore
	virtual void SetSkipFieldsMode( bool bSkipFields ) = 0;
	virtual bool GetSkipFieldsMode() = 0;

	// The entity being restored, SetCurrentEntity also matches it against the logging filter
	virtual CEntityInstance *GetCurrentEntity() = 0;
	virtual const CEntityInstance *GetCurrentEntity() const = 0;
	virtual void SetCurrentEntity( CEntityInstance *pEntity ) = 0;

	// The object read from the "Save Header" member by the entity block handler
	virtual void *GetSaveHeader() = 0;

	// The entity of an index in the entity table, null if there is none
	virtual CEntityInstance *EntityFromIndex( int nIndex ) = 0;

	// The handle bits of that entity, -1 if there is none
	virtual uint32 EntityHandleFromIndex( int nIndex ) = 0;

	// The entity index of that entity
	virtual CEntityIndex EntIndexFromIndex( int nIndex ) = 0;

	// Rebase a time or a tick from the base of the game save restore info
	virtual float ReadTime( float flTime ) = 0;
	virtual int ReadTick( int nTick ) = 0;

	virtual KeyValues3 *GetCurrentBlock() = 0;

	// Like StartBlock, returns the element count of an array, else whether the member is present
	virtual int StartArrayBlock( const CKV3MemberName &name ) = 0;

	// Make element nIndex of the current block the current block, TryStartArrayElement
	// only if the current block is an array
	virtual void StartArrayElement( int nIndex ) = 0;
	virtual bool TryStartArrayElement( int nIndex ) = 0;
	virtual void EndArrayElement() = 0;
	virtual void EndArrayBlock() = 0;

	// MarkBlock remembers the current block, IsAtMarkedBlock tells whether it is still current
	virtual KeyValues3 *MarkBlock() = 0;
	virtual bool IsAtMarkedBlock() = 0;

	virtual CKV3TransferLoadContext *GetTransferContext() = 0;
};

//-----------------------------------------------------------------------------
// Purpose: The operations necessary to save and restore custom types (FIELD_CUSTOM)
//
//

struct SaveRestoreFieldInfo_t
{
	void *			   pField;

	// Note that it is legal for the following two fields to be NULL,
	// though it may be disallowed by implementors of ISaveRestoreOps
	void *			   pOwner;
	typedescription_t *pTypeDesc;
};

abstract_class ISaveRestoreOps
{
public:
	// save data type interface
	virtual void Save( const SaveRestoreFieldInfo_t &fieldInfo, ISave *pSave ) = 0;
	virtual void Restore( const SaveRestoreFieldInfo_t &fieldInfo, IRestore *pRestore ) = 0;

	virtual bool IsEmpty( const SaveRestoreFieldInfo_t &fieldInfo ) = 0;
	virtual void MakeEmpty( const SaveRestoreFieldInfo_t &fieldInfo ) = 0;
	virtual bool Parse( const SaveRestoreFieldInfo_t &fieldInfo, char const* szValue ) = 0;

	//---------------------------------

	void Save( void *pField, ISave *pSave )				{ SaveRestoreFieldInfo_t fieldInfo = { pField, NULL, NULL }; Save( fieldInfo, pSave ); }
	void Restore( void *pField, IRestore *pRestore )	{ SaveRestoreFieldInfo_t fieldInfo = { pField, NULL, NULL }; Restore( fieldInfo, pRestore ); }

	bool IsEmpty( void *pField)							{ SaveRestoreFieldInfo_t fieldInfo = { pField, NULL, NULL }; return IsEmpty( fieldInfo ); }
	void MakeEmpty( void *pField)						{ SaveRestoreFieldInfo_t fieldInfo = { pField, NULL, NULL }; MakeEmpty( fieldInfo ); }
	bool Parse( void *pField, char const *pszValue )	{ SaveRestoreFieldInfo_t fieldInfo = { pField, NULL, NULL }; return Parse( fieldInfo, pszValue ); }
};

//-------------------------------------

class CDefSaveRestoreOps : public ISaveRestoreOps
{
public:
	// save data type interface
	virtual void Save( const SaveRestoreFieldInfo_t &fieldInfo, ISave *pSave ) {}
	virtual void Restore( const SaveRestoreFieldInfo_t &fieldInfo, IRestore *pRestore ) {}

	virtual bool IsEmpty( const SaveRestoreFieldInfo_t &fieldInfo ) { return false; }
	virtual void MakeEmpty( const SaveRestoreFieldInfo_t &fieldInfo ) {}
	virtual bool Parse( const SaveRestoreFieldInfo_t &fieldInfo, char const* szValue ) { return false; }
};


//-----------------------------------------------------------------------------
// Used by ops that deal with pointers
//-----------------------------------------------------------------------------
class CClassPtrSaveRestoreOps : public CDefSaveRestoreOps
{
public:
	virtual bool IsEmpty( const SaveRestoreFieldInfo_t &fieldInfo )
	{
		void **ppClassPtr = (void **)fieldInfo.pField;
		int nObjects = fieldInfo.pTypeDesc->fieldSize;
		for ( int i = 0; i < nObjects; i++ )
		{
			if ( ppClassPtr[i] != NULL )
				return false;
		}
		return true;
	}

	virtual void MakeEmpty( const SaveRestoreFieldInfo_t &fieldInfo )
	{
		memset( fieldInfo.pField, 0, fieldInfo.pTypeDesc->fieldSize * sizeof( void * ) );
	}
};


//-----------------------------------------------------------------------------
//
// IEntity2SaveRestore
//
// Saving and restoring the entities of the game (CEntity2SaveRestore, kept by the
// entity system). No virtual destructor: Release destroys the object. The Stub_
// methods are named by their slot (the same on both platforms); their behaviour is
// noted. Where only the types of the parameters are known, they have no names.
//
//-----------------------------------------------------------------------------

class CUtlString;
class CUtlBuffer;
class ILoadingSpawnGroup;
struct ElapsedTime_t;

abstract_class IEntity2SaveRestore
{
public:
	// Creates the "save/" search path and the save job
	virtual bool Init() = 0;
	virtual void Shutdown() = 0;
	virtual void Release() = 0;
	virtual bool IsRestoring() const = 0;
	virtual bool StreamEntitiesFromFile( int *, void *, void *, void *, void *, void *, int * ) = 0;
	virtual bool LoadAdjacentEnts( int *, void *, void *, void *, void *, void * ) = 0;
	// Records the handle of the entity in a list
	virtual void AddRestoredEntity( CEntityInstance *pEntity ) = 0;
	virtual bool OpenSaveFileAndExtractLevels( const char *pszName, int *, void *, int, int ) = 0;
	// Reads the save data of the loading spawn group ("save/<name>.hl1") into a save memory block
	virtual void ExtractResourcesFromSaveGame( void *, ILoadingSpawnGroup *pLoadingSpawnGroup, void *, void * ) = 0;
	// Restores the entities of a loading spawn group that is a level transition
	virtual void AppendTransitionResources( void *, void *, ILoadingSpawnGroup *pLoadingSpawnGroup, uint32, void * ) = 0;
	// Restores the entities of a loading spawn group that loads its entities from the save
	virtual void Stub_10( void *, void *, ILoadingSpawnGroup *pSpawnGroup, uint32, void * ) = 0;
	// Deletes the "save/*.hl?" files of the current mod
	virtual void ClearSaveDirectory() = 0;
	virtual void SetOneWayTransition( bool bOneWay ) = 0;
	virtual bool GetOneWayTransition() const = 0;
	// "save/<name>.sav" or "save/<dir>/<name>.sav"
	virtual CUtlString CalcSaveGameName( const char *pszName ) = 0;
	// Allocates the save data from the save memory stack (created on first use)
	virtual CSaveRestoreData *SaveInit( int nSize, int nTokenCount ) = 0;
	// Frees the save data and releases the save memory stack
	virtual void SaveFinish( CSaveRestoreData *pSaveData ) = 0;
	virtual void SetCurrentMod( const char *pszMod ) = 0;
	// Named by what it does: returns what SetCurrentMod set
	virtual const char *GetCurrentMod() const = 0;
	// Whether the physics system has the level's aggregate
	virtual bool IsReadyToSave() = 0;
	virtual void *SaveGame_Start( const void *pSaveParams ) = 0;
	// Writes the game header and the spawn groups; no .sav file for a transition
	virtual bool SaveGame_Finalize( void *pSaveData ) = 0;
	virtual bool StreamEntitiesToFile( void *pSaveData, const void *pEntities ) = 0;
	virtual const char *SaveGame_GetLastSaveFile() = 0;
	virtual void SaveGame_ForgetLastSave() = 0;
	// Walks the key values of the spawned entities
	virtual void Stub_25() = 0;
	virtual bool SaveGame( const char *pszName ) = 0;
	// Decides between auto and quick saves ("autosave", "autosavedangerous", "quicksave")
	virtual void Stub_27() = 0;
	// Deletes the oldest saves ("sav", "jpg")
	virtual void AgeSaveList() = 0;
	// Returns a member
	virtual void *Stub_29() = 0;
	virtual void SetMostRecentElapsedTime( const ElapsedTime_t &time ) = 0;
	virtual void AccumulateElapsedTime() = 0;
	// The elapsed seconds; AccumulateElapsedTime calls it
	virtual float Stub_32() = 0;
	// Formats a string from the argument
	virtual CUtlString Stub_33( const char *pszName ) = 0;
	// Returns a member
	virtual void *Stub_34() = 0;
	virtual void StartNewRestore() = 0;
	// "AsyncSave" on the save thread, "MainThread" otherwise
	virtual const char *Stub_36() const = 0;
	// Sets a string member that Stub_38 returns
	virtual void Stub_37( const char *pszValue ) = 0;
	virtual const char *Stub_38() const = 0;
	// Queries on the save state
	virtual bool Stub_39() const = 0;
	virtual bool Stub_40() const = 0;
	virtual bool Stub_41() const = 0;
	virtual void FinishAsyncSave() = 0;
	// Returns a byte member
	virtual bool Stub_43() const = 0;
	// Builds "save/<name>.hl1" for a file name
	virtual CUtlString Stub_44( const char *pszName ) = 0;
};

//-----------------------------------------------------------------------------
// The files of a save held in memory, by name (CSaveRestoreFileData). The names are
// named by what the methods do.
//-----------------------------------------------------------------------------
abstract_class ISaveRestoreFileData
{
public:
	virtual void AddFile( const char *pszName, const void *pData, int nSize ) = 0;
	// Copy the data of a file into buf; Stub_2 terminates it with a null byte
	virtual bool Stub_1( const char *pszName, CUtlBuffer &buf ) = 0;
	virtual bool Stub_2( const char *pszName, CUtlBuffer &buf ) = 0;
	virtual void RemoveAllFiles() = 0;
	virtual int GetFileCount() const = 0;
	// Logs every file with its size
	virtual void PrintFiles() = 0;
	virtual bool HasFile( const char *pszName ) = 0;
	virtual ~ISaveRestoreFileData() {}
};

//=============================================================================

#endif // ISAVERESTORE_H
