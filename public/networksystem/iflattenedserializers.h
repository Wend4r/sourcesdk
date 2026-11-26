#ifndef FLATTENEDSERIALIZERS_H
#define FLATTENEDSERIALIZERS_H

#pragma once

#include "tier0/basetypes.h"
#include "tier0/utlsymbol.h"
#include "tier0/utlstring.h"
#include "tier0/convar.h"
#include "tier0/utlsymbollarge.h"
#include "tier0/utlvector.h"
#include "tier0/utlleanvector.h"
#include "tier0/bitvec.h"
#include "const.h"
#include "tier0/utllinkedlist.h"
#include "tier0/utlhashtable.h"
#include "tier0/threadtools.h"
#include "tier1/utldelegate.h"
#include "entity2/entityindex.h"

#include "fieldpath.h"
#include "networksystem/inetworkserializer.h"

DECLARE_POINTER_HANDLE( FlattenedSerializerHandle_t );

class bf_read;
class bf_write;
class CCheckTransmitInfo;
class CEntityClass;
class CEntityInstance;
class CEntityInstancePolymorphicMetadataHelper;
class CNetworkSerializerClassInfo;
class CMemoryStack;
class CFlattenedSerializer;
class CNetworkSerializer;
class CSchemaClassBindingBase;
class CSchemaClassInfo;
class CSchemaType;
class PackedEntity;
class ServerClass;
class IMemAlloc;

struct SchemaClassFieldData_t;
struct FlattenedSerializerOwner_t;
struct FlattenedSerializerUserData_t;
struct NetworkFieldResult_t;
struct NetworkSerializerInfo_t;
struct NetworkUnserializerInfo_t;
struct NetworkableData_t;

struct FlattenedSerializerDesc_t
{
	FlattenedSerializerDesc_t() : m_name(), m_pSerializer( nullptr ) {}
	FlattenedSerializerDesc_t( const FlattenedSerializerDesc_t &other ) : m_name( other.m_name ), m_pSerializer( other.m_pSerializer ) {}
	FlattenedSerializerDesc_t &operator=( const FlattenedSerializerDesc_t &other ) = default;

	CUtlSymbolLarge m_name;

	union
	{
		CFlattenedSerializer *m_pSerializer;
		FlattenedSerializerHandle_t m_handle;
	};
};

//-----------------------------------------------------------------------------
// Asked by the serializer builder while it builds the serializer of a class
// (BuildFlattenedSerializerInfo_t::m_pFilter). The server implements it as
// CNetworkSerializerBindingBuildFilter. Linux slots 0-5, Windows 0-4.
//-----------------------------------------------------------------------------
abstract_class INetworkSerializerBindingBuildFilter
{
public:
	virtual ~INetworkSerializerBindingBuildFilter() {}

	// Named by what it does: the name of the module the classes belong to ("Entity2" in the server)
	virtual const char *GetModuleName() = 0;
	// Named by what it does: the replacement of pszName from the filter's list of name pairs, or pszName
	virtual const char *RemapName( void *pContext, const char *pszName ) = 0;
	// Walks the base classes of pClass; 0x80000000 when none has a priority
	virtual int GetFieldPriority( const CSchemaClassInfo *pClass ) = 0;
	// Named by what it does: the type registered for the field name in the override list of the
	// class chain, else the field's own type
	virtual const CSchemaType *OverrideFieldType( void *pTypeScope, const CSchemaClassInfo *pClass, const SchemaClassFieldData_t *pField ) = 0;
};

// One class to build, 48-byte elements of the BuildEntityClassSerializers array
// (filled by CEntitySystem::BuildNetworkClasses in the server and the client)
struct BuildFlattenedSerializerInfo_t
{
	FlattenedSerializerDesc_t *m_pSerializerHandle; // Receives the built serializer (ServerClass::m_Serializer)
	CNetworkSerializerClassInfo *m_pClassInfo;
	INetworkSerializerBindingBuildFilter *m_pFilter; // Asked for type and encoder overrides (CNetworkSerializerBindingBuildFilter in the game)
	int m_nReserved; // Named by what the code does with it: 0 from both callers, no reader found
	NetworkSerializationMode_t m_nSerializationMode; // Copied into CNetworkSerializer::m_nSerializationMode
	void *m_pMissingNetworkVarStructs; // Container of the caller that collects the structs reported as "MNetworkEnable but not using CNetworkVar"
	bool m_bServer; // m_nSerializationMode is NET_SERIALIZATION_MODE_SERVER; the offset lookup is built and the network var checks run
	bool m_bReservedFlag; // Named by what the code does with it: false from both callers, no reader found
	bool m_bSerializerAlreadyExisted; // Set when the serializer was found in a bucket instead of being built
};

struct FlattenedSerializerChange_t
{
	CFieldPath m_Path;
	uint64 m_nData;
	int16 m_nBitCount;
	int16 m_nFieldPathIndex;
	bool m_bExactPath;
};

struct FlattenedSerializerChangeArray_t
{
	CUtlVector< int > m_Offsets;
	CUtlVector< FlattenedSerializerChange_t > m_Changes;
};

struct FlattenedSerializerSpewInfo_t
{
	FlattenedSerializerUserData_t *m_pContext;
	void ( *m_pfnSpew )( FlattenedSerializerUserData_t *pContext, const char *pszText );
	int m_nLoggingChannel;
};

struct FlattenedSerializerEncodeResult_t
{
	CUtlVector< byte > m_Buffer;
	int m_nBitCount;
	int m_nFlags;
	CFieldPath m_RootPath;
};

struct FlattenedSerializerPackInfo_t
{
	CCheckTransmitInfo *m_pTransmitInfo; // PackEntity transmit filter/context.
	CEntityInstance *m_pEntity; // Entity instance being packed.
	CEntityIndex m_nEntityIndex; // Entity index passed to Encode.
	int m_nSerialNumber; // Entity serial number passed to Encode.
	int m_nTick; // Server tick used by PackEntity diagnostics.
	int m_nBaseline; // Baseline slot passed to Encode.
};

struct FlattenedSerializerPackResult_t
{
	CUtlVector< byte > m_Buffer;
	CUtlVector< CFieldPath > m_ChangedPaths;
	int m_nBitsWritten;
	bool m_bEncoded;
};

struct FlattenedSerializerFieldPathMap_t
{
	CUtlVector< int > m_Offsets;
	CUtlVector< CFieldPath > m_FieldPaths;
};

struct FlattenedSerializerLookupResult_t
{
	FlattenedSerializerDesc_t m_Serializer;
	CUtlString m_Error;
};

struct FlattenedSerializerContext_t
{
	FlattenedSerializerOwner_t *m_pOwner;
	FlattenedSerializerUserData_t *m_pUserData;
	int m_nFlags;
};

struct FlattenedSerializerSaveRestoreOps_t
{
	FlattenedSerializerUserData_t *m_pSaveOps;
	FlattenedSerializerUserData_t *m_pRestoreOps;
	FlattenedSerializerUserData_t *m_pContext;
};

