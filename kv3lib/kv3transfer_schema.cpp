#include "kv3lib/kv3transfer_schema.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
const KeyValues3 *KV3TransferDefaultKeys_t::FindMember( const CKV3MemberName &name ) const
{
	const KeyValues3 *pMember = nullptr;

	if ( m_pDefaultKeys && m_pDefaultKeys->IsTable() )
		pMember = m_pDefaultKeys->FindMember( name );

	return pMember ? pMember : &KeyValues3::GetNullValue();
}


//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
const KV3TransferSchemaEnumerator_t *KV3TransferSchemaEnum_t::FindEnumeratorByValue( int64 nValue ) const
{
	for ( int i = 0; i < m_nEnumeratorCount; ++i )
	{
		if ( m_pEnumerators[ i ].m_nValue == nValue )
			return &m_pEnumerators[ i ];
	}

	return nullptr;
}

const KV3TransferSchemaEnumerator_t *KV3TransferSchemaEnum_t::FindEnumeratorByName( const char *pszName ) const
{
	for ( int i = 0; i < m_nEnumeratorCount; ++i )
	{
		if ( !V_strcmp( m_pEnumerators[ i ].m_pszName, pszName ) )
			return &m_pEnumerators[ i ];
	}

	return nullptr;
}

static int64 KV3Transfer_ReadSchemaEnumValue( const KV3TransferSchemaEnum_t *pEnum, const void *pValue )
{
	switch ( pEnum->m_nSize )
	{
		case 1: return *static_cast< const int8 * >( pValue );
		case 2: return *static_cast< const int16 * >( pValue );
		case 8: return *static_cast< const int64 * >( pValue );
		default: return *static_cast< const int32 * >( pValue );
	}
}

static void KV3Transfer_WriteSchemaEnumValue( const KV3TransferSchemaEnum_t *pEnum, void *pValue, int64 nValue )
{
	switch ( pEnum->m_nSize )
	{
		case 1: *static_cast< int8 * >( pValue ) = static_cast< int8 >( nValue ); break;
		case 2: *static_cast< int16 * >( pValue ) = static_cast< int16 >( nValue ); break;
		case 8: *static_cast< int64 * >( pValue ) = nValue; break;
		default: *static_cast< int32 * >( pValue ) = static_cast< int32 >( nValue ); break;
	}
}


//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
CKV3TransferSchemaClass::CKV3TransferSchemaClass( const char *pszName, KV3TransferClassBehavior_t nBehavior, const KV3TransferSchemaField_t *pFields, int nFieldCount ) :
	m_pszName( pszName ),
	m_nBehavior( nBehavior ),
	m_pFields( pFields ),
	m_nFieldCount( nFieldCount ),
	m_pBaseClass( nullptr ),
	m_nBaseClassOffset( 0 ),
	m_pfnAllocate( nullptr ),
	m_pfnDeallocate( nullptr ),
	m_pfnPolymorphicClassname( nullptr ),
	m_pfnFindClass( nullptr ),
	m_pfnGetDefaultKeys( nullptr ),
	m_bDefaultsBuilt( false ),
	m_pDefaultInstance( nullptr ),
	m_DefaultKeys { nullptr, nullptr }
{
}

CKV3TransferSchemaClass::~CKV3TransferSchemaClass()
{
	if ( m_pDefaultInstance && m_pfnDeallocate )
		m_pfnDeallocate( m_pDefaultInstance );
}

const KV3TransferDefaultKeys_t *CKV3TransferSchemaClass::GetDefaultKeys() const
{
	if ( m_pfnGetDefaultKeys )
		return m_pfnGetDefaultKeys();

	KV3Transfer_DefaultKeys_Impl();

	return &m_DefaultKeys;
}

const void *CKV3TransferSchemaClass::GetDefaultInstance() const
{
	KV3Transfer_DefaultKeys_Impl();

	return m_pDefaultInstance;
}

bool CKV3TransferSchemaClass::FindBaseClassOffset( const CKV3TransferSchemaClass *pBaseClass, int *pOffset ) const
{
	int nOffset = 0;

	for ( const CKV3TransferSchemaClass *pClass = this; pClass; pClass = pClass->m_pBaseClass )
	{
		if ( pClass == pBaseClass )
		{
			*pOffset = nOffset;
			return true;
		}

		nOffset += pClass->m_nBaseClassOffset;
	}

	return false;
}

