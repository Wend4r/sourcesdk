#include "kv3lib/kv3transfer.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

CKV3TransferContextBase::CKV3TransferContextBase( const char *pszSourceName ) :
	m_Result( KV3TRANSFER_SUCCESS ),
	m_bUnk00C( false ),
	m_pszSourceName( pszSourceName )
{
}

void CKV3TransferContextBase::ReportMessage( const KV3TransferMessage_t &message )
{
	if ( message.m_nSeverity == KV3TRANSFER_MESSAGE_SEVERITY_ERROR )
		m_Result = KV3TRANSFER_FAIL;

	if ( !m_sErrorMessage.IsEmpty() )
		m_sErrorMessage.Append( "\n", 1 );

	FormatMessageLocation( message, m_sErrorMessage, ": " );

	m_sErrorMessage.Append( message.m_sMessage.Get() );
	m_sErrorMessage.TrimTail( "\t\r\n " );
}

bool CKV3TransferContextBase::FormatMessageLocation( const KV3TransferMessage_t &message, CBufferString &sOut, const char *pszSuffix )
{
	int nLineAndColumn[ 2 ] = { message.m_nLine, message.m_nColumn };

	const char *pszLocation = message.m_sLocation.Get();

	if ( !pszLocation || !pszLocation[ 0 ] )
		pszLocation = GetSourceName( nLineAndColumn );

	bool bAppended;

	if ( pszLocation && pszLocation[ 0 ] )
	{
		sOut.Append( pszLocation );

		if ( nLineAndColumn[ 1 ] )
			sOut.AppendFormat( "(%d,%d)", nLineAndColumn[ 0 ], nLineAndColumn[ 1 ] );
		else if ( nLineAndColumn[ 0 ] )
			sOut.AppendFormat( "(%d)", nLineAndColumn[ 0 ] );

		const int nSeparator = sOut.Length();

		sOut.Append( "|" );

		if ( !AppendContextPath( sOut ) )
			sOut.TruncateAt( nSeparator );

		bAppended = true;
	}
	else
	{
		bAppended = AppendContextPath( sOut );
	}

	if ( bAppended && pszSuffix )
		sOut.Append( pszSuffix );

	return bAppended;
}

void CKV3TransferContextBase::NoteFailure( const char *pszFormat, ... )
{
	KV3TransferMessage_t message {};

	message.m_nSeverity = KV3TRANSFER_MESSAGE_SEVERITY_ERROR;

	va_list params;

	va_start( params, pszFormat );
	message.m_sMessage.AppendFormatV( pszFormat, params );
	va_end( params );

	ReportMessage( message );
}

const char *CKV3TransferContextBase::GetErrorMessageSince( const ResultState_t &state ) const
{
	if ( state.m_nErrorMessageLength >= m_sErrorMessage.Length() )
		return "";

	const char *pszResult = m_sErrorMessage.Get() + state.m_nErrorMessageLength;

	while ( *pszResult == '\n' )
		++pszResult;

	return pszResult;
}

void *CKV3TransferContextBase::FindInterface( const CKV3MemberName &name, bool bOptional )
{
	FOR_EACH_VEC( m_InterfaceNames, i )
	{
		if ( m_InterfaceNames[ i ] == name.GetHashCode() )
			return m_Interfaces[ i ];
	}

	if ( !bOptional )
		NoteFailure( "Missing required transfer interface: %s", name.GetString() );

	return nullptr;
}

void CKV3TransferContextBase::AddInterface( const CKV3MemberName &name, void *pInterface )
{
	m_InterfaceNames.AddToTail( name.GetHashCode() );
	m_Interfaces.AddToTail( pInterface );
}

CKV3TransferSaveContext::CKV3TransferSaveContext( bool bOptionalInterfaces ) :
	m_nBlockAllocationSize( 0 ),
	m_pTargetObject( nullptr ),
	m_bOptionalInterfaces( bOptionalInterfaces ),
	m_pSave( nullptr ),
	m_bSaveDefaultValues( false )
{
}

CKV3TransferSaveContext::CKV3TransferSaveContext( ISave *pSave ) :
	m_nBlockAllocationSize( 0 ),
	m_pTargetObject( nullptr ),
	m_bOptionalInterfaces( false ),
	m_pSave( pSave ),
	m_bSaveDefaultValues( false )
{
}

bool CKV3TransferSaveContext::CanSaveField( int nFlags ) const
{
	if ( !m_pSave )
		return true;

	// The SDK's ISave declares a different interface, so the game's virtual is called by its slot.
	using Unk_10_t = bool ( * )( ISave * );

	return !( *reinterpret_cast< Unk_10_t *const * >( m_pSave ) )[ 10 ]( m_pSave ) || nFlags == 2;
}

KeyValues3 *CKV3TransferSaveContext::CreateTargetMember( const CKV3MemberName &name )
{
	bool bCreated = false;

	KeyValues3 *pMember = m_pTargetObject->FindOrCreateMember( name, &bCreated );

	if ( !bCreated )
		NoteFailure( "Double-save to Member '%s'", name.GetString() );

	return pMember;
}

