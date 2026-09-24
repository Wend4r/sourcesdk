#include "entitypulse.h"
#include "entitypulsecallcontext.h"
#include "interfaces/interfaces.h"
#include "resourcefile/resourcetype.h"
#include "tier0/bufferstring.h"
#include "tier0/globalsymbol.h"
#include "tier0/memalloc.h"
#include "tier0/utlstring.h"

// Storage of a PVAL_ARRAY value, shared by reference; the value holds a pointer to it.
struct PulseValueArray_t
{
	uint16 m_nRefCount;
	int32 m_nCount;
	int32 m_nCapacity;
	void *m_pElements;
};

// Storage of PVAL_RESOURCE_NAME: the name and two more fields that are copied along with it.
struct PulseValueResourceName_t
{
	CBufferStringN< 200 > m_Name;
	uint64 m_nUnk0208;
	uint64 m_nUnk0216;
};

// Storage of PVAL_SNDEVT_GUID, laid out as StartSoundEventInfo_t.
#pragma pack( push, 1 )
struct PulseValueSoundEventGuid_t
{
	int32 m_nGuid; // SoundEventGuid_t
	uint32 m_hStackHash; // HSOSLIBSTACKHASH
	uint32 m_nFlags;
	uint64 m_nRecipients;
};
#pragma pack( pop )

static void ReleaseArray( PulseValueArray_t *&pArray, const CPulseValueFullType &type );

static void ReleaseResource( ResourceHandle_t hResource )
{
	if ( !hResource || !g_pResourceHandleUtils )
		return;

	// Game: when the count drops to 0, also calls the third virtual of g_pResourceHandleUtils with the binding.
	// The interface is not declared in the SDK.
	--const_cast< ResourceBindingBase_t * >( hResource )->m_nRefCount;
}

CPulseValueFullType::CPulseValueFullType( PulseValueType_t nType, const char *pszSubType ) :
	m_nType( nType ),
	m_pElementType( nullptr ),
	m_subType( MakeGlobalSymbolCaseSensitive( pszSubType ) )
{
	if ( m_nType == PVAL_ARRAY )
		m_pElementType = Create< CPulseValueFullType >();
}

bool CPulseValueFullType::operator==( const CPulseValueFullType &other ) const
{
	const CPulseValueFullType *pType = this, *pOther = &other;

	while ( true )
	{
		if ( pType->m_nType != pOther->m_nType || pType->m_subType != pOther->m_subType )
			return false;

		pType = pType->m_pElementType;

		if ( !pType )
			return true;

		pOther = pOther->m_pElementType;
	}
}

void CPulseValueFullType::Assign( const CPulseValueFullType &other )
{
	CPulseValueFullType *pType = this;
	const CPulseValueFullType *pOther = &other;

	while ( true )
	{
		const char *pszSubType = pOther->m_subType.String();

		pType->m_nType = pOther->m_nType;
		pType->m_subType = MakeGlobalSymbolCaseSensitive( pszSubType ? pszSubType : "" );

		if ( pType->m_nType != PVAL_ARRAY )
			break;

		if ( !pType->m_pElementType )
			pType->m_pElementType = Create< CPulseValueFullType >();

		pType = pType->m_pElementType;
		pOther = pOther->m_pElementType;
	}

	pType->ReleaseElementType();
}

void CPulseValueFullType::ReleaseElementType()
{
	if ( m_pElementType )
	{
		Release( m_pElementType );
		m_pElementType = nullptr;
	}
}

size_t CPulseValueFullType::GetSize( size_t *pAlignment ) const
{
	size_t nSize = 0, nAlignment = 1;

	switch ( m_nType )
	{
		case PVAL_BOOL:
			nSize = 1;
			break;

		case PVAL_INT:
		case PVAL_FLOAT:
		case PVAL_COLOR_RGB:
		case PVAL_GAMETIME:
		case PVAL_EHANDLE:
		case PVAL_OPAQUE_HANDLE:
		case PVAL_TYPESAFE_INT:
		case PVAL_TEST_HANDLE:
			nSize = 4, nAlignment = 4;
			break;

		case PVAL_STRING:
		case PVAL_RESOURCE:
		case PVAL_ENTITY_NAME:
		case PVAL_MODEL_MATERIAL_GROUP:
		case PVAL_SCHEMA_ENUM:
		case PVAL_PANORAMA_PANEL_HANDLE:
		case PVAL_ARRAY:
		case PVAL_TYPESAFE_INT64:
		case PVAL_PARTICLE_EHANDLE:
		case PVAL_ANIM_SEQUENCE:
		case PVAL_VDATA_CHOICE:
			nSize = 8, nAlignment = 8;
			break;

		case PVAL_VEC2:
			nSize = 8, nAlignment = 4;
			break;

		case PVAL_VEC3:
		case PVAL_QANGLE:
		case PVAL_VEC3_WORLDSPACE:
			nSize = 12, nAlignment = 4;
			break;

		case PVAL_VEC4:
			nSize = 16, nAlignment = 4;
			break;

		case PVAL_TRANSFORM:
		case PVAL_TRANSFORM_WORLDSPACE:
		case PVAL_VARIANT:
			nSize = 32, nAlignment = 16;
			break;

		case PVAL_RESOURCE_NAME:
			nSize = sizeof( PulseValueResourceName_t ), nAlignment = 8;
			break;

		case PVAL_SNDEVT_GUID:
			nSize = sizeof( PulseValueSoundEventGuid_t ), nAlignment = 4;
			break;

		case PVAL_SNDEVT_NAME:
			nSize = sizeof( CBufferString ), nAlignment = 8;
			break;

		default:
			break;
	}

	if ( pAlignment )
		*pAlignment = nAlignment;

	return nSize;
}

