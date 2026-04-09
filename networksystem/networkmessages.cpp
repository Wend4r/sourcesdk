#include "networksystem/netmessage.h"
#include "serversideclient.h"

bool CNetMessage::Send( CPlayerSlot slot ) const
{
	return Send( CUtlVector< CPlayerSlot >{ slot } ) != 0;
}

int CNetMessage::Send( const CPlayerBitVec &playerBits ) const
{
	if ( !g_pNetworkServerService->IsServerRunning() )
	{
		return 0;
	}

	CNetworkGameServer *pNetServer = g_pNetworkServerService->GetNetworkServer();

	if ( !pNetServer )
	{
		return 0;
	}

	int nSent = 0;

	int index = playerBits.FindNextSetBit( 0 );

	while ( index > -1 )
	{
		CServerSideClientBase *pClient = pNetServer->GetClientBySlot( index );

		index = playerBits.FindNextSetBit( index + 1 );

		if ( !pClient )
		{
			continue;
		}

		if ( pClient->SendNetMessage( this, GetBufType() ) )
		{
			nSent++;
		}
	}

	return nSent;
}

int CNetMessage::Send( const CUtlVector< CPlayerSlot > &vecSlots ) const
{
	if ( !g_pNetworkServerService->IsServerRunning() )
	{
		return 0;
	}

	CNetworkGameServer *pNetServer = g_pNetworkServerService->GetNetworkServer();

	if ( !pNetServer )
	{
		return 0;
	}

	int nSent = 0;

	for ( auto slot : vecSlots )
	{
		CServerSideClientBase *pClient = pNetServer->GetClientBySlot( slot );

		if ( !pClient )
		{
			continue;
		}

		if ( pClient->SendNetMessage( this, GetBufType() ) )
		{
			nSent++;
		}
	}

	return nSent;
}

int CNetMessage::SendToAllClients() const
{
	if ( !g_pNetworkServerService->IsServerRunning() )
	{
		return 0;
	}

	CNetworkGameServer *pNetServer = g_pNetworkServerService->GetNetworkServer();

	if ( !pNetServer )
	{
		return 0;
	}

	int nSent = 0;

	for ( const auto &pClient : pNetServer->GetClients() )
	{
		if ( pClient->SendNetMessage( this, GetBufType() ) )
		{
			nSent++;
		}
	}

	return nSent;
}
