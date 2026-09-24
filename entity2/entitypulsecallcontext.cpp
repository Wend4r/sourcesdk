#include "entitypulsecallcontext.h"
#include "mathlib/vector.h"
#include "tier0/globalsymbol.h"
#include "tier0/memalloc.h"
#include "tier0/utlstring.h"

bool CPulseCallContextValue::IsStoredInline() const
{
	size_t nAlignment;
	const size_t nSize = m_Type.GetSize( &nAlignment );

	return nSize <= sizeof( m_pValue ) && !( reinterpret_cast< uintp >( this ) & ( nAlignment - 1 ) );
}

void CPulseCallContextValue::SetType( const CPulseValueFullType &type )
{
	if ( type == m_Type )
		return;

	if ( m_Type.IsValid() )
	{
		m_Type.DestructValue( GetData() );

		if ( !IsStoredInline() )
			MemAlloc_FreeAligned( m_pValue );
	}

	m_pValue = nullptr;
	m_Type = type;

	if ( m_Type.IsValid() )
	{
		size_t nAlignment;
		const size_t nSize = m_Type.GetSize( &nAlignment );

		if ( !IsStoredInline() )
			m_pValue = MemAlloc_AllocAligned( nSize, nAlignment );

		m_Type.ConstructValue( GetData() );
	}
}

CPulseCallContextValue &CPulseCallContextValue::operator=( const CPulseCallContextValue &other )
{
	if ( this != &other )
	{
		SetType( other.m_Type );

		if ( other.m_Type.IsValid() )
			m_Type.CopyValue( other.GetData(), GetData() );
	}

	return *this;
}

void CPulseCallContextValue::VariantCompatibility_SetPulseVariantFromEntityVariant( const CVariant &value )
{
	switch ( value.m_type )
	{
		case FIELD_FLOAT32:
		{
			SetType( CPulseValueFullType( PVAL_FLOAT ) );
			*static_cast< float32 * >( GetData() ) = value.m_float32;

			break;
		}

		case FIELD_STRING:
		case FIELD_CSTRING:
		case FIELD_GLOBALSYMBOL:
		{
			const char *pszString = value.m_pszString;

			if ( value.m_type != FIELD_CSTRING && !pszString )
				pszString = "";

			SetType( CPulseValueFullType( PVAL_STRING ) );
			static_cast< CUtlString * >( GetData() )->Set( pszString );

			break;
		}

		case FIELD_VECTOR:
		{
			SetType( CPulseValueFullType( PVAL_VEC3 ) );
			*static_cast< Vector * >( GetData() ) = *value.m_pVector;

			break;
		}

		case FIELD_INT32:
		case FIELD_INT16:
		case FIELD_INT64:
		{
			SetType( CPulseValueFullType( PVAL_INT ) );
			*static_cast< int32 * >( GetData() ) = value.m_int32;

			break;
		}

		case FIELD_BOOLEAN:
		{
			SetType( CPulseValueFullType( PVAL_BOOL ) );
			*static_cast< bool * >( GetData() ) = value.m_bool;

			break;
		}

		// The game keeps the color in the variant itself.
		case FIELD_COLOR32:
		{
			SetType( CPulseValueFullType( PVAL_COLOR_RGB ) );
			*static_cast< uint32 * >( GetData() ) = value.m_uint32;

			break;
		}

		case FIELD_EHANDLE:
		{
			SetType( CPulseValueFullType( PVAL_EHANDLE ) );
			*static_cast< CEntityHandle * >( GetData() ) = value.m_hEntity;

			// Game: then asks the pulse type registry of the module for the type of the entity the handle points to
			// and takes its sub type; without the registry the sub type stays empty, as it does here.

			break;
		}

		default:
		{
			SetType( CPulseValueFullType() );

			break;
		}
	}
}

