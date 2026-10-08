#ifndef SCHEMAMETATAG_H
#define SCHEMAMETATAG_H

#ifdef _WIN32
#pragma once
#endif

//--------------------------------------------------------------------------------------------------
// Schema metadata (META and TYPEMETA) lookup and tag declarations
//--------------------------------------------------------------------------------------------------
#include "schemasystem/schematypes.h"
#include "tier0/dbg.h"
#include "tier0/utlstring.h"
#include "tier1/utlvector.h"

enum SchemaMetaTagLocation_t
{
	META_TAG_ON_CLASS = 1 << 0,
	META_TAG_ON_FIELD = 1 << 1,
	META_TAG_ON_METHOD = 1 << 2,
	META_TAG_ON_ENUM = 1 << 3,
	META_TAG_ON_ENUMERATOR = 1 << 4,
	META_TAG_ON_ATOMIC = 1 << 5,
};

// Base classes a class lookup also visits; primary is the first base, in-memory order visits bases first
enum SchemaBaseClassTraversal_t
{
	SCHEMA_BASE_TRAVERSAL_NONE = 0,
	SCHEMA_BASE_TRAVERSAL_PRIMARY_ONLY,
	SCHEMA_BASE_TRAVERSAL_FULL,
	SCHEMA_BASE_TRAVERSAL_PRIMARY_ONLY_IN_MEMORY_ORDER,
	SCHEMA_BASE_TRAVERSAL_FULL_IN_MEMORY_ORDER,
};

typedef const CSchemaClassInfo *ClassIntrospectionHandle_t;
typedef const CSchemaClassField *FieldIntrospectionHandle_t;


//--------------------------------------------------------------------------------------------------
// m_pData points at the tag value, null for a tag without one
//--------------------------------------------------------------------------------------------------
class CSchemaMetadataEntry : public SchemaMetadataEntryData_t
{
};

//--------------------------------------------------------------------------------------------------
// Metadata of one class, field, enum, enumerator or atomic; a tag may repeat, names are case sensitive
//--------------------------------------------------------------------------------------------------
class CSchemaMetadataSet
{
public:
	CSchemaMetadataSet() : m_nNumEntries( 0 ), m_pEntries( nullptr ) {}
	CSchemaMetadataSet( int nNumEntries, const SchemaMetadataEntryData_t *pEntries ) : m_nNumEntries( pEntries ? nNumEntries : 0 ), m_pEntries( static_cast< const CSchemaMetadataEntry * >( pEntries ) ) {}

	// Value of the first instance; null when absent or valueless
	void *GetPtr( const char *pszTagName ) const
	{
		for ( int i = 0; i < m_nNumEntries; ++i )
		{
			if ( !V_strcmp( m_pEntries[ i ].m_pszName, pszTagName ) )
				return m_pEntries[ i ].m_pData;
		}

		return nullptr;
	}

	bool IsPresent( const char *pszTagName ) const
	{
		for ( int i = 0; i < m_nNumEntries; ++i )
		{
			if ( !V_strcmp( m_pEntries[ i ].m_pszName, pszTagName ) )
				return true;
		}

		return false;
	}

	int GetTagInstanceCount( const char *pszTagName ) const
	{
		int nCount = 0;

		for ( int i = 0; i < m_nNumEntries; ++i )
		{
			if ( !V_strcmp( m_pEntries[ i ].m_pszName, pszTagName ) )
				++nCount;
		}

		return nCount;
	}

	const CSchemaMetadataEntry *GetEntryByIndex( int nIndex ) const
	{
		Assert( nIndex >= 0 && nIndex < m_nNumEntries );

		return &m_pEntries[ nIndex ];
	}

	int GetEntryCount() const { return m_nNumEntries; }

	// Appends the value of every instance
	template < typename T >
	void GetAllPtrs( const char *pszTagName, CUtlVector< T * > &ptrs ) const
	{
		for ( int i = 0; i < m_nNumEntries; ++i )
		{
			if ( !V_strcmp( m_pEntries[ i ].m_pszName, pszTagName ) )
				ptrs.AddToTail( static_cast< T * >( m_pEntries[ i ].m_pData ) );
		}
	}