struct SerializedEntityFieldPathList_t
{
	int m_nRefCount; // Shared field-path storage is reference counted by serialized entity views.
	int m_nFieldPathCount; // Count read by DumpSerializedEntityToConsole before m_EncodedFieldPaths.
	int m_EncodedFieldPaths[1]; // Packed CFieldPath indices used by BuildMergedSerializedEntity and CullUnchangedFieldPaths.
};

struct SerializedEntityBitOffsetList_t
{
	int m_nFirstBit;
	int m_FieldEndBits[1]; // Entry N+1 is used as the end bit for field N.
};

struct SerializedEntityMetadataEntry_t
{
	int m_nFieldPath; // Encoded CFieldPath, or -1 for sentinel metadata.
	uint m_nValue;
	byte m_nType;
};

struct SerializedEntityMetadata_t
{
	int m_nRefCount;
	int m_nMetadataCount;
	SerializedEntityMetadataEntry_t m_Metadata[1];
};

struct SerializedEntityData_t
{
	SerializedEntityFieldPathList_t *m_pFieldPaths; // Packed field-path list, with the count at +0x04.
	SerializedEntityBitOffsetList_t *m_pFieldBitOffsets; // Bit offsets for each serialized field.
	SerializedEntityMetadata_t *m_pMetadata; // Optional ref-counted metadata dumped before field values.
	byte *m_pData; // Serialized bitstream payload.
	FlattenedSerializerDesc_t m_Serializer; // Serializer the fields were written with.
	int m_nFieldCount; // Number of fields in m_pFieldPaths and m_pFieldBitOffsets.
	int m_nBitCount; // Total number of valid bits in m_pData.
	uint m_nAllocatedDataBytes;
	bool m_bReuseDataBuffer;
};

struct NetworkEntityData_t
{
	SerializedEntityFieldPathList_t *m_pFieldPaths; // Same packed field-path shape as SerializedEntityData_t.
	SerializedEntityBitOffsetList_t *m_pFieldBitOffsets;
	SerializedEntityMetadata_t *m_pMetadata;
	byte *m_pData;
	uint m_nDataBytes;
	uint m_nAllocatedDataBytes;
	int m_nFlags;
	int m_nBaseline;
	int m_nFieldCount;
	int m_nBitCount;
};

abstract_class IFlattenedSerializerSpewFunc
{
public:
	virtual ~IFlattenedSerializerSpewFunc() {}
	virtual void SpewLine( const char *pszText ) = 0;
};

// Request passed to INetworkFieldChangedEventQueue::QueuePointerRecreation, copied into a CNetworkFieldPointerAllocationQueueInfo_t.
struct NetworkFieldScratchChange_t
{
	int m_nEntityIndex;
	const CFieldPath *m_pPath;
	const char *m_pszFieldName; // Compared with the debug callback field name.
	void *m_pContext; // Spewed as "[context entity 0x%p]" once copied into the queue info
	void *m_pFieldOwnerStruct; // Object holding the pointer field, the callback runs on it for delegate type 2 (NetworkChangePointerCallback_t::Callback_FieldOwner)
	const char *m_pszTypeName; // Spewed as "allocate %s *" / "deallocate %s *".
	CNetworkSerializerClassInfo *m_pPointeeClassInfo; // Frees the pointee of a processed deallocation through its manipulator
	void **m_ppField; // The pointer field; a recreation removes the queued deallocation of the same field
	CUtlAbstractDelegate m_delegate;
	uint8 m_nDelegateType; // Spewed through the delegate type name table
};

// Queued change callback, one per changed field and order.
struct CNetworkFieldChangedEventQueueChangeInfo_t
{
	const char *m_pszFieldName;
	CUtlAbstractDelegate m_delegate; // Its object is the field owner struct.
	void *m_pContext; // "[context entity 0x%p]"
	void *m_pData; // Copy of the new value, allocated from the scratch data.
	CFieldPath m_Path;
	uint32 m_unAllocSize; // Size of m_pData, rounded up to 16.
	int32 m_nObjectId;
	struct
	{
		void *m_pCollection; // The array or vector that holds the element
		const SchemaCollectionManipulatorFn_t *m_pCollectionManipulator; // Asked for the element count of m_pCollection before the callback runs
		int32 m_nArrayIndex; // Passed to the array delegates, -1 when unset; the change is cancelled when it is not below the element count
		bool m_bSpewInvalidElement; // Named by what it does: the cancellation of an element past the count is logged ("removing callback for invalid element")
	} m_ArrayInfo; // Copied from pArrayInfo for the array delegate types.
	uint8 m_type; // NetworkFieldChangedDelegateType_t, NETWORK_FIELD_CHANGED_COUNT_TYPE when unset.
	void *m_pReplayCompatData; // Passed by address to the replay compat delegates and freed after the call.
	int32 m_nTagValue; // Passed by address to the tag delegates.
	bool m_bCancelled; // Skipped by InvokeChangeCallbacks.
	uint32 m_nNextInChain; // Next change with the same key, ~0u at the end.
};

// Queued pointer recreation, one per allocated or deallocated pointer field.
struct CNetworkFieldPointerAllocationQueueInfo_t
{
	const char *m_pszFieldName;
	CUtlAbstractDelegate m_delegate;
	uint8 m_nDelegateType; // Spewed through the delegate type name table ("(%s)")
	void *m_pContext; // "[context entity 0x%p]"
	void *m_pFieldOwnerStruct; // The delegate object for delegate type 2; type 1 uses the current pointee instead
	CFieldPath m_Path;
	CUtlString m_TypeName;
	CNetworkSerializerClassInfo *m_pPointeeClassInfo; // A processed deallocation frees *m_ppField through it, then clears the field
	void **m_ppField; // Spewed as the current pointer value; compared when a recreation looks for a queued deallocation
	int m_nEntityIndex;
	uint8 m_nKind; // 1 "allocate %s *", 2 "deallocate %s *"
	bool m_bRemoved; // Set on a queued deallocation the recreation of the same field removes ("recreation removing deallocation")
};

// Per-thread part of CNetworkFieldChangedEventQueue, chosen by the thread pool thread index.
struct NetworkFieldScratchThreadQueue_t
{
	CAtomicMutex m_Mutex;
	CUtlVector< CUtlLinkedList< CNetworkFieldChangedEventQueueChangeInfo_t, unsigned int > > m_queuedChanges; // One list per order.
	CUtlVector< CUtlHashtable< uint32, uint32 > > m_queuedChangeLookup; // One table per order, maps a change key to its list index.
	CUtlLinkedList< CNetworkFieldPointerAllocationQueueInfo_t, unsigned int > m_PointerAllocations;
};

struct NetworkFieldScratchMemoryStackList_t
{
	CMemoryStack **m_ppMemoryStacks;
	int m_nMemoryStackCount;
};

class IFieldChangeLimitSpew;

// Scratch memory for queued field changes, implemented by the client (CNetworkFieldScratchData).
abstract_class INetworkFieldScratchData
{
public:
	virtual ~INetworkFieldScratchData() {}
	virtual void Init( uint32 nSize, LoggingChannelID_t channelID ) = 0;
	virtual void *Alloc( uint32 nSize ) = 0;
	virtual void Purge() = 0;
	virtual LoggingChannelID_t LoggingChannel() const = 0;
};

