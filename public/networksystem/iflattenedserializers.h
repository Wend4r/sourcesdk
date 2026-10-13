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
#include "tier0/utllinkedlist.h"
#include "tier0/utlhashtable.h"
#include "tier0/threadtools.h"
#include "tier1/utldelegate.h"
#include "entity2/entityindex.h"

#include "fieldpath.h"

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
class PackedEntity;
class ServerClass;
class IMemAlloc;

enum NetworkSerializationMode_t : int;

struct FlattenedSerializerOwner_t;
struct FlattenedSerializerUserData_t;

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

struct BuildFlattenedSerializerInfo_t
{
	CNetworkSerializerClassInfo *m_pClassInfo;
	const char *m_pszName;
};

struct FlattenedSerializerChange_t
{
	CFieldPath m_Path;
	uint64 m_nData;
	int16 m_nBitCount;
	int16 m_nFieldPathIndex;
	bool m_bExactPath;
	byte m_pad[3];
};

COMPILE_TIME_ASSERT( sizeof( FlattenedSerializerChange_t ) == 48 );

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
	byte m_pad0009[3];
};

COMPILE_TIME_ASSERT( sizeof( SerializedEntityMetadataEntry_t ) == 12 );

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
	byte m_pad003D[3];
};

COMPILE_TIME_ASSERT( sizeof( SerializedEntityData_t ) == 64 );

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

COMPILE_TIME_ASSERT( sizeof( NetworkEntityData_t ) == 56 );

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
	void *m_pUnk1;
	void *m_pUnk2;
	const char *m_pszTypeName; // Spewed as "allocate %s *" / "deallocate %s *".
	void *m_pUnk3;
	void *m_pUnk4;
	CUtlAbstractDelegate m_delegate;
	uint8 m_nUnk1;
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
		void *m_pUnk1;
		void *m_pUnk2; // Spewed through when m_nArrayIndex >= 0.
		int32 m_nArrayIndex; // Passed to the array delegates, -1 when unset.
		uint8 m_nUnk1;
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
	uint8 m_nUnk1;
	void *m_pUnk1;
	void *m_pUnk2;
	CFieldPath m_Path;
	CUtlString m_TypeName;
	void *m_pUnk3;
	void *m_pUnk4;
	int m_nEntityIndex;
	uint8 m_nKind; // 1 "allocate %s *", 2 "deallocate %s *"
	uint8 m_nUnk2;
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
	virtual void QueuePointerRecreation( const NetworkFieldScratchChange_t *pRequest, uint8 nKind, uint8 nUnk ) = 0; // nKind 1 allocates, 2 deallocates, 3 removes a queued deallocation
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

COMPILE_TIME_ASSERT( sizeof( CNetworkFieldChangedEventQueue ) == 232 );

struct FlattenedSerializerPackedField_t
{
	uint32 m_nFlags;
	CUtlString m_Name;
	CUtlString m_NetworkName;
	CUtlString m_TypeName;
	CUtlString m_EncoderName;
	byte m_pad[56];
};

struct FlattenedSerializerPolymorphicField_t
{
	byte m_pad00[64];
	const char *m_pszFieldName;
	byte m_pad48[8];
};

struct FlattenedSerializerFieldPathCache_t
{
	CUtlVector< int > m_FieldPaths; // Cached encoded field paths for serializer traversal.
	const int16 *m_pPath; // Root path used while building or resolving the cache.
	int16 m_nPathCount;
	bool m_bReadOnlyPath;
	byte m_pad0023[5];
};

COMPILE_TIME_ASSERT( sizeof( FlattenedSerializerFieldPathCache_t ) == 40 );

// Size and field offsets are from CFlattenedSerializer allocation/field access.
class CFlattenedSerializer
{
public:
	const char *m_pszName; // Logging, spew, hash bucket key.
	CNetworkSerializerClassInfo *m_pClassInfo; // Schema/network serializer source.
	byte m_pad0010[24];
	int m_nFieldCount; // Field iteration upper bound
	FlattenedSerializerPackedField_t **m_ppFields; // Flattened field table.
	byte m_pad0038[128];
	CUtlVector< int > m_ExcludedFieldIndices; // Console command net_why_field_excluded reports these toggles.
	byte m_pad00D0[32];
	byte m_FieldSerializerData[24]; // Returned by GetFieldSerializerData
	int m_nFieldSerializerDataCount;
	byte m_pad010C[92];
	int m_nPolymorphicFieldCount; // Table count scanned by HasField
	FlattenedSerializerPolymorphicField_t *m_pPolymorphicFields;
	byte m_pad0178[32];
	FlattenedSerializerFieldPathCache_t m_FieldPathCache; // Primary cache for serializer field-path traversal.
	FlattenedSerializerFieldPathCache_t m_SecondaryFieldPathCache; // Secondary cache used by the alternate traversal path.
	byte m_pad01E8[36];
	int m_nScratchPathIndex; // Cleared before scratch path traversal.
	byte m_pad0210;
	byte m_nFlags; // Built, valid, and polymorphic state bits.
	byte m_pad0212[6];
};

