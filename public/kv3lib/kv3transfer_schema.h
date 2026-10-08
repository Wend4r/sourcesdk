#ifndef KV3TRANSFER_SCHEMA_H
#define KV3TRANSFER_SCHEMA_H

#ifdef _WIN32
#pragma once
#endif

//--------------------------------------------------------------------------------------------------
// Transfers a class through a description of its fields instead of generated KV3TransferSave_<classname>
// and KV3TransferLoad_<classname> bodies. The description does not depend on the schema system: whoever
// owns the schema bindings fills it from the class, field and enum infos. The KV3 transfer schema tags
// are at the end
//--------------------------------------------------------------------------------------------------
#include "kv3lib/kv3transfer.h"
#include "schemasystem/schemametatag.h"
#include "tier0/threadtools.h"

#include <type_traits>
#include <utility>

class CKV3TransferSchemaClass;


//--------------------------------------------------------------------------------------------------
// The keys of a class saved from a default constructed instance. The getter the schema metadata
// MGetKV3ClassDefaults points at returns one, built once per class. The generated KV3TransferLoad_<classname>
// takes one, so that a derived class passes its own defaults on to the fields of its base classes.
//--------------------------------------------------------------------------------------------------
struct KV3TransferDefaultKeys_t
{
	// A table, or null when saving the default instance failed.
	const KeyValues3 *m_pDefaultKeys;

	// A null value in every class seen. The game looks the member name up in it before m_pDefaultKeys
	// without that changing which default it loads.
	const KeyValues3 *m_pUnk08;

	// The default value of a member; the null value when there is none.
	const KeyValues3 *FindMember( const CKV3MemberName &name ) const;
};


//--------------------------------------------------------------------------------------------------
// Whether two values are equal, for the types that can tell. A schema field holding the value of the
// default instance of its class is not saved to an ISave.
//--------------------------------------------------------------------------------------------------
template < typename T, typename = void >
struct KV3Transfer_HasEqualTo : std::false_type {};

template < typename T >
struct KV3Transfer_HasEqualTo< T, std::void_t< decltype( std::declval< const T & >() == std::declval< const T & >() ) > > : std::true_type {};

template < typename T >
struct CKV3TransferEquality
{
	static constexpr bool is_present = KV3Transfer_HasEqualTo< T >::value && !std::is_array_v< T >;

	static bool IsEqual( const T &value, const T &otherValue )
	{
		if constexpr ( is_present )
			return value == otherValue;
		else
			return false;
	}
};

template < typename T, size_t N >
struct CKV3TransferEquality< T[ N ] >
{
	static constexpr bool is_present = CKV3TransferEquality< T >::is_present;

	static bool IsEqual( const T ( &value )[ N ], const T ( &otherValue )[ N ] )
	{
		for ( size_t i = 0; i < N; ++i )
		{
			if ( !CKV3TransferEquality< T >::IsEqual( value[ i ], otherValue[ i ] ) )
				return false;
		}

		return true;
	}
};

template < typename TVector >
struct CKV3TransferEqualityVector
{
	using Element_t = std::remove_cv_t< std::remove_reference_t< decltype( std::declval< const TVector & >()[ 0 ] ) > >;

	static constexpr bool is_present = CKV3TransferEquality< Element_t >::is_present;

	static bool IsEqual( const TVector &value, const TVector &otherValue )
	{
		if ( value.Count() != otherValue.Count() )
			return false;

		for ( int i = 0; i < value.Count(); ++i )
		{
			if ( !CKV3TransferEquality< Element_t >::IsEqual( value[ i ], otherValue[ i ] ) )
				return false;
		}

		return true;
	}
};

template < typename T, typename I, typename A >
struct CKV3TransferEquality< CUtlVector< T, I, A > > : CKV3TransferEqualityVector< CUtlVector< T, I, A > > {};

template < typename T, int N >
struct CKV3TransferEquality< CUtlVectorFixedGrowable< T, N > > : CKV3TransferEqualityVector< CUtlVectorFixedGrowable< T, N > > {};

template < typename T, typename I, typename A >
struct CKV3TransferEquality< CUtlLeanVector< T, I, A > > : CKV3TransferEqualityVector< CUtlLeanVector< T, I, A > > {};


