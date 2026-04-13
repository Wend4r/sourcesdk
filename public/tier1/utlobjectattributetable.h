#ifndef UTLOBJECTATTRIBUTETABLE_H
#define UTLOBJECTATTRIBUTETABLE_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/dbg.h"
#include "tier0/utlstring.h"
#include "tier0/utlstringtoken.h"
#include "tier1/utlmap.h"
#include "tier1/utlsymbollarge.h"

#define FOR_EACH_ATTRIBUTES( attributes, iteratorName ) \
	for ( auto iteratorName = ( attributes ).First(); ( iteratorName ).IsValidIndex( iter ); ( iteratorName ) = ( attributes ).Next( iter ) )

enum ObjectAttributeValueType_t : uint32
{
	OBJECT_ATTRIBUTE_VALUE_INVALID = 0,
	OBJECT_ATTRIBUTE_VALUE_INT = 1,
	OBJECT_ATTRIBUTE_VALUE_FLOAT = 2,
	OBJECT_ATTRIBUTE_VALUE_UINT64 = 3,
	OBJECT_ATTRIBUTE_VALUE_MISC32 = 4,
	OBJECT_ATTRIBUTE_VALUE_STRING = 5,
};

struct ObjectAttributeKey_t
{
	ObjectAttributeKey_t( uint32 nHash = 0, UtlSymLargeId_t iNameSymbol = UTL_INVAL_SYMBOL_LARGE ) : m_nHash( nHash ), m_iNameSymbol( iNameSymbol ) {}

	bool operator==( const ObjectAttributeKey_t &other ) const
	{
		return m_nHash == other.m_nHash && m_iNameSymbol == other.m_iNameSymbol;
	}
	bool operator!=( const ObjectAttributeKey_t &other ) const
	{
		return operator==( other );
	}

	uint32 m_nHash;
	UtlSymLargeId_t m_iNameSymbol;
};
COMPILE_TIME_ASSERT( sizeof( ObjectAttributeKey_t ) == 0x8 );

struct ObjectAttributeValue_t
{
	ObjectAttributeValue_t()
	{
		Init();
	}

	ObjectAttributeValue_t( int32 nValue )
	{
		Init();
		SetInt( nValue );
	}

	ObjectAttributeValue_t( float flValue )
	{
		Init();
		SetFloat( flValue );
	}

	ObjectAttributeValue_t( uint64 nValue )
	{
		Init();
		SetUInt64( nValue );
	}

	ObjectAttributeValue_t( const char *pszValue )
	{
		Init();
		SetString( pszValue );
	}

	ObjectAttributeValue_t( const CUtlString &sValue )
	{
		Init();
		SetString( sValue );
	}

	ObjectAttributeValue_t( const ObjectAttributeValue_t &other )
	{
		Init();
		CopyFrom( other );
	}

	ObjectAttributeValue_t( ObjectAttributeValue_t &&other ) noexcept
	{
		Init();
		MoveFrom( Move( other ) );
	}

	~ObjectAttributeValue_t()
	{
		Destroy();
	}

	ObjectAttributeValue_t& operator=( const ObjectAttributeValue_t &other )
	{
		if ( this != &other )
			CopyFrom( other );

		return *this;
	}

	ObjectAttributeValue_t& operator=( ObjectAttributeValue_t &&other ) noexcept
	{
		if ( this != &other )
			MoveFrom( Move( other ) );

		return *this;
	}

	ObjectAttributeValueType_t GetType() const { return m_nType; }
	void Clear() { Destroy(); Init(); }

	// Copy the active payload while preserving the runtime type tag.
	void CopyFrom( const ObjectAttributeValue_t &other )
	{
		if ( this == &other )
			return;

		Clear();

		m_nType = other.m_nType;
		m_nReserved = other.m_nReserved;
		m_nAuxValue = other.m_nAuxValue;
		m_nPad = other.m_nPad;

		if ( other.IsString() )
			Construct( &m_sString, other.m_sString );
		else
			m_nUInt64 = other.m_nUInt64;
	}