void CPulseCallContextValue::VariantCompatibility_SetEntityVariantFromPulseVariant( CVariant &value ) const
{
	const void *pData = GetData();

	// Pointer types are copied from a variant that points at the pulse value.
	CVariant source;

	switch ( m_Type.m_nType )
	{
		case PVAL_BOOL:
		{
			value.Free();
			value.m_type = FIELD_BOOLEAN;
			value.m_bool = *static_cast< const bool * >( pData );

			return;
		}

		case PVAL_STRING:
		{
			const char *pszString = *static_cast< const char *const * >( pData );

			source.m_type = FIELD_CSTRING;
			source.m_pszString = pszString ? pszString : "";

			break;
		}

		case PVAL_ENTITY_NAME:
		{
			const char *pszName = *static_cast< const char *const * >( pData );
			const char *pszSymbol = MakeGlobalSymbol( pszName ? pszName : "" ).String();

			source.m_type = FIELD_CSTRING;
			source.m_pszString = pszSymbol ? pszSymbol : "";

			break;
		}

		case PVAL_FLOAT:
		{
			value.Free();
			value.m_type = FIELD_FLOAT32;
			value.m_float32 = *static_cast< const float32 * >( pData );

			return;
		}

		case PVAL_INT:
		case PVAL_TYPESAFE_INT:
		{
			value.Free();
			value.m_type = FIELD_INT32;
			value.m_int32 = *static_cast< const int32 * >( pData );

			return;
		}

		case PVAL_TYPESAFE_INT64:
		{
			value.Free();
			value.m_type = FIELD_INT64;
			value.m_int64 = *static_cast< const int64 * >( pData );

			return;
		}

		case PVAL_EHANDLE:
		{
			value.Free();
			value.m_type = FIELD_EHANDLE;
			value.m_hEntity = *static_cast< const CEntityHandle * >( pData );

			return;
		}

		case PVAL_VEC2:
		{
			source.m_type = FIELD_VECTOR2D;
			source.m_pVector2D = static_cast< const Vector2D * >( pData );

			break;
		}

		case PVAL_VEC3:
		{
			source.m_type = FIELD_VECTOR;
			source.m_pVector = static_cast< const Vector * >( pData );

			break;
		}

		case PVAL_VEC3_WORLDSPACE:
		{
			source.m_type = FIELD_POSITION_VECTOR;
			source.m_pVector = static_cast< const Vector * >( pData );

			break;
		}

		case PVAL_VEC4:
		{
			source.m_type = FIELD_VECTOR4D;
			source.m_pVector4D = static_cast< const Vector4D * >( pData );

			break;
		}

		case PVAL_COLOR_RGB:
		{
			source.m_type = FIELD_COLOR32;
			source.m_uint32 = *static_cast< const uint32 * >( pData );

			break;
		}

		case PVAL_VOID:
		case PVAL_ARRAY:
			break;

		default:
		{
			// Game: converts a schema enum, and any other type after logging "Unhandled type %s in
			// VariantCompatibility_SetEntityVariantFromPulseVariant; defaulting to string", to a string through
			// the KV3 serialization of pulse values, which the SDK does not have. The value becomes void here.
			break;
		}
	}

	value.CopyFrom( source );
}

CPulseArgumentPack::CPulseArgumentPack() : m_pInfo( Create< CPulseCallContextInfo >() )
{
}

CPulseArgumentPack::~CPulseArgumentPack()
{
	CPulseCallContextInfo *pInfo = m_pInfo;

	if ( !pInfo )
		return;

	for ( int i = 0; i < pInfo->m_Types.Count(); ++i )
	{
		if ( pInfo->m_Types[ i ] )
			Release( pInfo->m_Types[ i ] );
	}

	pInfo->m_Types.RemoveAll();

	for ( int i = 0; i < pInfo->m_Values.Count(); ++i )
	{
		if ( pInfo->m_Values[ i ] )
			Release( pInfo->m_Values[ i ] );
	}

	pInfo->m_Values.RemoveAll();

	// Destructs the keys and frees their memory unless it is external
	Release( pInfo );
}

CPulseArgumentPack &CPulseArgumentPack::operator=( const CPulseArgumentPack &other )
{
	if ( this == &other )
		return *this;

	Clear();

	for ( int i = 0; i < other.Count(); ++i )
	{
		CPulseCallContextValue value;

		other.GetArgument( i, value );
		SetArgument( other.GetArgumentName( i ), i, value );
	}

	return *this;
}

void CPulseArgumentPack::Clear()
{
	CPulseCallContextInfo *pInfo = m_pInfo;

	m_Values.RemoveAll();

	for ( int i = 0; i < pInfo->m_Values.Count(); ++i )
	{
		if ( pInfo->m_Values[ i ] )
			Release( pInfo->m_Values[ i ] );
	}

	pInfo->m_Values.RemoveAll();
	pInfo->m_Keys.RemoveAll();

	for ( int i = 0; i < pInfo->m_Types.Count(); ++i )
	{
		if ( pInfo->m_Types[ i ] )
			Release( pInfo->m_Types[ i ] );
	}

	pInfo->m_Types.RemoveAll();

	if ( pInfo->m_pArguments && pInfo->m_pArguments->m_nCount > 0 )
		EnsureCount( pInfo->m_pArguments->m_nCount - 1 );
}

void CPulseArgumentPack::EnsureCount( int nIndex )
{
	CPulseCallContextInfo *pInfo = m_pInfo;

	while ( nIndex >= m_Values.Count() )
	{
		m_Values.AddToTail( nullptr );
		pInfo->m_Values.AddToTail( nullptr );

		// The argument list gives the name and the type.
		if ( pInfo->m_pArguments )
		{
			pInfo->m_Types.AddToTail( nullptr );

			continue;
		}

		AddKey( pInfo->m_Keys, "" );

		pInfo->m_Types.AddToTail( Create< CPulseValueFullType >() );
	}
}