//--------------------------------------------------------------------------------------------------
// The value helper of a type behind function pointers. m_pfnIsEqual is null for a type that cannot be
// compared, whose field is then always saved.
//--------------------------------------------------------------------------------------------------
struct KV3TransferFieldOps_t
{
	void ( *m_pfnSave )( CKV3TransferSaveContext *pContext, KeyValues3 *pSaveToValue, const void *pValue );
	void ( *m_pfnLoad )( CKV3TransferLoadContext *pContext, const KeyValues3 *pLoadFromValue, void *pValue );
	bool ( *m_pfnIsEqual )( const void *pValue, const void *pOtherValue );
};

template < typename T >
struct CKV3TransferFieldOpsFor
{
	static void SaveValue( CKV3TransferSaveContext *pContext, KeyValues3 *pSaveToValue, const void *pValue )
	{
		pContext->SaveValueDirect( *static_cast< const T * >( pValue ), pSaveToValue );
	}

	static void LoadValue( CKV3TransferLoadContext *pContext, const KeyValues3 *pLoadFromValue, void *pValue )
	{
		pContext->LoadValueDirect( *static_cast< T * >( pValue ), pLoadFromValue );
	}

	static bool IsEqual( const void *pValue, const void *pOtherValue )
	{
		return CKV3TransferEquality< T >::IsEqual( *static_cast< const T * >( pValue ), *static_cast< const T * >( pOtherValue ) );
	}

	static constexpr KV3TransferFieldOps_t s_Ops = { &SaveValue, &LoadValue, CKV3TransferEquality< T >::is_present ? &IsEqual : nullptr };
};

template < typename T >
const KV3TransferFieldOps_t *KV3TransferFieldOps()
{
	return &CKV3TransferFieldOpsFor< T >::s_Ops;
}


//--------------------------------------------------------------------------------------------------
// An enum without a C++ type, as the schema describes it. An enumerator transfers as its name, any other
// value as an integer. Names compare case sensitively.
//--------------------------------------------------------------------------------------------------
struct KV3TransferSchemaEnumerator_t
{
	const char *m_pszName;
	int64 m_nValue;
};

struct KV3TransferSchemaEnum_t
{
	const char *m_pszName;

	// 1, 2, 4 or 8
	int m_nSize;

	const KV3TransferSchemaEnumerator_t *m_pEnumerators;
	int m_nEnumeratorCount;

	const KV3TransferSchemaEnumerator_t *FindEnumeratorByValue( int64 nValue ) const;
	const KV3TransferSchemaEnumerator_t *FindEnumeratorByName( const char *pszName ) const;
};


//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
enum KV3TransferSchemaFieldType_t
{
	// A type with a CKV3TransferValHelper; m_pOps.
	KV3TRANSFER_SCHEMA_FIELD_VALUE,

	// m_pEnum.
	KV3TRANSFER_SCHEMA_FIELD_ENUM,

	// A described class held by value; m_pClass.
	KV3TRANSFER_SCHEMA_FIELD_CLASS,

	// An owning pointer to a described class or a class derived from it; m_pClass.
	KV3TRANSFER_SCHEMA_FIELD_CLASS_POINTER,

	// m_pSaveRestoreOps, what the function MKV3TransferSaveOpsForField names returns.
	KV3TRANSFER_SCHEMA_FIELD_SAVE_RESTORE_OPS,
};

struct KV3TransferSchemaField_t
{
	CKV3MemberName m_Name;

	// From the start of the class that declares the field.
	int m_nOffset;

	KV3TransferSchemaFieldType_t m_nType;

	const KV3TransferFieldOps_t *m_pOps;
	const KV3TransferSchemaEnum_t *m_pEnum;
	const CKV3TransferSchemaClass *m_pClass;
	IKV3TransferSaveRestoreOps *m_pSaveRestoreOps;
};

template < typename T >
KV3TransferSchemaField_t KV3TransferSchema_ValueField( const CKV3MemberName &name, int nOffset )
{
	return { name, nOffset, KV3TRANSFER_SCHEMA_FIELD_VALUE, KV3TransferFieldOps< T >(), nullptr, nullptr, nullptr };
}

inline KV3TransferSchemaField_t KV3TransferSchema_EnumField( const CKV3MemberName &name, int nOffset, const KV3TransferSchemaEnum_t *pEnum )
{
	return { name, nOffset, KV3TRANSFER_SCHEMA_FIELD_ENUM, nullptr, pEnum, nullptr, nullptr };
}

