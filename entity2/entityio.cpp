#include "entityio.h"
#include "entityinstance.h"
#include "entitysystem.h"
#include "kv3lib/kv3transfer.h"
#include "tier0/strtools.h"

// The member that keeps m_bForwardAllArgs within the saved parameters
#define PULSE_INPUT_PARAM_MAP_FORWARD_ALL_ARGS "--forward-all-args--"

// The parameter of a connection whose "value" is the value override
#define PULSE_INPUT_PARAM_MAP_OLD_CONNECTION_LITERAL "--old-connection-literal--"

// Stores a value override as the "value" of the old connection literal, unless the map holds other parameters.
// An empty value override removes the literal.
static void SetParamMapValueOverride( CPulseInputParamMap &paramMap, const char *pszValueOverride )
{
	KeyValues3 &kv = paramMap.m_KV3;

	if ( !pszValueOverride || !pszValueOverride[ 0 ] )
	{
		if ( kv.IsTable() )
			kv.RemoveMember( PULSE_INPUT_PARAM_MAP_OLD_CONNECTION_LITERAL );

		return;
	}

	if ( !kv.IsTable() )
		kv.SetToEmptyTable();
	else if ( kv.GetMemberCount() > 0 && !kv.FindMember( PULSE_INPUT_PARAM_MAP_OLD_CONNECTION_LITERAL ) )
		return;

	KeyValues3 *pLiteral = kv.FindOrCreateMember( PULSE_INPUT_PARAM_MAP_OLD_CONNECTION_LITERAL );

	if ( !pLiteral->IsTable() )
		pLiteral->SetToEmptyTable();

	KeyValues3 *pValue = pLiteral->FindOrCreateMember( "value" );

	pValue->SetToNull();
	pValue->SetString( pszValueOverride );
}

// A map holding a single parameter with a "value" gives that value as the value override. An array value
// gives its elements when they all are integers or all are floats, otherwise an empty string.
static bool GetParamMapValueOverride( const CPulseInputParamMap &paramMap, CUtlString *pValueOverride )
{
	const KeyValues3 &kv = paramMap.m_KV3;

	if ( !kv.IsTable() || kv.GetMemberCount() != 1 )
		return false;

	const KeyValues3 *pParam = kv.GetMember( 0 );
	const KeyValues3 *pValue = pParam->IsTable() ? pParam->FindMember( "value" ) : nullptr;

	if ( !pValue )
		return false;

	if ( pValue->IsArray() )
	{
		const int nCount = pValue->GetArrayElementCount();
		const KV3Type_t nType = nCount > 0 ? pValue->GetArrayElement( 0 )->GetType() : KV3_TYPE_INVALID;

		bool bNumeric = nType == KV3_TYPE_INT || nType == KV3_TYPE_UINT || nType == KV3_TYPE_DOUBLE;

		for ( int i = 1; bNumeric && i < nCount; ++i )
			bNumeric = pValue->GetArrayElement( i )->GetType() == nType;

		if ( !bNumeric )
		{
			pValueOverride->Set( "" );
			return true;
		}
	}

	CBufferStringN< 128 > sValue;

	pValueOverride->Set( pValue->ToString( sValue, KV3_TO_STRING_APPEND_ONLY_NUMERICS | KV3_TO_STRING_RETURN_NON_NUMERICS ) );

	return true;
}

static void SaveParamMap( const CPulseInputParamMap &paramMap, CKV3TransferSaveContext *pContext )
{
	pContext->TargetObject()->CopyFrom( paramMap.m_KV3 );
	pContext->CreateTargetMember( PULSE_INPUT_PARAM_MAP_FORWARD_ALL_ARGS )->SetBool( paramMap.m_bForwardAllArgs );
}

