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
#include "tier1/utlvector.h"
#include "tier1/smartptr.h"
#include "../networkvar.h"
#include "entityhandle.h"
#include "utldelegateimpl.h"

#include "entityattributetable.h"
#include "entitycomponent.h"
#include "entityindex.h"
#include "entitytypes.h"
#include "entitynetwork.h"

#include "spawngrouptypes.h"

class CEntityClass;
class CEntityInstance;

class CEntityIdentity
{
	friend class CConcreteEntityList;

public:
	DECLARE_CLASS_NOBASE( CEntityIdentity );

	using AttributeTable_t = CEntityAttributeTable;
	using AttributeKey_t = AttributeTable_t::Key_t;
	using AttributeName_t = AttributeTable_t::AttributeName_t;
	using AttributeValue_t = AttributeTable_t::Attribute_t;

	// An empty name makes the zero key
	static AttributeKey_t MakeAttributeKey( const char *pszName ) { return AttributeTable_t::MakeKey( ( pszName && *pszName ) ? pszName : nullptr ); }

	CEntityHandle GetRefEHandle() const
	{
		CEntityHandle handle = m_EHandle;

		handle.m_Parts.m_Serial -= ( m_flags & EF_IS_INVALID_EHANDLE );

		return handle;
	}
	const char *GetName() const { return m_name.String(); }
	const char *GetClassname() const { return m_designerName.String(); }

	// The entity name, or the class name when it has none
	const char *GetDebugName() const;
	CEntityIndex GetEntityIndex() const { return m_EHandle.GetEntryIndex(); }
	SpawnGroupHandle_t GetSpawnGroup() const { return m_hSpawnGroup; }

	// An isolated allocation checks its networkable flag, any other entity its handle range
	bool IsNetworked() const;

	// Forwards data with the identity path index to the entity, if any
	void NetworkStateChanged( const NetworkStateChanged_t &data );

	// Resets the identity for slot reuse; the handle, links and attributes are kept
	void Clear()
	{
		m_flags = EF_NONE;
		m_fDataObjectTypes = 0;
		m_pInstance = nullptr;
		m_pClass = nullptr;
		m_name = CUtlSymbolLarge();
		m_designerName = CUtlSymbolLarge();
		m_nameStringTableIndex = -1;
		m_hSpawnGroup = SpawnGroupHandle_t();
		m_worldGroupId = WorldGroupId_t();
	}

	void ReleaseAttributes()
	{
		Release( m_pAttributes );
		m_pAttributes = nullptr;
	}

	bool HasAttributes() const { return m_pAttributes && !m_pAttributes->IsEmpty(); }
	const AttributeTable_t *GetAttributes() const { return m_pAttributes; }
	AttributeTable_t *GetAttributes() { return m_pAttributes; }
	AttributeTable_t *EnsureAttributes()
	{
		if ( !m_pAttributes )
			m_pAttributes = Create< AttributeTable_t >();

		return m_pAttributes;
	}