	// Move the active payload out of another value and reset the source.
	void MoveFrom( ObjectAttributeValue_t &&other )
	{
		if ( this == &other )
			return;

		Clear();

		m_nType = other.m_nType;
		m_nReserved = other.m_nReserved;
		m_nAuxValue = other.m_nAuxValue;
		m_nPad = other.m_nPad;

		if ( other.IsString() )
			Construct( &m_sString, Move( other.m_sString ) );
		else
			m_nUInt64 = other.m_nUInt64;

		other.Clear();
	}

	bool IsValid() const { return m_nType != OBJECT_ATTRIBUTE_VALUE_INVALID; }
	bool IsInt() const { return m_nType == OBJECT_ATTRIBUTE_VALUE_INT; }
	bool IsFloat() const { return m_nType == OBJECT_ATTRIBUTE_VALUE_FLOAT; }
	bool IsUInt64() const { return m_nType == OBJECT_ATTRIBUTE_VALUE_UINT64; }
	bool IsString() const { return m_nType == OBJECT_ATTRIBUTE_VALUE_STRING; }
	bool IsPointer() const { return m_nType == OBJECT_ATTRIBUTE_VALUE_MISC32; }
	bool IsNumber() const { return IsInt() || IsFloat() || IsUInt64(); }
	void SetInvalid() { Clear(); }

	void SetInt( int32 nValue, uint32 nAuxValue = 0 )
	{
		Clear();
		m_nType = OBJECT_ATTRIBUTE_VALUE_INT;
		m_nInt = nValue;
		m_nAuxValue = nAuxValue;
	}

	void SetFloat( float flValue, uint32 nAuxValue = 0 )
	{
		Clear();
		m_nType = OBJECT_ATTRIBUTE_VALUE_FLOAT;
		m_flValue = flValue;
		m_nAuxValue = nAuxValue;
	}

	void SetUInt64( uint64 nValue, uint32 nAuxValue = 0, ObjectAttributeValueType_t nType = OBJECT_ATTRIBUTE_VALUE_UINT64 )
	{
		Clear();
		m_nType = nType;
		m_nUInt64 = nValue;
		m_nAuxValue = nAuxValue;
	}

	void SetPointer( void *pValue, uint32 nAuxValue = 0, ObjectAttributeValueType_t nType = OBJECT_ATTRIBUTE_VALUE_MISC32 )
	{
		SetUInt64( reinterpret_cast< uint64 >( pValue ), nAuxValue, nType );
	}

	void SetString( const char *pszValue, uint32 nAuxValue = 0 )
	{
		Clear();
		m_nType = OBJECT_ATTRIBUTE_VALUE_STRING;
		Construct( &m_sString, pszValue ? pszValue : "" );
		m_nAuxValue = nAuxValue;
	}

	void SetString( const CUtlString &sValue, uint32 nAuxValue = 0 )
	{
		Clear();
		m_nType = OBJECT_ATTRIBUTE_VALUE_STRING;
		Construct( &m_sString, sValue );
		m_nAuxValue = nAuxValue;
	}

	void SetString( CUtlString &&sValue, uint32 nAuxValue = 0 )
	{
		Clear();
		m_nType = OBJECT_ATTRIBUTE_VALUE_STRING;
		Construct( &m_sString, Move( sValue ) );
		m_nAuxValue = nAuxValue;
	}

	int32 GetInt( int32 nDefaultValue = 0 ) const
	{
		return IsInt() ? m_nInt : nDefaultValue;
	}

	float GetFloat( float flDefaultValue = 0.0f ) const
	{
		return IsFloat() ? m_flValue : flDefaultValue;
	}

	uint64 GetUInt64( uint64 nDefaultValue = 0 ) const
	{
		return ( IsUInt64() || IsPointer() ) ? m_nUInt64 : nDefaultValue;
	}

	void *GetPointer( void *pDefaultValue = nullptr ) const
	{
		return IsPointer() ? m_pValue : pDefaultValue;
	}

	const char *GetString( const char *pszDefaultValue = "" ) const
	{
		return IsString() ? m_sString.String() : pszDefaultValue;
	}

	const CUtlString *GetStringPtr() const { return IsString() ? &m_sString : nullptr; }
	uint32 GetAuxValue() const { return m_nAuxValue; }
	void SetAuxValue( uint32 nAuxValue ) { m_nAuxValue = nAuxValue; }

