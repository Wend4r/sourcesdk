#ifndef FLATTENEDSERIALIZERS_H
#define FLATTENEDSERIALIZERS_H

#pragma once

#include "tier0/basetypes.h"
#include "tier0/utlsymbol.h"
#include "tier0/utlstring.h"
#include "tier1/convar.h"
#include "tier1/utlsymbollarge.h"
#include "tier1/utlvector.h"
#include "tier1/utlleanvector.h"
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

struct FlattenedSerializerOwner_t;
struct FlattenedSerializerUserData_t;

struct FlattenedSerializerDesc_t
{
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
	uint m_nDataBytes;
	uint m_nAllocatedDataBytes;
	int m_nFlags;
	int m_nBaseline;
	int m_nFieldCount; // Number of fields in m_pFieldPaths and m_pFieldBitOffsets.
	int m_nBitCount; // Total number of valid bits in m_pData.
};

COMPILE_TIME_ASSERT( sizeof( SerializedEntityData_t ) == 56 );

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

struct NetworkFieldScratchChange_t
{
	const char *m_pszFieldName; // Field owner or serializer name used by debug spew.
	byte *m_pField; // Pointer captured by queue/rebase/shrink processing.
	uintp m_nFieldOffset;
	uintp m_nFieldSize;
	FlattenedSerializerUserData_t *m_pContext;
	byte m_nFieldType;
	byte m_pad0021[7];
	CFieldPath m_Path;
	CUtlString m_DebugName;
	byte m_pad0060[24];
	int m_nEntityIndex;
	byte m_nChangeKind;
	bool m_bRemoved;
	bool m_bInvalid;
	byte m_pad0077;
	CUtlVector< CFieldPath > *m_pReplayCompatPaths;
	int m_nPrev;
	int m_nNext;
};

struct NetworkFieldScratchThreadQueue_t
{
	CAtomicMutex m_Mutex; // Used around per-thread queue writes.
	CUtlVector< byte > m_FieldChangeMarks; // Bit/byte marks for fields that have queued changes.
	int m_nChangeCount;
	int m_nChangeCapacity;
	NetworkFieldScratchChange_t *m_pChanges;
	int m_nHead;
	int m_nTail;
	int m_nQueuedCount;
	byte m_pad0044[4];
};

COMPILE_TIME_ASSERT( sizeof( NetworkFieldScratchThreadQueue_t ) == 72 );

struct NetworkFieldScratchMemoryStackList_t
{
	CMemoryStack **m_ppMemoryStacks;
	int m_nMemoryStackCount;
};

class INetworkFieldScratchAllocator
{
public:
	virtual ~INetworkFieldScratchAllocator() {}
	virtual void FreeAll() = 0;
	virtual byte *Alloc( uint nBytes, uintp nFieldOffset, uint nFieldType, uintp nFieldSize, uintp nFieldName, uintp nNetworkName, uintp nOwner, uintp nContext, uintp nSerializer, uintp nSerializerField, uintp nFieldPath, uintp nDebugName ) = 0;
};

abstract_class INetworkFieldScratchData
{
public:
	virtual ~INetworkFieldScratchData() {}
	virtual void ResetFrameAllocator() = 0; // Resets the backing allocator and clears m_nCurrentChangeFrame.
	virtual void ClearQueuedChanges() = 0;
	virtual bool QueueChange( int nEntityIndex, const CFieldPath &path, const char *pszFieldName, byte *pField, uintp nFieldOffset, uintp nFieldSize, int nChangeFrame, uint nFieldType, FlattenedSerializerUserData_t *pContext, int nFieldBytes, const char *pszDebugName, int *pArrayIndex ) = 0;
	virtual bool QueueReplayCompatFieldChange( int nEntityIndex, const CFieldPath &path, const char *pszFieldName, byte *pField, uintp nFieldOffset, int nChangeFrame, FlattenedSerializerUserData_t *pContext, byte nFlags, const CFieldPath *pReplayPath ) = 0;
	virtual void ProcessQueuedChanges( uint nChangeFrame ) = 0;
	virtual void RebaseQueuedDelegates( bool bSpew, byte *pOldBase, byte *pNewBase, uintp nSize ) = 0;
	virtual void MarkShrunkDelegatesInvalid( bool bSpew, byte *pOldBase, byte *pOldEnd ) = 0;
	virtual void SetContext( FlattenedSerializerUserData_t *pContext ) = 0;
	virtual void SetDebugCallback( bool bEnabled, const char *pszFieldName ) = 0;
	virtual bool IsDebugCallbackEnabled() const = 0;
	virtual void SetDebugFieldName( const char *pszFieldName ) = 0;
	virtual void QueuePointerRecreation( const NetworkFieldScratchChange_t *pChange, bool bSpew, FlattenedSerializerUserData_t *pContext ) = 0;
};

