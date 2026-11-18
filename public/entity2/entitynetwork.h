#ifndef ENTITYNETWORK_H
#define ENTITYNETWORK_H

#if _WIN32
#pragma once
#endif

#include "tier0/utlleanvector.h"
#include "tier0/utlmap.h"
#include "tier0/utlvector.h"
#include "tier0/threadtools.h"
#include "kv3lib/kv3transfer_constants.h"
#include "entityidentity.h"
#include "entitytypes.h"
#include "ientitylistener.h"

#include "spawngrouptypes.h"
#include "mathlib/vector.h"

#include "schemacompiler/schemaon.h"

class ServerClass;
class ClientClass;
class CNetworkTransmitComponent;
class CEntityInstancePolymorphicMetadataHelper;
class CEventRegister;
struct OffsetIgnore_t;
struct ChangeAccessorFieldPathIndexInfo_t;

schema struct ChangeAccessorFieldPathIndex_t
{
	DECLARE_SCHEMA_DATA_CLASS( ChangeAccessorFieldPathIndex_t );

	ChangeAccessorFieldPathIndex_t() { m_Value = -1; }
	ChangeAccessorFieldPathIndex_t( int32 value ) { m_Value = value; }

	ChangeAccessorFieldPathIndex_t &operator=( int32 value ) { m_Value = value; return *this; }

	int32 m_Value;
};

schema class CNetworkVarChainer : public CEntityOwnerPtr
{
public:
	DECLARE_SCHEMA_DATA_CLASS( CNetworkVarChainer );

	struct ChainUpdatePropagationLL_t
	{
		ChainUpdatePropagationLL_t *pNext;
		CUtlDelegate< void( const CNetworkVarChainer & ) > updateDelegate;
	};

	noschema CUtlVector< ChainUpdatePropagationLL_t > m_PropagationChain;
	ChangeAccessorFieldPathIndex_t m_PathIndex;

	// When false, NetworkStateChanged calls are no-ops
	noschema bool m_bNetworkingEnabled;
};

struct NetworkStateChanged_t
{
	NetworkStateChanged_t() : m_eChangeType( NETWORK_STATE_CHANGE_FIELDS ), m_nFileLine( -1 ), m_nArrayIndex( -1 ), m_nPathIndex( ChangeAccessorFieldPathIndex_t() ), m_bChainedPath( 0 ) { }
	explicit NetworkStateChanged_t( bool bFullChanged ) : m_eChangeType( bFullChanged ? NETWORK_STATE_CHANGE_FULL : NETWORK_STATE_CHANGE_FIELDS ), m_nFileLine( -1 ), m_nArrayIndex( -1 ), m_nPathIndex( ChangeAccessorFieldPathIndex_t() ), m_bChainedPath( 0 ) {}

	// nLocalOffset is the flattened field offset
	//		calculated taking into account embedded structures
	//		if PathIndex is specified, then the offset must start from the last object in the chain
	// nArrayIndex is the index of the array element 
	//		if the field is a CNetworkUtlVectorBase, otherwise pass -1
	// nPathIndex is the value to specify 
	//		if the path to the field goes through one or more pointers, otherwise pass -1
	// 		this value is usually a member of the CNetworkVarChainer and belongs to the last object in the chain
	NetworkStateChanged_t( uint32 nLocalOffset, int32 nArrayIndex = -1, ChangeAccessorFieldPathIndex_t nPathIndex = ChangeAccessorFieldPathIndex_t() )
		: m_eChangeType( NETWORK_STATE_CHANGE_FIELDS ), m_LocalOffsets{ nLocalOffset }, m_nFileLine( -1 ), m_nArrayIndex( nArrayIndex ), m_nPathIndex(nPathIndex), m_bChainedPath(0) { }
	NetworkStateChanged_t( CUtlVector< uint32 > vecLocalOffsets, int32 nArrayIndex = -1, ChangeAccessorFieldPathIndex_t nPathIndex = ChangeAccessorFieldPathIndex_t() )
		: m_eChangeType(NETWORK_STATE_CHANGE_FIELDS), m_LocalOffsets( Move( vecLocalOffsets ) ), m_nFileLine( -1 ), m_nArrayIndex( nArrayIndex ), m_nPathIndex( nPathIndex ), m_bChainedPath( 1 ) { }