	ObjectAttributeValueType_t m_nType;
	uint32 m_nReserved;

	union
	{
		int32 m_nInt;
		float m_flValue;
		uint64 m_nUInt64;
		void* m_pValue;
		CUtlString m_sString;
	};

	uint32 m_nAuxValue;
	uint32 m_nPad;

private:
	void Init() { m_nType = OBJECT_ATTRIBUTE_VALUE_INVALID; m_nReserved = 0; m_nUInt64 = 0; m_nAuxValue = 0; m_nPad = 0; }
	void Destroy() { if ( IsString() ) Destruct( &m_sString ); }
};
COMPILE_TIME_ASSERT( sizeof( ObjectAttributeValue_t ) == 24 );

class ObjectAttributeKeyLess_t
{
public:
	ObjectAttributeKeyLess_t() {}
	ObjectAttributeKeyLess_t( int i ) {}
	bool operator!() const { return false; }
	bool operator()( const ObjectAttributeKey_t &lhs, const ObjectAttributeKey_t &rhs ) const { return lhs.m_nHash < rhs.m_nHash; }
};

class ObjectAttributeKeyOps_t
{
public:
	static ObjectAttributeKey_t MakeKey( uint32 nHash, UtlSymLargeId_t iNameSymbol = UTL_INVAL_SYMBOL_LARGE ) { return ObjectAttributeKey_t( nHash, iNameSymbol ); }
	static ObjectAttributeKey_t MakeKey( const char *pszName, UtlSymLargeId_t iNameSymbol = UTL_INVAL_SYMBOL_LARGE ) { return MakeKey( pszName ? HashStringWithBuffer< true >( pszName ) : 0, iNameSymbol ); }
	static ObjectAttributeKey_t MakeKey( const CUtlString &sName, UtlSymLargeId_t iNameSymbol = UTL_INVAL_SYMBOL_LARGE ) { return MakeKey( sName.String(), iNameSymbol ); }
	static ObjectAttributeKey_t MakeKey( const CUtlStringToken &name, UtlSymLargeId_t iNameSymbol = UTL_INVAL_SYMBOL_LARGE ) { return MakeKey( static_cast< uint32 >( name ), iNameSymbol ); }

	template < typename K >
	static ObjectAttributeKey_t MakeKey( const K &name, UtlSymLargeId_t iNameSymbol = UTL_INVAL_SYMBOL_LARGE ) { return MakeKey( static_cast< uint32 >( name ), iNameSymbol ); }
};

template < typename K >
class ObjectAttributeNameLess_t : public CDefLess< K >
{
};

template <>
class ObjectAttributeNameLess_t< CUtlString >
{
public:
	ObjectAttributeNameLess_t() {}
	ObjectAttributeNameLess_t( int i ) {}

	bool operator!() const { return false; }
	bool operator()( const CUtlString &lhs, const CUtlString &rhs ) const { return V_strcmp( lhs.String(), rhs.String() ) < 0; }
};

template < typename N, typename V = ObjectAttributeValue_t, typename I = int >
class CUtlObjectAttributeTable
{
public:
	using NameType_t = N;
	using ValueType_t = V;
	using ValueMap_t = CUtlOrderedMap< ObjectAttributeKey_t, V, ObjectAttributeKeyLess_t, I >;
	using NameMap_t = CUtlOrderedMap< ObjectAttributeKey_t, N, ObjectAttributeKeyLess_t, I >;
	using IndexType_t = typename ValueMap_t::IndexType_t;

	CUtlObjectAttributeTable() = default;
	CUtlObjectAttributeTable( const CUtlObjectAttributeTable &other ) { CopyFrom( other ); }
	CUtlObjectAttributeTable( CUtlObjectAttributeTable &&other ) noexcept { MoveFrom( Move( other ) ); }

	CUtlObjectAttributeTable &operator=( const CUtlObjectAttributeTable &other )
	{
		if ( this != &other )
			CopyFrom( other );

		return *this;
	}
	CUtlObjectAttributeTable &operator=( CUtlObjectAttributeTable &&other ) noexcept
	{
		if ( this != &other )
			MoveFrom( Move( other ) );

		return *this;
	}