	bool HasAttribute( const AttributeKey_t &key ) const { return m_pAttributes && m_pAttributes->HasAttribute( key ); }
	bool HasAttribute( const AttributeName_t &name ) const { return m_pAttributes && m_pAttributes->HasAttribute( name ); }
	bool HasAttribute( const char *pszName ) const { return m_pAttributes && m_pAttributes->HasAttribute( pszName ); }
	AttributeValue_t *FindAttribute( const AttributeKey_t &key ) { return m_pAttributes ? m_pAttributes->FindValue( key ) : nullptr; }
	const AttributeValue_t *FindAttribute( const AttributeKey_t &key ) const { return m_pAttributes ? m_pAttributes->FindValue( key ) : nullptr; }
	AttributeValue_t *FindAttribute( const AttributeName_t &name ) { return m_pAttributes ? m_pAttributes->FindValue( name ) : nullptr; }
	const AttributeValue_t *FindAttribute( const AttributeName_t &name ) const { return m_pAttributes ? m_pAttributes->FindValue( name ) : nullptr; }
	AttributeValue_t *FindAttribute( const char *pszName ) { return m_pAttributes ? m_pAttributes->FindValue( pszName ) : nullptr; }
	const AttributeValue_t *FindAttribute( const char *pszName ) const { return m_pAttributes ? m_pAttributes->FindValue( pszName ) : nullptr; }
	AttributeValue_t &GetAttribute( const AttributeKey_t &key, const char *pszName = nullptr ) { return EnsureAttributes()->GetValue( key, pszName ); }
	AttributeValue_t &GetAttribute( const AttributeName_t &name ) { return EnsureAttributes()->GetValue( name ); }
	AttributeValue_t &GetAttribute( const char *pszName ) { return EnsureAttributes()->GetValue( pszName ); }
	int Attribute_GetIntValue( const char *pszName, int nDefault ) const { return m_pAttributes ? m_pAttributes->GetInt( pszName, nDefault ) : nDefault; }
	int Attribute_GetIntValue( const AttributeName_t &name, int nDefault ) const { return m_pAttributes ? m_pAttributes->GetInt( name, nDefault ) : nDefault; }
	float Attribute_GetFloatValue( const char *pszName, float flDefault ) const { return m_pAttributes ? m_pAttributes->GetFloat( pszName, flDefault ) : flDefault; }
	float Attribute_GetFloatValue( const AttributeName_t &name, float flDefault ) const { return m_pAttributes ? m_pAttributes->GetFloat( name, flDefault ) : flDefault; }
	const char *Attribute_GetStringValue( const char *pszName, const char *pszDefault = "" ) const { return m_pAttributes ? m_pAttributes->GetString( pszName, pszDefault ) : pszDefault; }
	const char *Attribute_GetStringValue( const AttributeName_t &name, const char *pszDefault = "" ) const { return m_pAttributes ? m_pAttributes->GetString( name, pszDefault ) : pszDefault; }
	uint64 Attribute_GetUInt64Value( const char *pszName, uint64 nDefault = 0 ) const { return m_pAttributes ? m_pAttributes->GetUInt64( pszName, nDefault ) : nDefault; }
	uint64 Attribute_GetUInt64Value( const AttributeName_t &name, uint64 nDefault = 0 ) const { return m_pAttributes ? m_pAttributes->GetUInt64( name, nDefault ) : nDefault; }
	void *Attribute_GetPointerValue( const char *pszName, void *pDefault = nullptr ) const { return m_pAttributes ? m_pAttributes->GetPointer( pszName, pDefault ) : pDefault; }
	void *Attribute_GetPointerValue( const AttributeName_t &name, void *pDefault = nullptr ) const { return m_pAttributes ? m_pAttributes->GetPointer( name, pDefault ) : pDefault; }
	void Attribute_SetValue( const AttributeKey_t &key, const AttributeValue_t &value, const char *pszName = nullptr ) { EnsureAttributes()->SetValue( key, value, pszName ); }
	void Attribute_SetValue( const AttributeKey_t &key, AttributeValue_t &&value, const char *pszName = nullptr ) { EnsureAttributes()->SetValue( key, Move( value ), pszName ); }
	void Attribute_SetValue( const AttributeName_t &name, const AttributeValue_t &value ) { EnsureAttributes()->SetValue( name, value ); }
	void Attribute_SetValue( const AttributeName_t &name, AttributeValue_t &&value ) { EnsureAttributes()->SetValue( name, Move( value ) ); }
	void Attribute_SetValue( const char *pszName, const AttributeValue_t &value ) { EnsureAttributes()->SetValue( pszName, value ); }
	void Attribute_SetValue( const char *pszName, AttributeValue_t &&value ) { EnsureAttributes()->SetValue( pszName, Move( value ) ); }
	void Attribute_SetIntValue( const char *pszName, int nValue, uint32 nAuxValue = 0 ) { EnsureAttributes()->SetInt( pszName, nValue, nAuxValue ); }
	void Attribute_SetIntValue( const AttributeName_t &name, int nValue, uint32 nAuxValue = 0 ) { EnsureAttributes()->SetInt( name, nValue, nAuxValue ); }
	void Attribute_SetFloatValue( const char *pszName, float flValue, uint32 nAuxValue = 0 ) { EnsureAttributes()->SetFloat( pszName, flValue, nAuxValue ); }
	void Attribute_SetFloatValue( const AttributeName_t &name, float flValue, uint32 nAuxValue = 0 ) { EnsureAttributes()->SetFloat( name, flValue, nAuxValue ); }
	void Attribute_SetStringValue( const char *pszName, const char *pszValue, uint32 nAuxValue = 0 ) { EnsureAttributes()->SetString( pszName, pszValue, nAuxValue ); }
	void Attribute_SetStringValue( const AttributeName_t &name, const char *pszValue, uint32 nAuxValue = 0 ) { EnsureAttributes()->SetString( name, pszValue, nAuxValue ); }
	void Attribute_SetUInt64Value( const char *pszName, uint64 nValue, uint32 nAuxValue = 0, ObjectAttributeValueType_t nType = OBJECT_ATTRIBUTE_VALUE_UINT64 ) { EnsureAttributes()->SetUInt64( pszName, nValue, nAuxValue, nType ); }
	void Attribute_SetUInt64Value( const AttributeName_t &name, uint64 nValue, uint32 nAuxValue = 0, ObjectAttributeValueType_t nType = OBJECT_ATTRIBUTE_VALUE_UINT64 ) { EnsureAttributes()->SetUInt64( name, nValue, nAuxValue, nType ); }
	void Attribute_SetPointerValue( const char *pszName, void *pValue, uint32 nAuxValue = 0, ObjectAttributeValueType_t nType = OBJECT_ATTRIBUTE_VALUE_MISC32 ) { EnsureAttributes()->SetPointer( pszName, pValue, nAuxValue, nType ); }
	void Attribute_SetPointerValue( const AttributeName_t &name, void *pValue, uint32 nAuxValue = 0, ObjectAttributeValueType_t nType = OBJECT_ATTRIBUTE_VALUE_MISC32 ) { EnsureAttributes()->SetPointer( name, pValue, nAuxValue, nType ); }
	bool DeleteAttribute( const char *pszName ) { return m_pAttributes && m_pAttributes->Remove( pszName ); }
	bool DeleteAttribute( const AttributeName_t &name ) { return m_pAttributes && m_pAttributes->Remove( name ); }
	void RemoveAllAttributes() { if ( m_pAttributes ) m_pAttributes->RemoveAll(); }

	bool NameMatches( const char *pszNameOrWildcard ) const;
	bool ClassMatches( const char *pszClassOrWildcard ) const;

public:
	CEntityInstance *m_pInstance;
	CEntityClass *m_pClass;
	CEntityHandle m_EHandle;
	CNetworkVar( int32, m_nameStringTableIndex );
	CUtlSymbolLarge m_name;
	CUtlSymbolLarge m_designerName;

public:
	uint64 m_hPublicScope; // CEntityPublicScriptScope

public:
	EntityFlags_t m_flags;

private:
	SpawnGroupHandle_t m_hSpawnGroup;

public:
	WorldGroupId_t m_worldGroupId;
	uint32 m_fDataObjectTypes;
	ChangeAccessorFieldPathIndex_t m_PathIndex;
	CEntityAttributeTable *m_pAttributes;
	CEntityIdentity *m_pPrev;
	CEntityIdentity *m_pNext;
	CEntityIdentity *m_pPrevByClass;
	CEntityIdentity *m_pNextByClass;
};

#endif // ENTITYIDENTITY_H
