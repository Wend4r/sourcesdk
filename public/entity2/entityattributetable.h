#ifndef ENTITYATTRIBUTETABLE_H
#define ENTITYATTRIBUTETABLE_H

#if _WIN32
#pragma once
#endif

#include "tier0/utlstring.h"
#include "tier0/utlstringtoken.h"
#include "tier1/utlmap.h"
#include "tier1/utlobjectattributetable.h"

// Attributes of an entity, keyed by the lowercase hash of the name.
class CEntityAttributeTable
{
public:
	using Key_t = CUtlStringTokenNoRegistration;
	using Attribute_t = ObjectAttributeValue_t;
	using AttributeName_t = CUtlString;
	using IndexType_t = int;

	CEntityAttributeTable() = default;
	CEntityAttributeTable( const CEntityAttributeTable &other ) { CopyFrom( other ); }
	CEntityAttributeTable( CEntityAttributeTable &&other ) noexcept { MoveFrom( Move( other ) ); }

	CEntityAttributeTable &operator=( const CEntityAttributeTable &other )
	{
		if ( this != &other )
			CopyFrom( other );

		return *this;
	}
	CEntityAttributeTable &operator=( CEntityAttributeTable &&other ) noexcept
	{
		if ( this != &other )
			MoveFrom( Move( other ) );

		return *this;
	}

	void Swap( CEntityAttributeTable &other )
	{
		m_Attributes.Swap( other.m_Attributes );
		m_Names.Swap( other.m_Names );
	}
	void CopyFrom( const CEntityAttributeTable &other )
	{
		m_Attributes = other.m_Attributes;
		m_Names = other.m_Names;
	}
	void MoveFrom( CEntityAttributeTable &&other )
	{
		Swap( other );
	}

	void EnsureCapacity( int nCapacity ) { m_Attributes.EnsureCapacity( nCapacity ); m_Names.EnsureCapacity( nCapacity ); }
	static IndexType_t InvalidIndex() { return CUtlOrderedMap< Key_t, Attribute_t >::InvalidIndex(); }
	IndexType_t First() const { return m_Attributes.FirstInorder(); }
	IndexType_t Next( IndexType_t it ) const { return m_Attributes.NextInorder( it ); }
	IndexType_t Prev( IndexType_t it ) const { return m_Attributes.PrevInorder( it ); }
	IndexType_t Last() const { return m_Attributes.LastInorder(); }
	bool IsValidIndex( IndexType_t it ) const { return m_Attributes.IsValidIndex( it ); }
	unsigned int Count() const { return m_Attributes.Count(); }
	IndexType_t MaxElement() const { return m_Attributes.MaxElement(); }
	bool IsEmpty() const { return Count() == 0; }
	const Key_t &Key( IndexType_t it ) const { return m_Attributes.Key( it ); }
	Attribute_t &Element( IndexType_t it ) { return m_Attributes.Element( it ); }
	const Attribute_t &Element( IndexType_t it ) const { return m_Attributes.Element( it ); }
	const char *GetAttributeName( IndexType_t it ) const { const AttributeName_t *pName = FindStoredName( Key( it ) ); return pName ? pName->String() : nullptr; }
	void RemoveAll() { m_Attributes.RemoveAll(); m_Names.RemoveAll(); }
	void Purge() { m_Attributes.Purge(); m_Names.Purge(); }
	static Key_t MakeKey( uint32 nHash ) { return Key_t( nHash ); }
	static Key_t MakeKey( const char *pszName ) { return MakeKey( pszName ? HashStringWithBuffer< true >( pszName ) : 0 ); }
	static Key_t MakeKey( const AttributeName_t &name ) { return MakeKey( name.String() ); }
	IndexType_t FindIndex( const Key_t &key ) const { return m_Attributes.Find( key ); }
	IndexType_t FindIndex( const AttributeName_t &name ) const { return FindIndex( MakeKey( name ) ); }
	IndexType_t FindIndex( const char *pszName ) const { return FindIndex( MakeKey( pszName ) ); }
	bool HasElement( const Key_t &key ) const { return HasAttribute( key ); }
	bool HasElement( const AttributeName_t &name ) const { return HasAttribute( name ); }
	bool HasElement( const char *pszName ) const { return HasAttribute( pszName ); }
	bool HasAttribute( const Key_t &key ) const { return FindIndex( key ) != InvalidIndex(); }
	bool HasAttribute( const AttributeName_t &name ) const { return FindIndex( name ) != InvalidIndex(); }
	bool HasAttribute( const char *pszName ) const { return FindIndex( pszName ) != InvalidIndex(); }
	Attribute_t *FindValue( const Key_t &key ) { IndexType_t it = FindIndex( key ); return it != InvalidIndex() ? &m_Attributes.Element( it ) : nullptr; }
	const Attribute_t *FindValue( const Key_t &key ) const { IndexType_t it = FindIndex( key ); return it != InvalidIndex() ? &m_Attributes.Element( it ) : nullptr; }
	Attribute_t *FindValue( const AttributeName_t &name ) { return FindValue( MakeKey( name ) ); }
	const Attribute_t *FindValue( const AttributeName_t &name ) const { return FindValue( MakeKey( name ) ); }
	Attribute_t *FindValue( const char *pszName ) { return FindValue( MakeKey( pszName ) ); }
	const Attribute_t *FindValue( const char *pszName ) const { return FindValue( MakeKey( pszName ) ); }
	const AttributeName_t *FindStoredName( const Key_t &key ) const
	{
		IndexType_t it = m_Names.Find( key );

		if ( it != InvalidIndex() )
			return &m_Names.Element( it );

		return nullptr;
	}
	const char *FindAttributeName( const Key_t &key ) const { const AttributeName_t *pName = FindStoredName( key ); return pName ? pName->String() : nullptr; }
	const char *FindAttributeName( const AttributeName_t &name ) const { return FindAttributeName( MakeKey( name ) ); }
	const char *FindAttributeName( const char *pszName ) const { return FindAttributeName( MakeKey( pszName ) ); }