COMPILE_TIME_ASSERT( sizeof( CFlattenedSerializer ) == 536 );

struct CFlattenedSerializerBucket_t
{
	CUtlVector< FlattenedSerializerDesc_t > m_Serializers; // Symbol-indexed serializers in bucket.
	byte m_pad0018[16];
	uint m_nSymbolTableGrowSize;
	byte m_pad002C[4];
	CUtlSymbolTable m_SymbolTable; // Symbol table used to map serializer names to bucket entries.
	byte m_pad0090[48];
};
COMPILE_TIME_ASSERT( sizeof( CFlattenedSerializerBucket_t ) == 192 );

// One written field as reported to serializer spew listeners (the VConsole2 flattened serializer view).
// Filled per field by CFlattenedSerializer::WriteFieldList.
// The comments give the key each member is exported under to VConsole2.
struct FlattenedSerializerSpewField_t
{
	CEntityIndex m_nEntityIndex;
	byte m_pad0004[4];
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
	byte m_pad0080[4];
	int m_nArrayElement; // Set only for array element fields
	int m_nStartBit;
	int m_nEndBit;
	bool m_bIsNullPtr; // "is_nullptr"
};
COMPILE_TIME_ASSERT( sizeof( FlattenedSerializerSpewField_t ) == 152 );

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
	virtual bool Encode( const FlattenedSerializerDesc_t &pSerializer, SerializedEntityData_t *pOut, CEntityInstance *pEntity, int nEntityIndex, NetworkSerializationMode_t eSerializationMode, FlattenedSerializerFieldPathCache_t *pFieldPathCache ) = 0; // Uses a local cache when pFieldPathCache is NULL
	virtual bool GatherSendProxyResults( const FlattenedSerializerDesc_t &pSerializer, CEntityInstance *pEntity, int nEntityIndex, NetworkSerializationMode_t eSerializationMode, FlattenedSerializerChangeArray_t *pOutChangeInfo ) = 0; // "GatherSendProxyResults"
	virtual bool DecodeEntity( const FlattenedSerializerDesc_t &pSerializer, const SerializedEntityData_t *pFrom, void *pDestObject, int nEntityIndex, NetworkSerializationMode_t eSerializationMode, INetworkFieldChangedEventQueue *pChangedEventQueue, int nFlags ) = 0; // "CFlattenedSerializer::Decode"; field change callbacks are queued only when pChangedEventQueue is set
	virtual int CalcDelta( const FlattenedSerializerDesc_t &pSerializer, const SerializedEntityData_t *pFrom, const SerializedEntityData_t *pTo, CUtlLeanVectorFixedGrowable< int, 4 > *pOutFieldPaths, int nEntityIndex, NetworkSerializationMode_t eSerializationMode, int *pOutChanges ) = 0; // "CDeltaCalculator::FieldCalcDelta"

	virtual SerializedEntityData_t *BuildDeltaProperties( const FlattenedSerializerDesc_t &pSerializer, CEntityInstance *pEntity, int nEntityIndex, NetworkSerializationMode_t eSerializationMode, const SerializedEntityData_t *pData, CUtlLeanVectorFixedGrowable< int, 4 > *pFieldPaths, CUtlLeanVectorFixedGrowable< int, 4 > *pOutFieldPaths, FlattenedSerializerFieldPathCache_t *pFieldPathCache, bool *pbOut1, bool *pbOut2 ) = 0;

	// Copies the fields of pData into pBuf, only those whose encoded field path is in pFieldPaths when it is not NULL (the engine passes the CalcDelta changes).
	// Each written field is reported to the spew listeners a FlattenedSerializerSpewField_t, filtered by nEntityIndex and nRecipient. Always returns true.
	virtual bool WriteFieldList( const FlattenedSerializerDesc_t &pSerializer, bf_write *pBuf, const SerializedEntityData_t *pData, int nEntityIndex, NetworkSerializationMode_t eSerializationMode, const CUtlLeanVectorFixedGrowable< int, 4 > *pFieldPaths, int nRecipient ) = 0; // "CFlattenedSerializer::WriteFieldList"
	virtual bool MergeDeltas( const FlattenedSerializerDesc_t &pSerializer, const SerializedEntityData_t *pOld, const SerializedEntityData_t *p2, SerializedEntityData_t *p3, int nEntityIndex, NetworkSerializationMode_t eSerializationMode, int *pOut ) = 0; // "CFlattenedSerializer::MergeDeltas"
	virtual bool BuildMergedSerializedEntity( const FlattenedSerializerDesc_t &pSerializer, byte *pDeltaData, SerializedEntityData_t *pBase, CUtlVector< int > *pFieldPaths, bool bCull, int nEntityIndex ) = 0; // "BuildMergedSerializedEntity"
	virtual int CullUnchangedFieldPaths( const FlattenedSerializerDesc_t &pSerializer, int nPlayerSlot, int nEntityIndex, CUtlLeanVectorFixedGrowable< int, 4 > *pFieldPaths, const void *pCullRules, CUtlLeanVectorFixedGrowable< int, 4 > *pOutFieldPaths ) = 0;
	virtual uint32 RemoveArrayElementsOutsideOfArrayMetadataBounds( const FlattenedSerializerDesc_t &pSerializer, SerializedEntityData_t *pData, uint32 *pInOutCount, int nEntityIndex, int nFlags ) = 0; // Returns *pInOutCount unchanged without the serializer or the data
	virtual void SpewSerializer( const FlattenedSerializerDesc_t &pSerializer, int nBucket, IFlattenedSerializerSpewFunc *pSpew ) = 0;
	virtual bool MakeSerializersMatchByMeta( const char *pszSerializerName, FlattenedSerializerFieldPathMap_t *pFieldPathMap, const FlattenedSerializerDesc_t &pSerializer, int nBucket, int nFlags, bool bCreateMissing ) = 0; // Rebuilds serializer field paths from metadata when layouts differ.
	virtual bool CreateReplayCompatSerializerFromMeta( const char *pszSerializerName, FlattenedSerializerDesc_t *pOut, int nBucket, bool bCreateFake, FlattenedSerializerUserData_t *pContext ) = 0; // Creates replay-compatible metadata serializers and optional "%s!fake" entries.
	virtual void BuildFlattenedFieldPathMap( const FlattenedSerializerDesc_t &pSerializer, const NetworkEntityData_t *pEntityData, int nBucket, FlattenedSerializerFieldPathMap_t *pOut, FlattenedSerializerFieldPathMap_t *pScratch ) = 0; // Builds offset-to-field-path mappings for flattened network data.
	virtual void ResolvePathToOffset( const FlattenedSerializerDesc_t &pSerializer, const char *pszSerializerName, const CFieldPath &pPath, int nBucket, FlattenedSerializerFieldPathMap_t *pMap, CUtlVector< int > *pOutOffsets, CUtlVector< int > *pOutFieldPaths ) = 0; // Resolves field paths to offsets and logs "Couldn't resolve offset"
	virtual CFieldPath OffsetToFieldPath( const FlattenedSerializerDesc_t &pSerializer, int nOffset, int nBucket, bool *pOutExact ) = 0; // Converts a networked offset to a field path.

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
	virtual bool HasPolymorphicFields( const FlattenedSerializerDesc_t &pSerializer ) = 0; // Tests CFlattenedSerializer::m_nFlags bit (1 << 7)
	virtual void ResetFieldPathPool() = 0;
	virtual bool UserGroupHasFields( const FlattenedSerializerDesc_t &pSerializer, const char *pszUserGroup ) = 0;
	virtual bool GetFieldPathCaches( const FlattenedSerializerDesc_t &pSerializer, FlattenedSerializerFieldPathCache_t **ppSecondaryFieldPathCache, FlattenedSerializerFieldPathCache_t **ppFieldPathCache ) = 0; // Exposes the serializer field-path caches for callers that need to reuse traversal state.
};

