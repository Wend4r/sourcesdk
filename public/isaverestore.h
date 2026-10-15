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
// named members of the current target and arrays hold one table per element
//
//-----------------------------------------------------------------------------

abstract_class ISave : public CMutableRefCounted< CMutableRefCountBase< CRefMT > >
{
public:
	// Starts a member of the current target and makes it the target, bReuseExisting
	// takes a member that was already saved instead of failing with a double save
	virtual void StartBlock( const CKV3MemberName &name, bool bReuseExisting ) = 0;
	virtual void EndBlock() = 0;

	virtual CGameSaveRestoreInfo *GetGameSaveRestoreInfo() = 0;
	virtual CSaveRestoreLoggingInfo *GetSaveRestoreLogging() = 0;
	virtual CSaveRestoreData *unk006() = 0;
	virtual int unk007() = 0;

	// The base tick of the game save restore info
	virtual int unk008() = 0;

	// KV3 transfers skip a field while unk010 returns true, unless the field asks otherwise
	virtual void unk009( bool bUnknown ) = 0;
	virtual bool unk010() = 0;

	virtual bool unk011() = 0;
	virtual bool unk012() = 0;
	virtual bool unk013() = 0;

	// The entity being saved, set by unk016
	virtual CEntityInstance *unk014() = 0;
	virtual CEntityInstance *unk015() = 0;
	virtual void unk016( CEntityInstance *pEntity ) = 0;

	// Index of the resource name in the symbol table of the game save restore info
	virtual int unk017( const CBufferString &sName ) = 0;

	// Index of the entity in the entity table, -1 if it is not saved
	virtual int EntityIndex( const CEntityInstance *pEntity ) = 0;
	virtual int EntityIndex( CEntityHandle hEntity ) = 0;
	virtual int unk020( int nEntityIndex ) = 0;

	// Rebase a time or a tick to the base of the game save restore info
	virtual float WriteTime( float flTime ) = 0;
	virtual int WriteTick( int nTick ) = 0;

	// True when every byte of the data is zero
	virtual bool DataEmpty( const char *pData, int nSize ) = 0;

	// The current target
	virtual KeyValues3 *unk024() = 0;

	// Makes the current target an array of nCount tables, if nCount is above 1
	virtual void unk025( int nCount ) = 0;

	// Like StartBlock, and makes the member an array of nCount tables if nCount is above 1 or bArray is set
	virtual void unk026( const CKV3MemberName &name, int nCount, bool bArray ) = 0;

	// Make element nIndex of the current target the target, unk028 only if the target is an array
	virtual void unk027( int nIndex ) = 0;
	virtual bool unk028( int nIndex ) = 0;

	// End unk028 and unk026
	virtual void unk029() = 0;
	virtual void unk030() = 0;

	// unk031 remembers the current target, unk032 tells whether it is still the target
	virtual KeyValues3 *unk031() = 0;
	virtual bool unk032() = 0;

	virtual CKV3TransferSaveContext *unk033() = 0;
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
	// Makes the member of the current target the target, null when it is missing
	virtual void StartBlock( const CKV3MemberName &name ) = 0;

	// Like StartBlock unless the current target is an array, false when the member is missing
	virtual bool unk003( const CKV3MemberName &name ) = 0;
	virtual void EndBlock() = 0;

	// KV3 transfers skip global fields in global mode
	virtual void SetGlobalMode( int nGlobalMode ) = 0;
	virtual bool GetGlobalMode() = 0;

	virtual CGameSaveRestoreInfo *GetGameSaveRestoreInfo() = 0;
	virtual CSaveRestoreLoggingInfo *GetSaveRestoreLogging() = 0;
	virtual CSaveRestoreData *unk009() = 0;
	virtual int unk010() = 0;

	virtual void unk011( bool bUnknown ) = 0;
	virtual bool unk012() = 0;

	// The entity being restored, set by unk015
	virtual CEntityInstance *unk013() = 0;
	virtual CEntityInstance *unk014() = 0;
	virtual void unk015( CEntityInstance *pEntity ) = 0;

	virtual void *unk016() = 0;

	// The entity of an index in the entity table, unk018 gives its handle and unk019 its entity index
	virtual CEntityInstance *EntityFromIndex( int nIndex ) = 0;
	// MSVC orders these two the other way
#ifdef _WIN32
	virtual int unk019( int nIndex ) = 0;
	virtual CEntityHandle unk018( int nIndex ) = 0;
#else
	virtual CEntityHandle unk018( int nIndex ) = 0;
	virtual int unk019( int nIndex ) = 0;
#endif

	// Rebase a time or a tick from the base of the game save restore info
	virtual float ReadTime( float flTime ) = 0;
	virtual int ReadTick( int nTick ) = 0;

	// The current target
	virtual KeyValues3 *unk022() = 0;

	// Like StartBlock, returns the element count of an array, else whether the member is present
	virtual int unk023( const CKV3MemberName &name ) = 0;

	// Make element nIndex of the current target the target, unk025 only if the target is an array
	virtual void unk024( int nIndex ) = 0;
	virtual bool unk025( int nIndex ) = 0;

	// End unk025 and unk023
	virtual void unk026() = 0;
	virtual void unk027() = 0;

	// unk028 remembers the current target, unk029 tells whether it is still the target
	virtual KeyValues3 *unk028() = 0;
	virtual bool unk029() = 0;

	virtual CKV3TransferLoadContext *unk030() = 0;
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