	// NETWORK_STATE_CHANGE_FULL marks the whole entity changed instead of the offsets
	NetworkStateChangeType_t m_eChangeType;

	// Local byte offsets of the changed fields
	CUtlVector< uint32 > m_LocalOffsets;

	// Debug-only source location (empty in release builds). CBaseEntity::NetworkStateChanged prints
	// the three as "CNetworkTransmitComponent::StateChanged(%s) @%s:%d" (the file through
	// V_UnqualifiedFileName) when a full change is refused. Named by what it does: what changed
	CUtlString m_FieldName;
	CUtlString m_FileName;
	// Source line of the change, -1 when unset
	int32 m_nFileLine;

	// Array element index for a CNetworkUtlVectorBase field, otherwise -1
	int32 m_nArrayIndex;

	// Path index through the pointer chain, -1 if direct; -2 drops the change
	ChangeAccessorFieldPathIndex_t m_nPathIndex;

	// Set by the multi-offset constructor; not read by StateChanged
	int16 m_bChainedPath;
};

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

// ChangeAccessorFieldPathIndexInfo_t::IgnoreCache_t in the Dota 2017 DWARF (field names from there):
// the last 16 (offset, path index) lookups of one result, searched four at a time with SSE
struct alignas( 16 ) NetworkStateChangedLookupCache_t
{
	uint32 m_Offsets[ 16 ];
	ChangeAccessorFieldPathIndex_t m_FieldPaths[ 16 ];
	int32 m_nCount;

	// Slot of the last hit or insertion; a full cache inserts one slot below it
	int32 m_nFirstElement;
};

struct NetworkSharedChangeRecord_t
{
	uint32 m_nLocalOffset;
	ChangeAccessorFieldPathIndex_t m_PathIndex;

	// 0xFFFF without array indices. Bits 0-6 count the indices, bits 8-14 select the list in
	// NetworkSharedChangeInfo_t::m_ArrayIndexLists, bit 15 marks the whole array changed
	uint16 m_nArrayIndexCountAndFlags;
};

struct NetworkStateChangedBranchPath_t
{
	union
	{
		uint16 *m_pPath;
		uint16 m_Path[12];
	};

	uint16 m_nPathCount;
	bool m_bHasExternalStorage;
};

// CEdictChangeInfo in the Dota 2017 DWARF. engine2 keeps 3000 entries with a serial number
// (server.dll reaches them through qword_18219C2F0); the inline capacities come from the
// engine2 constructor of that array
struct NetworkSharedChangeInfo_t
{
	// At most CNetworkTransmitComponent::m_nMaxChangedOffsets records (48 when it is -1)
	CUtlVectorFixedGrowable< NetworkSharedChangeRecord_t, 48 > m_Records;

	// 128-byte blocks of up to 64 array indices, owned
	CUtlVectorFixedGrowable< uint16 *, 16 > m_ArrayIndexLists;

	CUtlVector< NetworkStateChangedBranchPath_t > m_BranchPaths;
};

struct NetworkSharedChangeInfoEntry_t
{
	// The path info of the entity that took the entry (CEdictChangeInfo::m_pChangeAccessorFieldPathInfo)
	ChangeAccessorFieldPathIndexInfo_t *m_pChangeAccessorFieldPathInfo;
	NetworkSharedChangeInfo_t m_ChangeInfo;
};

// Name and field names from the Dota 2017 DWARF. Allocated on the first state change of the entity
struct ChangeAccessorFieldPathIndexInfo_t
{
	// Packed field paths indexed by ChangeAccessorFieldPathIndex_t
	CUtlLeanVectorFixedGrowable< int32, 4 > m_ChangeAccessorFieldPathIdList;