	void Swap( CUtlObjectAttributeTable &other )
	{
		m_Values.Swap( other.m_Values );
		m_Names.Swap( other.m_Names );
	}
	void CopyFrom( const CUtlObjectAttributeTable &other )
	{
		m_Values = other.m_Values;
		m_Names = other.m_Names;
	}
	void MoveFrom( CUtlObjectAttributeTable &&other )
	{
		Swap( other );
	}

	void EnsureCapacity( I nCapacity ) { m_Values.EnsureCapacity( nCapacity ); m_Names.EnsureCapacity( nCapacity ); }
	static IndexType_t InvalidIndex() { return ValueMap_t::InvalidIndex(); }
	IndexType_t First() const { return m_Values.FirstInorder(); }
	IndexType_t Next( IndexType_t it ) const { return m_Values.NextInorder( it ); }
	IndexType_t Prev( IndexType_t it ) const { return m_Values.PrevInorder( it ); }
	IndexType_t Last() const { return m_Values.LastInorder(); }
	bool IsValidIndex( IndexType_t it ) const { return m_Values.IsValidIndex( it ); }
	unsigned int Count() const { return m_Values.Count(); }
	IndexType_t MaxElement() const { return m_Values.MaxElement(); }
	bool IsEmpty() const { return Count() == 0; }
	const ObjectAttributeKey_t &Key( IndexType_t it ) const { return m_Values.Key( it ); }
	V &Element( IndexType_t it ) { return m_Values.Element( it ); }
	const V &Element( IndexType_t it ) const { return m_Values.Element( it ); }
	const char *GetAttributeName( IndexType_t it ) const { const N *pName = FindStoredName( Key( it ) ); return pName ? pName->String() : nullptr; }
	void RemoveAll() { m_Values.RemoveAll(); m_Names.RemoveAll(); }
	void Purge() { m_Values.Purge(); m_Names.Purge(); }
	static ObjectAttributeKey_t MakeKey( uint32 nHash, UtlSymLargeId_t iNameSymbol = UTL_INVAL_SYMBOL_LARGE ) { return ObjectAttributeKeyOps_t::MakeKey( nHash, iNameSymbol ); }
	static ObjectAttributeKey_t MakeKey( const N &name, UtlSymLargeId_t iNameSymbol = UTL_INVAL_SYMBOL_LARGE ) { return ObjectAttributeKeyOps_t::MakeKey( name, iNameSymbol ); }
	static ObjectAttributeKey_t MakeKey( const char *pszName, UtlSymLargeId_t iNameSymbol = UTL_INVAL_SYMBOL_LARGE ) { return ObjectAttributeKeyOps_t::MakeKey( pszName, iNameSymbol ); }
	IndexType_t FindIndex( const ObjectAttributeKey_t &key ) const { return m_Values.Find( key ); }
	IndexType_t FindIndex( const N &name ) const { return FindIndex( MakeKey( name ) ); }
	IndexType_t FindIndex( const char *pszName ) const { return FindIndex( MakeKey( pszName ) ); }
	bool HasElement( const ObjectAttributeKey_t &key ) const { return HasAttribute( key ); }
	bool HasElement( const N &name ) const { return HasAttribute( name ); }
	bool HasElement( const char *pszName ) const { return HasAttribute( pszName ); }
	bool HasAttribute( const ObjectAttributeKey_t &key ) const { return FindIndex( key ) != InvalidIndex(); }
	bool HasAttribute( const N &name ) const { return FindIndex( name ) != InvalidIndex(); }
	bool HasAttribute( const char *pszName ) const { return FindIndex( pszName ) != InvalidIndex(); }
	V *FindValue( const ObjectAttributeKey_t &key ) { IndexType_t it = FindIndex( key ); return it != InvalidIndex() ? &m_Values.Element( it ) : nullptr; }
	const V *FindValue( const ObjectAttributeKey_t &key ) const { IndexType_t it = FindIndex( key ); return it != InvalidIndex() ? &m_Values.Element( it ) : nullptr; }
	V *FindValue( const N &name ) { return FindValue( MakeKey( name ) ); }
	const V *FindValue( const N &name ) const { return FindValue( MakeKey( name ) ); }
	V *FindValue( const char *pszName ) { return FindValue( MakeKey( pszName ) ); }
	const V *FindValue( const char *pszName ) const { return FindValue( MakeKey( pszName ) ); }
	const N *FindStoredName( const ObjectAttributeKey_t &key ) const
	{
		IndexType_t it = m_Names.Find( key );

		if ( it != InvalidIndex() )
			return &m_Names.Element( it );

		return nullptr;
	}
	const char *FindAttributeName( const ObjectAttributeKey_t &key ) const { const N *pName = FindStoredName( key ); return pName ? pName->String() : nullptr; }
	const char *FindAttributeName( const N &name ) const { return FindAttributeName( MakeKey( name ) ); }
	const char *FindAttributeName( const char *pszName ) const { return FindAttributeName( MakeKey( pszName ) ); }

