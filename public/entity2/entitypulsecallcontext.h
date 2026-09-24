#ifndef ENTITYPULSECALLCONTEXT_H
#define ENTITYPULSECALLCONTEXT_H

#include "entitypulse.h"
#include "tier1/keyvalues3.h"
#include "tier1/utlleanvector.h"
#include "tier1/utlvector.h"

class CPulseCallContextValue
{
public:
	CPulseCallContextValue() : m_pValue( nullptr ) {}
	CPulseCallContextValue( const CPulseCallContextValue &other ) : CPulseCallContextValue() { *this = other; }
	~CPulseCallContextValue() { SetType( CPulseValueFullType() ); }

	CPulseCallContextValue &operator=( const CPulseCallContextValue &other );

	bool IsStoredInline() const;
	void *GetData() { return IsStoredInline() ? static_cast< void * >( &m_pValue ) : m_pValue; }
	const void *GetData() const { return IsStoredInline() ? static_cast< const void * >( &m_pValue ) : m_pValue; }

	// Replaces the value with a default one of the new type
	void SetType( const CPulseValueFullType &type );

	void VariantCompatibility_SetPulseVariantFromEntityVariant( const CVariant &value );
	void VariantCompatibility_SetEntityVariantFromPulseVariant( CVariant &value ) const;

public:
	void *m_pValue;
	CPulseValueFullType m_Type;
};
COMPILE_TIME_ASSERT( sizeof( CPulseCallContextValue ) == 32 );

class CPulseCallContextInfo
{
public:
	// The runtime uses the argument list when present and keeps key/type overrides here
	const PulseMethodArgList_t *m_pArguments = nullptr;
	CUtlLeanVector< CKV3MemberNameWithStorage > m_Keys;
	CUtlVectorFixed< CPulseValueFullType *, 16 > m_Types;
	CUtlVectorFixed< CPulseCallContextValue *, 16 > m_Values;
};
COMPILE_TIME_ASSERT( sizeof( CPulseCallContextInfo ) == 296 );
COMPILE_TIME_ASSERT( alignof( CPulseCallContextInfo ) == 8 );

class CPulseArgumentPack
{
public:
	// A pack without an argument list
	CPulseArgumentPack();
	CPulseArgumentPack( const CPulseArgumentPack &other ) : CPulseArgumentPack() { *this = other; }
	~CPulseArgumentPack();

	CPulseArgumentPack &operator=( const CPulseArgumentPack &other );

	int Count() const { return m_Values.Count(); }
	bool HasValue( int nIndex ) const { return m_pInfo->m_Values[ nIndex ] != nullptr; }

	// Removes the arguments, a pack with an argument list keeps them empty
	void Clear();

	void EnsureCount( int nIndex );

	const CPulseValueFullType *GetArgumentType( int nIndex ) const;
	CKV3MemberName GetArgumentName( int nIndex ) const;
	void GetArgument( int nIndex, CPulseCallContextValue &value ) const;

	// False when the value cannot be stored
	bool SetArgument( const CKV3MemberName &name, int nIndex, const CPulseCallContextValue &value, const CPulseValueFullType &type = CPulseValueFullType() );

	void SetParamFromVariant( const CVariant &value );

	void GetParamAsVariant( CVariant &value ) const;

private:
	static void AddKey( CUtlLeanVector< CKV3MemberNameWithStorage > &keys, const char *pszName );
	void SetKeyName( int nIndex, const char *pszName );

	void EnsureValue( int nIndex );

public:
	// The runtime owns the info block and the objects referenced by its arrays
	CUtlVectorFixed< void *, 16 > m_Values;
	CPulseCallContextInfo *m_pInfo;
};
COMPILE_TIME_ASSERT( sizeof( CPulseArgumentPack ) == 144 );
COMPILE_TIME_ASSERT( alignof( CPulseArgumentPack ) == 8 );

using CPulseCallContext = CPulseArgumentPack;

class CPulseInputParamMap
{
public:
	void Merge( const CPulseInputParamMap &other )
	{
		m_KV3.OverlayKeysFrom( other.m_KV3 );
		m_bForwardAllArgs = m_bForwardAllArgs || other.m_bForwardAllArgs;
	}

public:
	KeyValues3 m_KV3;
	bool m_bForwardAllArgs = false;
};
COMPILE_TIME_ASSERT( sizeof( CPulseInputParamMap ) == 24 );

#endif // ENTITYPULSECALLCONTEXT_H
