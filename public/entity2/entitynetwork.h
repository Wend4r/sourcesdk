#ifndef ENTITYNETWORK_H
#define ENTITYNETWORK_H

#if _WIN32
#pragma once
#endif

#include "tier1/utlleanvector.h"
#include "tier1/utlmap.h"
#include "tier1/utlvector.h"
#include "tier0/threadtools.h"
#include "entityidentity.h"
#include "entitytypes.h"
#include "ientitylistener.h"

#include "spawngrouptypes.h"

class ServerClass;
class ClientClass;
class CNetworkTransmitComponent;
class CChangeInfoAccessor;
class CEntityInstancePolymorphicMetadataHelper;

struct ChangeAccessorFieldPathIndex_t
{
	ChangeAccessorFieldPathIndex_t() { m_Value = -1; }
	ChangeAccessorFieldPathIndex_t( int32 value ) { m_Value = value; }

	ChangeAccessorFieldPathIndex_t &operator=( int32 value ) { m_Value = value; return *this; }

	int32 m_Value;
};

class CNetworkVarChainer : public CEntityOwnerPtr
{
public:
	struct ChainUpdatePropagationLL_t
	{
		ChainUpdatePropagationLL_t *pNext;
		CUtlDelegate< void( const CNetworkVarChainer & ) > updateDelegate;
	};

	CUtlVector< ChainUpdatePropagationLL_t > m_PropagationChain;
	ChangeAccessorFieldPathIndex_t m_PathIndex;

	// When false, NetworkStateChanged calls are no-ops
	bool m_bNetworkingEnabled;
};

struct NetworkStateChanged_t
{
	NetworkStateChanged_t() : m_eChangeType( NETWORK_STATE_CHANGE_FIELDS ), m_Unk48( -1 ), m_nArrayIndex( -1 ), m_nPathIndex( ChangeAccessorFieldPathIndex_t() ), m_bChainedPath( 0 ) { }
	explicit NetworkStateChanged_t( bool bFullChanged ) : m_eChangeType( bFullChanged ? NETWORK_STATE_CHANGE_FULL : NETWORK_STATE_CHANGE_FIELDS ), m_Unk48( -1 ), m_nArrayIndex( -1 ), m_nPathIndex( ChangeAccessorFieldPathIndex_t() ), m_bChainedPath( 0 ) {}

	// nLocalOffset is the flattened field offset
	//		calculated taking into account embedded structures
	//		if PathIndex is specified, then the offset must start from the last object in the chain
	// nArrayIndex is the index of the array element 
	//		if the field is a CNetworkUtlVectorBase, otherwise pass -1
	// nPathIndex is the value to specify 
	//		if the path to the field goes through one or more pointers, otherwise pass -1
	// 		this value is usually a member of the CNetworkVarChainer and belongs to the last object in the chain
	NetworkStateChanged_t( uint32 nLocalOffset, int32 nArrayIndex = -1, ChangeAccessorFieldPathIndex_t nPathIndex = ChangeAccessorFieldPathIndex_t() )
		: m_eChangeType( NETWORK_STATE_CHANGE_FIELDS ), m_LocalOffsets{ nLocalOffset }, m_Unk48( -1 ), m_nArrayIndex( nArrayIndex ), m_nPathIndex(nPathIndex), m_bChainedPath(0) { }
	NetworkStateChanged_t( CUtlVector< uint32 > vecLocalOffsets, int32 nArrayIndex = -1, ChangeAccessorFieldPathIndex_t nPathIndex = ChangeAccessorFieldPathIndex_t() )
		: m_eChangeType(NETWORK_STATE_CHANGE_FIELDS), m_LocalOffsets( Move( vecLocalOffsets ) ), m_Unk48( -1 ), m_nArrayIndex( nArrayIndex ), m_nPathIndex( nPathIndex ), m_bChainedPath( 1 ) { }

	// NETWORK_STATE_CHANGE_FULL marks the whole entity changed instead of the offsets
	NetworkStateChangeType_t m_eChangeType;

	// Local byte offsets of the changed fields
	CUtlVector< uint32 > m_LocalOffsets;

	// Debug-only strings (always empty in release builds).
	CUtlString m_ClassName;
	CUtlString m_FieldName;

	// Not read by StateChanged
	int32 m_Unk48;

	// Array element index for a CNetworkUtlVectorBase field, otherwise -1
	int32 m_nArrayIndex;