abstract_class INetworkFieldChangedEventQueue
{
public:
	virtual ~INetworkFieldChangedEventQueue() {}
	// nOrder < 0 queues into the last order. pData receives the scratch copy of the value. bCancel marks an already queued change as cancelled instead of queuing one.
	virtual bool QueueChange( int nObjectId, const CFieldPath &path, const char *pszFieldName, void *pContext, void *pFieldOwnerStruct, void *&pData, int nOrder, uint8 type, const CUtlAbstractDelegate &delegate, int nAllocSize, const void *pArrayInfo, const int *pTagValue, bool bCancel ) = 0;
	virtual bool QueueReplayCompatFieldChange( int nObjectId, const CFieldPath &path, const char *pszFieldName, void *pContext, void *pFieldOwnerStruct, int nOrder, const CUtlAbstractDelegate &delegate, uint8 nFlags, const void *pReplayCompatData ) = 0; // "nullptr != pFieldOwnerStruct"

	// Invokes the queued change callbacks, reporting each callback slower than flTimeLimitMs to the change limit spew
	virtual void InvokeChangeCallbacks( int nChangeFrame, float flTimeLimitMs ) = 0;
	virtual void ResetFrameAllocator() = 0; // Purges the scratch data and clears m_nProcessedOrder.
	virtual void ClearQueuedChanges() = 0;
	virtual void RebaseQueuedDelegates( bool bSpew, byte *pOldBase, byte *pNewBase, uintp nSize ) = 0;
	virtual void MarkShrunkDelegatesInvalid( bool bSpew, byte *pOldBase, byte *pOldEnd ) = 0;
	virtual void SetChangeCallbackTimeLimitSpewFunc( IFieldChangeLimitSpew *pSpew ) = 0;
	virtual void SetDebugCallback( bool bEnabled, const char *pszFieldName ) = 0;
	virtual bool IsDebugCallbackEnabled() const = 0;
	virtual void SetDebugFieldName( const char *pszFieldName ) = 0;
	virtual void QueuePointerRecreation( const NetworkFieldScratchChange_t *pRequest, uint8 nKind, uint8 bSpew ) = 0; // nKind 1 allocates, 2 deallocates, 3 removes a queued deallocation of the same field (spewed as "recreation removing deallocation" when bSpew is set)
};

class CNetworkFieldChangedEventQueue : public INetworkFieldChangedEventQueue
{
public:
	CUtlVector< NetworkFieldScratchThreadQueue_t > m_ThreadQueues; // One queue per worker thread.
	CUtlVectorFixedGrowable< int, 32 > m_orderPriority; // Snapshot of network change frames from the entity system.
	CUtlString m_DebugCallbackName; // Field name that triggers DebugCallback spew.
	CUtlString m_DebugFieldName; // Extra debug field filter.
	INetworkFieldScratchData *m_pScratchData; // Scratch memory used by QueueChange.
	IFieldChangeLimitSpew *m_pfnChangeLimitSpew;
	int m_nProcessedOrder; // Last processed change frame.
	int m_nEntityChangeFrameCount; // Count copied from the entity system.
	bool m_bDebugQueueChanges; // Enables QueueChange spew.
};

// Same declarations as in networksystem/inetworkmessages.h
typedef NetworkFieldResult_t ( *NetworkFieldSerializeCB )(NetworkSerializerInfo_t const&, int, NetworkableData_t *);
typedef NetworkFieldResult_t ( *NetworkFieldUnserializeCB )(NetworkUnserializerInfo_t const&, int, NetworkableData_t *);
typedef NetworkFieldResult_t ( *NetworkFieldSerializeBufferCB )(NetworkSerializerInfo_t const&, bf_write &, int *, void const **, bool);
typedef NetworkFieldResult_t ( *NetworkFieldUnserializeBufferCB )(NetworkUnserializerInfo_t const&, bf_read &, int *, void **, bool, CSchemaClassBindingBase const*);
typedef bool ( *NetworkableDataCB )(NetworkableData_t const*, CUtlString &);
typedef const char* ( *NetworkFieldSerializerNameCB )(void);

// Callbacks of a field serializer for one NetworkSerializationMode_t, copied from NetworkFieldSerializerGroup_t::Callbacks_t
// (the primary or the alternate pair, as the network serializer selects)
struct NetworkFieldSerializerCallbacks_t
{
	NetworkFieldSerializeCB m_pfnSerialize;
	NetworkFieldUnserializeCB m_pfnUnserialize;
};

// Same for a user array serializer, copied from NetworkArrayFieldSerializerGroup_t::Callbacks_t
struct NetworkArrayFieldSerializerCallbacks_t
{
	NetworkFieldSerializeBufferCB m_pfnSerialize;
	NetworkFieldUnserializeBufferCB m_pfnUnserialize;
};

// Encoder chosen by name for a field (MNetworkEncoder, "default" otherwise), copied from the encoder table of its data type
// with the encode/decode pair selected by net_validatemessages
struct NetworkFieldEncoder_t
{
	void *m_pfnEncode; // Writes the value to the bf_write; the validating variant checks the NetworkableData_t type first
	void *m_pfnDecode; // Reads the value from the bf_read into a NetworkableData_t
	const char *( *m_pfnGetName )(); // Spewed as "encoder(%s)", "default" is omitted
	bool ( *m_pfnWriteMetaInfo )( void *pMetaInfo, const void *pEncoderData ); // Named by what it does: stores the encoder settings (bit count) into the field metadata
	bool ( *m_pfnReadMetaInfo )( const void *pMetaInfo, void *pEncoderData, int *pBitCount ); // Named by what it does: the reverse, for fields built from metadata
};

// Change callback or change tag of a field (MNetworkChangeCallback / MNetworkChangeTag), copied from the registered callback
struct NetworkFieldChangeCallback_t
{
	CUtlAbstractDelegate m_Delegate;
	const char *m_pszName; // Registered callback name, or the tag name
	uint32 m_nTagHash; // NETWORK_FIELD_CHANGED_TAG only, spewed as "tag='%s':%u"
	int16 m_nSharedCallbackIndex; // Set by CNetworkSerializer::AssignSharedChangeCallbackIndex, -1 when not shared
	uint8 m_nType; // NetworkFieldChangedDelegateType_t
	uint8 m_nReservedType; // Named by what the code does with it: copied with the callback, always 0
	void *m_pReserved; // Named by what the code does with it: zeroed and copied, never set
	bool m_bPerformOnChangeOrCreate; // Registration flag 1 (NETWORK_FIELD_CHANGE_CALLBACK_PERFORM_ON_CHANGE_OR_CREATE); always set for tags
	bool m_bNotQueuedByDecode; // Registration flag 4, named by what it does: the decoder does not queue the callback
	bool m_bPerformFlag2; // Registration flag 2, named by what the code does with it: hashed with the field, no other reader found
	bool m_bPerformFlag8; // Registration flag 8, named by what the code does with it: copied, no reader found
};