void CPulseValueFullType::ConstructValue( void *pValue ) const
{
	switch ( m_nType )
	{
		case PVAL_BOOL:
			*static_cast< bool * >( pValue ) = false;
			break;

		case PVAL_INT:
		case PVAL_FLOAT:
		case PVAL_COLOR_RGB:
		case PVAL_GAMETIME:
		case PVAL_TYPESAFE_INT:
			*static_cast< uint32 * >( pValue ) = 0;
			break;

		case PVAL_STRING:
		case PVAL_VEC2:
		case PVAL_RESOURCE:
		case PVAL_ENTITY_NAME:
		case PVAL_MODEL_MATERIAL_GROUP:
		case PVAL_SCHEMA_ENUM:
		case PVAL_ARRAY:
		case PVAL_TYPESAFE_INT64:
		case PVAL_ANIM_SEQUENCE:
		case PVAL_VDATA_CHOICE:
			*static_cast< uint64 * >( pValue ) = 0;
			break;

		case PVAL_VEC3:
		case PVAL_QANGLE:
		case PVAL_VEC3_WORLDSPACE:
			V_memset( pValue, 0, 12 );
			break;

		case PVAL_VEC4:
			V_memset( pValue, 0, 16 );
			break;

		// An unset transform has every bit set.
		case PVAL_TRANSFORM:
		case PVAL_TRANSFORM_WORLDSPACE:
			V_memset( pValue, 0xFF, 32 );
			break;

		case PVAL_EHANDLE:
		case PVAL_OPAQUE_HANDLE:
		case PVAL_TEST_HANDLE:
			*static_cast< int32 * >( pValue ) = -1;
			break;

		case PVAL_RESOURCE_NAME:
		{
			PulseValueResourceName_t *pName = static_cast< PulseValueResourceName_t * >( pValue );

			Construct( &pName->m_Name );
			pName->m_nUnk0208 = 0;
			pName->m_nUnk0216 = 0;

			break;
		}

		case PVAL_SNDEVT_GUID:
		{
			PulseValueSoundEventGuid_t *pGuid = static_cast< PulseValueSoundEventGuid_t * >( pValue );

			pGuid->m_nGuid = 0;
			pGuid->m_hStackHash = -1;
			pGuid->m_nFlags = 0;
			pGuid->m_nRecipients = 0;

			break;
		}

		case PVAL_SNDEVT_NAME:
			Construct( static_cast< CBufferString * >( pValue ) );
			break;

		case PVAL_VARIANT:
			Construct( static_cast< CPulseCallContextValue * >( pValue ) );
			break;

		case PVAL_PANORAMA_PANEL_HANDLE:
			static_cast< int32 * >( pValue )[ 0 ] = 0;
			static_cast< int32 * >( pValue )[ 1 ] = -1;
			break;

		default:
			break;
	}
}

void CPulseValueFullType::DestructValue( void *pValue ) const
{
	switch ( m_nType )
	{
		case PVAL_STRING:
		case PVAL_MODEL_MATERIAL_GROUP:
		case PVAL_ANIM_SEQUENCE:
			Destruct( static_cast< CUtlString * >( pValue ) );
			break;

		case PVAL_RESOURCE:
			ReleaseResource( *static_cast< ResourceHandle_t * >( pValue ) );
			break;

		case PVAL_RESOURCE_NAME:
		case PVAL_SNDEVT_NAME:
			static_cast< CBufferString * >( pValue )->Purge( 0 );
			break;

		case PVAL_VARIANT:
			Destruct( static_cast< CPulseCallContextValue * >( pValue ) );
			break;

		case PVAL_ARRAY:
			ReleaseArray( *static_cast< PulseValueArray_t ** >( pValue ), *this );
			break;

		default:
			break;
	}
}