	// Offsets to ignore for a change without a path index, and per path index
	const OffsetIgnore_t *m_pBaseOffsetToIgnore;
	CUtlVectorFixedGrowableCompat< const OffsetIgnore_t *, 4 > m_OffsetsToIgnoreForPaths;

	// Lookups found in the offsets to ignore, and lookups not found there
	NetworkStateChangedLookupCache_t m_ShouldIgnore;
	NetworkStateChangedLookupCache_t m_ShouldNotIgnore;
};

// Where a networked entity's change info is in the engine's shared change info array
// (CChangeInfoAccessor in the Dota 2017 DWARF, field names from there)
class CChangeInfoAccessor
{
public:
	uint16 m_iChangeInfo;

	// The entry is the entity's only while this matches the serial number of the array
	uint16 m_iChangeInfoSerialNumber;
};

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

// Named by what it does: one 32-bit word of the PVS cluster bit vector that an entity touches
// (CNetworkTransmitComponent::m_PVSClusterWords)
struct NetworkTransmitPVSClusterWord_t
{
	// The number of used words, kept in the first entry only (0 in the others)
	uint16 m_nWordCount;

	// Index of the word in the 128-word cluster bit vector
	uint16 m_nWordIndex;
	uint32 m_nClusterBits;
};

// Base of CNetworkTransmitComponent
abstract_class IEventRegisterCallback
{
public:
	virtual void FireEvent() = 0;
};

// The network state of an entity
schema class CNetworkTransmitComponent : public IEventRegisterCallback
{
public:
	TYPEMETA( MGetKV3ClassDefaults; MHasKV3TransferPolymorphicClassname );

	// Turns a pending state change into a full change
	void FireEvent() override = 0;

	// TODO(@Wend4r): Implement schemacompiler2 & kv3lib stuff
	virtual SchemaMetaInfoHandle_t< CSchemaClassInfo > Schema_DynamicBinding() = 0;
	// Save and load the only schema fields
	CLASS_USES_KV3TRANSFER_VIRTUAL( CNetworkTransmitComponent );

	virtual const char *GetClassName() const = 0;

	// Frees the polymorphic metadata helper, the timer event and the change info overflow
	virtual ~CNetworkTransmitComponent() = 0;

public:
	// Allocated on the first state change (Dota 2017 DWARF name)
	noschema ChangeAccessorFieldPathIndexInfo_t *m_pChangeChangeAccessorFieldPathIndexInfo;

	// Calls FireEvent when it fires; while it is registered, state changes only set
	// m_bPendingStateChange (Dota 2017 DWARF name; 32 bytes, owned)
	noschema CEventRegister *m_pTimerEvent;

	// Named by what it does: the PVS clusters the entity touches, rebuilt from its world bounds
	// while m_bPVSClustersDirty is set (server.dll sub_1801A8DF0); the center of those bounds, the
	// number of used words and the words
	noschema Vector m_vecPVSClusterCenter;
	noschema int32 m_nPVSClusterWordCount;
	noschema NetworkTransmitPVSClusterWord_t m_PVSClusterWords[ 32 ];

	// No user found in server.dll and libserver.so besides the constructor and the destructor
	noschema CThreadRWLock_FastRead m_StateChangeLock;

	// Named by what it does: the box that the sv_pvs_max_distance check measures against the
	// viewer when sv_use_pvs_cache is set (the entity origin and radius otherwise)
	noschema Vector m_vecPVSCacheMins;
	noschema Vector m_vecPVSCacheMaxs;

	// NETWORK_STATE_CHANGED / NETWORK_STATE_FULL_CHANGE; bits 2-4 are the transmit state that
	// is mirrored into the global bit vectors indexed by m_nEntityIndex
	noschema NetworkStateChangeFlags_t m_eStateChangeFlags;

	// The only schema field; while it is non-zero the transmit state is not recomputed
	uint8 m_nTransmitStateOwnedCounter;

	// A state change arrived while m_pTimerEvent was registered; FireEvent makes it a full change
	noschema bool m_bPendingStateChange;

	// Named by what it does: m_PhysicalSpawnGroupHandle is looked up again from the spawn group
	// bounds that contain the entity
	noschema bool m_bPhysicalSpawnGroupDirty;

	// Named by what it does: m_PVSClusterWords is rebuilt
	noschema bool m_bPVSClustersDirty;

	// State changes are dropped while it is set
	noschema bool m_bNetworkUpdatesDisabled;

	// Most offsets one shared change info record holds, -1 for the default 48
	noschema int32 m_nMaxChangedOffsets;

	// The entity that embeds the component (Dota 2017 DWARF name)
	noschema CEntityInstance *m_pOuter;

	// Named by what it does: the entity index, -1 until the entity is added to the entity database
	noschema int32 m_nEntityIndex;

	// Dota 2017 DWARF names
	noschema CChangeInfoAccessor m_Accessor;
	noschema SpawnGroupHandle_t m_PhysicalSpawnGroupHandle;

	// Named by what they guard: the UpdateTransmitState call and the PVS cluster rebuild
	noschema CAtomicMutex m_UpdateTransmitStateMutex;
	noschema CAtomicMutex m_PVSClustersMutex;

	// Built on demand and rebuilt while marked dirty
	noschema CEntityInstancePolymorphicMetadataHelper *m_pPolymorphicMetadataHelper;
	noschema bool m_bPolymorphicMetadataDirty; // True at construction
};