// A networked field of a CNetworkSerializer, built from its CNetworkSerializerFieldInfo.
// Allocated from the 216-byte pool of CNetworkFieldSerializerAllocator, which also shares equal fields by m_nCachedHash.
struct NetworkFieldSerializer_t
{
	CUtlStringToken m_FieldNameHash;
	const char *m_pszFieldName;
	CNetworkSerializerClassInfo *m_pTypeClassInfo; // Class of the field type for embedded structs, pointers and vectors of them
	SchemaCollectionManipulatorFn_t m_pfnCollectionManipulator; // Vectors only, returns the element count for action 0
	uint16 m_nOffset; // Offset in the owner, the base class offset included
	uint16 m_nFixedArraySize;
	uint16 m_nElementSize; // Element size of arrays and vectors, 8 for pointers
	uint16 m_nPriority : 7; // MNetworkPriority, 64 by default
	uint16 m_nDataType : 5; // NetworkFieldSerializerGroup_t::m_nDataType
	uint16 m_nComponentCount : 4; // NetworkFieldSerializerGroup_t::m_nComponentCount
	NetworkArrayFieldSerializerCallbacks_t *m_pArraySerializerCallbacks; // [ NET_SERIALIZATION_MODE_COUNT ], set for arrays and vectors
	NetworkFieldSerializerCallbacks_t *m_pSerializerCallbacks; // [ NET_SERIALIZATION_MODE_COUNT ]
	NetworkFieldEncoder_t *m_pEncoder; // 40-byte copy of the encoder table entry chosen by name ("Unable to find network encoder named %s!")
	byte *m_pSerializerData; // m_nSerializerDataSize bytes for the serializer, array serializer and encoder data
	uint8 m_nSerializerDataOffsets[ 2 ]; // Offsets in m_pSerializerData, 0xFF when unused (one per NetworkSerializationMode_t)
	uint8 m_nArraySerializerDataOffsets[ 2 ];
	int m_nEmbeddedFieldOffsetDelta; // MNetworkVarEmbeddedFieldOffsetDelta, spewed as "[+%d]"
	NetworkableDataCB m_pfnNetworkableData;
	NetworkFieldSerializerNameCB m_pfnSerializerName; // Spewed as "serializer(%s)"
	CNetworkSerializer *m_pTypeSerializer; // Network serializer of the field type, set for embedded structs
	CUtlLeanVector< NetworkFieldChangeCallback_t > *m_pChangeCallbacks; // MNetworkChangeCallback and change tags, spewed as "cb(...)"
	NetworkChangePointerCallback_t *m_pChangePointerCallback;
	NetworkRecipientsFilter_t *m_pSendProxyRecipientsFilter; // MNetworkSendProxyRecipientsFilter
	CUtlStringToken m_NetworkAliasHash;
	const char *m_pszNetworkAlias; // MNetworkAlias, used instead of m_pszFieldName when set
	CUtlStringToken m_NetworkTypeAliasHash;
	const char *m_pszNetworkTypeAlias; // MNetworkTypeAlias, used instead of m_TypeName when set
	CUtlStringToken m_ClassNameHash;
	const char *m_pszClassName; // Class that declares the field
	CUtlString m_TypeName;
	uint64 m_nCachedHash; // CRC64 of the field, cleared before it is computed again
	int m_nFieldSize;
	uint8 m_nClassHierarchyIndex : 6; // Position of the declaring class in the class hierarchy walk, copied into its send node
	uint8 m_nUserGroupCount : 2; // MNetworkIncludeByUserGroup, at most 3
	uint8 m_nUserGroupIndices[ 3 ]; // Indices in the global user group name table
	uint8 m_nSerializerDataSize;
	uint8 m_nEncoderDataOffset; // Offset in m_pSerializerData, 0xFF when unused
	int8 m_nChangeCallbackOrder; // Order of the change callbacks, -1 when none; spewed as "[%d]"
	OOPVSUpdates_t m_nOutOfPVSUpdates; // OOPVSUpdates_Default by default
	uint8 m_bIsPointer : 1;
	uint8 m_bIsArrayOfPointers : 1; // An array or vector of pointers
	uint8 m_bFake : 1; // Built from metadata ("InitFakeField"), spewed as " fake"
	uint8 m_bReservedFlag : 1; // Named by what the code does with it: only copied with the field, no reader found
	uint8 m_bSignedEnum : 1; // Spewed as " [signed enum]"
	uint8 m_bEmbeddedNotFlattened : 1; // MNetworkVarEmbeddedNotFlattened
	uint8 m_bPolymorphic : 1; // MNetworkPolymorphic, spewed as " polymorphic"
	uint8 m_bIsFixedArray : 1;
};

// A class of the hierarchy of a CNetworkSerializer, most derived first
struct NetworkSerializerHierarchyClass_t
{
	CUtlStringToken m_ClassNameHash;
	const char *m_pszClassName;
	OOPVSUpdates_t m_nOutOfPVSUpdates; // MNetworkOutOfPVSUpdates of the class, at most OOPVSUpdates_Default
	int m_nBaseClassIndex; // Entry consulted when m_nOutOfPVSUpdates is OOPVSUpdates_Default, -1 at the root
};

// Networked fields of one class, shared by all flattened serializers that contain the class
// (96 bytes; created by sub_1800F5240 from a CNetworkSerializerClassInfo, or from metadata for a fake serializer)
class CNetworkSerializer
{
public:
	const char *m_pszName; // Class name, points into m_FakeName for a fake serializer
	CNetworkSerializerClassInfo *m_pClassInfo;
	CUtlVector< NetworkFieldSerializer_t * > m_Fields;
	int m_nValidateMessages; // net_validatemessages when the serializer was built, selects the validating serializer callbacks and encoders
	NetworkSerializationMode_t m_nSerializationMode; // From BuildFlattenedSerializerInfo_t, the client renames CUtlVectorEmbeddedNetworkVar types
	uint8 m_nSharedChangeCallbackCount; // At most 256 ("m_nSharedChangeCallbackCount must be <= EncounteredSharedChangedCallbackBitVec_t::knFixedBitSize")
	CUtlVector< NetworkSerializerHierarchyClass_t > m_Hierarchy;
	CUtlString m_FakeName;
	bool m_bBuilt; // Named by what the code does with it: set by both constructors, no reader found
	bool m_bEmbeddedNotFlattened; // CNetworkSerializerClassInfo::m_NetworkVarEmbeddedNotFlattened ("is not declared DECLARE_EMBEDDED_NETWORKVAR_NOTFLATTENED()")
	bool m_bStructNotInNetworkUtlVectorEmbedded; // CNetworkSerializerClassInfo::m_NetworkStructNotInNetworkUtlVectorEmbedded
	bool m_bOwnsClassInfo; // The class info was created from metadata and is freed with the serializer
};