	// Insert or fetch the value slot and optionally retain the original string name.
	Attribute_t &GetValue( const Key_t &key, const char *pszName = nullptr )
	{
		IndexType_t it = m_Attributes.Find( key );
		if ( it == InvalidIndex() )
			it = m_Attributes.Insert( key );

		UpdateStoredName( key, pszName );
		return m_Attributes.Element( it );
	}
	Attribute_t &GetValue( const AttributeName_t &name ) { return GetValue( MakeKey( name ) ); }
	Attribute_t &GetValue( const char *pszName ) { Key_t key = MakeKey( pszName ); return GetValue( key, pszName ); }
	Attribute_t &FindOrAdd( const Key_t &key, const char *pszName = nullptr ) { return GetValue( key, pszName ); }
	Attribute_t &FindOrAdd( const AttributeName_t &name ) { return GetValue( name ); }
	Attribute_t &FindOrAdd( const char *pszName ) { return GetValue( pszName ); }
	Attribute_t &operator[]( const Key_t &key ) { return GetValue( key ); }
	Attribute_t &operator[]( const AttributeName_t &name ) { return GetValue( name ); }
	Attribute_t &operator[]( const char *pszName ) { return GetValue( pszName ); }

	// Replace the stored value and keep the mirrored name table in sync.
	void SetValue( const Key_t &key, const Attribute_t &value, const char *pszName = nullptr )
	{
		m_Attributes.InsertOrReplace( key, value );
		UpdateStoredName( key, pszName );
	}
	void SetValue( const Key_t &key, Attribute_t &&value, const char *pszName = nullptr )
	{
		m_Attributes.InsertOrReplace( key, Move( value ) );
		UpdateStoredName( key, pszName );
	}
	void SetValue( const AttributeName_t &name, const Attribute_t &value ) { SetValue( MakeKey( name ), value ); }
	void SetValue( const AttributeName_t &name, Attribute_t &&value ) { SetValue( MakeKey( name ), Move( value ) ); }
	void SetValue( const char *pszName, const Attribute_t &value ) { SetValue( MakeKey( pszName ), value, pszName ); }
	void SetValue( const char *pszName, Attribute_t &&value ) { SetValue( MakeKey( pszName ), Move( value ), pszName ); }

	void SetInt( const Key_t &key, int32 nValue, const char *pszName = nullptr, uint32 nAuxValue = 0 ) { Attribute_t value; value.SetInt( nValue, nAuxValue ); SetValue( key, Move( value ), pszName ); }
	void SetInt( const AttributeName_t &name, int32 nValue, uint32 nAuxValue = 0 ) { SetInt( MakeKey( name ), nValue, nullptr, nAuxValue ); }
	void SetInt( const char *pszName, int32 nValue, uint32 nAuxValue = 0 ) { SetInt( MakeKey( pszName ), nValue, pszName, nAuxValue ); }