void CPulseValueFullType::CopyValue( const void *pSrc, void *pDest ) const
{
	switch ( m_nType )
	{
		case PVAL_BOOL:
			*static_cast< bool * >( pDest ) = *static_cast< const bool * >( pSrc );
			break;

		case PVAL_INT:
		case PVAL_FLOAT:
		case PVAL_COLOR_RGB:
		case PVAL_GAMETIME:
		case PVAL_EHANDLE:
		case PVAL_OPAQUE_HANDLE:
		case PVAL_TYPESAFE_INT:
		case PVAL_TEST_HANDLE:
			*static_cast< uint32 * >( pDest ) = *static_cast< const uint32 * >( pSrc );
			break;

		case PVAL_STRING:
		case PVAL_MODEL_MATERIAL_GROUP:
		case PVAL_ANIM_SEQUENCE:
		{
			const char *pszString = *static_cast< const char *const * >( pSrc );

			static_cast< CUtlString * >( pDest )->SetDirect( pszString ? pszString : "", pszString ? V_strlen( pszString ) : 0 );

			break;
		}

		case PVAL_VEC2:
		case PVAL_ENTITY_NAME:
		case PVAL_SCHEMA_ENUM:
		case PVAL_PANORAMA_PANEL_HANDLE:
		case PVAL_TYPESAFE_INT64:
		case PVAL_PARTICLE_EHANDLE:
		case PVAL_VDATA_CHOICE:
			*static_cast< uint64 * >( pDest ) = *static_cast< const uint64 * >( pSrc );
			break;

		case PVAL_VEC3:
		case PVAL_QANGLE:
		case PVAL_VEC3_WORLDSPACE:
			V_memcpy( pDest, pSrc, 12 );
			break;

		case PVAL_VEC4:
			V_memcpy( pDest, pSrc, 16 );
			break;

		case PVAL_TRANSFORM:
		case PVAL_TRANSFORM_WORLDSPACE:
			V_memcpy( pDest, pSrc, 32 );
			break;

		case PVAL_RESOURCE:
		{
			ResourceHandle_t hResource = *static_cast< const ResourceHandle_t * >( pSrc );

			ReleaseResource( *static_cast< ResourceHandle_t * >( pDest ) );
			*static_cast< ResourceHandle_t * >( pDest ) = hResource;

			if ( hResource )
				++const_cast< ResourceBindingBase_t * >( hResource )->m_nRefCount;

			break;
		}

		case PVAL_RESOURCE_NAME:
		{
			const PulseValueResourceName_t *pSrcName = static_cast< const PulseValueResourceName_t * >( pSrc );
			PulseValueResourceName_t *pDestName = static_cast< PulseValueResourceName_t * >( pDest );

			pDestName->m_Name.Set( pSrcName->m_Name.Get(), pSrcName->m_Name.Length() );
			pDestName->m_nUnk0208 = pSrcName->m_nUnk0208;
			pDestName->m_nUnk0216 = pSrcName->m_nUnk0216;

			break;
		}

		case PVAL_SNDEVT_GUID:
			V_memcpy( pDest, pSrc, sizeof( PulseValueSoundEventGuid_t ) );
			break;

		case PVAL_SNDEVT_NAME:
			static_cast< CBufferString * >( pDest )->Set( static_cast< const CBufferString * >( pSrc )->Get() );
			break;

		case PVAL_VARIANT:
			*static_cast< CPulseCallContextValue * >( pDest ) = *static_cast< const CPulseCallContextValue * >( pSrc );
			break;

		case PVAL_ARRAY:
		{
			PulseValueArray_t *pSrcArray = *static_cast< PulseValueArray_t *const * >( pSrc );
			PulseValueArray_t *&pDestArray = *static_cast< PulseValueArray_t ** >( pDest );

			// Arrays are shared: the copy references the same storage.
			if ( pSrcArray == pDestArray )
				break;

			ReleaseArray( pDestArray, *this );

			if ( pSrcArray )
				++pSrcArray->m_nRefCount;

			pDestArray = pSrcArray;

			break;
		}

		default:
			break;
	}
}

static void ReleaseArray( PulseValueArray_t *&pArray, const CPulseValueFullType &type )
{
	if ( !pArray )
		return;

	if ( pArray->m_nRefCount > 1 )
	{
		--pArray->m_nRefCount;

		return;
	}

	const CPulseValueFullType *pElementType = type.m_pElementType;
	const size_t nElementSize = pElementType->GetSize();

	for ( int i = 0; i < pArray->m_nCount; ++i )
		pElementType->DestructValue( static_cast< byte * >( pArray->m_pElements ) + i * nElementSize );

	pArray->m_nCount = 0;

	MemAlloc_FreeAligned( pArray->m_pElements );
	pArray->m_pElements = nullptr;
	pArray->m_nCapacity = 0;

	if ( !--pArray->m_nRefCount )
		MemAlloc_Free( pArray );

	pArray = nullptr;
}