// Node of the send tree of a flattened serializer: the serializer itself (the root) and each struct field under it
// (96 bytes; the name is from a std::function signature in the networksystem RTTI)
class CSendNode
{
public:
	const char *m_pszName; // Field name, "" for the root; spewed as "sendnode"
	const char *m_pszNetworkAlias; // MNetworkAlias of the field
	CUtlVector< CSendNode * > m_Children;
	CNetworkSerializer *m_pNetworkSerializer; // Serializer of the struct the node stands for
	CUtlStringToken m_NameHash;
	CUtlStringToken m_NetworkAliasHash;
	CUtlLeanVector< NetworkFieldChangeCallback_t > m_ChangeCallbacks; // Copy of the field's change callbacks
	uint16 m_nProxyIndex; // CSendNode::SetRecursiveProxyIndices_R, warns at 64 ("Too many proxies for serializer %s.")
	uint16 m_nSharedChangeCallbackBase; // Shared change callback count of the owner when the node was added, 0xFFFF at the root
	uint16 m_nFieldIndex; // Named by what it does: index of the field in the hierarchy field list being built
	uint16 m_nOffset; // NetworkFieldSerializer_t::m_nOffset of the field
	int8 m_nChangeCallbackOrder; // NetworkFieldSerializer_t::m_nChangeCallbackOrder, -1 at the root
	int m_nClassHierarchyIndex; // Entry of the parent's CNetworkSerializer::m_Hierarchy that declares the field, -1 at the root
	CSendNode *m_pParent;
};

// Variant of a polymorphic field, found by the class binding of the pointee
// ("FlattenedSerializerField_t::FindPolymorphicFieldInfoByCodegenBinding")
struct FlattenedSerializerPolymorphicField_t
{
	CFlattenedSerializer *m_pFlattenedSerializer;
	uint32 m_unPolymorphicTypeId; // 1-based variant index, written to the field path
};

// The game packs the field list of CFlattenedSerializer: elements are 46 bytes apart
#pragma pack( push, 2 )
// One field of a flattened serializer (the name is from a std::function signature in the networksystem RTTI)
struct FlattenedSerializerField_t
{
	NetworkFieldSerializer_t *m_pFieldSerializer;
	CFlattenedSerializer *m_pSerializer; // Serializer of an embedded struct or pointer field, NULL for primitives
	CUtlVector< FlattenedSerializerPolymorphicField_t > m_PolymorphicFields; // Variants of a polymorphic field
	uint16 m_nOffset; // Offset in the entity, the embedded parents included, spewed as "offset %u"
	int8 m_nSendNodeIndex; // Index in CFlattenedSerializer::m_SendNodes
	uint8 m_nSharedChangeCallbackCount; // Of the field serializer type, added to the serializer count
	uint8 m_nEmbeddedChainIndex; // Index in CFlattenedSerializer::m_EmbeddedChains, 0 when the field is not under an embedded struct
	uint8 m_nEmbeddedSerializerIndex; // Index in CFlattenedSerializer::m_EmbeddedSerializers of the struct that holds the field
};
#pragma pack( pop )

// Fields under embedded (not flattened) structs: m_nCount entries of CFlattenedSerializer::m_EmbeddedChainIndices from m_nFirst
struct FlattenedSerializerEmbeddedChain_t
{
	uint16 m_nFirst;
	uint16 m_nCount;
};

// Embedded struct field that keeps its own serializer (CNetworkVarEmbeddedNotFlattened)
struct FlattenedSerializerEmbedded_t
{
	CFlattenedSerializer *m_pSerializer;
	NetworkFieldSerializer_t *m_pFieldSerializer;
	CUtlVector< int > m_FieldIndices; // Fields of the owner serializer under this struct, sorted
};

// Struct field removed by the include/exclude culling while its serializer is still walked
// (field path index after the remaining fields)
struct FlattenedSerializerCulledField_t
{
	CFlattenedSerializer *m_pSerializer;
	NetworkFieldSerializer_t *m_pFieldSerializer;
};

// Clone made to match a different meta variant; RemoveFakeFields restores the original
struct FlattenedSerializerClone_t
{
	CFlattenedSerializer *m_pOriginal;
	CFlattenedSerializer *m_pClone;
};

// Local field offsets left out of the networked offsets ("temporarily adding to ignore list")
struct FlattenedSerializerIgnoredOffsets_t
{
	const char *m_pszSerializerName;
	CUtlHashtable< uint16 > m_Offsets;
	uint16 m_nMaxOffset;
};

// Recipients computed by a send proxy for a field path, or for a range of field paths
struct CSendProxyData
{
	const char *m_pszName; // Taken from the proxy record, "" when it has none
	CFieldPath m_Path;
	CFieldPath m_EndPath;
	CPlayerBitVec m_Recipients; // Filled by the proxy's recipients filter callback
	bool m_bIsRange; // m_EndPath differs from m_Path
};

// Send proxy results gathered while encoding, kept sorted by path
struct CSendProxyDataList
{
	CUtlVector< CSendProxyData > m_Data;
	int64 m_nUniqueId; // The engine assigns it from a global counter
	uint8 m_bSkipProxies; // Named by what it does: when set, the proxies are not called and every recipient mask is cleared
	bool m_bProxyReturnedNoRecipients; // Named by what it does: set when a proxy returns no recipients
	bool m_bGathered; // The proxies are evaluated only while it is false
};

// MNetworkUserGroupProxy of a flattened serializer with the field path ranges of the fields in the user group
// (72 bytes on Windows, 80 on Linux: the proxy is a member function pointer)
struct FlattenedSerializerUserGroup_t
{
	CSendProxyDataList m_SendProxyData; // One entry per contiguous range of field paths, rebuilt by BuildUserGroupPaths_R
	NetworkRecipientsFilter_t::CFilterOwner *m_pProxyOwner; // UserGroupProxy_t::m_ProxyOwner
	NetworkRecipientsFilter_t::FilterCb m_pfnProxy; // UserGroupProxy_t::m_ProxyFn
	const char *m_pszUserGroup; // UserGroupProxy_t::m_UserGroup, compared by UserGroupHasFields
	uint8 m_nProxyIndex; // Named by what it does: proxy index while the hierarchy is built (at most 255 proxies), 0xFF when unset
	NetworkRecipientsFilter_t::FilterOwnerType m_nProxyOwnerType; // UserGroupProxy_t::m_ProxyOwnerType, Filter_Other by default
};

// Include/exclude decisions for the fields of a flattened serializer while it is built
// (104 bytes; spewed by net_why_field_excluded as "Include/Exclude reasons for field %s::%s")
struct FlattenedSerializerExcludeInfo_t
{
	CFlattenedSerializer *m_pSerializer;
	INetworkSerializerBindingBuildFilter *m_pFilter; // BuildFlattenedSerializerInfo_t::m_pFilter
	CUtlHashtable< NetworkFieldSerializer_t *, bool > m_FieldIncluded; // Fields not in it, or mapped to false, are culled
	CUtlHashtable< uint32, int > m_FieldIndexByName; // Field name hash to field index, rebuilt after the fields are gathered
	CUtlString m_WhyClassName; // net_why_field_excluded <classname>
	CUtlString m_WhyFieldName; // net_why_field_excluded <fieldname>
	CUtlStringToken m_WhyFieldNameHash;
	bool m_bWhyHeaderSpewed; // The "Include/Exclude reasons" header line was written
};

