#ifndef SCHEMAGEN_H
#define SCHEMAGEN_H

#ifdef _WIN32
#pragma once
#endif

//--------------------------------------------------------------------------------------------------
// Runtime support of the code schemacompiler generates (*_schema.cpp, *_schema_registration.cpp).
// Not meant for hand-written code
//--------------------------------------------------------------------------------------------------
#include "tier0/platform.h"
#include "tier0/bufferstring.h"
#include "tier0/dbg.h"
#include "tier0/strtools.h"
#include "interfaces/interfaces.h"
#include "schemasystem/schemasystem.h"
#include "schemasystem/schemaregistration.h"
#include "schemasystem/schemaversionnumbers.h"

#include <cstddef>
#include <new>
#include <type_traits>

// Generated records stay inside the module, also without an export map
#if defined( _WIN32 )
#define SCHEMAGEN_LOCAL
#else
#define SCHEMAGEN_LOCAL __attribute__(( visibility( "hidden" ) ))
#endif

// Script helper of DECLARE_SCHEMA_CODEGEN_TAG, e.g. MEmitKV3Transfer
#define SCHEMA_TYPE_TRAITS_is_polymorphic( T ) std::is_polymorphic< T >::value

// SchemaClassFieldData_t::m_pType before registration: 0xF top nibble plus a slot of the project type table
#define SCHEMAGEN_TYPE_SLOT( nSlot ) reinterpret_cast< CSchemaType * >( static_cast< uintp >( 0xF000000000000000ull | static_cast< uint64 >( nSlot ) ) )

//--------------------------------------------------------------------------------------------------
// Member pointer of a private or protected field. An explicit instantiation may name an inaccessible
// member, the friend defined by it hands the pointer out:
//
//	struct SchemaGenAccess_CFoo_m_nBar { using Type_t = int CFoo::*; friend Type_t SchemaGen_MemberPointer( SchemaGenAccess_CFoo_m_nBar ); };
//	template struct CSchemaGenMemberAccess< SchemaGenAccess_CFoo_m_nBar, &CFoo::m_nBar >;
//--------------------------------------------------------------------------------------------------
template < typename TTag, typename TTag::Type_t pMember >
struct CSchemaGenMemberAccess
{
	friend typename TTag::Type_t SchemaGen_MemberPointer( TTag ) { return pMember; }
};

// Spells member pointers to arrays too, e.g. int ( CFoo::* )[ 4 ]
template < typename TMember, typename T >
using SchemaGen_MemberPtr_t = TMember T::*;

// Offset of a member from the start of T; no T is constructed
template < typename T, typename TMember >
inline int SchemaGen_MemberOffset( TMember T::*pMember )
{
	alignas( T ) static unsigned char s_Storage[ sizeof( T ) ];

	const T *pObject = reinterpret_cast< const T * >( s_Storage );

	return static_cast< int >( reinterpret_cast< const unsigned char * >( &( pObject->*pMember ) ) - s_Storage );
}

// Offset of the TBase subobject in TDerived; no TDerived is constructed
template < typename TDerived, typename TBase >
inline uint SchemaGen_BaseOffset()
{
	alignas( TDerived ) static unsigned char s_Storage[ sizeof( TDerived ) ];

	TDerived *pDerived = reinterpret_cast< TDerived * >( s_Storage );

	return static_cast< uint >( reinterpret_cast< unsigned char * >( static_cast< TBase * >( pDerived ) ) - s_Storage );
}

//--------------------------------------------------------------------------------------------------
// Class manipulator body; REGISTER is handled by the generated function itself
//--------------------------------------------------------------------------------------------------
template < typename T, typename = void >
struct CSchemaGenHasDynamicBinding : std::false_type {};

template < typename T >
struct CSchemaGenHasDynamicBinding< T, std::void_t< decltype( std::declval< T & >().Schema_DynamicBinding() ) > > : std::true_type {};

template < typename T >
inline constexpr bool SchemaGen_IsConstructible()
{
	return !std::is_abstract< T >::value && std::is_default_constructible< T >::value && std::is_destructible< T >::value;
}