class CNetworkFieldScratchData : public INetworkFieldScratchData
{
public:
	CUtlVector< NetworkFieldScratchThreadQueue_t > m_ThreadQueues; // One queue per worker thread.
	CUtlVector< int > m_EntityChangeFrames; // Snapshot of network change frames from the entity system.
	byte m_EntityChangeFrameStorage[128]; // Inline storage used before m_EntityChangeFrames grows.
	CUtlString m_DebugCallbackName; // Field name that triggers DebugCallback spew.
	CUtlString m_DebugFieldName; // Extra debug field filter.
	INetworkFieldScratchAllocator *m_pAllocator; // Scratch allocator used by QueueChange.
	FlattenedSerializerUserData_t *m_pContext; // Context set through the vtable setter.
	int m_nCurrentChangeFrame; // Last processed change frame.
	int m_nEntityChangeFrameCount; // Count copied from the entity system.
	bool m_bDebugQueueChanges; // Enables QueueChange spew.
	byte m_pad00E1[7];
};

COMPILE_TIME_ASSERT( sizeof( CNetworkFieldScratchData ) == 232 );

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
	virtual bool ReadFields( const FlattenedSerializerDesc_t &pSerializer, bf_read *pBuf, SerializedEntityData_t *pOut, int nEntityIndex, bool bUnk1, bool bUnk2 ) = 0; // "ReadFields"
	virtual bool Encode( const FlattenedSerializerDesc_t &pSerializer, int nEntityIndex, int nSerialNumber, int nBucket, int nBaseline, FlattenedSerializerEncodeResult_t *pOut ) = 0; // Used by PackEntity for the non-delta encode path.
	virtual bool GatherSendProxyResults( const FlattenedSerializerDesc_t &pSerializer, CEntityInstance *pEntity, int nEntityIndex, int nUnk, FlattenedSerializerChangeArray_t *pOutChangeInfo ) = 0; // "GatherSendProxyResults"
	virtual bool DecodeEntity( const FlattenedSerializerDesc_t &pSerializer, int nEntityIndex, int nSerialNumber, int nBucket, int nBaseline, SerializedEntityData_t *pFrom, int nFlags ) = 0; // "CFlattenedSerializer::Decode"
	virtual void CalcDelta( const FlattenedSerializerDesc_t &pSerializer, const SerializedEntityData_t *pFrom, const SerializedEntityData_t *pTo, int *pUnk, int nEntityIndex, int nUnk, int *pOutChanges ) = 0; // "CDeltaCalculator::FieldCalcDelta"

	// Argument types of BuildDeltaProperties and MergeDeltas are not verified
	virtual void BuildDeltaProperties( const FlattenedSerializerDesc_t &pSerializer, void *p1, int nEntityIndex, int nUnk, const SerializedEntityData_t *pData, const CUtlVector< int > *pFieldPaths, void *pOutFieldData, void *p2, bool *pbOut1, bool *pbOut2 ) = 0; // "CFlattenedSerializer::BuildDeltaProperties", clears both flags first

	// Copies the fields of pData into pBuf, only those whose encoded field path is in pFieldPaths when it is not NULL (the engine passes the CalcDelta changes).
	// Each written field is reported to the spew listeners a FlattenedSerializerSpewField_t, filtered by nEntityIndex and nRecipient. Always returns true.
	virtual bool WriteFieldList( const FlattenedSerializerDesc_t &pSerializer, bf_write *pBuf, const SerializedEntityData_t *pData, int nEntityIndex, int nUnk, const CUtlLeanVectorFixedGrowable< int, 4 > *pFieldPaths, int nRecipient ) = 0; // "CFlattenedSerializer::WriteFieldList"
	virtual void MergeDeltas( const FlattenedSerializerDesc_t &pSerializer, const SerializedEntityData_t *pOld, void *p2, void *p3, int nEntityIndex, bool bUnk, int *pOut ) = 0; // "CFlattenedSerializer::MergeDeltas"
	virtual bool BuildMergedSerializedEntity( const FlattenedSerializerDesc_t &pSerializer, byte *pDeltaData, SerializedEntityData_t *pBase, CUtlVector< int > *pFieldPaths, bool bCull, int nEntityIndex ) = 0; // "BuildMergedSerializedEntity"
	virtual int CullUnchangedFieldPaths( const FlattenedSerializerDesc_t &pSerializer, int nEntityIndex, CUtlVector< int > *pFieldPaths, SerializedEntityData_t *pSerialized, CUtlVector< int > *pOutFieldPaths ) = 0; // Filters unchanged paths and logs "culled".
	virtual uint32 RemoveArrayElementsOutsideOfArrayMetadataBounds( const FlattenedSerializerDesc_t &pSerializer, SerializedEntityData_t *pData, uint32 *pInOutCount, int nEntityIndex, int nFlags ) = 0; // Returns *pInOutCount unchanged without the serializer or the data
	virtual void SpewSerializer( const FlattenedSerializerDesc_t &pSerializer, int nBucket, IFlattenedSerializerSpewFunc *pSpew ) = 0;
	virtual bool MakeSerializersMatchByMeta( const char *pszSerializerName, FlattenedSerializerFieldPathMap_t *pFieldPathMap, const FlattenedSerializerDesc_t &pSerializer, int nBucket, int nFlags, bool bCreateMissing ) = 0; // Rebuilds serializer field paths from metadata when layouts differ.
	virtual bool CreateReplayCompatSerializerFromMeta( const char *pszSerializerName, FlattenedSerializerDesc_t *pOut, int nBucket, bool bCreateFake, FlattenedSerializerUserData_t *pContext ) = 0; // Creates replay-compatible metadata serializers and optional "%s!fake" entries.
	virtual void BuildFlattenedFieldPathMap( const FlattenedSerializerDesc_t &pSerializer, const NetworkEntityData_t *pEntityData, int nBucket, FlattenedSerializerFieldPathMap_t *pOut, FlattenedSerializerFieldPathMap_t *pScratch ) = 0; // Builds offset-to-field-path mappings for flattened network data.
	virtual void ResolvePathToOffset( const FlattenedSerializerDesc_t &pSerializer, const char *pszSerializerName, const CFieldPath &pPath, int nBucket, FlattenedSerializerFieldPathMap_t *pMap, CUtlVector< int > *pOutOffsets, CUtlVector< int > *pOutFieldPaths ) = 0; // Resolves field paths to offsets and logs "Couldn't resolve offset"
	virtual CFieldPath OffsetToFieldPath( const FlattenedSerializerDesc_t &pSerializer, int nOffset, int nBucket, bool *pOutExact ) = 0; // Converts a networked offset to a field path.

	// The metadata helper and the int are not read
	virtual bool GetFieldPathChildIndices( const FlattenedSerializerDesc_t &pSerializer, CEntityInstancePolymorphicMetadataHelper *, int, const CFieldPath &path, CUtlVector< int > *pOutChildIndices ) = 0;

	virtual bool CollectChanges( const FlattenedSerializerDesc_t &pSerializer, int nEntityIndex, CEntityInstance *pEntity, int nBucket, int nBaseline, const CFieldPath *pRootPath, FlattenedSerializerChangeArray_t *pOut, int nFlags ) = 0; // Collects StateChangedBranch output for the requested root path.
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
	virtual void SpewCounts( const FlattenedSerializerDesc_t &pSerializer, bool bVerbose ) = 0;
	virtual void RegisterSaveRestoreOps( FlattenedSerializerSaveRestoreOps_t *pSaveRestoreOps ) = 0; // Appends a unique save/restore ops record
	virtual void UnregisterSaveRestoreOps( FlattenedSerializerSaveRestoreOps_t *pSaveRestoreOps ) = 0; // Removes a save/restore ops record
	virtual INetworkFieldScratchData *CreateNetworkFieldScratchData( INetworkFieldScratchAllocator *pAllocator, FlattenedSerializerUserData_t *pContext ) = 0; // Allocates a 232-byte scratch-data object and seeds it with allocator/context pointers.
	virtual void BuildFieldNamePool( bool bForce ) = 0; // Creates m_pFieldNamePool
	virtual void DestroyFieldNamePool() = 0; // Frees m_pFieldNamePool
	virtual void SetFieldExcluded( const FlattenedSerializerDesc_t &pSerializer, const CFieldPath &pPath, bool bExcluded ) = 0; // "Setting FS %s field %s"

	virtual void BuildPolymorphicMetadata( const FlattenedSerializerDesc_t &pSerializer, CEntityInstance *pEntity, int nEntityIndex, int nFlags, void *pOutMetadata ) = 0; // "Polymorphic"

	virtual void Unk_DumpSerializerToLines( const FlattenedSerializerDesc_t &pSerializer, void *p1, void *, CUtlVector< CUtlString > *pOutLines ) = 0; // The third argument is not read
	virtual bool IsSerializerPolymorphic( const FlattenedSerializerDesc_t &pSerializer ) = 0; // Tests CFlattenedSerializer::m_nFlags bit (1 << 7)
	virtual void ResetFieldPathPool() = 0;
	virtual bool HasPolymorphicField( const FlattenedSerializerDesc_t &pSerializer, const char *pszFieldName ) = 0; // Scans the polymorphic field table.
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
	byte m_pad18F0[29];
	bool m_bForceRebuildFieldNamePool;
};

#endif /* FLATTENEDSERIALIZERS_H */
