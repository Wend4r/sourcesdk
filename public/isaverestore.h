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

	virtual void PreSave( CSaveRestoreData * ) = 0;					// Called immediately prior to save, generally used to set up any necessary tables
	virtual void Save( ISave * ) = 0;
	virtual void WriteSaveHeaders( ISave * ) = 0;					// Called after save to allow the writing out of any dictionaries/tables/indexes generated during save
	virtual void PostSave() = 0;
	
	virtual void PreRestore() = 0;
	virtual void ReadRestoreHeaders( IRestore * ) = 0;				// Called prior to Restore()
	virtual void Restore( IRestore *, bool fCreatePlayers ) = 0;
	virtual void PostRestore() = 0;
};

//-------------------------------------

abstract_class ISaveRestoreBlockSet : public ISaveRestoreBlockHandler
{
public:
	virtual void AddBlockHandler( ISaveRestoreBlockHandler *pHandler ) = 0;
	virtual void RemoveBlockHandler( ISaveRestoreBlockHandler *pHandler ) = 0;
	virtual void CallBlockHandlerRestore( ISaveRestoreBlockHandler *pHandler, int baseFilePos, IRestore *pRestore, bool fCreatePlayers ) = 0;
};

extern ISaveRestoreBlockSet *g_pGameSaveRestoreBlockSet;

//-------------------------------------

abstract_class CDefSaveRestoreBlockHandler : public ISaveRestoreBlockHandler
{
	virtual const char *GetBlockName() = 0;

	virtual void PreSave( CSaveRestoreData * ) {}
	virtual void Save( ISave * ) {}
	virtual void WriteSaveHeaders( ISave * ) {}
	virtual void PostSave() {}
	
	virtual void PreRestore() {}
	virtual void ReadRestoreHeaders( IRestore * ) {}
	virtual void Restore( IRestore *, bool fCreatePlayers ) {}
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


//=============================================================================

#endif // ISAVERESTORE_H
