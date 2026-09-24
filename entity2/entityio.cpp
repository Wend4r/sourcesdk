#include "entityio.h"
#include "entityinstance.h"
#include "entitysystem.h"
#include "tier0/strtools.h"

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