// Saves a default instance the way the generated KV3Transfer_DefaultKeys_Impl does: a context with optional
// interfaces, and null keys when that fails.
void CKV3TransferSchemaClass::KV3Transfer_DefaultKeys_Impl() const
{
	AUTO_LOCK( m_DefaultsMutex );

	if ( m_bDefaultsBuilt )
		return;

	m_bDefaultsBuilt = true;

	if ( m_pfnAllocate )
		m_pDefaultInstance = m_pfnAllocate();

	m_DefaultKeys.m_pDefaultKeys = &m_DefaultKeysTable;
	m_DefaultKeys.m_pUnk08 = &KeyValues3::GetNullValue();

	if ( !m_pDefaultInstance )
		return;

	CKV3TransferSaveContext context;

	KV3Transfer_SaveSchemaClass( &context, this, m_pDefaultInstance, &m_DefaultKeysTable );

	if ( context.GetResult() != KV3TRANSFER_SUCCESS )
		m_DefaultKeysTable.SetToNull();
}


//--------------------------------------------------------------------------------------------------
// The class an object of pClass is, and where that object starts. Fails for a polymorphic class name
// that does not resolve to pClass or a class derived from it.
//--------------------------------------------------------------------------------------------------
static const CKV3TransferSchemaClass *KV3Transfer_ResolveSchemaClass( const CKV3TransferSchemaClass *pClass, const char *pszClassName, int *pBaseClassOffset )
{
	*pBaseClassOffset = 0;

	if ( !V_strcmp( pszClassName, pClass->m_pszName ) )
		return pClass;

	const CKV3TransferSchemaClass *pDerivedClass = pClass->m_pfnFindClass ? pClass->m_pfnFindClass( pszClassName ) : nullptr;

	if ( !pDerivedClass || !pDerivedClass->FindBaseClassOffset( pClass, pBaseClassOffset ) )
		return nullptr;

	return pDerivedClass;
}

static const CKV3TransferSchemaClass *KV3Transfer_ResolveSchemaObject( CKV3TransferContextBase *pContext, const CKV3TransferSchemaClass *pClass, const void *pObject, CBufferString &sClassName, int *pBaseClassOffset )
{
	*pBaseClassOffset = 0;

	if ( pClass->m_nBehavior != KV3TRANSFER_CLASS_AS_POLYMORPHIC_TABLE )
		return pClass;

	if ( pClass->m_pfnPolymorphicClassname )
		pClass->m_pfnPolymorphicClassname( pObject, sClassName );
	else
		sClassName.Set( pClass->m_pszName );

	const CKV3TransferSchemaClass *pObjectClass = KV3Transfer_ResolveSchemaClass( pClass, sClassName.Get(), pBaseClassOffset );

	if ( !pObjectClass )
		pContext->NoteFailure( "Failed to determine polymorphic class name" );

	return pObjectClass;
}


//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
static bool KV3Transfer_IsSchemaFieldDefault( const KV3TransferSchemaField_t &field, const void *pField, const void *pDefaultField )
{
	if ( !pDefaultField )
		return false;

	switch ( field.m_nType )
	{
		case KV3TRANSFER_SCHEMA_FIELD_VALUE:
			return field.m_pOps->m_pfnIsEqual && field.m_pOps->m_pfnIsEqual( pField, pDefaultField );

		case KV3TRANSFER_SCHEMA_FIELD_ENUM:
			return KV3Transfer_ReadSchemaEnumValue( field.m_pEnum, pField ) == KV3Transfer_ReadSchemaEnumValue( field.m_pEnum, pDefaultField );

		case KV3TRANSFER_SCHEMA_FIELD_CLASS_POINTER:
			return !*static_cast< void *const * >( pField ) && !*static_cast< void *const * >( pDefaultField );

		default:
			return false;
	}
}