// m_nFlags1 bits the compiler knows; scope and tag bits come from the generator
template < typename T >
inline constexpr uint32 SchemaGen_ClassFlags1()
{
	uint32 nFlags = SchemaGen_IsConstructible< T >() ? SCHEMA_CF1_CONSTRUCT_ALLOWED : SCHEMA_CF1_CONSTRUCT_DISALLOWED;

	if constexpr ( std::is_polymorphic< T >::value )
		nFlags |= SCHEMA_CF1_HAS_VIRTUAL_MEMBERS;

	if constexpr ( std::is_abstract< T >::value )
		nFlags |= SCHEMA_CF1_IS_ABSTRACT;

	if constexpr ( std::is_trivially_default_constructible< T >::value )
		nFlags |= SCHEMA_CF1_HAS_TRIVIAL_CONSTRUCTOR;

	if constexpr ( std::is_trivially_destructible< T >::value )
		nFlags |= SCHEMA_CF1_HAS_TRIVIAL_DESTRUCTOR;

	return nFlags;
}

template < typename T >
inline void *SchemaGen_ClassManipulator( SchemaClassManipulatorAction_t eAction, void *pObject, const SchemaClassInfoData_t &classInfo )
{
	switch ( eAction )
	{
		case SCHEMA_CLASS_MANIPULATOR_ACTION_ALLOCATE:
		{
			if constexpr ( SchemaGen_IsConstructible< T >() )
				return new T;
			else
				return nullptr;
		}

		case SCHEMA_CLASS_MANIPULATOR_ACTION_DEALLOCATE:
		{
			if constexpr ( SchemaGen_IsConstructible< T >() )
			{
				delete static_cast< T * >( pObject );
				return reinterpret_cast< void * >( 1 );
			}
			else
			{
				return nullptr;
			}
		}

		case SCHEMA_CLASS_MANIPULATOR_ACTION_CONSTRUCT_IN_PLACE:
		{
			if constexpr ( SchemaGen_IsConstructible< T >() )
				return new ( pObject ) T;
			else
				return nullptr;
		}

		case SCHEMA_CLASS_MANIPULATOR_ACTION_DESCTRUCT_IN_PLACE:
		{
			if constexpr ( SchemaGen_IsConstructible< T >() )
			{
				static_cast< T * >( pObject )->~T();
				return reinterpret_cast< void * >( 1 );
			}
			else
			{
				return nullptr;
			}
		}

		case SCHEMA_CLASS_MANIPULATOR_ACTION_GET_SCHEMA_BINDING:
		{
			// The dynamic type of the object knows its binding
			if constexpr ( CSchemaGenHasDynamicBinding< T >::value )
				return static_cast< T * >( pObject )->Schema_DynamicBinding().Get();
			else
				return classInfo.m_pSchemaBinding;
		}

		default:
			return nullptr;
	}
}

//--------------------------------------------------------------------------------------------------
// Collection manipulator of the CUtlVector family; the shape of the CS2 ones
//--------------------------------------------------------------------------------------------------
template < typename TCollection >
void *SchemaGen_UtlVectorManipulator( SchemaCollectionManipulatorAction_t eAction, void *pCollection, int nIndex1, int nIndex2 )
{
	TCollection *pVector = static_cast< TCollection * >( pCollection );

	switch ( eAction )
	{
		case SCHEMA_COLLECTION_MANIPULATOR_ACTION_GET_COUNT:
			return reinterpret_cast< void * >( static_cast< intp >( pVector->Count() ) );

		case SCHEMA_COLLECTION_MANIPULATOR_ACTION_GET_ELEMENT_CONST:
		case SCHEMA_COLLECTION_MANIPULATOR_ACTION_GET_ELEMENT:
			return &pVector->Element( nIndex1 );

		case SCHEMA_COLLECTION_MANIPULATOR_ACTION_SWAP_ELEMENTS:
		{
			V_swap( pVector->Element( nIndex1 ), pVector->Element( nIndex2 ) );
			return pCollection;
		}

		case SCHEMA_COLLECTION_MANIPULATOR_ACTION_INSERT_BEFORE:
		{
			if ( nIndex2 > 0 )
				pVector->InsertMultipleBefore( nIndex1, nIndex2 );

			return pCollection;
		}

		case SCHEMA_COLLECTION_MANIPULATOR_ACTION_REMOVE_MULTIPLE:
		{
			if ( nIndex2 > 0 )
				pVector->RemoveMultiple( nIndex1, nIndex2 );

			return pCollection;
		}

		case SCHEMA_COLLECTION_MANIPULATOR_ACTION_SET_COUNT:
		{
			pVector->SetCount( nIndex1 );
			return pCollection;
		}

		default:
			return reinterpret_cast< void * >( static_cast< intp >( -1 ) );
	}
}