inline KV3TransferSchemaField_t KV3TransferSchema_ClassField( const CKV3MemberName &name, int nOffset, const CKV3TransferSchemaClass *pClass )
{
	return { name, nOffset, KV3TRANSFER_SCHEMA_FIELD_CLASS, nullptr, nullptr, pClass, nullptr };
}

inline KV3TransferSchemaField_t KV3TransferSchema_ClassPointerField( const CKV3MemberName &name, int nOffset, const CKV3TransferSchemaClass *pClass )
{
	return { name, nOffset, KV3TRANSFER_SCHEMA_FIELD_CLASS_POINTER, nullptr, nullptr, pClass, nullptr };
}

inline KV3TransferSchemaField_t KV3TransferSchema_SaveRestoreOpsField( const CKV3MemberName &name, int nOffset, IKV3TransferSaveRestoreOps *pSaveRestoreOps )
{
	return { name, nOffset, KV3TRANSFER_SCHEMA_FIELD_SAVE_RESTORE_OPS, nullptr, nullptr, nullptr, pSaveRestoreOps };
}


//--------------------------------------------------------------------------------------------------
// A class, its fields and the chain to its base class. Only single inheritance is described: the base
// class lies at m_nBaseClassOffset.
//
// The default instance and the default keys are built on first use and kept until destruction.
//--------------------------------------------------------------------------------------------------
class CKV3TransferSchemaClass
{
public:
	// Names the class of pObject, which is an instance of this class or of a derived one.
	typedef void ( *PolymorphicClassnameFn_t )( const void *pObject, CBufferString &sOutClassName );

	// The description of a class by its polymorphic class name; null when there is none.
	typedef const CKV3TransferSchemaClass *( *FindClassFn_t )( const char *pszClassName );

	// A default constructed instance, released through DeallocateFn_t.
	typedef void *( *AllocateFn_t )();
	typedef void ( *DeallocateFn_t )( void *pObject );

	typedef const KV3TransferDefaultKeys_t *( *GetDefaultKeysFn_t )();

	CKV3TransferSchemaClass( const char *pszName, KV3TransferClassBehavior_t nBehavior, const KV3TransferSchemaField_t *pFields, int nFieldCount );
	~CKV3TransferSchemaClass();

	CKV3TransferSchemaClass( const CKV3TransferSchemaClass & ) = delete;
	CKV3TransferSchemaClass &operator=( const CKV3TransferSchemaClass & ) = delete;

	// m_pfnGetDefaultKeys when set, otherwise the keys a default instance saves to with optional interfaces.
	const KV3TransferDefaultKeys_t *GetDefaultKeys() const;

	// Null for a class without m_pfnAllocate.
	const void *GetDefaultInstance() const;

	// Whether this class is pBaseClass or derives from it; pOffset receives where pBaseClass lies in it.
	bool FindBaseClassOffset( const CKV3TransferSchemaClass *pBaseClass, int *pOffset ) const;

	const char *m_pszName;
	KV3TransferClassBehavior_t m_nBehavior;

	const KV3TransferSchemaField_t *m_pFields;
	int m_nFieldCount;

	const CKV3TransferSchemaClass *m_pBaseClass;
	int m_nBaseClassOffset;

	// Null for an abstract class.
	AllocateFn_t m_pfnAllocate;
	DeallocateFn_t m_pfnDeallocate;

	// KV3TRANSFER_CLASS_AS_POLYMORPHIC_TABLE only. Without m_pfnFindClass only the class itself resolves.
	PolymorphicClassnameFn_t m_pfnPolymorphicClassname;
	FindClassFn_t m_pfnFindClass;

	// The game's getter, from the schema metadata MGetKV3ClassDefaults of the class.
	GetDefaultKeysFn_t m_pfnGetDefaultKeys;

private:
	void KV3Transfer_DefaultKeys_Impl() const;

	mutable CThreadFastMutex m_DefaultsMutex;
	mutable bool m_bDefaultsBuilt;
	mutable void *m_pDefaultInstance;
	mutable KeyValues3 m_DefaultKeysTable;
	mutable KV3TransferDefaultKeys_t m_DefaultKeys;
};