	// Path index through the pointer chain, -1 if direct; -2 drops the change
	ChangeAccessorFieldPathIndex_t m_nPathIndex;

	// Set by the multi-offset constructor; not read by StateChanged
	int16 m_bChainedPath;
};
COMPILE_TIME_ASSERT( sizeof( NetworkStateChanged_t ) == 64 );

struct NetworkStateChangedRemove_t
{
	NetworkStateChangedRemove_t() : m_nArrayFieldLocalOffset( 0 ) { }

	// Removes all pending changes associated with the specified field-path indices.
	explicit NetworkStateChangedRemove_t( CUtlVector< ChangeAccessorFieldPathIndex_t > pathIndices )
		: m_PathIndices( Move( pathIndices ) ), m_nArrayFieldLocalOffset( 0 ) { }

	// Removes the specified array element changes for a single flattened field offset.
	NetworkStateChangedRemove_t( uint32 nArrayFieldLocalOffset, CUtlVector< uint32 > arrayIndices )
		: m_ArrayIndices( Move( arrayIndices ) ), m_nArrayFieldLocalOffset( nArrayFieldLocalOffset ) { }

	// Removes a single flattened field offset.
	NetworkStateChangedRemove_t( uint32 nArrayFieldLocalOffset )
		: m_nArrayFieldLocalOffset( nArrayFieldLocalOffset ) { }

	CUtlVector< ChangeAccessorFieldPathIndex_t > m_PathIndices;
	CUtlVector< uint32 > m_ArrayIndices;
	uint32 m_nArrayFieldLocalOffset;
};

struct NetworkStateChangedLookupKey_t
{
	uint32 m_nLocalOffset;
	ChangeAccessorFieldPathIndex_t m_PathIndex;
};
COMPILE_TIME_ASSERT( sizeof( NetworkStateChangedLookupKey_t ) == 8 );

struct NetworkStateChangedLookupCache_t
{
	NetworkStateChangedLookupKey_t m_Keys[16];
	int32 m_nCount;
	int32 m_nNextSlot;
};
COMPILE_TIME_ASSERT( sizeof( NetworkStateChangedLookupCache_t ) == 136 );

struct NetworkSharedChangeRecord_t
{
	uint32 m_nLocalOffset;
	ChangeAccessorFieldPathIndex_t m_PathIndex;
	uint16 m_nArrayIndexCountAndFlags;
};
COMPILE_TIME_ASSERT( sizeof( NetworkSharedChangeRecord_t ) == 12 );

struct NetworkStateChangedBranchPath_t
{
	union
	{
		uint16 *m_pPath;
		uint16 m_Path[12];
	};

	uint16 m_nPathCount;
	bool m_bHasExternalStorage;
	uint8 m_pad1B[5];
};
COMPILE_TIME_ASSERT( sizeof( NetworkStateChangedBranchPath_t ) == 32 );

struct NetworkSharedChangeInfo_t
{
	CUtlVector< NetworkSharedChangeRecord_t > m_Records;
	uint8 m_pad018[0x240];
	CUtlVector< uint16 * > m_ArrayIndexLists;
	uint8 m_pad270[0x80];
	CUtlVector< NetworkStateChangedBranchPath_t > m_BranchPaths;
};
COMPILE_TIME_ASSERT( sizeof( NetworkSharedChangeInfo_t ) == 776 );

struct NetworkSharedChangeInfoEntry_t
{
	void *m_pLookupCache;
	NetworkSharedChangeInfo_t m_ChangeInfo;
};
COMPILE_TIME_ASSERT( sizeof( NetworkSharedChangeInfoEntry_t ) == 784 );

// Created on the first state change through a pointer chain
struct NetworkSharedChangeInfoOverflow_t
{
	// Packed field paths indexed by ChangeAccessorFieldPathIndex_t
	CUtlLeanVectorFixedGrowable< int32, 4 > m_ChangeAccessorPaths;
	void *m_pRootPathOffsetLookup;
	CUtlVectorFixedGrowableCompat< void *, 4 > m_PathOffsetLookups;
	uint8 m_pad058[0x08];
	NetworkStateChangedLookupCache_t m_CachedIgnoredLookups;
	uint8 m_pad0E8[0x08];
	NetworkStateChangedLookupCache_t m_CachedTrackedLookups;
	uint8 m_pad178[0x08];
};
COMPILE_TIME_ASSERT( sizeof( NetworkSharedChangeInfoOverflow_t ) == 384 );

