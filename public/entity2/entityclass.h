#ifndef ENTITYCLASS_H
#define ENTITYCLASS_H

#if _WIN32
#pragma once
#endif

#include "tier1/utlsymbollarge.h"
#include "tier1/utlvector.h"
#include "tier1/utldict.h"
#include "entity2/entitycomponent.h"
#include "entity2/entityinstance.h"
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

class CNetworkSerializerClassInfo;
class CSchemaClassInfo;
class CEntityClass;
class CEntityIdentity;
class CEntityClassPulseSignature;
class CPulseAPIExtensionRegistrationContext;
class ServerClass;
class CEntityIOOutput;
struct EntOutput_t;
struct EntInput_t;
struct datamap_t;

typedef void (*BASEPTR)(CEntityInstance *ent);

struct EntClassComponentOverride_t
{
	const char* pszBaseComponent;
	const char* pszOverrideComponent;
};

struct EntComponentNameEntry_t
{
	const char* pszComponentClassName;
	size_t nOffsetInEntity;
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

// Size: 0x168
class CEntityClass
{
public:
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

	CSchemaClassInfo *GetSchemaBinding() const
	{
		return m_pClassInfo->m_pSchemaBinding;
	}

	datamap_t *GetDataDescMap() const
	{
		return m_pClassInfo->m_pDataDescMap;
	}

	void DestructInstance( CEntityInstance *pInstance )
	{
		if ( pInstance )
			GetSchemaBinding()->DestructInPlace( pInstance );
	}

	void FreeInstance( CEntityInstance *pInstance )
	{
		if ( pInstance )
			MemAlloc_Free( pInstance );
	}

	// Searches the base classes too
	CEntityIOOutput *FindOutput( const char *pszName, CEntityInstance *pEntity );

	// Offers the input to the pulse signatures of the class chain; true when handled
	bool AcceptInput( CEntityInstance *pEntity, const CUtlSymbolLarge &sInputName, CEntityInstance *pActivator, CEntityInstance *pCaller, const variant_t &value, const CPulseArgumentPack *pArgs, const CPulseInputParamMap *pParamMap );

	void LinkToClassList( CEntityIdentity *pIdentity )
	{
		pIdentity->m_pPrevByClass = nullptr;
		pIdentity->m_pNextByClass = m_pFirstEntity;

		if ( m_pFirstEntity )
			m_pFirstEntity->m_pPrevByClass = pIdentity;

		m_pFirstEntity = pIdentity;
	}

	void UnlinkFromClassList( CEntityIdentity *pIdentity )
	{
		if ( pIdentity->m_pPrevByClass )
			pIdentity->m_pPrevByClass->m_pNextByClass = pIdentity->m_pNextByClass;
		else if ( m_pFirstEntity == pIdentity )
			m_pFirstEntity = pIdentity->m_pNextByClass;

		if ( pIdentity->m_pNextByClass )
			pIdentity->m_pNextByClass->m_pPrevByClass = pIdentity->m_pPrevByClass;

		pIdentity->m_pPrevByClass = nullptr;
		pIdentity->m_pNextByClass = nullptr;
	}

public:
	using FuncToNameCb = const char *(*)(BASEPTR think_fn);
	using NameToFuncCb = BASEPTR (*)(const char *fn_name);
	using RegisterPulseBindingsCb = void (*)(CPulseAPIExtensionRegistrationContext *pContext);
	using EnumerateComponentsCb = void (*)(CUtlVector<EntComponentNameEntry_t> *pOut);

	ScriptClassDesc_t* m_pScriptDesc;

	CNetworkSerializerClassInfo* m_pNetworkSerializerInfo;

	EntInput_t* m_pInputs;
	EntOutput_t* m_pOutputs;
	int m_nInputCount;
	int m_nOutputCount;

	CEntityClassPulseSignature* m_pSharedPulseSignature;

	RegisterPulseBindingsCb m_pfnRegisterPulseBindings;

	// Allows to get any think functions in use or to get its string name for this class
	// does searches to the parent classes as well
	NameToFuncCb m_NameToThinkFunc;
	FuncToNameCb m_ThinkFuncToName;

	EnumerateComponentsCb m_pfnEnumerateComponents;

	EntClassComponentOverride_t* m_pComponentOverrides;

	CEntityClassInfo* m_pClassInfo;
	CEntityClassInfo* m_pBaseClassInfo;
	CUtlSymbolLarge m_designerName;

	// Uses FENTCLASS_* flags
	uint m_flags;

	int m_SpawnOrder;
	
	EntityComponentHelperFlags_t m_nAllHelpersFlags;

	CUtlVector<ComponentOffsets_t> m_ComponentOffsets;
	CUtlVector<ComponentHelper_t> m_AllHelpers;
	
	ComponentUnserializerClassInfo_t m_componentUnserializerClassInfo;
	
	FlattenedSerializerDesc_t m_flattenedSerializer;
	CUtlVector<ClassOutputInfo_t> m_classOutputInfos;

	CEntityHandle m_requiredEHandle;

	CEntityClass* m_pNext;
	CEntityIdentity* m_pFirstEntity;
	ServerClass* m_pServerClass;
	int m_nClassIndex;
};

#endif // ENTITYCLASS_H