	int32 m_nNumEntries;
	const CSchemaMetadataEntry *m_pEntries;
};


//--------------------------------------------------------------------------------------------------
// Classes a lookup visits with their offsets from the start class. m_nDerivedClassIndex links a base to
// its derived class, -1 for the start class; a skipped class skips its bases too
//--------------------------------------------------------------------------------------------------
class CSchemaInheritanceIterator
{
public:
	struct ClassToTraverse_t
	{
		ClassIntrospectionHandle_t m_Class;
		uint32 m_nClassOffset;
		int m_nDerivedClassIndex;
	};

	CSchemaInheritanceIterator() : m_CurrentClass( nullptr ), m_ClassToSkip( nullptr ), m_nCurrentClassIndex( 0 ), m_nCurrentClassOffset( 0 ) {}

	CSchemaInheritanceIterator( ClassIntrospectionHandle_t traverseClass, SchemaBaseClassTraversal_t nBaseTraversal, ClassIntrospectionHandle_t classToSkip = nullptr ) : CSchemaInheritanceIterator()
	{
		PopulateTraversal( traverseClass, nBaseTraversal, classToSkip );
	}

	void PopulateTraversal( ClassIntrospectionHandle_t traverseClass, SchemaBaseClassTraversal_t nBaseTraversal, ClassIntrospectionHandle_t classToSkip )
	{
		m_ClassesToTraverse.RemoveAll();
		m_ClassToSkip = classToSkip;

		if ( traverseClass )
		{
			if ( nBaseTraversal != SCHEMA_BASE_TRAVERSAL_NONE )
			{
				const bool bFull = nBaseTraversal == SCHEMA_BASE_TRAVERSAL_FULL || nBaseTraversal == SCHEMA_BASE_TRAVERSAL_FULL_IN_MEMORY_ORDER;
				const bool bMemoryOrder = nBaseTraversal == SCHEMA_BASE_TRAVERSAL_PRIMARY_ONLY_IN_MEMORY_ORDER || nBaseTraversal == SCHEMA_BASE_TRAVERSAL_FULL_IN_MEMORY_ORDER;

				PopulateTraversal_R( traverseClass, classToSkip, 0, bFull, bMemoryOrder );
			}
			else if ( traverseClass != classToSkip )
			{
				m_ClassesToTraverse.AddToTail( { traverseClass, 0, -1 } );
			}
		}

		SetCurrentClassIndex( 0 );
	}

	bool IsValid() const { return m_CurrentClass != nullptr; }
	void Advance() { SetCurrentClassIndex( m_nCurrentClassIndex + 1 ); }

	ClassIntrospectionHandle_t CurrentClass() const { return m_CurrentClass; }
	uint32 CurrentClassOffset() const { return m_nCurrentClassOffset; }

	CUtlVectorFixedGrowable< ClassToTraverse_t, 8 > m_ClassesToTraverse;
	ClassIntrospectionHandle_t m_CurrentClass;
	ClassIntrospectionHandle_t m_ClassToSkip;
	int m_nCurrentClassIndex;
	uint32 m_nCurrentClassOffset;

private:
	void SetCurrentClassIndex( int nIndex )
	{
		m_nCurrentClassIndex = nIndex;

		if ( nIndex < m_ClassesToTraverse.Count() )
		{
			m_CurrentClass = m_ClassesToTraverse[ nIndex ].m_Class;
			m_nCurrentClassOffset = m_ClassesToTraverse[ nIndex ].m_nClassOffset;
		}
		else
		{
			m_CurrentClass = nullptr;
			m_nCurrentClassOffset = 0;
		}
	}