class CFlattenedSerializers : public IFlattenedSerializers
{
public:
	CConCommandMemberAccessor< CFlattenedSerializers > m_NetPermutationInfoCommand; // Command "net_permutation_info"; shows permutation information for a serializer
	CFlattenedSerializerBucket_t m_SerializerBuckets[32]; // Hash(name) & 0x1F serializer buckets
	byte m_pad1848[112];
	CUtlVector< FlattenedSerializerListener_t * > m_SpewListeners;
	CUtlVector< FlattenedSerializerSaveRestoreOps_t > m_SaveRestoreOps; // Save/restore operation records registered by RegisterSaveRestoreOps
	FlattenedSerializerFieldNamePool_t *m_pFieldNamePool; // Field-name pool created by BuildFieldNamePool
	CUtlString m_NetworkSpewCreate; // "-networkspewcreate" value
	int m_nUnk18F8;
	int m_nNetworkSpewEntity; // "-networkspewentity", -999 when absent
	int m_nSpewReadFieldListEntIndex; // "fs_spew_readfieldlist entindex <optional min change bytes>"
	int m_nSpewReadFieldListMinChangeBytes;
	bool m_bNetworkSpewCreate; // "-networkspewcreate"
	bool m_bUnk1909;
	bool m_bNetworkSpew; // "-networkspew"
	bool m_bNetworkSpewExcludes; // "-networkspewexcludes"
	bool m_bUnk190C;
	bool m_bForceRebuildFieldNamePool;
	bool m_bNetworkVarValidate; // "-networkvar_validate"
	bool m_bUnk190F;
};

#endif /* FLATTENEDSERIALIZERS_H */