// Flattened network serializer of a class: its fields with the embedded structs flattened into them.
// Layout from the constructor, destructor and CopyFrom of networksystem (560 bytes, "sizeof serializer %d").
class CFlattenedSerializer
{
public:
	const char *m_pszName; // Logging, spew, hash bucket key.
	CNetworkSerializer *m_pNetworkSerializer;
	FlattenedSerializerExcludeInfo_t *m_pExcludeInfo; // Include/exclude decisions of the fields, reported by net_why_field_excluded
	CSendNode *m_pRootSendNode;
	CFlattenedSerializer *m_pNextPermutation; // Next serializer with the same name ("has %d CFlattendSerializer permutations")
	CUtlVector< FlattenedSerializerField_t > m_Fields;
	CUtlVector< uint16 > m_EmbeddedChainIndices; // Indices in m_EmbeddedSerializers, ranges given by m_EmbeddedChains
	CUtlVector< FlattenedSerializerEmbeddedChain_t > m_EmbeddedChains;
	CUtlVector< CFlattenedSerializer * > m_ChildSerializers; // Serializers of embedded struct fields, freed with this one when m_bOwnedByParent is set on them
	CUtlVector< FlattenedSerializerCulledField_t > m_CulledFields;
	CUtlVector< CSendNode * > m_SendNodes; // FlattenedSerializerField_t::m_nSendNodeIndex indexes it
	CUtlVector< CUtlStringToken > m_DisabledFields; // Field name hashes turned off by SetFieldEnabled
	CUtlHashtable< uint16, uint16 > m_OffsetToFieldIndex; // Fast path of OffsetToFieldPath, built when m_bBuildOffsetLookup is set
	FlattenedSerializerIgnoredOffsets_t m_IgnoredOffsets;
	CUtlVector< FlattenedSerializerEmbedded_t > m_EmbeddedSerializers;
	CUtlVector< CFlattenedSerializer * > m_PolymorphicSerializers; // Variants of the polymorphic fields
	CUtlVector< uint16 > m_SendProxyFieldIndices; // Fields with a send proxy recipients filter
	CUtlVector< uint16 > m_NonScalarFieldIndices; // Struct, array and vector fields, the only ones walked with net_gather_child_fields_only
	CUtlVector< FlattenedSerializerUserGroup_t > m_UserGroups; // MNetworkUserGroupProxy records with the paths of their fields
	CUtlVector< uint16 > m_NetworkVarChainOffsets; // Offsets of the network var chainers, from CNetworkSerializerClassInfo::m_MemberOffsets
	CSendProxyDataList m_SendProxyData;
	CSendProxyDataList m_SecondarySendProxyData;
	CUtlVector< FlattenedSerializerClone_t > m_Clones;
	CUtlString m_FakeName; // Name storage of a fake serializer, m_pszName points into it
	int m_nPermutationIndex; // Position in the m_pNextPermutation chain, written as the serializer version
	int m_nMetaVariant; // Meta variant matched by MakeSerializersMatchByMeta, -1 when none
	uint8 m_nSharedChangeCallbackCount;
	uint8 m_bOwnedByParent : 1; // Cleared when the serializer is registered in a bucket
	uint8 m_bWritten : 1; // Set by MaybeWriteFlattenedSerializers_R
	uint8 m_bBuildOffsetLookup : 1; // Copied from BuildFlattenedSerializerInfo_t::m_bServer, m_OffsetToFieldIndex is built
	uint8 m_bVisited : 1; // Set while the referenced serializers are collected
	uint8 m_bNetworkVarChainOffsetsBuilt : 1;
	uint8 m_bSpewCreate : 1; // The name matches "-networkspewcreate", the serializer is spewed once built
	uint8 m_bFake : 1; // Created from metadata ("%s!fake")
	uint8 m_bHasPolymorphicFields : 1; // HasPolymorphicFields
	uint8 m_bIsClone : 1; // Set by CopyFrom, the field lists are not freed by the destructor
	uint8 m_bValidated : 1; // The network var checks ("MNetworkEnable but not using CNetworkVar") ran
};

struct CFlattenedSerializerBucket_t
{
	CAtomicMutex m_Mutex;
	CUtlVector< FlattenedSerializerDesc_t > m_Serializers; // Symbol-indexed serializers in bucket.
	CUtlSymbolTable m_SymbolTable; // Symbol table used to map serializer names to bucket entries.
	CThreadSpinRWLock m_SymbolTableLock;
};

// One written field as reported to serializer spew listeners (the VConsole2 flattened serializer view).
// Filled per field by CFlattenedSerializer::WriteFieldList.
// The comments give the key each member is exported under to VConsole2.
struct FlattenedSerializerSpewField_t
{
	CEntityIndex m_nEntityIndex;
	CFieldPath m_Path; // "path"
	CUtlString m_sShortName; // "shortname"; field name, with "[n]" appended for array elements
	CUtlString m_sFullPath; // "fullpath"
	CUtlString m_sUserGroup; // "usergroup"
	CUtlString m_sBits; // "bits"; bit count as text
	CUtlString m_sValue; // "value"
	CUtlString m_sSerializer; // "serializer"
	CUtlString m_sEncoder; // "encoder"
	CUtlString m_sSendNode; // "sendnode"
	CUtlString m_sType; // "type"; " *" is appended for polymorphic pointers
	CUtlString m_sNotes; // "notes"
	int m_nRecipient; // "recip"; listeners filter on it, -1 means any
	CEntityIndex m_nObjectID; // "objectid"; listeners filter on it
	int m_nReservedIndex; // Named by what the code does with it: set to -1 with m_nArrayElement, no writer or reader found in networksystem or engine2
	int m_nArrayElement; // Set only for array element fields
	int m_nStartBit;
	int m_nEndBit;
	bool m_bIsNullPtr; // "is_nullptr"
};

struct FlattenedSerializerListener_t;
struct FlattenedSerializerFieldData_t;
struct FlattenedSerializerFieldNamePool_t;