	// Index of pClass, -1 when skipped or null; without bFull only the first base is followed
	int PopulateTraversal_R( ClassIntrospectionHandle_t pClass, ClassIntrospectionHandle_t classToSkip, uint32 nClassOffset, bool bFull, bool bMemoryOrder )
	{
		if ( !pClass || pClass == classToSkip )
			return -1;

		int nBaseClassCount = pClass->m_nBaseClassCount;

		if ( nBaseClassCount > 1 && !bFull )
			nBaseClassCount = 1;

		int nIndex = bMemoryOrder ? -1 : m_ClassesToTraverse.AddToTail();

		int *pBaseClassIndices = static_cast< int * >( stackalloc( nBaseClassCount * sizeof( int ) ) );
		int nBaseClassIndexCount = 0;

		for ( int i = 0; i < nBaseClassCount; ++i )
		{
			const SchemaBaseClassInfoData_t &baseClass = pClass->m_pBaseClasses[ i ];
			const int nBaseClassIndex = PopulateTraversal_R( baseClass.m_pClass, classToSkip, nClassOffset + baseClass.m_nOffset, bFull, bMemoryOrder );

			if ( nBaseClassIndex != -1 )
				pBaseClassIndices[ nBaseClassIndexCount++ ] = nBaseClassIndex;
		}

		if ( bMemoryOrder )
			nIndex = m_ClassesToTraverse.AddToTail();

		for ( int i = 0; i < nBaseClassIndexCount; ++i )
			m_ClassesToTraverse[ pBaseClassIndices[ i ] ].m_nDerivedClassIndex = nIndex;

		m_ClassesToTraverse[ nIndex ] = { pClass, nClassOffset, -1 };

		return nIndex;
	}
};

//--------------------------------------------------------------------------------------------------
// Fields of the visited classes in traversal order; offsets are from the start class
//--------------------------------------------------------------------------------------------------
class CSchemaFieldIterator
{
public:
	CSchemaFieldIterator( ClassIntrospectionHandle_t classInfo, SchemaBaseClassTraversal_t nBaseTraversal ) : m_InheritanceIterator( classInfo, nBaseTraversal ), m_CurrentField( nullptr ), m_nFieldIndex( 0 )
	{
		AdvanceUntilCurrentClassHasMembers();
	}

	bool IsValid() const { return m_CurrentField != nullptr; }

	void Advance()
	{
		Assert( IsValid() );

		if ( ++m_nFieldIndex < m_InheritanceIterator.CurrentClass()->m_nFieldCount )
		{
			m_CurrentField = static_cast< FieldIntrospectionHandle_t >( &m_InheritanceIterator.CurrentClass()->m_pFields[ m_nFieldIndex ] );
			return;
		}

		m_InheritanceIterator.Advance();
		AdvanceUntilCurrentClassHasMembers();
	}

	int GetFieldMemoryOffset() const { return m_InheritanceIterator.CurrentClassOffset() + m_CurrentField->m_nSingleInheritanceOffset; }
	const char *GetFieldName() const { return m_CurrentField->m_pszName; }

	// "<class>::<field>"
	CUtlString GetFullyQualifiedFieldName() const { return CUtlString( m_InheritanceIterator.CurrentClass()->m_pszName ) + "::" + m_CurrentField->m_pszName; }

	CSchemaType *GetType() const { return m_CurrentField ? m_CurrentField->m_pType : nullptr; }
	FieldIntrospectionHandle_t GetField() const { return m_CurrentField; }
	ClassIntrospectionHandle_t CurrentClass() const { return m_InheritanceIterator.CurrentClass(); }

	CSchemaInheritanceIterator m_InheritanceIterator;
	FieldIntrospectionHandle_t m_CurrentField;
	int m_nFieldIndex;

private:
	void AdvanceUntilCurrentClassHasMembers()
	{
		m_nFieldIndex = 0;

		for ( ; m_InheritanceIterator.IsValid(); m_InheritanceIterator.Advance() )
		{
			if ( m_InheritanceIterator.CurrentClass()->m_nFieldCount != 0 )
			{
				m_CurrentField = static_cast< FieldIntrospectionHandle_t >( &m_InheritanceIterator.CurrentClass()->m_pFields[ 0 ] );
				return;
			}
		}

		m_CurrentField = nullptr;
	}
};

