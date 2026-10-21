//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//
#ifndef VSTDLIB_IKEYVALUESSYSTEM_H
#define VSTDLIB_IKEYVALUESSYSTEM_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/basetypes.h"
#include "tier0/memstack.h"
#include "tier0/mempool.h"
#include "tier0/threadtools.h"
#include "tier0/utlvector.h"
#include "tier1/utlhashtable.h"
#include "tier0/utlmap.h"

class CUtlCharConversion;
class KeyValues;

DECLARE_POINTER_HANDLE( HTemporaryKeyValueAllocationScope );

class HKeySymbol
{
public:
	HKeySymbol() : nIndex(0xFFFFFF) { }
	HKeySymbol(uint32 idx) : nIndex(idx) { }

	inline uint32 Get() { return nIndex; }

private:
	uint32 nIndex;
};

//-----------------------------------------------------------------------------
// Purpose: Interface to shared data repository for KeyValues (included in vgui_controls.lib)
//			allows for central data storage point of KeyValues symbol table
//-----------------------------------------------------------------------------
class IKeyValuesSystem
{
public:
	virtual ~IKeyValuesSystem() = 0;

	// allocates/frees a KeyValues object from the shared mempool
	virtual KeyValues *AllocKeyValuesMemory() = 0;
	virtual void FreeKeyValuesMemory(KeyValues *pKV) = 0;

	// symbol table access (used for key names)
	virtual HKeySymbol GetSymbolForString( const char *name, bool bCreate = true ) = 0;
	virtual const char *GetStringForSymbol(HKeySymbol symbol) = 0;

	// for debugging, adds KeyValues record into global list so we can track memory leaks
	virtual void AddKeyValuesToMemoryLeakList(void *pMem, HKeySymbol name) = 0;
	virtual void RemoveKeyValuesFromMemoryLeakList(void *pMem) = 0;

	virtual void SetMemoryLeakListContextName( const void *pMem, const char *pszContextName ) = 0;

	// set/get a value for keyvalues resolution symbol
	// e.g.: SetKeyValuesExpressionSymbol( "LOWVIOLENCE", true ) - enables [$LOWVIOLENCE]
	virtual void SetKeyValuesExpressionSymbol( const char *name, bool bValue ) = 0;
	virtual bool GetKeyValuesExpressionSymbol( const char *name ) = 0;

	// symbol table access from code with case-preserving requirements (used for key names)
	virtual HKeySymbol GetSymbolForStringCaseSensitive( HKeySymbol &hCaseInsensitiveSymbol, const char *name, bool bCreate = true ) = 0;
	virtual HKeySymbol GetCaseInsensitiveSymbolFromCaseSensitiveSymbol( HKeySymbol symbol ) = 0;
	
	virtual const char *CopyString( const char * ) = 0;
	virtual void ReleaseStringCopy( const char * ) = 0;
	virtual const wchar_t *CopyWString( const wchar_t * ) = 0;
	virtual void ReleaseWStringCopy( const wchar_t * ) = 0;

	virtual CUtlCharConversion *GetCharacterConversion( bool is_cstring ) = 0;

	virtual HTemporaryKeyValueAllocationScope AllocTemporaryKeyValueAllocationScope() = 0;
	virtual void FreeTemporaryKeyValueAllocationScope( HTemporaryKeyValueAllocationScope hScope ) = 0;

	// Returns the previous scope
	virtual HTemporaryKeyValueAllocationScope SetTemporaryKeyValueAllocationScope( HTemporaryKeyValueAllocationScope hScope ) = 0;

	// Source2Main of engine2 passes true before it starts the engine and false after
	// Plat_OnExecutableExitingMain; tier0 stores it in a member that nothing reads
	virtual void unk101( bool ) = 0;

	// Source2Main of engine2 passes true last, after unk101( false ); empty in tier0
	virtual void unk102( bool ) = 0;

	// engine2 calls it after the engine loop has shut down its systems; empty in tier0
	virtual void unk103() = 0;
};

PLATFORM_INTERFACE IKeyValuesSystem *KeyValuesSystem();

// A new key values system with symbol tables of its own, for KeyValues that should not grow the
// global one
PLATFORM_INTERFACE IKeyValuesSystem *CreateDisposableKeyValuesSystem();

// The shared implementation; CDisposableKeyValuesSystem derives from it without adding members.
class CKeyValuesSystemBase : public IKeyValuesSystem
{
public:
	struct hash_item_t
	{
		int stringIndex;
		hash_item_t *next;
	};

	struct IndexedString_t
	{
		HKeySymbol hCaseInsensitiveSymbol;
		uint32 nStringMemoryOffset;
	};

	struct SharedStringData_t
	{
		uint8 nReferences;
		char stringData[1];
	};

	struct ReferenceMap_t
	{
		CUtlHashtable< SharedStringData_t *, CInterlockedInt > Hash;
		CThreadRWLock_FastRead RWLock;
	};

	struct SharingMap_t
	{
		CUtlHashtable< uint32, SharedStringData_t * > Hash;
		CThreadRWLock_FastRead RWLock;
		ReferenceMap_t OverflowReferences;
		bool bHasHashEverCollided;
	};

	CMemoryStack m_Strings;
	CUtlVector< hash_item_t > m_HashTable;
	CUtlVector< IndexedString_t > m_IndexedStrings;
	CThreadRWLock_FastRead m_Symbol_RWLock;
	CThreadRWLock_FastRead m_ConditionalSymbolTable_RWLock;
	SharingMap_t m_SharedStrings;
};

// The global system that KeyValuesSystem() returns.
class CKeyValuesSystem : public CKeyValuesSystemBase
{
public:
	CUtlMemoryPoolBase *m_pMemPool;
	bool m_bIgnoreMemPool;
	CUtlMap< HKeySymbol, bool, int > m_KvConditionalSymbolTable;
	bool m_bUnk1896; // Only written, by the virtual after SetTemporaryKeyValueAllocationScope
	bool m_bUnk1897;
};

#endif // VSTDLIB_IKEYVALUESSYSTEM_H