bool CKV3TransferSaveContext::PrepareTargetForClass( KeyValues3 *pObjectValue, KV3TransferClassBehavior_t nClassBehavior, const char *pszPolymorphicClassName )
{
	switch ( nClassBehavior )
	{
		case KV3TRANSFER_CLASS_AS_POLYMORPHIC_TABLE:
		{
			Assert( pszPolymorphicClassName && pszPolymorphicClassName[ 0 ] );

			pObjectValue->SetToEmptyTable();
			pObjectValue->SetMemberString( KV3TRANSFER_CLASSNAME_MEMBER, pszPolymorphicClassName );

			return true;
		}

		case KV3TRANSFER_CLASS_AS_DATA:
		{
			pObjectValue->SetToNull();

			return true;
		}

		case KV3TRANSFER_CLASS_AS_SIMPLE_TABLE:
		{
			pObjectValue->SetToEmptyTable();

			return true;
		}

		default:
		{
			NoteFailure( "Failed to save unsupported class" );
			pObjectValue->SetToNull();

			return false;
		}
	}
}

void CKV3TransferSaveContext::PushTarget( KeyValues3 *pTarget )
{
	m_pTargetObject = pTarget;
	m_TargetStack.AddToTail( pTarget );
}

void CKV3TransferSaveContext::PopTarget()
{
	m_TargetStack.RemoveMultipleFromTail( 1 );
	m_pTargetObject = m_TargetStack.Count() > 0 ? m_TargetStack.Tail() : nullptr;
}

CKV3TransferLoadContext::CKV3TransferLoadContext() :
	m_bUnk118( true ),
	m_pBlockAllocator( nullptr ),
	m_pSourceObject( nullptr ),
	m_pRestore( nullptr )
{
}

CKV3TransferLoadContext::CKV3TransferLoadContext( const char *pszSourceName ) :
	CKV3TransferContextBase( pszSourceName ),
	m_bUnk118( true ),
	m_pBlockAllocator( nullptr ),
	m_pSourceObject( nullptr ),
	m_pRestore( nullptr )
{
}

CKV3TransferLoadContext::CKV3TransferLoadContext( IRestore *pRestore ) :
	m_bUnk118( true ),
	m_pBlockAllocator( nullptr ),
	m_pSourceObject( nullptr ),
	m_pRestore( pRestore )
{
}

bool CKV3TransferLoadContext::AppendContextPath( CBufferString &sOut )
{
	bool bAppended = false;

	for ( int i = 0; i < m_ContextPath.Count(); ++i )
	{
		const ContextPathEntry_t &entry = m_ContextPath[ i ];

		if ( !entry.m_pszName )
			continue;

		if ( entry.m_nTag == CONTEXT_PATH_TAG_INDEX )
		{
			sOut.AppendFormat( "[%u]", entry.m_nValue );
		}
		else if ( entry.m_nTag == CONTEXT_PATH_TAG_NAME_HASH )
		{
			sOut.AppendFormat( "{0x%x}", entry.m_nValue );
		}
		else
		{
			if ( i != 0 )
				sOut.Append( "." );

			sOut.Append( entry.m_pszName );
		}

		bAppended = true;
	}

	return bAppended;
}

bool CKV3TransferLoadContext::CanLoadField( int nFlags ) const
{
	if ( nFlags != 1 )
		return true;

	// The SDK's IRestore declares a different interface, so the game's virtual is called by its slot.
	using Unk_06_t = bool ( * )( IRestore * );

	return !( *reinterpret_cast< Unk_06_t *const * >( m_pRestore ) )[ 6 ]( m_pRestore );
}

const KeyValues3 *CKV3TransferLoadContext::FindClassMember( const CKV3MemberName &name )
{
	if ( m_pSourceObject->IsNull() )
		return nullptr;

	if ( !m_pSourceObject->IsTable() )
	{
		NoteFailure( "Class data is a '%s', not a table\n", m_pSourceObject->GetTypeAsString() );
		return nullptr;
	}

	return m_pSourceObject->FindMember( name );
}

void CKV3TransferLoadContext::PushSource( const KeyValues3 *pSource )
{
	m_pSourceObject = pSource;
	m_SourceStack.AddToTail( pSource );
}

void CKV3TransferLoadContext::PopSource()
{
	m_SourceStack.RemoveMultipleFromTail( 1 );
	m_pSourceObject = m_SourceStack.Count() > 0 ? m_SourceStack.Tail() : nullptr;
}

void CKV3TransferLoadContext::PushContextPath( const CKV3MemberName &name )
{
	const char *pszName = name.GetString();

	if ( pszName && pszName[ 0 ] )
	{
		PushContextPath( pszName );
	}
	else if ( name.GetHashCode() )
	{
		ContextPathEntry_t entry;

		entry.m_nValue = name.GetHashCode();
		entry.m_nTag = CONTEXT_PATH_TAG_NAME_HASH;

		m_ContextPath.AddToTail( entry );
	}
	else
	{
		PushContextPath( "<empty member name>" );
	}
}

void CKV3TransferLoadContext::PushContextPath( const char *pszName )
{
	ContextPathEntry_t entry;

	entry.m_pszName = pszName;

	m_ContextPath.AddToTail( entry );
}

void CKV3TransferLoadContext::PushContextPathIndex( int nIndex )
{
	ContextPathEntry_t entry;

	entry.m_nValue = static_cast< uint32 >( nIndex );
	entry.m_nTag = CONTEXT_PATH_TAG_INDEX;

	m_ContextPath.AddToTail( entry );
}