//--------------------------------------------------------------------------------------------------
// Metadata of a schema object; for a class, of the class itself only
//--------------------------------------------------------------------------------------------------
inline CSchemaMetadataSet SchemaMetadataSetOf( const CSchemaClassInfo *pClass ) { return CSchemaMetadataSet( pClass->m_nStaticMetadataCount, pClass->m_pStaticMetadata ); }
inline CSchemaMetadataSet SchemaMetadataSetOf( const CSchemaClassField *pField ) { return CSchemaMetadataSet( pField->m_nStaticMetadataCount, pField->m_pStaticMetadata ); }
inline CSchemaMetadataSet SchemaMetadataSetOf( const CSchemaStaticField *pField ) { return CSchemaMetadataSet( pField->m_nStaticMetadataCount, pField->m_pStaticMetadata ); }
inline CSchemaMetadataSet SchemaMetadataSetOf( const CSchemaEnumInfo *pEnum ) { return CSchemaMetadataSet( pEnum->m_nStaticMetadataCount, pEnum->m_pStaticMetadata ); }
inline CSchemaMetadataSet SchemaMetadataSetOf( const CSchemaEnumeratorInfo *pEnumerator ) { return CSchemaMetadataSet( pEnumerator->m_nStaticMetadataCount, pEnumerator->m_pStaticMetadata ); }
inline CSchemaMetadataSet SchemaMetadataSetOf( const SchemaAtomicTypeInfo_t *pAtomic ) { return CSchemaMetadataSet( pAtomic->m_nStaticMetadataCount, pAtomic->m_pStaticMetadata ); }
inline CSchemaMetadataSet SchemaMetadataSetOf( const CSchemaFieldIterator &fieldIterator ) { return SchemaMetadataSetOf( fieldIterator.GetField() ); }

// Asserts that the tag may be placed at nLocation
inline void SchemaMetaTag_AssertValidLocationFlag( uint nMetaFlags, uint nLocation, const char *pszTagName )
{
	AssertMsg( ( nMetaFlags & nLocation ) != 0, "Schema meta tag looked up where it is never placed" );
}

// Only pClassToSkip itself is skipped, not when reached as a base; only SCHEMA_BASE_TRAVERSAL_PRIMARY_ONLY
// stops at the first base
inline bool SchemaMetaIsPresent2( const CSchemaClassInfo *pClass, const char *pszTagName, SchemaBaseClassTraversal_t nBaseTraversal, const CSchemaClassInfo *pClassToSkip )
{
	if ( pClass == pClassToSkip )
		return false;

	if ( SchemaMetadataSetOf( pClass ).IsPresent( pszTagName ) )
		return true;

	if ( nBaseTraversal == SCHEMA_BASE_TRAVERSAL_NONE )
		return false;

	int nBaseClassCount = pClass->m_nBaseClassCount;

	if ( nBaseTraversal == SCHEMA_BASE_TRAVERSAL_PRIMARY_ONLY && nBaseClassCount > 1 )
		nBaseClassCount = 1;

	for ( int i = 0; i < nBaseClassCount; ++i )
	{
		const CSchemaClassInfo *pBaseClass = pClass->m_pBaseClasses[ i ].m_pClass;

		if ( pBaseClass && SchemaMetaIsPresent2( pBaseClass, pszTagName, nBaseTraversal, nullptr ) )
			return true;
	}

	return false;
}

// Value from the first visited class with the tag
inline void *SchemaMetaGetPtr2( const CSchemaClassInfo *pClass, const char *pszTagName, SchemaBaseClassTraversal_t nBaseTraversal, const CSchemaClassInfo *pClassToSkip )
{
	for ( CSchemaInheritanceIterator iterator( pClass, nBaseTraversal, pClassToSkip ); iterator.IsValid(); iterator.Advance() )
	{
		const CSchemaMetadataSet metadata = SchemaMetadataSetOf( iterator.CurrentClass() );

		if ( metadata.IsPresent( pszTagName ) )
			return metadata.GetPtr( pszTagName );
	}

	return nullptr;
}

