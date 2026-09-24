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

#endif // UTLOBJECTATTRIBUTETABLE_H