	// Insert or fetch the value slot and optionally retain the original string name.
	V &GetValue( const ObjectAttributeKey_t &key, const char *pszName = nullptr )
	{
		IndexType_t it = m_Values.Find( key );
		if ( it == InvalidIndex() )
			it = m_Values.Insert( key );

		UpdateStoredName( key, pszName );
		return m_Values.Element( it );
	}
	V &GetValue( const N &name ) { return GetValue( MakeKey( name ) ); }
	V &GetValue( const char *pszName ) { ObjectAttributeKey_t key = MakeKey( pszName ); return GetValue( key, pszName ); }
	V &FindOrAdd( const ObjectAttributeKey_t &key, const char *pszName = nullptr ) { return GetValue( key, pszName ); }
	V &FindOrAdd( const N &name ) { return GetValue( name ); }
	V &FindOrAdd( const char *pszName ) { return GetValue( pszName ); }
	V &operator[]( const ObjectAttributeKey_t &key ) { return GetValue( key ); }
	V &operator[]( const N &name ) { return GetValue( name ); }
	V &operator[]( const char *pszName ) { return GetValue( pszName ); }

	// Replace the stored value and keep the mirrored name table in sync.
	void SetValue( const ObjectAttributeKey_t &key, const V &value, const char *pszName = nullptr )
	{
		m_Values.InsertOrReplace( key, value );
		UpdateStoredName( key, pszName );
	}
	void SetValue( const ObjectAttributeKey_t &key, V &&value, const char *pszName = nullptr )
	{
		m_Values.InsertOrReplace( key, Move( value ) );
		UpdateStoredName( key, pszName );
	}
	void SetValue( const N &name, const V &value ) { SetValue( MakeKey( name ), value ); }
	void SetValue( const N &name, V &&value ) { SetValue( MakeKey( name ), Move( value ) ); }
	void SetValue( const char *pszName, const V &value ) { SetValue( MakeKey( pszName ), value, pszName ); }
	void SetValue( const char *pszName, V &&value ) { SetValue( MakeKey( pszName ), Move( value ), pszName ); }

	void SetInt( const ObjectAttributeKey_t &key, int32 nValue, const char *pszName = nullptr, uint32 nAuxValue = 0 ) { V value; value.SetInt( nValue, nAuxValue ); SetValue( key, Move( value ), pszName ); }
	void SetInt( const N &name, int32 nValue, uint32 nAuxValue = 0 ) { SetInt( MakeKey( name ), nValue, nullptr, nAuxValue ); }
	void SetInt( const char *pszName, int32 nValue, uint32 nAuxValue = 0 ) { SetInt( MakeKey( pszName ), nValue, pszName, nAuxValue ); }

	void SetFloat( const ObjectAttributeKey_t &key, float flValue, const char *pszName = nullptr, uint32 nAuxValue = 0 ) { V value; value.SetFloat( flValue, nAuxValue ); SetValue( key, Move( value ), pszName ); }
	void SetFloat( const N &name, float flValue, uint32 nAuxValue = 0 ) { SetFloat( MakeKey( name ), flValue, nullptr, nAuxValue ); }
	void SetFloat( const char *pszName, float flValue, uint32 nAuxValue = 0 ) { SetFloat( MakeKey( pszName ), flValue, pszName, nAuxValue ); }