static void KV3Transfer_SaveSchemaEnum( const KV3TransferSchemaEnum_t *pEnum, const void *pValue, KeyValues3 *pSaveToValue )
{
	const int64 nValue = KV3Transfer_ReadSchemaEnumValue( pEnum, pValue );
	const KV3TransferSchemaEnumerator_t *pEnumerator = pEnum->FindEnumeratorByValue( nValue );

	if ( pEnumerator )
		pSaveToValue->SetString( pEnumerator->m_pszName );
	else if ( pEnum->m_nSize == sizeof( int64 ) )
		pSaveToValue->SetInt64( nValue );
	else
		pSaveToValue->SetInt( static_cast< int32 >( nValue ) );
}

static void KV3Transfer_LoadSchemaEnum( const KV3TransferSchemaEnum_t *pEnum, void *pValue, const KeyValues3 *pLoadFromValue )
{
	if ( pLoadFromValue->IsString() )
	{
		const KV3TransferSchemaEnumerator_t *pEnumerator = pEnum->FindEnumeratorByName( pLoadFromValue->GetString() );

		if ( pEnumerator )
		{
			KV3Transfer_WriteSchemaEnumValue( pEnum, pValue, pEnumerator->m_nValue );
			return;
		}
	}

	KV3Transfer_WriteSchemaEnumValue( pEnum, pValue, pLoadFromValue->GetInt64() );
}

static void KV3Transfer_SaveSchemaFields( CKV3TransferSaveContext *pContext, const CKV3TransferSchemaClass *pClass, const byte *pObject )
{
	if ( pClass->m_pBaseClass )
		KV3Transfer_SaveSchemaFields( pContext, pClass->m_pBaseClass, pObject + pClass->m_nBaseClassOffset );

	// The defaults are those of the class declaring the fields, as the literals of its generated save are.
	const byte *pDefaultObject = pContext->GetSave() ? static_cast< const byte * >( pClass->GetDefaultInstance() ) : nullptr;

	for ( int i = 0; i < pClass->m_nFieldCount; ++i )
	{
		const KV3TransferSchemaField_t &field = pClass->m_pFields[ i ];
		const void *pField = pObject + field.m_nOffset;

		if ( field.m_nType == KV3TRANSFER_SCHEMA_FIELD_SAVE_RESTORE_OPS )
		{
			// An empty field is skipped with or without an ISave.
			if ( !pContext->ShouldSaveField( false ) || field.m_pSaveRestoreOps->IsEmpty( pField ) )
				continue;

			pContext->PushTarget( pContext->CreateTargetMember( field.m_Name ) );
			field.m_pSaveRestoreOps->KV3TransferSave( pField, pObject, pContext );
			pContext->PopTarget();

			continue;
		}

		const void *pDefaultField = pDefaultObject ? pDefaultObject + field.m_nOffset : nullptr;

		if ( !pContext->ShouldSaveField( KV3Transfer_IsSchemaFieldDefault( field, pField, pDefaultField ) ) )
			continue;

		KeyValues3 *pMember = pContext->CreateTargetMember( field.m_Name );

		switch ( field.m_nType )
		{
			case KV3TRANSFER_SCHEMA_FIELD_VALUE:
				field.m_pOps->m_pfnSave( pContext, pMember, pField );
				break;

			case KV3TRANSFER_SCHEMA_FIELD_ENUM:
				KV3Transfer_SaveSchemaEnum( field.m_pEnum, pField, pMember );
				break;

			case KV3TRANSFER_SCHEMA_FIELD_CLASS:
				KV3Transfer_SaveSchemaClass( pContext, field.m_pClass, pField, pMember );
				break;

			case KV3TRANSFER_SCHEMA_FIELD_CLASS_POINTER:
				KV3Transfer_SaveSchemaClassPointer( pContext, field.m_pClass, *static_cast< const void *const * >( pField ), pMember );
				break;

			default:
				break;
		}
	}
}

void KV3Transfer_SaveSchemaClassFields( CKV3TransferSaveContext *pContext, const CKV3TransferSchemaClass *pClass, const void *pObject )
{
	KV3Transfer_SaveSchemaFields( pContext, pClass, static_cast< const byte * >( pObject ) );
}