	void SetFloat( const Key_t &key, float flValue, const char *pszName = nullptr, uint32 nAuxValue = 0 ) { Attribute_t value; value.SetFloat( flValue, nAuxValue ); SetValue( key, Move( value ), pszName ); }
	void SetFloat( const AttributeName_t &name, float flValue, uint32 nAuxValue = 0 ) { SetFloat( MakeKey( name ), flValue, nullptr, nAuxValue ); }
	void SetFloat( const char *pszName, float flValue, uint32 nAuxValue = 0 ) { SetFloat( MakeKey( pszName ), flValue, pszName, nAuxValue ); }

	void SetUInt64( const Key_t &key, uint64 nValue, const char *pszName = nullptr, uint32 nAuxValue = 0, ObjectAttributeValueType_t nType = OBJECT_ATTRIBUTE_VALUE_UINT64 ) { Attribute_t value; value.SetUInt64( nValue, nAuxValue, nType ); SetValue( key, Move( value ), pszName ); }
	void SetUInt64( const AttributeName_t &name, uint64 nValue, uint32 nAuxValue = 0, ObjectAttributeValueType_t nType = OBJECT_ATTRIBUTE_VALUE_UINT64 ) { SetUInt64( MakeKey( name ), nValue, nullptr, nAuxValue, nType ); }
	void SetUInt64( const char *pszName, uint64 nValue, uint32 nAuxValue = 0, ObjectAttributeValueType_t nType = OBJECT_ATTRIBUTE_VALUE_UINT64 ) { SetUInt64( MakeKey( pszName ), nValue, pszName, nAuxValue, nType ); }

	void SetPointer( const Key_t &key, void *pValue, const char *pszName = nullptr, uint32 nAuxValue = 0, ObjectAttributeValueType_t nType = OBJECT_ATTRIBUTE_VALUE_MISC32 ) { Attribute_t value; value.SetPointer( pValue, nAuxValue, nType ); SetValue( key, Move( value ), pszName ); }
	void SetPointer( const AttributeName_t &name, void *pValue, uint32 nAuxValue = 0, ObjectAttributeValueType_t nType = OBJECT_ATTRIBUTE_VALUE_MISC32 ) { SetPointer( MakeKey( name ), pValue, nullptr, nAuxValue, nType ); }
	void SetPointer( const char *pszName, void *pValue, uint32 nAuxValue = 0, ObjectAttributeValueType_t nType = OBJECT_ATTRIBUTE_VALUE_MISC32 ) { SetPointer( MakeKey( pszName ), pValue, pszName, nAuxValue, nType ); }

	void SetString( const Key_t &key, const char *pszValue, const char *pszName = nullptr, uint32 nAuxValue = 0 ) { Attribute_t value; value.SetString( pszValue, nAuxValue ); SetValue( key, Move( value ), pszName ); }
	void SetString( const AttributeName_t &name, const char *pszValue, uint32 nAuxValue = 0 ) { SetString( MakeKey( name ), pszValue, nullptr, nAuxValue ); }
	void SetString( const char *pszName, const char *pszValue, uint32 nAuxValue = 0 ) { SetString( MakeKey( pszName ), pszValue, pszName, nAuxValue ); }
	void SetRawValue( const Key_t &key, const Attribute_t &value, const char *pszName = nullptr ) { SetValue( key, value, pszName ); }
	void SetRawValue( const Key_t &key, Attribute_t &&value, const char *pszName = nullptr ) { SetValue( key, Move( value ), pszName ); }
	void SetRawValue( const AttributeName_t &name, const Attribute_t &value ) { SetValue( name, value ); }
	void SetRawValue( const AttributeName_t &name, Attribute_t &&value ) { SetValue( name, Move( value ) ); }
	void SetRawValue( const char *pszName, const Attribute_t &value ) { SetValue( pszName, value ); }
	void SetRawValue( const char *pszName, Attribute_t &&value ) { SetValue( pszName, Move( value ) ); }

	int32 GetInt( const Key_t &key, int32 nDefaultValue = 0 ) const { const Attribute_t *pValue = FindValue( key ); return pValue ? pValue->GetInt( nDefaultValue ) : nDefaultValue; }
	int32 GetInt( const AttributeName_t &name, int32 nDefaultValue = 0 ) const { return GetInt( MakeKey( name ), nDefaultValue ); }
	int32 GetInt( const char *pszName, int32 nDefaultValue = 0 ) const { return GetInt( MakeKey( pszName ), nDefaultValue ); }