	void SetUInt64( const ObjectAttributeKey_t &key, uint64 nValue, const char *pszName = nullptr, uint32 nAuxValue = 0, ObjectAttributeValueType_t nType = OBJECT_ATTRIBUTE_VALUE_UINT64 ) { V value; value.SetUInt64( nValue, nAuxValue, nType ); SetValue( key, Move( value ), pszName ); }
	void SetUInt64( const N &name, uint64 nValue, uint32 nAuxValue = 0, ObjectAttributeValueType_t nType = OBJECT_ATTRIBUTE_VALUE_UINT64 ) { SetUInt64( MakeKey( name ), nValue, nullptr, nAuxValue, nType ); }
	void SetUInt64( const char *pszName, uint64 nValue, uint32 nAuxValue = 0, ObjectAttributeValueType_t nType = OBJECT_ATTRIBUTE_VALUE_UINT64 ) { SetUInt64( MakeKey( pszName ), nValue, pszName, nAuxValue, nType ); }

	void SetPointer( const ObjectAttributeKey_t &key, void *pValue, const char *pszName = nullptr, uint32 nAuxValue = 0, ObjectAttributeValueType_t nType = OBJECT_ATTRIBUTE_VALUE_MISC32 ) { V value; value.SetPointer( pValue, nAuxValue, nType ); SetValue( key, Move( value ), pszName ); }
	void SetPointer( const N &name, void *pValue, uint32 nAuxValue = 0, ObjectAttributeValueType_t nType = OBJECT_ATTRIBUTE_VALUE_MISC32 ) { SetPointer( MakeKey( name ), pValue, nullptr, nAuxValue, nType ); }
	void SetPointer( const char *pszName, void *pValue, uint32 nAuxValue = 0, ObjectAttributeValueType_t nType = OBJECT_ATTRIBUTE_VALUE_MISC32 ) { SetPointer( MakeKey( pszName ), pValue, pszName, nAuxValue, nType ); }

	void SetString( const ObjectAttributeKey_t &key, const char *pszValue, const char *pszName = nullptr, uint32 nAuxValue = 0 ) { V value; value.SetString( pszValue, nAuxValue ); SetValue( key, Move( value ), pszName ); }
	void SetString( const N &name, const char *pszValue, uint32 nAuxValue = 0 ) { SetString( MakeKey( name ), pszValue, nullptr, nAuxValue ); }
	void SetString( const char *pszName, const char *pszValue, uint32 nAuxValue = 0 ) { SetString( MakeKey( pszName ), pszValue, pszName, nAuxValue ); }
	void SetRawValue( const ObjectAttributeKey_t &key, const V &value, const char *pszName = nullptr ) { SetValue( key, value, pszName ); }
	void SetRawValue( const ObjectAttributeKey_t &key, V &&value, const char *pszName = nullptr ) { SetValue( key, Move( value ), pszName ); }
	void SetRawValue( const N &name, const V &value ) { SetValue( name, value ); }
	void SetRawValue( const N &name, V &&value ) { SetValue( name, Move( value ) ); }
	void SetRawValue( const char *pszName, const V &value ) { SetValue( pszName, value ); }
	void SetRawValue( const char *pszName, V &&value ) { SetValue( pszName, Move( value ) ); }

	int32 GetInt( const ObjectAttributeKey_t &key, int32 nDefaultValue = 0 ) const { const V *pValue = FindValue( key ); return pValue ? pValue->GetInt( nDefaultValue ) : nDefaultValue; }
	int32 GetInt( const N &name, int32 nDefaultValue = 0 ) const { return GetInt( MakeKey( name ), nDefaultValue ); }
	int32 GetInt( const char *pszName, int32 nDefaultValue = 0 ) const { return GetInt( MakeKey( pszName ), nDefaultValue ); }

