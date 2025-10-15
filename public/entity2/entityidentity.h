#ifndef ENTITYIDENTITY_H
#define ENTITYIDENTITY_H

#if _WIN32
#pragma once
#endif

#define MAX_ENTITIES_IN_LIST 512
#define MAX_ENTITY_LISTS 64 // 0x3F
#define MAX_TOTAL_ENTITIES MAX_ENTITIES_IN_LIST * MAX_ENTITY_LISTS // 0x8000

#include "tier0/utlstring.h"
#include "tier0/utlstringtoken.h"
#include "tier1/utlsymbollarge.h"
#include "tier1/smartptr.h"
#include "entity2/entitycomponent.h"
#include "entityhandle.h"
#include "utldelegateimpl.h"

#define INVALID_SPAWNGROUP_HANDLE ((SpawnGroupHandle_t)-1)

class CEntityClass;
class CEntityInstance;

struct ChangeAccessorFieldPathIndex_t
{
	ChangeAccessorFieldPathIndex_t() { m_Value = -1; }
	ChangeAccessorFieldPathIndex_t( int32 value ) { m_Value = value; }

	ChangeAccessorFieldPathIndex_t& operator=( int32 value ) { m_Value = value; return *this; }

	int32 m_Value;
};


class CEntityOwnerPtr : public CSmartPtr<CEntityInstance, CNullRefCountAccessor>
{

};

class CNetworkVarChainer : public CEntityOwnerPtr
{
public:
	struct ChainUpdatePropagationLL_t
	{
		ChainUpdatePropagationLL_t* pNext;
		CUtlDelegate<void(const CNetworkVarChainer&)> updateDelegate;
	};

	uint8_t unk[24];
	ChangeAccessorFieldPathIndex_t m_PathIndex;
};
static_assert(offsetof(CNetworkVarChainer, m_PathIndex) == 32);

typedef uint32 SpawnGroupHandle_t;
typedef CUtlStringToken WorldGroupId_t;

class CEntityIndex
{
public:
	CEntityIndex( int index )
	{
		_index = index;
	}

	int Get() const
	{
		return _index;
	}

	operator int() const
	{
		return _index;
	}

	bool operator==( const CEntityIndex &other ) const { return other._index == _index; }
	bool operator!=( const CEntityIndex &other ) const { return other._index != _index; }
	
private:
	int _index;
};

enum EntityNetworkingMode_t : uint32
{
	ENTITY_NETWORKING_MODE_DEFAULT = 0,
	ENTITY_NETWORKING_MODE_NETWORKED,
	ENTITY_NETWORKING_MODE_NOT_NETWORKED,
};

enum EntityFlags_t : uint32
{
	EF_IS_INVALID_EHANDLE = 0x1,
	EF_SPAWN_IN_PROGRESS = 0x2,
	EF_IN_STAGING_LIST = 0x4,
	EF_IN_POST_DATA_UPDATE = 0x8,
	EF_DELETE_IN_PROGRESS = 0x10,
	EF_IN_STASIS = 0x20,
	EF_IS_ISOLATED_ALLOCATION_NETWORKABLE = 0x40,
	EF_IS_DORMANT = 0x80,
	EF_IS_PRE_SPAWN = 0x100,
	EF_MARKED_FOR_DELETE = 0x200,
	EF_IS_CONSTRUCTION_IN_PROGRESS = 0x400,
	EF_IS_ISOLATED_ALLOCATION = 0x800,
	EF_HAS_BEEN_UNSERIALIZED = 0x1000,
	EF_IS_SUSPENDED = 0x2000,
	EF_IS_ANONYMOUS_ALLOCATION = 0x4000,
	EF_SUSPEND_OUTSIDE_PVS = 0x8000,
};

// Size: 0x70
class CEntityIdentity
{
public:
	inline CEntityHandle GetRefEHandle() const
	{
		CEntityHandle handle = m_EHandle;
		handle.m_Parts.m_Serial -= (m_flags & EF_IS_INVALID_EHANDLE);

		return handle;
	}
	
	inline const char *GetClassname() const
	{
		return m_designerName.String();
	}

	inline CEntityIndex GetEntityIndex() const
	{
		return m_EHandle.GetEntryIndex();
	}

	inline SpawnGroupHandle_t GetSpawnGroup() const
	{
		return m_hSpawnGroup;
	}

	bool NameMatches( const char* pszNameOrWildcard ) const;
	bool ClassMatches( const char* pszClassOrWildcard ) const;

public:
	CEntityInstance* m_pInstance; // 0x0
	CEntityClass* m_pClass; // 0x8
	CEntityHandle m_EHandle; // 0x10
	int32 m_nameStringableIndex; // 0x14	
	CUtlSymbolLarge m_name; // 0x18
	CUtlSymbolLarge m_designerName; // 0x20
private:
	uint64 m_hPublicScope; // 0x28 - CEntityPublicScriptScope
public:
	EntityFlags_t m_flags; // 0x30	
private:
	SpawnGroupHandle_t m_hSpawnGroup; // 0x34
public:
	WorldGroupId_t m_worldGroupId; // 0x38
	uint32 m_fDataObjectTypes; // 0x3c	
	ChangeAccessorFieldPathIndex_t m_PathIndex; // 0x40
private:
	void* m_pAttributes; // 0x48 - CUtlObjectAttributeTable<CEntityIdentity, CUtlStringToken>
public:
	CEntityIdentity* m_pPrev; // 0x50
	CEntityIdentity* m_pNext; // 0x58
	CEntityIdentity* m_pPrevByClass; // 0x60
	CEntityIdentity* m_pNextByClass; // 0x68
};

#endif // ENTITYIDENTITY_H