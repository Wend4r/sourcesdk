#include "entityidentity.h"
#include "entitynetwork.h"
#include "kv3lib/kv3transfer.h"

void CNetworkTransmitComponent::KV3TransferSave( CKV3TransferSaveContext *pContext ) const
{
	KV3TransferSave_CNetworkTransmitComponent( pContext );
}

void CNetworkTransmitComponent::KV3TransferLoad( CKV3TransferLoadContext *pContext )
{
	KV3TransferLoad_CNetworkTransmitComponent( pContext );
}

void CNetworkTransmitComponent::KV3TransferSave_CNetworkTransmitComponent( CKV3TransferSaveContext *pContext ) const
{
	// Saved as a uint32
	if ( pContext->ShouldSaveField( false ) )
		pContext->SaveValueToMember( "m_nTransmitStateOwnedCounter", static_cast< uint32 >( m_nTransmitStateOwnedCounter ) );
}

void CNetworkTransmitComponent::KV3TransferLoad_CNetworkTransmitComponent( CKV3TransferLoadContext *pContext )
{
	pContext->LoadSchemaField( "m_nTransmitStateOwnedCounter", m_nTransmitStateOwnedCounter, static_cast< uint8 >( 0 ) );
}
