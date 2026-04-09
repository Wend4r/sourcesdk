#ifndef ENTITYNETWORK_H
#define ENTITYNETWORK_H

#if _WIN32
#pragma once
#endif

#include "tier1/utlmap.h"
#include "tier1/utlvector.h"
#include "tier0/threadtools.h"
#include "entityidentity.h"

class ServerClass;
class ClientClass;
class CNetworkTransmitComponent;
class CChangeInfoAccessor;
class CEntityInstancePolymorphicMetadataHelper;

struct NetworkStateChanged_t
{
	NetworkStateChanged_t() : m_Unk00(1), m_Unk48(-1), m_nArrayIndex(-1), m_nPathIndex(ChangeAccessorFieldPathIndex_t()), m_Unk60(0) { }
	explicit NetworkStateChanged_t( bool bFullChanged ) : m_Unk00(static_cast<uint32>(!bFullChanged)), m_Unk48(-1), m_nArrayIndex(-1), m_nPathIndex(ChangeAccessorFieldPathIndex_t()), m_Unk60(0) { }

	// nLocalOffset is the flattened field offset
	//		calculated taking into account embedded structures
	//		if PathIndex is specified, then the offset must start from the last object in the chain
	// nArrayIndex is the index of the array element 
	//		if the field is a CNetworkUtlVectorBase, otherwise pass -1
	// nPathIndex is the value to specify 
	//		if the path to the field goes through one or more pointers, otherwise pass -1
	// 		this value is usually a member of the CNetworkVarChainer and belongs to the last object in the chain
	NetworkStateChanged_t( uint32 nLocalOffset, int32 nArrayIndex = -1, ChangeAccessorFieldPathIndex_t nPathIndex = ChangeAccessorFieldPathIndex_t() ) : m_Unk00(1), m_LocalOffsets{ nLocalOffset }, m_Unk48(-1), m_nArrayIndex(nArrayIndex), m_nPathIndex(nPathIndex), m_Unk60(0) { }
	NetworkStateChanged_t( CUtlVector<uint32> vecLocalOffsets, int32 nArrayIndex = -1, ChangeAccessorFieldPathIndex_t nPathIndex = ChangeAccessorFieldPathIndex_t() ) : m_Unk00(1), m_LocalOffsets(Move(vecLocalOffsets)), m_Unk48(-1), m_nArrayIndex(nArrayIndex), m_nPathIndex(nPathIndex), m_Unk60(1) { }

	uint32 m_Unk00; // Perhaps it is an enum, default 1, when 0 adds FL_FULL_EDICT_CHANGED
	CUtlVector<uint32> m_LocalOffsets;
	// Probably only works in the debug build, as it has always been empty
	CUtlString m_ClassName;
	CUtlString m_FieldName;
	int32 m_Unk48; // default -1
	int32 m_nArrayIndex; // default -1
	ChangeAccessorFieldPathIndex_t m_nPathIndex; // default -1 (can also be -2)
	int16 m_Unk60; // default 0, if m_LocalOffsets has multiple values, it is set to 1
};
COMPILE_TIME_ASSERT( sizeof( NetworkStateChanged_t ) == 64 );

// Not entirely sure
struct NetworkStateChanged3_t
{
	CUtlVector<uint32> m_Unk0;
	CUtlVector<uint32> m_Unk24;
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
	uint16 m_nPad;
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

struct NetworkSharedChangeInfoOverflow_t
{
	uint8 m_pad000[0x18];
	void *m_pRootPathOffsetLookup;
	CUtlVectorFixedGrowableCompat< void *, 4 > m_PathOffsetLookups;
	uint8 m_pad058[0x08];
	NetworkStateChangedLookupCache_t m_CachedIgnoredLookups;
	NetworkStateChangedLookupCache_t m_CachedTrackedLookups;
	uint8 m_pad170[0x10];
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
static_assert( sizeof( Entity2Networkable_t ) == 96 );

class CNetworkTransmitComponent
{
public:
	virtual ~CNetworkTransmitComponent() = default;

public:
	NetworkSharedChangeInfoOverflow_t *m_pSharedChangeInfoOverflow; // Allocated by StateChanged / StateChangedBranch, owns path lookup caches
	void *m_pNetworkStateChangedRouter; // SetNetworkStateChangedRouter, wrappers check byte +0x18 on this object
	uint32 m_nNetworkStateChangedRouterData;
	uint32 m_nUnk024;
	uint8 m_pad028[0x100];
	uint8 m_pad120[0x08];

	CThreadRWLock_FastRead m_StateChangeLock;

#if defined( POSIX )
	uint8 m_pad2C0[24];
#else
	uint8 m_pad2C0[368];
#endif

	uint32 m_nStateChangeFlags; // PackEntity helper
	uint8 m_nUnk2DC;
	bool m_bPendingStateChange;
	uint8 m_pad2DE[2];
	bool m_bNetworkUpdatesDisabled;
	uint8 m_pad2E1[3];
	int32 m_nUnk2E4;
	Entity2Networkable_t *m_pNetworkable; // StateChanged walks this back to CEntityInstance / CEntityIdentity
	int32 m_nUnk2F0;
	uint16 m_nSharedChangeInfoIndex;
	uint16 m_nSharedChangeInfoSerial; // Serial paired with m_nSharedChangeInfoIndex
	uint8 m_pad2F8[0x28];
	void *m_pUnk320;
	bool m_bUnk328;
	uint8 m_pad329[7];
};
COMPILE_TIME_ASSERT( sizeof( CNetworkTransmitComponent ) == 816 );

class IEntity2Networkables
{
public:
	virtual ~IEntity2Networkables() = 0;

	virtual bool GetEntity2Networkable( CEntityIndex nEntryIndex, Entity2Networkable_t *info ) = 0;
	virtual CUtlMap< int, Entity2Networkable_t > &GetEntity2Networkables( void ) const = 0;
	virtual void Unk01() = 0;
	virtual void Unk02() = 0;
	virtual ServerClass **GetClassList() = 0;
};

class CEntity2NetworkClasses : public IEntity2Networkables, public IEntityListener
{
public:
	ServerClass *m_pClassList;
	void *m_pPad[2];
	CUtlMap< int, Entity2Networkable_t > m_NetworkedEntities2;
};
COMPILE_TIME_ASSERT( sizeof( CEntity2NetworkClasses ) == 72 );

#endif // ENTITYNETWORK_H