static void LoadParamMap( CPulseInputParamMap &paramMap, CKV3TransferLoadContext *pContext )
{
	paramMap.m_KV3.CopyFrom( pContext->SourceObject() );

	const KeyValues3 *pForwardAllArgs = pContext->SourceObject()->FindMember( PULSE_INPUT_PARAM_MAP_FORWARD_ALL_ARGS );

	if ( pForwardAllArgs )
	{
		paramMap.m_bForwardAllArgs = pForwardAllArgs->GetBool();
		paramMap.m_KV3.RemoveMember( PULSE_INPUT_PARAM_MAP_FORWARD_ALL_ARGS );
	}
	else
	{
		paramMap.m_bForwardAllArgs = false;
	}
}

// Writes the members of the target table. The output name is only written when given.
static void SaveConnection( const EntityIOConnection_t &connection, CKV3TransferSaveContext *pContext, const char *pszOutputName )
{
	KeyValues3 *pTarget = pContext->TargetObject();

	if ( pszOutputName )
		pTarget->SetMemberString( "output", pszOutputName );

	pTarget->SetMemberInt( "target_type", connection.m_nTargetType );
	pTarget->SetMemberString( "target", STRING( connection.m_targetDesc ) );
	pTarget->SetMemberString( "input", STRING( connection.m_targetInput ) );
	pTarget->SetMemberString( "value_override", STRING( connection.m_valueOverride ) );
	pTarget->SetMemberInt( "times_to_fire", connection.m_nTimesToFire );
	pTarget->SetMemberFloat( "delay", connection.m_flDelay );

	pContext->SaveValueToMember( "m_hTarget", connection.m_hTarget );

	KeyValues3 *pParamMap = pContext->CreateTargetMember( "m_paramMap" );

	if ( !pContext->PrepareTargetForClass( pParamMap, KV3TRANSFER_CLASS_AS_SIMPLE_TABLE, "" ) )
		return;

	pContext->PushTarget( pParamMap );

	if ( pContext->TargetDepth() > KV3TRANSFER_MAX_STACK_DEPTH )
		pContext->NoteFailure( "Stack depth limit hit (%d)", pContext->TargetDepth() );
	else
		SaveParamMap( connection.m_paramMap, pContext );

	pContext->PopTarget();

	if ( pContext->GetResult() == KV3TRANSFER_FAIL )
		pParamMap->SetToNull();
}

// Reads the members of the source table. Strings go to the entity system's string pool.
static void LoadConnection( EntityIOConnection_t &connection, CKV3TransferLoadContext *pContext, const char **ppszOutputName )
{
	const KeyValues3 *pSource = pContext->SourceObject();
	CEntitySystem *pEntitySystem = GameEntitySystem();

	*ppszOutputName = pSource->GetMemberString( "output" );

	connection.m_nTargetType = static_cast< EntityIOTargetType_t >( pSource->GetMemberInt( "target_type", ENTITY_IO_TARGET_ENTITYNAME_OR_CLASSNAME ) );
	connection.m_targetDesc = MAKE_STRING( pEntitySystem->AllocPooledString( pSource->GetMemberString( "target" ) ).String() );
	connection.m_targetInput = MAKE_STRING( pEntitySystem->AllocPooledString( pSource->GetMemberString( "input" ) ).String() );
	connection.m_valueOverride = MAKE_STRING( pEntitySystem->AllocPooledString( pSource->GetMemberString( "value_override" ) ).String() );
	connection.m_nTimesToFire = pSource->GetMemberInt( "times_to_fire" );
	connection.m_flDelay = pSource->GetMemberFloat( "delay" );

	pContext->LoadSchemaField( "m_hTarget", connection.m_hTarget, CEntityHandle() );

	const KeyValues3 *pParamMap = pContext->FindClassMember( "m_paramMap" );
	KeyValues3 nullValue;

	if ( pParamMap )
		pContext->PushContextPath( "m_paramMap" );

	pContext->PushSource( pParamMap ? pParamMap : &nullValue );

	if ( pContext->SourceDepth() > KV3TRANSFER_MAX_STACK_DEPTH )
		pContext->NoteFailure( "Stack depth limit hit (%d)", pContext->SourceDepth() );
	else
		LoadParamMap( connection.m_paramMap, pContext );

	pContext->PopSource();

	if ( pParamMap )
		pContext->PopContextPath();
}