//--------------------------------------------------------------------------------------------------
// The KV3TransferSave_<classname> and KV3TransferLoad_<classname> of a described class: the fields of the
// base classes first, then the fields of the class.
//
// A load without an IRestore gives a missing member its value in pDefaultKeys; null stands for the
// default keys of pClass.
//--------------------------------------------------------------------------------------------------
void KV3Transfer_SaveSchemaClassFields( CKV3TransferSaveContext *pContext, const CKV3TransferSchemaClass *pClass, const void *pObject );
void KV3Transfer_LoadSchemaClassFields( CKV3TransferLoadContext *pContext, const CKV3TransferSchemaClass *pClass, void *pObject, const KV3TransferDefaultKeys_t *pDefaultKeys = nullptr );

// A class value: a table, with the polymorphic class name for a polymorphic class. A polymorphic object
// transfers as the class its name resolves to. A save that fails leaves null.
void KV3Transfer_SaveSchemaClass( CKV3TransferSaveContext *pContext, const CKV3TransferSchemaClass *pClass, const void *pObject, KeyValues3 *pSaveToValue );
void KV3Transfer_LoadSchemaClass( CKV3TransferLoadContext *pContext, const CKV3TransferSchemaClass *pClass, void *pObject, const KeyValues3 *pLoadFromValue );

// An owning pointer; null saves and loads null. pObject points at the pClass part of the allocated object.
void KV3Transfer_SaveSchemaClassPointer( CKV3TransferSaveContext *pContext, const CKV3TransferSchemaClass *pClass, const void *pObject, KeyValues3 *pSaveToValue );
void KV3Transfer_LoadSchemaClassOwningPointer( CKV3TransferLoadContext *pContext, const CKV3TransferSchemaClass *pClass, void *&pObject, const KeyValues3 *pLoadFromValue );