	float GetFloat( const Key_t &key, float flDefaultValue = 0.0f ) const { const Attribute_t *pValue = FindValue( key ); return pValue ? pValue->GetFloat( flDefaultValue ) : flDefaultValue; }
	float GetFloat( const AttributeName_t &name, float flDefaultValue = 0.0f ) const { return GetFloat( MakeKey( name ), flDefaultValue ); }
	float GetFloat( const char *pszName, float flDefaultValue = 0.0f ) const { return GetFloat( MakeKey( pszName ), flDefaultValue ); }

	uint64 GetUInt64( const Key_t &key, uint64 nDefaultValue = 0 ) const { const Attribute_t *pValue = FindValue( key ); return pValue ? pValue->GetUInt64( nDefaultValue ) : nDefaultValue; }
	uint64 GetUInt64( const AttributeName_t &name, uint64 nDefaultValue = 0 ) const { return GetUInt64( MakeKey( name ), nDefaultValue ); }
	uint64 GetUInt64( const char *pszName, uint64 nDefaultValue = 0 ) const { return GetUInt64( MakeKey( pszName ), nDefaultValue ); }

	void *GetPointer( const Key_t &key, void *pDefaultValue = nullptr ) const { const Attribute_t *pValue = FindValue( key ); return pValue ? pValue->GetPointer( pDefaultValue ) : pDefaultValue; }
	void *GetPointer( const AttributeName_t &name, void *pDefaultValue = nullptr ) const { return GetPointer( MakeKey( name ), pDefaultValue ); }
	void *GetPointer( const char *pszName, void *pDefaultValue = nullptr ) const { return GetPointer( MakeKey( pszName ), pDefaultValue ); }

	const char *GetString( const Key_t &key, const char *pszDefaultValue = "" ) const { const Attribute_t *pValue = FindValue( key ); return pValue ? pValue->GetString( pszDefaultValue ) : pszDefaultValue; }
	const char *GetString( const AttributeName_t &name, const char *pszDefaultValue = "" ) const { return GetString( MakeKey( name ), pszDefaultValue ); }
	const char *GetString( const char *pszName, const char *pszDefaultValue = "" ) const { return GetString( MakeKey( pszName ), pszDefaultValue ); }

	const Attribute_t &GetValueOrDefault( const Key_t &key, const Attribute_t &defaultValue = {} ) const { const Attribute_t *pValue = FindValue( key ); return pValue ? *pValue : defaultValue; }
	const Attribute_t &GetValueOrDefault( const AttributeName_t &name, const Attribute_t &defaultValue = {} ) const { return GetValueOrDefault( MakeKey( name ), defaultValue ); }
	const Attribute_t &GetValueOrDefault( const char *pszName, const Attribute_t &defaultValue = {} ) const { return GetValueOrDefault( MakeKey( pszName ), defaultValue ); }

	// Remove the attribute from both maps so the table stays internally consistent.
	bool Remove( const Key_t &key )
	{
		bool bRemovedValue = m_Attributes.Remove( key );
		bool bRemovedName = RemoveStoredName( key );

		return bRemovedValue || bRemovedName;
	}
	bool Remove( const AttributeName_t &name ) { return Remove( MakeKey( name ) ); }
	bool Remove( const char *pszName ) { return Remove( MakeKey( pszName ) ); }

	void RemoveAt( IndexType_t it )
	{
		if ( !IsValidIndex( it ) )
			return;

		const Key_t key = Key( it );

		m_Attributes.RemoveAt( it );
		RemoveStoredName( key );
	}
	void SetStoredName( const Key_t &key, const char *pszName ) { UpdateStoredName( key, pszName ); }

public:
	CUtlOrderedMap< CUtlStringTokenNoRegistration, Attribute_t > m_Attributes;
	CUtlOrderedMap< CUtlStringTokenNoRegistration, CUtlString > m_Names;

private:
	// Preserve the original attribute spelling separately from the hashed lookup key.
	void UpdateStoredName( const Key_t &key, const char *pszName )
	{
		if ( !pszName )
			return;

		if ( !pszName[0] )
		{
			RemoveStoredName( key );

			return;
		}

		m_Names.InsertOrReplace( key, AttributeName_t( pszName ) );
	}
	bool RemoveStoredName( const Key_t &key ) { return m_Names.Remove( key ); }
};

#endif // ENTITYATTRIBUTETABLE_H