	float GetFloat( const ObjectAttributeKey_t &key, float flDefaultValue = 0.0f ) const { const V *pValue = FindValue( key ); return pValue ? pValue->GetFloat( flDefaultValue ) : flDefaultValue; }
	float GetFloat( const N &name, float flDefaultValue = 0.0f ) const { return GetFloat( MakeKey( name ), flDefaultValue ); }
	float GetFloat( const char *pszName, float flDefaultValue = 0.0f ) const { return GetFloat( MakeKey( pszName ), flDefaultValue ); }

	uint64 GetUInt64( const ObjectAttributeKey_t &key, uint64 nDefaultValue = 0 ) const { const V *pValue = FindValue( key ); return pValue ? pValue->GetUInt64( nDefaultValue ) : nDefaultValue; }
	uint64 GetUInt64( const N &name, uint64 nDefaultValue = 0 ) const { return GetUInt64( MakeKey( name ), nDefaultValue ); }
	uint64 GetUInt64( const char *pszName, uint64 nDefaultValue = 0 ) const { return GetUInt64( MakeKey( pszName ), nDefaultValue ); }

	void *GetPointer( const ObjectAttributeKey_t &key, void *pDefaultValue = nullptr ) const { const V *pValue = FindValue( key ); return pValue ? pValue->GetPointer( pDefaultValue ) : pDefaultValue; }
	void *GetPointer( const N &name, void *pDefaultValue = nullptr ) const { return GetPointer( MakeKey( name ), pDefaultValue ); }
	void *GetPointer( const char *pszName, void *pDefaultValue = nullptr ) const { return GetPointer( MakeKey( pszName ), pDefaultValue ); }

	const char *GetString( const ObjectAttributeKey_t &key, const char *pszDefaultValue = "" ) const { const V *pValue = FindValue( key ); return pValue ? pValue->GetString( pszDefaultValue ) : pszDefaultValue; }
	const char *GetString( const N &name, const char *pszDefaultValue = "" ) const { return GetString( MakeKey( name ), pszDefaultValue ); }
	const char *GetString( const char *pszName, const char *pszDefaultValue = "" ) const { return GetString( MakeKey( pszName ), pszDefaultValue ); }

	const V &GetValueOrDefault( const ObjectAttributeKey_t &key, const V &defaultValue = {} ) const { const V *pValue = FindValue( key ); return pValue ? *pValue : defaultValue; }
	const V &GetValueOrDefault( const N &name, const V &defaultValue = {} ) const { return GetValueOrDefault( MakeKey( name ), defaultValue ); }
	const V &GetValueOrDefault( const char *pszName, const V &defaultValue = {} ) const { return GetValueOrDefault( MakeKey( pszName ), defaultValue ); }

	// Remove the attribute from both maps so the table stays internally consistent.
	bool Remove( const ObjectAttributeKey_t &key )
	{
		bool bRemovedValue = m_Values.Remove( key );
		bool bRemovedName = RemoveStoredName( key );

		return bRemovedValue || bRemovedName;
	}
	bool Remove( const N &name ) { return Remove( MakeKey( name ) ); }
	bool Remove( const char *pszName ) { return Remove( MakeKey( pszName ) ); }

	void RemoveAt( IndexType_t it )
	{
		if ( !IsValidIndex( it ) )
			return;

		const ObjectAttributeKey_t key = Key( it );

		m_Values.RemoveAt( it );
		RemoveStoredName( key );
	}
	void SetStoredName( const ObjectAttributeKey_t &key, const char *pszName ) { UpdateStoredName( key, pszName ); }

public:
	ValueMap_t m_Values;
	NameMap_t m_Names;

private:
	// Preserve the original attribute spelling separately from the hashed lookup key.
	void UpdateStoredName( const ObjectAttributeKey_t &key, const char *pszName )
	{
		if ( !pszName )
			return;

		if ( !pszName[0] )
		{
			RemoveStoredName( key );

			return;
		}

		m_Names.InsertOrReplace( key, N( pszName ) );
	}
	bool RemoveStoredName( const ObjectAttributeKey_t &key ) { return m_Names.Remove( key ); }
};
COMPILE_TIME_ASSERT( sizeof( CUtlObjectAttributeTable< CUtlString, ObjectAttributeValue_t > ) == 80 );

#endif // UTLOBJECTATTRIBUTETABLE_H