//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
static void KV3Transfer_LoadSchemaFieldValue( CKV3TransferLoadContext *pContext, const KV3TransferSchemaField_t &field, void *pField, const KeyValues3 *pLoadFromValue )
{
	switch ( field.m_nType )
	{
		case KV3TRANSFER_SCHEMA_FIELD_VALUE:
			field.m_pOps->m_pfnLoad( pContext, pLoadFromValue, pField );
			break;

		case KV3TRANSFER_SCHEMA_FIELD_ENUM:
			KV3Transfer_LoadSchemaEnum( field.m_pEnum, pField, pLoadFromValue );
			break;

		case KV3TRANSFER_SCHEMA_FIELD_CLASS:
			KV3Transfer_LoadSchemaClass( pContext, field.m_pClass, pField, pLoadFromValue );
			break;

		case KV3TRANSFER_SCHEMA_FIELD_CLASS_POINTER:
			KV3Transfer_LoadSchemaClassOwningPointer( pContext, field.m_pClass, *static_cast< void ** >( pField ), pLoadFromValue );
			break;

		default:
			break;
	}
}

static void KV3Transfer_LoadSchemaFields( CKV3TransferLoadContext *pContext, const CKV3TransferSchemaClass *pClass, byte *pObject, const KV3TransferDefaultKeys_t *pDefaultKeys )
{
	if ( pClass->m_pBaseClass )
		KV3Transfer_LoadSchemaFields( pContext, pClass->m_pBaseClass, pObject + pClass->m_nBaseClassOffset, pDefaultKeys );

	const bool bRestore = pContext->GetRestore() != nullptr;

	for ( int i = 0; i < pClass->m_nFieldCount; ++i )
	{
		const KV3TransferSchemaField_t &field = pClass->m_pFields[ i ];
		void *pField = pObject + field.m_nOffset;

		if ( field.m_nType == KV3TRANSFER_SCHEMA_FIELD_SAVE_RESTORE_OPS )
		{
			// Only a present member loads, from the source itself.
			const KeyValues3 *pSource = pContext->SourceObject();
			const KeyValues3 *pMember = pSource->IsTable() ? pSource->FindMember( field.m_Name ) : nullptr;

			if ( !pMember )
				continue;

			pContext->PushSource( pMember );
			field.m_pSaveRestoreOps->MakeEmpty( pField );
			field.m_pSaveRestoreOps->KV3TransferLoad( pField, pObject, pContext );
			pContext->PopSource();

			continue;
		}

		if ( bRestore && !pContext->CanLoadField( 0 ) )
			continue;

		const KeyValues3 *pMember = pContext->FindClassMember( field.m_Name );

		if ( pMember )
		{
			pContext->PushContextPath( field.m_Name );
			KV3Transfer_LoadSchemaFieldValue( pContext, field, pField, pMember );
			pContext->PopContextPath();
		}
		else if ( !bRestore )
		{
			KV3Transfer_LoadSchemaFieldValue( pContext, field, pField, pDefaultKeys->FindMember( field.m_Name ) );
		}
	}
}

void KV3Transfer_LoadSchemaClassFields( CKV3TransferLoadContext *pContext, const CKV3TransferSchemaClass *pClass, void *pObject, const KV3TransferDefaultKeys_t *pDefaultKeys )
{
	// An IRestore keeps missing members, so the defaults are not needed.
	if ( !pDefaultKeys && !pContext->GetRestore() )
		pDefaultKeys = pClass->GetDefaultKeys();

	KV3Transfer_LoadSchemaFields( pContext, pClass, static_cast< byte * >( pObject ), pDefaultKeys );
}


//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
void KV3Transfer_SaveSchemaClass( CKV3TransferSaveContext *pContext, const CKV3TransferSchemaClass *pClass, const void *pObject, KeyValues3 *pSaveToValue )
{
	CBufferStringN< KV3TRANSFER_CLASSNAME_MAX_LENGTH > sClassName;
	int nBaseClassOffset;

	const CKV3TransferSchemaClass *pObjectClass = KV3Transfer_ResolveSchemaObject( pContext, pClass, pObject, sClassName, &nBaseClassOffset );

	if ( !pObjectClass )
	{
		pSaveToValue->SetToNull();
		return;
	}

	if ( !pContext->PrepareTargetForClass( pSaveToValue, pClass->m_nBehavior, sClassName.Get() ) )
		return;

	pContext->PushTarget( pSaveToValue );

	if ( pContext->TargetDepth() > KV3TRANSFER_MAX_STACK_DEPTH )
		pContext->NoteFailure( "Stack depth limit hit (%d)", pContext->TargetDepth() );
	else
		KV3Transfer_SaveSchemaFields( pContext, pObjectClass, static_cast< const byte * >( pObject ) - nBaseClassOffset );

	pContext->PopTarget();

	if ( pContext->GetResult() != KV3TRANSFER_SUCCESS )
		pSaveToValue->SetToNull();
}