template < typename T >
void SchemaMetaGetAllPtrs2( const CSchemaClassInfo *pClass, const char *pszTagName, CUtlVector< T * > &ptrs, SchemaBaseClassTraversal_t nBaseTraversal, const CSchemaClassInfo *pClassToSkip )
{
	for ( CSchemaInheritanceIterator iterator( pClass, nBaseTraversal, pClassToSkip ); iterator.IsValid(); iterator.Advance() )
		SchemaMetadataSetOf( iterator.CurrentClass() ).GetAllPtrs( pszTagName, ptrs );
}

template < typename TOwner >
bool SchemaMetaIsPresent2( TOwner owner, const char *pszTagName )
{
	return SchemaMetadataSetOf( owner ).IsPresent( pszTagName );
}

template < typename TOwner >
void *SchemaMetaGetPtr2( TOwner owner, const char *pszTagName )
{
	return SchemaMetadataSetOf( owner ).GetPtr( pszTagName );
}


//--------------------------------------------------------------------------------------------------
// DECLARE_SCHEMA_META_TAG( name, location flags, META_TAG_ONLY() or META_VALUE( type ) ) declares a class
// named after the tag with static lookups for every metadata owner; a value tag also reads Storage_t.
// A lookup at a location the flags exclude asserts
//
//	DECLARE_SCHEMA_META_TAG( MNotSaved, META_TAG_ON_FIELD, META_TAG_ONLY() );
//	if ( MNotSaved::IsPresent( pField ) )
//--------------------------------------------------------------------------------------------------
#define META_TAG_ONLY() SCHEMA_META_TAG_ONLY_MEMBERS, void
#define META_VALUE( storageType ) SCHEMA_META_TAG_VALUE_MEMBERS, storageType

#define SCHEMA_META_TAG_EXPAND( x ) x
#define DECLARE_SCHEMA_META_TAG( tagName, locationFlags, ... ) SCHEMA_META_TAG_EXPAND( SCHEMA_META_TAG_DECLARE( tagName, locationFlags, __VA_ARGS__ ) )