abstract_class IFlattenedSerializers
{
public:
	virtual bool BuildEntityClassSerializers( BuildFlattenedSerializerInfo_t *pClasses, int nClassCount ) = 0; // "BuildEntityClassNetworkSerializer"
	virtual CUtlString GetDebugFieldPathName( const FlattenedSerializerDesc_t &pSerializer, const CFieldPath &pPath, CEntityInstance *pEntity, int nBucket, bool bUseFieldPath, bool bServer ) = 0; // Formats sentinel and field-path names for debug output.
	virtual const char *FieldPathToName( const FlattenedSerializerDesc_t &pSerializer, const CFieldPath &pPath, CEntityInstance *pEntity, int nBucket, CUtlString *pOut ) = 0; // Converts a CFieldPath to a display name and falls back to "unknown"/"???"
	virtual const char *FieldPathIndexToName( const FlattenedSerializerDesc_t &pSerializer, int nFieldPath, CEntityInstance *pEntity, int nBucket, CUtlString *pOut ) = 0; // Converts a packed field-path index to a name.
	virtual bool ReadFields( const FlattenedSerializerDesc_t &pSerializer, bf_read *pBuf, SerializedEntityData_t *pOut, int nEntityIndex, NetworkSerializationMode_t eSerializationMode, bool bTrackEncodingStats ) = 0; // "ReadFields"; bTrackEncodingStats reports every read field to CNetworkEncodingStats when it is collecting
	virtual bool Encode( const FlattenedSerializerDesc_t &pSerializer, SerializedEntityData_t *pOut, CEntityInstance *pEntity, int nEntityIndex, NetworkSerializationMode_t eSerializationMode, CSendProxyDataList *pSendProxyData ) = 0; // Uses a local list when pSendProxyData is NULL
	virtual bool GatherSendProxyResults( const FlattenedSerializerDesc_t &pSerializer, CEntityInstance *pEntity, int nEntityIndex, NetworkSerializationMode_t eSerializationMode, FlattenedSerializerChangeArray_t *pOutChangeInfo ) = 0; // "GatherSendProxyResults"
	virtual bool DecodeEntity( const FlattenedSerializerDesc_t &pSerializer, const SerializedEntityData_t *pFrom, void *pDestObject, int nEntityIndex, NetworkSerializationMode_t eSerializationMode, INetworkFieldChangedEventQueue *pChangedEventQueue, int nFlags ) = 0; // "CFlattenedSerializer::Decode"; field change callbacks are queued only when pChangedEventQueue is set
	// Reads the fields from pBuf and decodes them into pDestObject in one pass, without handing the intermediate SerializedEntityData_t out.
	// pOutFieldCount receives the number of fields read; it is zeroed first when it is not NULL.
	virtual bool ReadAndDecodeEntity( const FlattenedSerializerDesc_t &pSerializer, bf_read *pBuf, void *pDestObject, int nEntityIndex, NetworkSerializationMode_t eSerializationMode, INetworkFieldChangedEventQueue *pChangedEventQueue, int *pOutFieldCount ) = 0; // "CFlattenedSerializer::DecodeSerializedEntity"
	virtual int CalcDelta( const FlattenedSerializerDesc_t &pSerializer, const SerializedEntityData_t *pFrom, const SerializedEntityData_t *pTo, CUtlLeanVectorFixedGrowable< int, 4 > *pOutFieldPaths, int nEntityIndex, NetworkSerializationMode_t eSerializationMode, int *pOutChanges ) = 0; // "CDeltaCalculator::FieldCalcDelta"

	virtual SerializedEntityData_t *BuildDeltaProperties( const FlattenedSerializerDesc_t &pSerializer, CEntityInstance *pEntity, int nEntityIndex, NetworkSerializationMode_t eSerializationMode, const SerializedEntityData_t *pData, CUtlLeanVectorFixedGrowable< int, 4 > *pFieldPaths, CUtlLeanVectorFixedGrowable< int, 4 > *pOutFieldPaths, CSendProxyDataList *pSendProxyData, bool *pbOut1, bool *pbOut2 ) = 0;

	// Copies the fields of pData into pBuf, only those whose encoded field path is in pFieldPaths when it is not NULL (the engine passes the CalcDelta changes).
	// Each written field is reported to the spew listeners a FlattenedSerializerSpewField_t, filtered by nEntityIndex and nRecipient. Always returns true.
	virtual bool WriteFieldList( const FlattenedSerializerDesc_t &pSerializer, bf_write *pBuf, const SerializedEntityData_t *pData, int nEntityIndex, NetworkSerializationMode_t eSerializationMode, const CUtlLeanVectorFixedGrowable< int, 4 > *pFieldPaths, int nRecipient ) = 0; // "CFlattenedSerializer::WriteFieldList"
	virtual bool MergeDeltas( const FlattenedSerializerDesc_t &pSerializer, const SerializedEntityData_t *pOld, const SerializedEntityData_t *p2, SerializedEntityData_t *p3, int nEntityIndex, NetworkSerializationMode_t eSerializationMode, int *pOut ) = 0; // "CFlattenedSerializer::MergeDeltas"
	virtual bool BuildMergedSerializedEntity( const FlattenedSerializerDesc_t &pSerializer, byte *pDeltaData, SerializedEntityData_t *pBase, CUtlVector< int > *pFieldPaths, bool bCull, int nEntityIndex ) = 0; // "BuildMergedSerializedEntity"
	virtual int CullUnchangedFieldPaths( const FlattenedSerializerDesc_t &pSerializer, int nPlayerSlot, int nEntityIndex, CUtlLeanVectorFixedGrowable< int, 4 > *pFieldPaths, const CSendProxyDataList *pSendProxyData, CUtlLeanVectorFixedGrowable< int, 4 > *pOutFieldPaths ) = 0;
	virtual uint32 RemoveArrayElementsOutsideOfArrayMetadataBounds( const FlattenedSerializerDesc_t &pSerializer, SerializedEntityData_t *pData, uint32 *pInOutCount, int nEntityIndex, int nFlags ) = 0; // Returns *pInOutCount unchanged without the serializer or the data
	virtual void SpewSerializer( const FlattenedSerializerDesc_t &pSerializer, int nBucket, IFlattenedSerializerSpewFunc *pSpew ) = 0;
	virtual bool MakeSerializersMatchByMeta( const char *pszSerializerName, FlattenedSerializerFieldPathMap_t *pFieldPathMap, const FlattenedSerializerDesc_t &pSerializer, int nBucket, int nFlags, bool bCreateMissing ) = 0; // Rebuilds serializer field paths from metadata when layouts differ.
	virtual bool CreateReplayCompatSerializerFromMeta( const char *pszSerializerName, FlattenedSerializerDesc_t *pOut, int nBucket, bool bCreateFake, FlattenedSerializerUserData_t *pContext ) = 0; // Creates replay-compatible metadata serializers and optional "%s!fake" entries.
	virtual void BuildFlattenedFieldPathMap( const FlattenedSerializerDesc_t &pSerializer, const NetworkEntityData_t *pEntityData, int nBucket, FlattenedSerializerFieldPathMap_t *pOut, FlattenedSerializerFieldPathMap_t *pScratch ) = 0; // Builds offset-to-field-path mappings for flattened network data.
	virtual void ResolvePathToOffset( const FlattenedSerializerDesc_t &pSerializer, const char *pszSerializerName, const CFieldPath &pPath, int nBucket, FlattenedSerializerFieldPathMap_t *pMap, CUtlVector< int > *pOutOffsets, CUtlVector< int > *pOutFieldPaths ) = 0; // Resolves field paths to offsets and logs "Couldn't resolve offset"
	virtual CFieldPath OffsetToFieldPath( const FlattenedSerializerDesc_t &pSerializer, const void *pBase, const void *pField, bool *pOutExact ) = 0; // Converts the offset of pField from pBase to a field path; "CNetworkUtlVectorEmbedded: successfully late-resolved"

	// The metadata helper and the int are not read
	virtual bool GetFieldPathChildIndices( const FlattenedSerializerDesc_t &pSerializer, CEntityInstancePolymorphicMetadataHelper *, int, const CFieldPath &path, CUtlVector< int > *pOutChildIndices ) = 0;

	// AMNOTE: GatherSendProxyResults starting at path
	virtual bool GatherSendProxyResultsForPath( const FlattenedSerializerDesc_t &pSerializer, CEntityInstance *pEntity, CEntityInstancePolymorphicMetadataHelper *pMetadataHelper, int nEntityIndex, NetworkSerializationMode_t eSerializationMode, const CFieldPath *pRootPath, FlattenedSerializerChangeArray_t *pOut ) = 0;
	virtual const FlattenedSerializerFieldData_t *GetFieldSerializerData( const FlattenedSerializerDesc_t &pSerializer, const CFieldPath &pPath, CEntityInstance *pEntity, int nBucket ) = 0;
	virtual bool HasField( const FlattenedSerializerDesc_t &pSerializer, const char *pszFieldName ) = 0; // Scans field and subfield names.
	virtual bool HasFieldInternal( const FlattenedSerializerDesc_t &pSerializer, const char *pszFieldName ) = 0; // Scans flattened fields directly by name
	virtual void ValidateSerializedEntity( const FlattenedSerializerDesc_t &pSerializer, const SerializedEntityData_t *pData, int nEntityIndex, int nFlags ) = 0; // "CFlattenedSerializer::ValidateSerializedEntity"

	// nEntityIndex is -1 when not tied to an entity
	virtual void DumpSerializedEntityToConsole( const FlattenedSerializerDesc_t &pSerializer, const char *pszLabel, const SerializedEntityData_t *pData, int nEntityIndex, int nFlags ) = 0;
	virtual void DumpSerializedEntityToLines( const FlattenedSerializerDesc_t &pSerializer, const char *pszLabel, const SerializedEntityData_t *pData, int nEntityIndex, int nFlags, CUtlVector< CUtlString > *pOutLines ) = 0; // "CFlattenedSerializer::DumpSerializedEntityToConsole"
	virtual void AddSerializerListener( FlattenedSerializerListener_t *pListener ) = 0; // Appends a listener to m_SpewListeners .
	virtual void RemoveSerializerListener( FlattenedSerializerListener_t *pListener ) = 0; // Removes a listener from m_SpewListeners .
	virtual bool BuildFlattenedSerializersMessage( CUtlVector< FlattenedSerializerDesc_t > *pSerializers, SerializedEntityData_t *pOut ) = 0; // CSVCMsg_FlattenedSerializer_t
	virtual void SpewCounts( int nBucket, bool bVerbose ) = 0;
	virtual void RegisterSaveRestoreOps( FlattenedSerializerSaveRestoreOps_t *pSaveRestoreOps ) = 0; // Appends a unique save/restore ops record
	virtual void UnregisterSaveRestoreOps( FlattenedSerializerSaveRestoreOps_t *pSaveRestoreOps ) = 0; // Removes a save/restore ops record
	virtual INetworkFieldChangedEventQueue *CreateNetworkFieldScratchData( INetworkFieldScratchData *pScratchData, IFieldChangeLimitSpew *pChangeLimitSpew ) = 0; // Allocates a CNetworkFieldChangedEventQueue
	virtual void BuildFieldNamePool( bool bForce ) = 0; // Creates m_pFieldNamePool
	virtual void DestroyFieldNamePool() = 0; // Frees m_pFieldNamePool
	// AMNOTE: Disabled fields are skipped when gathering send proxy results, building deltas and gathering polymorphic metadata
	virtual void SetFieldEnabled( const FlattenedSerializerDesc_t &pSerializer, const char *pszFieldName, bool bEnabled ) = 0; // "Setting FS %s field %s to %s"

	virtual void GatherPolymorphicMetadata( const FlattenedSerializerDesc_t &pSerializer, CEntityInstance *pEntity, int nEntityIndex, NetworkSerializationMode_t eSerializationMode, void *pOutMetadata ) = 0; // "Polymorphic"

	// AMNOTE: Writes the variant class names of metadata gathered by GatherPolymorphicMetadata, the third argument is unused
	virtual void GetPolymorphicMetadataDescription( const FlattenedSerializerDesc_t &pSerializer, const void *pMetadata, void *, CUtlVector< CUtlString > *pOutLines ) = 0;
	virtual bool HasPolymorphicFields( const FlattenedSerializerDesc_t &pSerializer ) = 0; // Returns CFlattenedSerializer::m_bHasPolymorphicFields
	virtual void ResetFieldPathPool() = 0;
	virtual bool UserGroupHasFields( const FlattenedSerializerDesc_t &pSerializer, const char *pszUserGroup ) = 0;
	virtual bool GetFieldPathCaches( const FlattenedSerializerDesc_t &pSerializer, CSendProxyDataList **ppSecondarySendProxyData, CSendProxyDataList **ppSendProxyData ) = 0; // Exposes the serializer's send proxy data lists
};