struct Entity2Networkable_t
{
	union
	{
		ServerClass *m_pServerClass;
		ClientClass *m_pClientClass;
	} u;

	CEntityIdentity *m_pEntity;
	CEntityInstance *m_pEntityInstance;
	CEntityInstance *m_pParentEntity;
	const char *m_pszDesignerName;
	const char* m_pszCPPClassname;
	CNetworkTransmitComponent *m_pTransmitComponent; // PackEntity helper
	CChangeInfoAccessor *m_pChangeAccessor;
	uint32 *m_pnChangeFlags;
	CEntityInstancePolymorphicMetadataHelper *m_pMetadataHelper;

	int m_nSerialNumber; // PackEntity helper
	SpawnGroupHandle_t m_SpawnGroupHandle;
	CEntityHandle m_hEntity;
};
COMPILE_TIME_ASSERT( sizeof( Entity2Networkable_t ) == 96 );

// Base of CNetworkTransmitComponent
abstract_class IEventRegisterCallback
{
public:
	virtual void FireEvent() = 0;
};

// The network state of an entity
class CNetworkTransmitComponent : public IEventRegisterCallback
{
public:
	// Turns a pending state change into a full change
	void FireEvent() override = 0;

	// TODO(@Wend4r): Implement schemacompiler2 & kv3lib stuff
	virtual SchemaMetaInfoHandle_t< CSchemaClassInfo > Schema_DynamicBinding() = 0;
	// Save and load the only schema fields
	virtual void KV3TransferSave( CKV3TransferSaveContext *pContext ) const = 0;
	virtual void KV3TransferLoad( CKV3TransferLoadContext *pContext ) = 0;

	virtual const char *GetClassName() const = 0;

	// Frees the change info overflow, the router and the polymorphic metadata helper
	virtual ~CNetworkTransmitComponent() = 0;

public:
	// Allocated on first use; owns the path lookup caches
	NetworkSharedChangeInfoOverflow_t *m_pSharedChangeInfoOverflow;

	// While it holds changes, state changes are only marked pending
	void *m_pNetworkStateChangedRouter;

	uint32 m_nNetworkStateChangedRouterData; // 0 at construction
	uint32 m_nUnk024; // 0 at construction
	uint64 m_nUnk020; // 0 at construction

	// Not set by the constructor
	uint8 m_pad028[0x100];

	CThreadRWLock_FastRead m_StateChangeLock;

	// Where the lock is smaller
	uint8 m_padAfterLock[24];

	// 0 at construction
	NetworkStateChangeFlags_t m_eStateChangeFlags;

	// The only schema field
	uint8 m_nTransmitStateOwnedCounter;

	// A state change arrived while the router held pending changes
	bool m_bPendingStateChange;

	uint8 m_pad2DE[2];

	bool m_bNetworkUpdatesDisabled;

	// Most offsets one shared change info record holds
	int32 m_nMaxChangedOffsets;

	Entity2Networkable_t *m_pNetworkable;

	int32 m_nUnk2F0; // -1 at construction
	uint16 m_nSharedChangeInfoIndex;
	uint16 m_nSharedChangeInfoSerial; // Serial paired with m_nSharedChangeInfoIndex
	uint32 m_nUnk2F8; // 0 at construction

	// Never locked by the server
	CAtomicMutex m_UnkMutex300;
	CAtomicMutex m_UnkMutex310;

	// Built on demand and rebuilt while marked dirty
	CEntityInstancePolymorphicMetadataHelper *m_pPolymorphicMetadataHelper;
	bool m_bPolymorphicMetadataDirty; // True at construction
};

class IEntity2Networkables
{
public:
	virtual ~IEntity2Networkables() = 0;

	virtual bool GetEntity2Networkable( CEntityIndex nEntryIndex, Entity2Networkable_t *info ) = 0;
	virtual CUtlMap< int, Entity2Networkable_t > &GetEntity2Networkables( void ) const = 0;
	virtual void Unk01() = 0;
	virtual void Unk02() = 0;
	virtual CUtlVector< ServerClass * > *GetClassList() = 0;
};

class CEntity2NetworkClasses : public IEntity2Networkables, public IEntityListener
{
public:
	// One ServerClass per networkable entity class, sorted by name
	CUtlVector< ServerClass * > m_ClassList;
	CUtlMap< int, Entity2Networkable_t > m_NetworkedEntities2;
};
COMPILE_TIME_ASSERT( sizeof( CEntity2NetworkClasses ) == 72 );

#endif // ENTITYNETWORK_H