#define SCHEMA_META_TAG_DECLARE( tagName, locationFlags, members, storageType ) \
	class tagName \
	{ \
	public: \
		static const char *TagName() { return #tagName; } \
		static uint MetaFlags() { return ( locationFlags ); } \
		members( storageType ) \
	}

#define SCHEMA_META_TAG_ONLY_MEMBERS( storageType ) \
	static bool IsPresent( CSchemaMetadataSet metadata ) { return metadata.IsPresent( TagName() ); } \
	static bool IsPresent( const CSchemaClassInfo *pClass, SchemaBaseClassTraversal_t nBaseTraversal, const CSchemaClassInfo *pClassToSkip = nullptr ) { SchemaMetaTag_AssertValidLocationFlag( MetaFlags(), META_TAG_ON_CLASS, TagName() ); return SchemaMetaIsPresent2( pClass, TagName(), nBaseTraversal, pClassToSkip ); } \
	static bool IsPresent( const CSchemaClassField *pField ) { SchemaMetaTag_AssertValidLocationFlag( MetaFlags(), META_TAG_ON_FIELD, TagName() ); return IsPresent( SchemaMetadataSetOf( pField ) ); } \
	static bool IsPresent( const CSchemaStaticField *pField ) { SchemaMetaTag_AssertValidLocationFlag( MetaFlags(), META_TAG_ON_FIELD, TagName() ); return IsPresent( SchemaMetadataSetOf( pField ) ); } \
	static bool IsPresent( const CSchemaEnumInfo *pEnum ) { SchemaMetaTag_AssertValidLocationFlag( MetaFlags(), META_TAG_ON_ENUM, TagName() ); return IsPresent( SchemaMetadataSetOf( pEnum ) ); } \
	static bool IsPresent( const CSchemaEnumeratorInfo *pEnumerator ) { SchemaMetaTag_AssertValidLocationFlag( MetaFlags(), META_TAG_ON_ENUMERATOR, TagName() ); return IsPresent( SchemaMetadataSetOf( pEnumerator ) ); } \
	static bool IsPresent( const SchemaAtomicTypeInfo_t *pAtomic ) { SchemaMetaTag_AssertValidLocationFlag( MetaFlags(), META_TAG_ON_ATOMIC, TagName() ); return IsPresent( SchemaMetadataSetOf( pAtomic ) ); } \
	static bool IsPresent( const CSchemaFieldIterator &fieldIterator ) { return IsPresent( fieldIterator.GetField() ); }

#define SCHEMA_META_TAG_VALUE_MEMBERS( storageType ) \
	using Storage_t = storageType; \
	SCHEMA_META_TAG_ONLY_MEMBERS( storageType ) \
	static Storage_t *GetPtr( CSchemaMetadataSet metadata ) { return static_cast< Storage_t * >( metadata.GetPtr( TagName() ) ); } \
	static Storage_t *GetPtr( const CSchemaClassInfo *pClass, SchemaBaseClassTraversal_t nBaseTraversal, const CSchemaClassInfo *pClassToSkip = nullptr ) { SchemaMetaTag_AssertValidLocationFlag( MetaFlags(), META_TAG_ON_CLASS, TagName() ); return static_cast< Storage_t * >( SchemaMetaGetPtr2( pClass, TagName(), nBaseTraversal, pClassToSkip ) ); } \
	static Storage_t *GetPtr( const CSchemaClassField *pField ) { SchemaMetaTag_AssertValidLocationFlag( MetaFlags(), META_TAG_ON_FIELD, TagName() ); return GetPtr( SchemaMetadataSetOf( pField ) ); } \
	static Storage_t *GetPtr( const CSchemaStaticField *pField ) { SchemaMetaTag_AssertValidLocationFlag( MetaFlags(), META_TAG_ON_FIELD, TagName() ); return GetPtr( SchemaMetadataSetOf( pField ) ); } \
	static Storage_t *GetPtr( const CSchemaEnumInfo *pEnum ) { SchemaMetaTag_AssertValidLocationFlag( MetaFlags(), META_TAG_ON_ENUM, TagName() ); return GetPtr( SchemaMetadataSetOf( pEnum ) ); } \
	static Storage_t *GetPtr( const CSchemaEnumeratorInfo *pEnumerator ) { SchemaMetaTag_AssertValidLocationFlag( MetaFlags(), META_TAG_ON_ENUMERATOR, TagName() ); return GetPtr( SchemaMetadataSetOf( pEnumerator ) ); } \
	static Storage_t *GetPtr( const SchemaAtomicTypeInfo_t *pAtomic ) { SchemaMetaTag_AssertValidLocationFlag( MetaFlags(), META_TAG_ON_ATOMIC, TagName() ); return GetPtr( SchemaMetadataSetOf( pAtomic ) ); } \
	static Storage_t *GetPtr( const CSchemaFieldIterator &fieldIterator ) { return GetPtr( fieldIterator.GetField() ); } \
	static Storage_t GetValue( CSchemaMetadataSet metadata, Storage_t defaultValue ) { Storage_t *pValue = GetPtr( metadata ); return pValue ? *pValue : defaultValue; } \
	static Storage_t GetValue( const CSchemaClassInfo *pClass, Storage_t defaultValue, SchemaBaseClassTraversal_t nBaseTraversal, const CSchemaClassInfo *pClassToSkip = nullptr ) { Storage_t *pValue = GetPtr( pClass, nBaseTraversal, pClassToSkip ); return pValue ? *pValue : defaultValue; } \
	static Storage_t GetValue( const CSchemaClassField *pField, Storage_t defaultValue ) { Storage_t *pValue = GetPtr( pField ); return pValue ? *pValue : defaultValue; } \
	static Storage_t GetValue( const CSchemaStaticField *pField, Storage_t defaultValue ) { Storage_t *pValue = GetPtr( pField ); return pValue ? *pValue : defaultValue; } \
	static Storage_t GetValue( const CSchemaEnumInfo *pEnum, Storage_t defaultValue ) { Storage_t *pValue = GetPtr( pEnum ); return pValue ? *pValue : defaultValue; } \
	static Storage_t GetValue( const CSchemaEnumeratorInfo *pEnumerator, Storage_t defaultValue ) { Storage_t *pValue = GetPtr( pEnumerator ); return pValue ? *pValue : defaultValue; } \
	static Storage_t GetValue( const SchemaAtomicTypeInfo_t *pAtomic, Storage_t defaultValue ) { Storage_t *pValue = GetPtr( pAtomic ); return pValue ? *pValue : defaultValue; } \
	static Storage_t GetValue( const CSchemaFieldIterator &fieldIterator, Storage_t defaultValue ) { return GetValue( fieldIterator.GetField(), defaultValue ); } \
	static void GetAllPtrs( CSchemaMetadataSet metadata, CUtlVector< Storage_t * > &ptrs ) { metadata.GetAllPtrs( TagName(), ptrs ); } \
	static void GetAllPtrs( const CSchemaClassInfo *pClass, CUtlVector< Storage_t * > &ptrs, SchemaBaseClassTraversal_t nBaseTraversal, const CSchemaClassInfo *pClassToSkip = nullptr ) { SchemaMetaTag_AssertValidLocationFlag( MetaFlags(), META_TAG_ON_CLASS, TagName() ); SchemaMetaGetAllPtrs2( pClass, TagName(), ptrs, nBaseTraversal, pClassToSkip ); } \
	static void GetAllPtrs( const CSchemaClassField *pField, CUtlVector< Storage_t * > &ptrs ) { SchemaMetaTag_AssertValidLocationFlag( MetaFlags(), META_TAG_ON_FIELD, TagName() ); GetAllPtrs( SchemaMetadataSetOf( pField ), ptrs ); } \
	static void GetAllPtrs( const CSchemaStaticField *pField, CUtlVector< Storage_t * > &ptrs ) { SchemaMetaTag_AssertValidLocationFlag( MetaFlags(), META_TAG_ON_FIELD, TagName() ); GetAllPtrs( SchemaMetadataSetOf( pField ), ptrs ); } \
	static void GetAllPtrs( const CSchemaEnumInfo *pEnum, CUtlVector< Storage_t * > &ptrs ) { SchemaMetaTag_AssertValidLocationFlag( MetaFlags(), META_TAG_ON_ENUM, TagName() ); GetAllPtrs( SchemaMetadataSetOf( pEnum ), ptrs ); } \
	static void GetAllPtrs( const CSchemaEnumeratorInfo *pEnumerator, CUtlVector< Storage_t * > &ptrs ) { SchemaMetaTag_AssertValidLocationFlag( MetaFlags(), META_TAG_ON_ENUMERATOR, TagName() ); GetAllPtrs( SchemaMetadataSetOf( pEnumerator ), ptrs ); } \
	static void GetAllPtrs( const SchemaAtomicTypeInfo_t *pAtomic, CUtlVector< Storage_t * > &ptrs ) { SchemaMetaTag_AssertValidLocationFlag( MetaFlags(), META_TAG_ON_ATOMIC, TagName() ); GetAllPtrs( SchemaMetadataSetOf( pAtomic ), ptrs ); } \
	static void GetAllPtrs( const CSchemaFieldIterator &fieldIterator, CUtlVector< Storage_t * > &ptrs ) { GetAllPtrs( fieldIterator.GetField(), ptrs ); }

//--------------------------------------------------------------------------------------------------
// DECLARE_SCHEMA_CODEGEN_TAG( name, location flags, ( script ) ): the schema compiler runs the script for
// every class with META_USE_CODEGEN_TAG( name ); never present in the runtime metadata
//--------------------------------------------------------------------------------------------------
#define DECLARE_SCHEMA_CODEGEN_TAG( tagName, locationFlags, codegenScript ) DECLARE_SCHEMA_META_TAG( tagName, locationFlags, META_TAG_ONLY() )

#endif // SCHEMAMETATAG_H