void CEntityIOOutput::KV3TransferSave( CKV3TransferSaveContext *pContext ) const
{
	if ( !m_pConnections )
		return;

	KeyValues3 *pConnections = pContext->CreateTargetMember( "connections" );

	for ( const EntityIOConnection_t *pConnection = m_pConnections; pConnection; pConnection = pConnection->m_pNext )
	{
		pContext->PushTarget( pConnections->ArrayAddElementToTail() );
		SaveConnection( *pConnection, pContext, nullptr );
		pContext->PopTarget();
	}
}

void CEntityIOOutput::KV3TransferLoad( CKV3TransferLoadContext *pContext )
{
	for ( EntityIOConnection_t *pConnection = m_pConnections; pConnection; )
	{
		EntityIOConnection_t *pNext = pConnection->m_pNext;

		Release( pConnection );

		pConnection = pNext;
	}

	m_pConnections = nullptr;

	const KeyValues3 *pConnections = pContext->SourceObject()->FindMember( "connections" );

	if ( !pConnections )
		return;

	// From the last element, so that adding to the head keeps the saved order
	for ( int i = pConnections->GetArrayElementCount(); i-- > 0; )
	{
		EntityIOConnection_t *pConnection = Create< EntityIOConnection_t >();

		pConnection->m_nTimesToFire = 0;
		pConnection->m_flDelay = 0.0f;
		pConnection->m_bMarkedForRemoval = false;

		const char *pszOutputName;

		pContext->PushSource( pConnections->GetArrayElement( i ) );
		LoadConnection( *pConnection, pContext, &pszOutputName );
		pContext->PopSource();

		if ( pConnection->m_valueOverride != NULL_STRING )
		{
			SetParamMapValueOverride( pConnection->m_paramMap, STRING( pConnection->m_valueOverride ) );
		}
		else
		{
			CUtlString sValueOverride;

			if ( GetParamMapValueOverride( pConnection->m_paramMap, &sValueOverride ) )
				pConnection->m_valueOverride = MAKE_STRING( GameEntitySystem()->AllocPooledString( sValueOverride.Get() ).String() );
		}

		AddConnection( pConnection );
	}
}

void CEntityIOOutput::FireOutput( CEntityInstance *pActivator, CEntityInstance *pCaller, const CVariant &value, float flDelay )
{
	CPulseArgumentPack args;

	args.SetParamFromVariant( value );

	FireOutputInternal( pActivator, pCaller, &args, nullptr, &value, flDelay );
}