//--------------------------------------------------------------------------------------------------
// Classes and enums of other modules: the own module scope first, then the global scope, then the
// type scopes of the import modules (file names, e.g. "libserver.so")
//--------------------------------------------------------------------------------------------------
inline CSchemaClassInfo *SchemaGen_FindClass( ISchemaSystem *pSchemaSystem, const char *pszClassName, const char *const *ppszImportModules )
{
	CSchemaSystemTypeScope *pModuleScope = pSchemaSystem->FindTypeScopeForModule( GetNameOfModule() );

	if ( pModuleScope )
	{
		if ( CSchemaClassInfo *pClassInfo = pModuleScope->FindDeclaredClass( pszClassName ).Get() )
			return pClassInfo;
	}

	if ( CSchemaClassInfo *pClassInfo = pSchemaSystem->GlobalTypeScope()->FindDeclaredClass( pszClassName ).Get() )
		return pClassInfo;

	for ( const char *const *ppszModule = ppszImportModules; ppszModule && *ppszModule; ++ppszModule )
	{
		CSchemaSystemTypeScope *pScope = pSchemaSystem->FindTypeScopeForModule( *ppszModule );

		if ( !pScope )
			continue;

		if ( CSchemaClassInfo *pClassInfo = pScope->FindDeclaredClass( pszClassName ).Get() )
			return pClassInfo;
	}

	return nullptr;
}

inline CSchemaEnumInfo *SchemaGen_FindEnum( ISchemaSystem *pSchemaSystem, const char *pszEnumName, const char *const *ppszImportModules )
{
	CSchemaSystemTypeScope *pModuleScope = pSchemaSystem->FindTypeScopeForModule( GetNameOfModule() );

	if ( pModuleScope )
	{
		if ( CSchemaEnumInfo *pEnumInfo = pModuleScope->FindDeclaredEnum( pszEnumName ).Get() )
			return pEnumInfo;
	}

	if ( CSchemaEnumInfo *pEnumInfo = pSchemaSystem->GlobalTypeScope()->FindDeclaredEnum( pszEnumName ).Get() )
		return pEnumInfo;

	for ( const char *const *ppszModule = ppszImportModules; ppszModule && *ppszModule; ++ppszModule )
	{
		CSchemaSystemTypeScope *pScope = pSchemaSystem->FindTypeScopeForModule( *ppszModule );

		if ( !pScope )
			continue;

		if ( CSchemaEnumInfo *pEnumInfo = pScope->FindDeclaredEnum( pszEnumName ).Get() )
			return pEnumInfo;
	}

	return nullptr;
}

// Reason of a retry, only requested on the last attempt
inline void SchemaGen_AppendFailure( CBufferString *pFailureReason, const char *pszProject, const char *pszOwner, const char *pszMissing )
{
	if ( !pFailureReason )
		return;

	pFailureReason->Append( pszProject );
	pFailureReason->Append( ": " );
	pFailureReason->Append( pszOwner );
	pFailureReason->Append( " needs " );
	pFailureReason->Append( pszMissing );
	pFailureReason->Append( ", which no reachable type scope has registered\n" );
}

// Type of a class of another module, the declared type of the scope that registered it
inline CSchemaType *SchemaGen_ExternalClassType( ISchemaSystem *pSchemaSystem, const char *pszClassName, const char *const *ppszImportModules )
{
	CSchemaClassInfo *pClassInfo = SchemaGen_FindClass( pSchemaSystem, pszClassName, ppszImportModules );

	return pClassInfo ? pClassInfo->m_pDeclaredClass : nullptr;
}

inline CSchemaType *SchemaGen_ExternalEnumType( ISchemaSystem *pSchemaSystem, const char *pszEnumName, const char *const *ppszImportModules )
{
	CSchemaEnumInfo *pEnumInfo = SchemaGen_FindEnum( pSchemaSystem, pszEnumName, ppszImportModules );

	if ( !pEnumInfo || !pEnumInfo->m_pTypeScope )
		return nullptr;

	return pEnumInfo->m_pTypeScope->FindType_DeclaredEnum( pszEnumName );
}

#endif // SCHEMAGEN_H