//--------------------------------------------------------------------------------------------------
// Defines the functions CLASS_USES_KV3TRANSFER_DATA or CLASS_USES_KV3TRANSFER_VIRTUAL declares, except
// KV3TransferPolymorphicClassname and KV3TransferAllocateClassInstance, from the description pSchemaClass.
//--------------------------------------------------------------------------------------------------
#define KV3TRANSFER_SCHEMA_CLASS_BODIES( classname, pSchemaClass ) \
	void classname::KV3TransferSave( CKV3TransferSaveContext *pContext ) const { KV3TransferSave_##classname( pContext ); } \
	void classname::KV3TransferLoad( CKV3TransferLoadContext *pContext ) { KV3TransferLoad_##classname( pContext ); } \
	void classname::KV3TransferSave_##classname( CKV3TransferSaveContext *pContext ) const { KV3Transfer_SaveSchemaClassFields( pContext, ( pSchemaClass ), this ); } \
	void classname::KV3TransferLoad_##classname( CKV3TransferLoadContext *pContext ) { KV3Transfer_LoadSchemaClassFields( pContext, ( pSchemaClass ), this ); }


//--------------------------------------------------------------------------------------------------
// Runtime metadata
//--------------------------------------------------------------------------------------------------

// Getter of the keys a default constructed instance saves to
DECLARE_SCHEMA_META_TAG( MGetKV3ClassDefaults, META_TAG_ON_CLASS, META_VALUE( CKV3TransferSchemaClass::GetDefaultKeysFn_t ) );

// Name of the function returning the field IKV3TransferSaveRestoreOps, e.g. "GetEngineTimeSaveRestoreOps";
// the function itself is not reachable from it
DECLARE_SCHEMA_META_TAG( MKV3TransferSaveOpsForField, META_TAG_ON_FIELD, META_VALUE( const char * ) );

// Atomic transfers as a string, or as a key/value table
DECLARE_SCHEMA_META_TAG( MAtomicTransfersAsPlainString, META_TAG_ON_ATOMIC, META_TAG_ONLY() );
DECLARE_SCHEMA_META_TAG( MAtomicTransfersAsMap, META_TAG_ON_ATOMIC, META_TAG_ONLY() );

// Default of a missing member, parsed by CKV3TransferValHelper< T >::LoadDefault
DECLARE_SCHEMA_META_TAG( MDefaultString, META_TAG_ON_FIELD, META_VALUE( const char * ) );


//--------------------------------------------------------------------------------------------------
// Schema compiler only
//--------------------------------------------------------------------------------------------------
DECLARE_SCHEMA_META_TAG( MEmitKV3TransferIgnoreMultipleInheritance, META_TAG_ON_CLASS, META_TAG_ONLY() );
DECLARE_SCHEMA_META_TAG( MEmitKV3DontClearMissingFields, META_TAG_ON_CLASS, META_TAG_ONLY() );

//--------------------------------------------------------------------------------------------------
// Custom Save/Load functions
// Eg.
//	META( MEmitKV3TransferCustomSaveFn="SaveFn"; MEmitKV3TransferCustomLoadFn="LoadFn" );
// Where your class has methods:
//	void SaveFn( CKV3TransferSaveContext *pContext, const CKV3MemberName &name, const T &value )
//	void LoadFn( CKV3TransferLoadContext *pContext, const CKV3MemberName &name, T &value )
//--------------------------------------------------------------------------------------------------
DECLARE_SCHEMA_META_TAG( MEmitKV3TransferCustomSaveFn, META_TAG_ON_FIELD, META_VALUE( const char * ) );
DECLARE_SCHEMA_META_TAG( MEmitKV3TransferCustomLoadFn, META_TAG_ON_FIELD, META_VALUE( const char * ) );

// Alternate name to use for the KV3 member instead of the C++ field name
DECLARE_SCHEMA_META_TAG( MKV3TransferName, META_TAG_ON_FIELD, META_VALUE( const char * ) );

// Signature is: void Transfer[Pre|Post]SaveFn( CKV3TransferSaveContext *pContext )
// Signature is: void Transfer[Pre|Post]LoadFn( CKV3TransferLoadContext *pContext )
// TYPEMETA( MEmitKV3TransferPreLoadFn = "MyFunc" );
DECLARE_SCHEMA_META_TAG( MEmitKV3TransferPreLoadFn, META_TAG_ON_CLASS, META_VALUE( const char * ) );
DECLARE_SCHEMA_META_TAG( MEmitKV3TransferPostLoadFn, META_TAG_ON_CLASS, META_VALUE( const char * ) );
DECLARE_SCHEMA_META_TAG( MEmitKV3TransferPreSaveFn, META_TAG_ON_CLASS, META_VALUE( const char * ) );
DECLARE_SCHEMA_META_TAG( MEmitKV3TransferPostSaveFn, META_TAG_ON_CLASS, META_VALUE( const char * ) );

//--------------------------------------------------------------------------------------------------
// Bodies of CLASS_USES_KV3TRANSFER_DATA and CLASS_USES_KV3TRANSFER_VIRTUAL: base class first, then fields
//--------------------------------------------------------------------------------------------------
DECLARE_SCHEMA_CODEGEN_TAG( MEmitKV3Transfer, META_TAG_ON_FIELD,
(
	// Multiple inheritance is not supported (could implement pointer tracking but it's a pain)
	if ( class_has_multiple_bases() && !class_has_meta( MEmitKV3TransferIgnoreMultipleInheritance ) )
	{
		emit( "" )
		emit( "#error Cannot emit KV3Transfer function for class '$class_name$' because it uses multiple inheritance. Mark the class with MEmitKV3TransferIgnoreMultipleInheritance to treat it as single-inheritance" )
		for_each_base
		{
			emit( "// Multiply inherited base class: $loop_value$" )
		}
		emit( "" )
	}

	// Emit Save Function
	emit( "void $class_name$::KV3TransferSave( CKV3TransferSaveContext *pContext ) const" )
	emit( "{" )
	emit( "	KV3TransferSave_$class_name$( pContext );" )
	emit( "}" )
	emit( "" )
	emit( "void $class_name$::KV3TransferSave_$class_name$( CKV3TransferSaveContext *pContext ) const" )
	emit( "{" )
	if ( class_has_meta( MEmitKV3TransferPreSaveFn ) )
	{
		emit( "	$@remove_quotes(class_tag_value.MEmitKV3TransferPreSaveFn)$( pContext ); // MEmitKV3TransferPreSaveFn" )
	}

	if ( class_has_bases() )
	{
		emit( "// !!! NOTE: if you're getting a compiler error here you probably forgot your CLASS_USES_KV3TRANSFER macro in your base class!" )
		emit( "	KV3TransferSave_$baseclass_name$( pContext ); // chain to base" )
		emit( "" )
	}

	for_all_fields
	{
		if ( item_has_meta( MEmitKV3TransferCustomSaveFn ) )
		{
			emit( "	$@remove_quotes(tag_value.MEmitKV3TransferCustomSaveFn)$( pContext, CKV3MemberName( $item_quoted_name$, StringTokenFromHashCode($@as_string_token_hash(item_name)$) ), $item_name$ ); // MEmitKV3TransferCustomSave" )
		}
		else if ( item_has_meta( MKV3TransferName ) )
		{
			emit( "	pContext->SaveValueToMember( CKV3MemberName( $tag_value.MKV3TransferName$, StringTokenFromHashCode( $@as_string_token_hash( @remove_quotes( tag_value.MKV3TransferName ) )$ ) ), $item_name$ ); // MKV3TransferName" )
		}
		else
		{
			emit( "	pContext->SaveValueToMember( CKV3MemberName( $item_quoted_name$, StringTokenFromHashCode($@as_string_token_hash(item_name)$) ), $item_name$ );" )
		}
	}

	if ( class_has_meta( MEmitKV3TransferPostSaveFn ) )
	{
		emit( "	$@remove_quotes(class_tag_value.MEmitKV3TransferPostSaveFn)$( pContext ); // MEmitKV3TransferPostSaveFn" )
	}
	emit( "}" )
	emit( "" )

	// Emit Load Function
	emit( "void $class_name$::KV3TransferLoad( CKV3TransferLoadContext *pContext )" )
	emit( "{" )
	emit( "	KV3TransferLoad_$class_name$( pContext );" )
	emit( "}" )
	emit( "" )
	emit( "void $class_name$::KV3TransferLoad_$class_name$( CKV3TransferLoadContext *pContext )" )
	emit( "{" )
	if ( class_has_meta( MEmitKV3TransferPreLoadFn ) )
	{
		emit( "	$@remove_quotes(class_tag_value.MEmitKV3TransferPreLoadFn)$( pContext ); // MEmitKV3TransferPreLoadFn" )
	}

	if ( class_has_bases() )
	{
		emit( "// !!! NOTE: if you're getting a compiler error here you probably forgot your CLASS_USES_KV3TRANSFER macro in your base class!" )
		emit( "	KV3TransferLoad_$baseclass_name$( pContext ); // chain to base" )
		emit( "" )
	}

	for_all_fields
	{
		if ( item_has_meta( MEmitKV3TransferCustomLoadFn ) )
		{
			emit( "	$@remove_quotes(tag_value.MEmitKV3TransferCustomLoadFn)$( pContext, CKV3MemberName( $item_quoted_name$, StringTokenFromHashCode($@as_string_token_hash(item_name)$) ), $item_name$ ); // MEmitKV3TransferCustomLoad" )
		}
		else if ( item_has_meta( MKV3TransferName ) )
		{
			emit( "	pContext->LoadValueFromMember( CKV3MemberName( $tag_value.MKV3TransferName$, StringTokenFromHashCode( $@as_string_token_hash( @remove_quotes( tag_value.MKV3TransferName ) )$ ) ), $item_name$ ); // MKV3TransferName" )
		}
		else if ( class_has_meta( MEmitKV3DontClearMissingFields ) )
		{
			emit( "	pContext->LoadValueFromMemberIfPresent( CKV3MemberName( $item_quoted_name$, StringTokenFromHashCode($@as_string_token_hash(item_name)$) ), $item_name$ ); // MEmitKV3DontClearMissingFields" )
		}
		else if ( item_has_meta( MDefaultString ) )
		{
			emit( "	pContext->LoadValueFromMemberOrDefault( CKV3MemberName( $item_quoted_name$, StringTokenFromHashCode($@as_string_token_hash(item_name)$) ), $item_name$, $tag_value.MDefaultString$ );" )
		}
		else
		{
			emit( "	pContext->LoadValueFromMember( CKV3MemberName( $item_quoted_name$, StringTokenFromHashCode($@as_string_token_hash(item_name)$) ), $item_name$ );" )
		}
	}

	if ( class_has_meta( MEmitKV3TransferPostLoadFn ) )
	{
		emit( "	$@remove_quotes(class_tag_value.MEmitKV3TransferPostLoadFn)$( pContext ); // MEmitKV3TransferPostLoadFn" )
	}

	emit( "}" )
	emit( "" )

	// Ensure that we didn't make a nonvirtual KV3Transfer on a virtual class
	emit( "COMPILE_TIME_ASSERT( SCHEMA_TYPE_TRAITS_is_polymorphic( $class_name$ ) == $class_name$::KV3TRANSFER_IS_VIRTUAL );" )
) );

#endif // KV3TRANSFER_SCHEMA_H