class CFlattenedSerializers : public IFlattenedSerializers
{
public:
	CConCommandMemberAccessor< CFlattenedSerializers > m_NetPermutationInfoCommand; // Command "net_permutation_info"; shows permutation information for a serializer
	CFlattenedSerializerBucket_t m_SerializerBuckets[32]; // Hash(name) & 0x1F serializer buckets
	CUtlSymbolTableLargeMT m_ReservedSymbolTable; // Named by what the code does with it: only constructed and destroyed
	CUtlVector< FlattenedSerializerListener_t * > m_SpewListeners;
	CUtlVector< FlattenedSerializerSaveRestoreOps_t > m_SaveRestoreOps; // Save/restore operation records registered by RegisterSaveRestoreOps
	FlattenedSerializerFieldNamePool_t *m_pFieldNamePool; // Field-name pool created by BuildFieldNamePool
	CUtlString m_NetworkSpewCreate; // "-networkspewcreate" value
	int m_nBuildThreadCount; // A hashed command line option, 100 without a value; BuildEntityClassSerializers builds in parallel above 1
	int m_nNetworkSpewEntity; // "-networkspewentity", -999 when absent
	int m_nSpewReadFieldListEntIndex; // "fs_spew_readfieldlist entindex <optional min change bytes>"
	int m_nSpewReadFieldListMinChangeBytes;
	bool m_bNetworkSpewCreate; // "-networkspewcreate"
	bool m_bBuildForServer; // Named by what it does: set by the value "server" of the m_nBuildThreadCount option
	bool m_bNetworkSpew; // "-networkspew"
	bool m_bNetworkSpewExcludes; // "-networkspewexcludes"
	bool m_bReservedFlag; // Named by what the code does with it: no access found in networksystem
	bool m_bForceRebuildFieldNamePool;
	bool m_bNetworkVarValidate; // "-networkvar_validate"
	bool m_bBuildingSpewCreateSerializer; // Named by what it does: set while a serializer named by -networkspewcreate is being built
};

#endif /* FLATTENEDSERIALIZERS_H */