void CEntityIOOutput::FireOutputInternal( CEntityInstance *pActivator, CEntityInstance *pCaller, const CPulseArgumentPack *pArgs, const CPulseInputParamMap *pParamMap, const CVariant *pValue, float flDelay )
{
	CEntitySystem *pEntitySystem = GameEntitySystem();

	for ( IEntityIONotify *pNotify : pEntitySystem->m_entityIONotifiers )
		pNotify->OnOutputFired( pActivator, pCaller, m_pDesc, pArgs, flDelay );

	EntityIOConnection_t *pPrev = nullptr;

	for ( EntityIOConnection_t *pConnection = m_pConnections; pConnection; )
	{
		bool bRemove = pConnection->m_bMarkedForRemoval;

		if ( bRemove )
		{
			if ( pConnection->m_nTimesToFire != -1 )
				--pConnection->m_nTimesToFire;
		}
		else
		{
			CVariant value;

			if ( pValue )
				value.CopyFrom( *pValue );
			else if ( pArgs )
				pArgs->GetParamAsVariant( value );

			if ( const char *pszValueOverride = STRING( pConnection->m_valueOverride ) )
			{
				value.Free();
				value.m_type = FIELD_CSTRING;
				value.m_pszString = pszValueOverride;
			}

			// The parameters of the connection are merged into a copy of pParamMap.
			CPulseInputParamMap *pMergedParamMap = nullptr;
			const CPulseInputParamMap *pConnectionParamMap = &pConnection->m_paramMap;

			if ( pParamMap )
			{
				pMergedParamMap = Create< CPulseInputParamMap >();
				pMergedParamMap->Merge( *pParamMap );
				pMergedParamMap->Merge( pConnection->m_paramMap );

				pConnectionParamMap = pMergedParamMap;
			}

			const float flFireDelay = flDelay + pConnection->m_flDelay;

			if ( pConnection->m_nTargetType == ENTITY_IO_TARGET_EHANDLE )
				pEntitySystem->AddEvent( pConnection->m_hTarget, CUtlSymbolLarge( STRING( pConnection->m_targetInput ) ), pActivator, pCaller, value, flFireDelay, pArgs, pConnectionParamMap );
			else
				pEntitySystem->AddEvent( pConnection->m_nTargetType, CUtlSymbolLarge( STRING( pConnection->m_targetDesc ) ), CUtlSymbolLarge( STRING( pConnection->m_targetInput ) ), pActivator, pCaller, flFireDelay, value, pArgs, pConnectionParamMap );

			for ( IEntityIONotify *pNotify : pEntitySystem->m_entityIONotifiers )
				pNotify->OnConnectionFired( pActivator, pCaller, pConnection, pArgs );

			if ( pMergedParamMap )
				Release( pMergedParamMap );

			bRemove = pConnection->m_nTimesToFire != -1 && !--pConnection->m_nTimesToFire;
		}

		if ( !bRemove )
		{
			pPrev = pConnection;
			pConnection = pConnection->m_pNext;

			continue;
		}

		for ( IEntityIONotify *pNotify : pEntitySystem->m_entityIONotifiers )
			pNotify->OnConnectionRemoved( pActivator, pCaller, pConnection );

		EntityIOConnection_t *pNext = pConnection->m_pNext;

		if ( pPrev )
			pPrev->m_pNext = pNext;
		else
			m_pConnections = pNext;

		Release( pConnection );

		pConnection = pNext;
	}
}

int32 CBaseDynamicIOSignature::FindOutputIndex( const char *pszName ) const
{
	UtlSymId_t nSymbol = UTL_INVAL_SYMBOL;

	// The game takes the static symbol from its case-insensitive symbol table (CBaseDynamicIOSignature::sm_pSymbolTable).
	// Every output name is in that table, and its symbol is stored at the same index
	if ( pszName )
	{
		for ( int i = 0; i < m_outputs.Count(); ++i )
		{
			if ( !V_stricmp( m_outputs[ i ].m_pName, pszName ) )
			{
				nSymbol = m_outputNames[ i ];

				break;
			}
		}
	}

	const UtlHashHandle_t hIndex = m_outputNameToIndex.Find( nSymbol );

	return hIndex != m_outputNameToIndex.InvalidHandle() ? m_outputNameToIndex.Element( hIndex ) : -1;
}

void CDynamicIOInstance::FindOutputs( const char *pszName, CUtlVector< CEntityIOOutput * > &outputs )
{
	if ( !m_pSignature || !m_outputs.Count() )
		return;

	if ( CEntityIOOutput *pOutput = FindOutput( pszName ) )
		outputs.AddToTail( pOutput );
}

CEntityIOOutput *CDynamicIOInstance::FindOutput( const char *pszName )
{
	if ( !m_pSignature )
		return nullptr;

	const int32 nIndex = m_pSignature->FindOutputIndex( pszName );

	return nIndex != -1 ? &m_outputs[ nIndex ] : nullptr;
}