void KV3Transfer_LoadSchemaClass( CKV3TransferLoadContext *pContext, const CKV3TransferSchemaClass *pClass, void *pObject, const KeyValues3 *pLoadFromValue )
{
	CBufferStringN< KV3TRANSFER_CLASSNAME_MAX_LENGTH > sClassName;
	int nBaseClassOffset;

	// The object exists already, so its own class decides, not the class name of the value.
	const CKV3TransferSchemaClass *pObjectClass = KV3Transfer_ResolveSchemaObject( pContext, pClass, pObject, sClassName, &nBaseClassOffset );

	if ( !pObjectClass )
		return;

	pContext->PushSource( pLoadFromValue );

	if ( pContext->SourceDepth() > KV3TRANSFER_MAX_STACK_DEPTH )
		pContext->NoteFailure( "Stack depth limit hit (%d)", pContext->SourceDepth() );
	else
		KV3Transfer_LoadSchemaClassFields( pContext, pObjectClass, static_cast< byte * >( pObject ) - nBaseClassOffset );

	pContext->PopSource();
}

void KV3Transfer_SaveSchemaClassPointer( CKV3TransferSaveContext *pContext, const CKV3TransferSchemaClass *pClass, const void *pObject, KeyValues3 *pSaveToValue )
{
	if ( !pObject )
	{
		pSaveToValue->SetToNull();
		return;
	}

	KV3Transfer_SaveSchemaClass( pContext, pClass, pObject, pSaveToValue );
}

void KV3Transfer_LoadSchemaClassOwningPointer( CKV3TransferLoadContext *pContext, const CKV3TransferSchemaClass *pClass, void *&pObject, const KeyValues3 *pLoadFromValue )
{
	Assert( pLoadFromValue );

	pObject = nullptr;

	if ( pLoadFromValue->IsNull() )
		return;

	const CKV3TransferSchemaClass *pObjectClass = pClass;
	int nBaseClassOffset = 0;

	if ( pClass->m_nBehavior == KV3TRANSFER_CLASS_AS_POLYMORPHIC_TABLE )
	{
		const char *pszClassName = pLoadFromValue->GetMemberString( KV3TRANSFER_CLASSNAME_MEMBER );

		if ( !pszClassName[ 0 ] )
		{
			pContext->NoteFailure( "Tried to load a polymorphic pointer with no '%s' key", KV3TRANSFER_CLASSNAME_MEMBER );
			return;
		}

		pObjectClass = KV3Transfer_ResolveSchemaClass( pClass, pszClassName, &nBaseClassOffset );

		void *pAllocated = pObjectClass && pObjectClass->m_pfnAllocate ? pObjectClass->m_pfnAllocate() : nullptr;

		if ( !pAllocated )
		{
			pContext->NoteFailure( "Failed to allocate an instance of class '%s'", pszClassName );
			return;
		}

		pObject = static_cast< byte * >( pAllocated ) + nBaseClassOffset;
	}
	else
	{
		pObject = pClass->m_pfnAllocate ? pClass->m_pfnAllocate() : nullptr;

		if ( !pObject )
		{
			pContext->NoteFailure( "Failed to allocate an instance of a class" );
			return;
		}
	}

	pContext->PushSource( pLoadFromValue );

	if ( pContext->SourceDepth() > KV3TRANSFER_MAX_STACK_DEPTH )
		pContext->NoteFailure( "Stack depth limit hit (%d)", pContext->SourceDepth() );
	else
		KV3Transfer_LoadSchemaClassFields( pContext, pObjectClass, static_cast< byte * >( pObject ) - nBaseClassOffset );

	pContext->PopSource();
}