// The entity system's view of the networked entities (CEntitySystem::m_pEntity2Networkables)
class IEntity2Networkables
{
public:
	virtual ~IEntity2Networkables() = 0;

	// Copies the entry of the entity index; false when there is none
	virtual bool GetEntity2Networkable( CEntityIndex nEntryIndex, Entity2Networkable_t *info ) = 0;
	virtual CUtlMap< int, Entity2Networkable_t > &GetEntity2Networkables( void ) const = 0;

	// Client only (empty on the server). pCreateInfo holds the class (+0x00), the entity index
	// (+0x08), the serial number (+0x0C), the spawn group (+0x10) and a slot reuse flag (+0x14);
	// creates the entity in its slot and returns GetEntity2Networkable for it.
	// Named after its error message "CreateClientSideNetworkedEntity( %d:%d )"
	virtual bool CreateClientSideNetworkedEntity( const void *pCreateInfo, Entity2Networkable_t *info ) = 0;

	// Client only (empty on the server). Named by what it does: deletes the entity of the entry,
	// queued for deferred deletion when bDeferred is set, otherwise at once
	virtual void DestroyClientSideNetworkedEntity( Entity2Networkable_t *info, bool bDeferred ) = 0;

	// The class of every networkable entity class (ClientClass on the client)
	virtual CUtlVector< ServerClass * > *GetClassList() = 0;
};

// Lives in server.dll and client.dll (72 bytes on both platforms); listens to the entity
// system to keep m_Entity2Networkables up to date
class CEntity2NetworkClasses : public IEntity2Networkables, public IEntityListener
{
public:
	// Removes itself from the entity listeners and frees the classes
	virtual ~CEntity2NetworkClasses() = 0;

	// IEntityListener; OnEntitySpawned keeps the empty default
	void OnEntityCreated( CEntityInstance *pEntity ) override = 0;
	void OnEntityDeleted( CEntityInstance *pEntity ) override = 0;
	// Stores the new parent in the entry's m_pParentEntity
	void OnEntityParentChanged( CEntityInstance *pEntity, CEntityInstance *pNewParent ) override = 0;

public:
	// One ServerClass per networkable entity class, sorted by name; owned (56 bytes each)
	CUtlVector< ServerClass * > m_ClassList;

	// Named by what it does: the entry of every networked entity by entity index, returned by
	// GetEntity2Networkables
	CUtlMap< int, Entity2Networkable_t > m_Entity2Networkables;
};

#include "schemacompiler/schemaoff.h"

#endif // ENTITYNETWORK_H
