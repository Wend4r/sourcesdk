#ifndef ENTITYCLASS_H
#define ENTITYCLASS_H

#if _WIN32
#pragma once
#endif

#include "tier1/utlsymbollarge.h"
#include "tier1/utlvector.h"
#include "tier1/utldict.h"
#include "entity2/entitycomponent.h"
#include "entityhandle.h"
#include "networksystem/iflattenedserializers.h"

enum EntityClassFlags_t
{
	ECF_NOT_NETWORKED						= (1 << 0), // If the EntityClass is non-networkable
	ECF_ALIAS								= (1 << 1), // If the EntityClass is an alias
	ECF_SPAWN_GROUP_HANDLE_INVALID			= (1 << 2), // Don't use spawngroups when creating entity
	ECF_HAS_REQUIRED_ENTITY_HANDLE			= (1 << 3), // Forces m_requiredEHandle on created entities
	ECF_ALWAYS_SPAWN_ON_CLIENT				= (1 << 4),
	ECF_BECOME_SUSPENDED_INSTEAD_OF_DORMANT = (1 << 5), // Suspend entities outside of PVS
	ECF_ANONYMOUS_ENTITY					= (1 << 6), // If the EntityClass is anonymous
	ECF_PRECACHE_NETWORKED_ENTITY_ON_CLIENT	= (1 << 7),
	ECF_UNK001								= (1 << 8),
	ECF_UNK002								= (1 << 9),
	ECF_FORCE_WORLDGROUPID					= (1 << 10) // Forces worldgroupid to be 1 on created entities
};

class CSchemaClassInfo;
class CEntityClass;
class CEntityIdentity;
class CEntitySharedPulseSignature;
class ServerClass;
struct EntInput_t;
struct EntOutput_t;
struct datamap_t;

struct CNetworkSerializerFieldInfo
{
	uint32 m_nHash;
	CUtlString m_pszFieldName;
	CUtlString m_pszTypeName;
	CUtlString m_pszRawType;
	CUtlString m_pszEncodedType;
	uint32 m_nClassHash;
	CUtlString m_pszClassName;
	int32 m_nFieldSize;
	int32 m_nFieldOffset;

private:
	char pad_040[0xD0];

public:
	CUtlString m_pszCodeGenType;

private:
	char pad_118[0x40];
};
static_assert(sizeof(CNetworkSerializerFieldInfo) == 0x158);

struct CNetworkSerializerClassInfo
{
	uint32 m_nHash;
	CUtlString m_pszClassName;
	CUtlVector<CNetworkSerializerFieldInfo*> m_Fields;

private:
	char _pad_028[0x178];

public:
	struct CNetworkSerializerCodeGenDatabase* m_pDatabase;
	int32 m_nClassSize;

private:
	char pad_1AC[0x1C];
};
static_assert(sizeof(CNetworkSerializerClassInfo) == 0x1C8);

struct CNetworkSerializerCodeGenDatabase
{
	struct EnumInfo_t
	{
		int32 m_nValue;
		int8 m_nFlags;
	};

	CUtlString m_ModuleName;
	CUtlDict<CNetworkSerializerClassInfo*> m_ClassInfos;
	CUtlDict<EnumInfo_t> m_EnumInfos;

private:
	CUtlDict<void*> _unk_map_058;

public:
	bool m_bDebugSpew;

private:
	char pad_81[0x27];

public:
	int32 m_nDuplicateCount;
};
static_assert(sizeof(CNetworkSerializerCodeGenDatabase) == 0xB0);

struct EntClassComponentOverride_t
{
	const char* pszBaseComponent;
	const char* pszOverrideComponent;
};

class CEntityClassInfo
{
public:
	const char* m_pszClassname;
	const char* m_pszCPPClassname;
	const char* m_pszDescription;
	CEntityClass *m_pClass;
	CEntityClassInfo *m_pBaseClassInfo;
	CSchemaClassInfo* m_pSchemaBinding;
	datamap_t* m_pDataDescMap;
	datamap_t* m_pPredDescMap;
};

// Size: 0x160
class CEntityClass
{
	struct ComponentOffsets_t
	{
		uint16 m_nOffset;
	};

	struct ComponentHelper_t
	{
		size_t m_nOffset;
		CEntityComponentHelper* m_pComponentHelper;
	};
	
	struct ClassInputInfo_t
	{
		CUtlSymbolLarge m_sName;
		EntInput_t* m_pInput;
	};
	
	struct ClassOutputInfo_t
	{
		CUtlSymbolLarge m_sName;
		EntOutput_t* m_pOutput;
	};
	
public:
	inline CSchemaClassInfo *GetSchemaBinding() const
	{
		return m_pClassInfo->m_pSchemaBinding;
	}

	inline datamap_t *GetDataDescMap() const
	{
		return m_pClassInfo->m_pDataDescMap;
	}
	
public:
	ScriptClassDesc_t* m_pScriptDesc;

	CNetworkSerializerFieldInfo* m_pNetworkSerializerInfo;

	EntInput_t* m_pInputs;
	EntOutput_t* m_pOutputs;
	int m_nInputCount;
	int m_nOutputCount;

#ifdef CS2_BETA
	void* m_pUnk40;
#else
	#ifdef _WIN32
		char pad[48];
	#else
		char pad[16];
	#endif
#endif

	void* m_pfnPulseBindingTraits;

	CEntitySharedPulseSignature* m_pSharedPulseSignature;
	CEntitySharedPulseSignature* m_unk201;

	EntClassComponentOverride_t* m_pComponentOverrides;

	CEntityClassInfo* m_pClassInfo;
	CEntityClassInfo* m_pBaseClassInfo;
	CUtlSymbolLarge m_designerName;

	// Uses FENTCLASS_* flags
	uint m_flags;

	// Special class group?
	int m_Unk1;
	
	uint m_nAllHelpersFlags;

	CUtlVector<ComponentOffsets_t> m_ComponentOffsets;
	CUtlVector<ComponentHelper_t> m_AllHelpers;
	
	ComponentUnserializerClassInfo_t m_componentUnserializerClassInfo;
	
	FlattenedSerializerDesc_t m_flattenedSerializer;

	CUtlVector<ClassInputInfo_t> m_classInputInfos;
	CUtlVector<ClassOutputInfo_t> m_classOutputInfos;
	
	CEntityHandle m_requiredEHandle;

	CEntityClass* m_pNext;
	CEntityIdentity* m_pFirstEntity;
	ServerClass* m_pServerClass;
};

#endif // ENTITYCLASS_H