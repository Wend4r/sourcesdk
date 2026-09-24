#ifndef ENTITYCOMPONENT_H
#define ENTITYCOMPONENT_H

#if _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "tier0/utlstring.h"
#include "tier0/utlstringtoken.h"
#include "tier1/utlsymbollarge.h"
#include "tier1/smartptr.h"
#include "datamap.h"
#include "schemasystem/schematypes.h"
#include "entitytypes.h"

class CEntityInstance;
class CEntityIdentity;
class CEntityComponentHelper;
struct ScriptClassDesc_t;
struct ComponentUnserializerClassInfo_t;
struct EntOutput_t;

struct ComponentUnserializerKeyNamesChunk_t
{
	CUtlStringToken m_keyNames[4];
};

struct ComponentUnserializerFieldInfo_t
{
	CSchemaEnumInfo* m_pSchemaEnum;
	const char* m_pKeyName;
	
	uint16 m_nOffset;
	uint16 m_nArraySize;
	
	SpawnKeyType_t m_Type;
	
	bool m_bUnserializeAsMatrix : 1;
	bool m_bArrayElement : 1;
	bool m_bRemovedKeyField : 1;
};

struct ComponentUnserializerPtrToClassInfo_t
{
	uint m_nOffset;
	ComponentUnserializerClassInfo_t* m_pClassInfo;
};

struct ComponentUnserializerClassInfo_t
{
	ComponentUnserializerClassInfo_t* m_pBaseClassInfo;

	ComponentUnserializerKeyNamesChunk_t* m_pKeyNamesChunks;
	ComponentUnserializerFieldInfo_t* m_pFieldInfos;
	ComponentUnserializerPtrToClassInfo_t* m_pClassInfoPtrs;

	uint16 m_nFieldInfoCount;
	uint16 m_nKeyNamesChunkCount;
	uint16 m_nClassInfoPtrCount;
};

struct EntComponentInfo_t
{
	const char* m_pName;
	const char* m_pCPPClassname;
	const char* m_pNetworkDataReferencedDescription;
	const char* m_pNetworkDataReferencedPtrPropDescription;
	int m_nRuntimeIndex;
	uint m_nFlags;
	ComponentUnserializerClassInfo_t m_componentUnserializerClassInfo;
	ScriptClassDesc_t* m_pScriptDesc;
	CEntityComponentHelper* m_pBaseClassComponentHelper;
};

class CEntityComponentHelper
{
public:
	virtual SchemaMetaInfoHandle_t<CSchemaClassInfo> Schema_DynamicBinding() = 0;
	virtual void Finalize() = 0;
	virtual SchemaMetaInfoHandle_t<CSchemaClassInfo> GetSchemaBinding() = 0;
	virtual datamap_t* GetDataDescMap() = 0;
	virtual bool Allocate( CEntityIdentity* pEntity, void* pComponent ) = 0;
	virtual void Free( CEntityIdentity* pEntity, void* pComponent ) = 0;

public:
	EntityComponentHelperFlags_t m_flags;
	EntComponentInfo_t* m_pInfo;
	int m_nPriority;
	CEntityComponentHelper* m_pNext;
};

class CKV3TransferLoadContext;
class CKV3TransferSaveContext;

class CEntityComponent
{
public:
	// TODO(@Wend4r): Implement schemacompiler2 & kv3lib stuff
	virtual SchemaMetaInfoHandle_t< CSchemaClassInfo > Schema_DynamicBinding() = 0;
	virtual datamap_t *GetDataDescMap() = 0;
	virtual SchemaMetaInfoHandle_t< CSchemaClassInfo > GetSchemaBinding() = 0;
	virtual void KV3TransferSave( CKV3TransferSaveContext *pContext ) const = 0;
	virtual void KV3TransferLoad( CKV3TransferLoadContext *pContext ) = 0;

	virtual CEntityComponentHelper *GetComponentHelper() = 0;
	virtual const char *GetComponentName() = 0;
};

#endif // ENTITYCOMPONENT_H