const CPulseValueFullType *CPulseArgumentPack::GetArgumentType( int nIndex ) const
{
	const CPulseValueFullType *pType = m_pInfo->m_Types[ nIndex ];

	if ( !pType && m_pInfo->m_pArguments )
		pType = &m_pInfo->m_pArguments->m_pElements[ nIndex ].m_Type;

	return pType;
}

CKV3MemberName CPulseArgumentPack::GetArgumentName( int nIndex ) const
{
	if ( const PulseMethodArgList_t *pArguments = m_pInfo->m_pArguments )
	{
		const CPulseRuntimeMethodArg &argument = pArguments->m_pElements[ nIndex ];

		return CKV3MemberName( argument.m_nNameHash, static_cast< UtlSymLargeId_t >( argument.m_nUnk0004 ), argument.m_pName );
	}

	return m_pInfo->m_Keys[ nIndex ];
}

void CPulseArgumentPack::GetArgument( int nIndex, CPulseCallContextValue &value ) const
{
	const CPulseValueFullType *pType = GetArgumentType( nIndex );

	value.SetType( *pType );
	pType->CopyValue( m_Values[ nIndex ], value.GetData() );
}

bool CPulseArgumentPack::SetArgument( const CKV3MemberName &name, int nIndex, const CPulseCallContextValue &value, const CPulseValueFullType &type )
{
	if ( !value.m_Type.IsValid() )
		return false;

	EnsureCount( nIndex );

	CPulseCallContextInfo *pInfo = m_pInfo;

	if ( !pInfo->m_pArguments )
		SetKeyName( nIndex, name.GetString() );

	// With an argument list the argument keeps the type the list or an earlier value gave it.
	const CPulseValueFullType &targetType = pInfo->m_pArguments ? *GetArgumentType( nIndex ) : type;

	if ( targetType.IsValid() && value.m_Type != targetType )
	{
		// Game: when the types differ, the pulse type registry of the module decides whether value is compatible
		// with or converts to the type and stores it accordingly; the registry is not reachable from the SDK.
		EnsureValue( nIndex );

		return false;
	}

	if ( !targetType.IsValid() && pInfo->m_pArguments )
	{
		EnsureValue( nIndex );

		return false;
	}

	if ( CPulseValueFullType *pType = pInfo->m_Types[ nIndex ] )
		*pType = value.m_Type;
	else
		pInfo->m_Types[ nIndex ] = Create< CPulseValueFullType >( value.m_Type );

	EnsureValue( nIndex );
	value.m_Type.CopyValue( value.GetData(), m_Values[ nIndex ] );

	return true;
}

void CPulseArgumentPack::SetParamFromVariant( const CVariant &value )
{
	CPulseCallContextValue param;

	param.VariantCompatibility_SetPulseVariantFromEntityVariant( value );
	SetArgument( CKV3MemberName( "param" ), 0, param );
}

void CPulseArgumentPack::GetParamAsVariant( CVariant &value ) const
{
	if ( Count() <= 0 )
		return;

	CPulseCallContextValue param;

	GetArgument( 0, param );
	param.VariantCompatibility_SetEntityVariantFromPulseVariant( value );
}

void CPulseArgumentPack::AddKey( CUtlLeanVector< CKV3MemberNameWithStorage > &keys, const char *pszName )
{
	const CKV3MemberNameWithStorage *pOldKeys = keys.Base();
	CKV3MemberNameWithStorage *pKey = keys.AddToTailGetPtr();

	// A key points into its own storage, so the moved keys are pointed at it again
	if ( keys.Base() != pOldKeys )
	{
		for ( int i = 0; i < keys.Count() - 1; ++i )
		{
			CKV3MemberNameWithStorage &key = keys[ i ];

			key = CKV3MemberName( key.GetHashCode(), key.GetSymLargeId(), key.GetStorage().Get() );
		}
	}

	*pKey = CKV3MemberName( 0u, UTL_INVAL_SYMBOL_LARGE );
	pKey->Set( pszName );
}

void CPulseArgumentPack::SetKeyName( int nIndex, const char *pszName )
{
	m_pInfo->m_Keys[ nIndex ].Set( pszName );
}

void CPulseArgumentPack::EnsureValue( int nIndex )
{
	if ( m_Values[ nIndex ] )
		return;

	CPulseCallContextValue *pValue = Create< CPulseCallContextValue >();

	m_pInfo->m_Values[ nIndex ] = pValue;
	pValue->SetType( *GetArgumentType( nIndex ) );

	m_Values[ nIndex ] = pValue->GetData();
}
