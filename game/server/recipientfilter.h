//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#ifndef RECIPIENTFILTER_H
#define RECIPIENTFILTER_H
#ifdef _WIN32
#pragma once
#endif

#include "irecipientfilter.h"
#include "iserver.h"
#include "const.h"
#include "bitvec.h"

class CBasePlayer;
class CTeam;

//-----------------------------------------------------------------------------
// Purpose: A generic filter for determining whom to send message/sounds etc. to and
//  providing a bit of additional state information
//-----------------------------------------------------------------------------
class CRecipientFilter : public IRecipientFilter
{
public:
	CRecipientFilter( NetChannelBufType_t nBufType = BUF_RELIABLE, bool bInitMessage = false ) :
		m_nBufType(nBufType), m_bInitMessage(bInitMessage) {}

	CRecipientFilter( IRecipientFilter *source, CPlayerSlot exceptSlot = INVALID_PLAYER_SLOT )
	{
		m_Recipients = source->GetRecipients();
		m_nBufType = source->GetNetworkBufType();
		m_bInitMessage = source->IsInitMessage();

		if (exceptSlot != INVALID_PLAYER_SLOT)
		{
			m_Recipients.Clear(exceptSlot);
		}
	}

	~CRecipientFilter() override {}

	NetChannelBufType_t GetNetworkBufType( void ) const override { return m_nBufType; }
	bool IsInitMessage( void ) const override { return m_bInitMessage; }
	const CPlayerBitVec &GetRecipients( void ) const override { return m_Recipients; }

	void SetRecipients( uint64 nRecipients )
	{
		m_Recipients.Set(0UL, static_cast<uint32>(nRecipients & 0xFFFFFFFF));
		m_Recipients.Set(1UL, static_cast<uint32>(nRecipients >> 32));
	}

	void AddRecipient( CPlayerSlot slot )
	{
		if(slot >= 0 && slot < ABSOLUTE_PLAYER_LIMIT)
		{
			m_Recipients.Set(slot);
		}
	}

	void RemoveRecipient( CPlayerSlot slot )
	{
		if(slot >= 0 && slot < ABSOLUTE_PLAYER_LIMIT)
		{
			m_Recipients.Clear(slot);
		}
	}

	void AddPlayersFromBitMask( const CPlayerBitVec& playerbits )
	{
		int index = playerbits.FindNextSetBit( 0 );

		while ( index > -1 )
		{
			AddRecipient( index );

			index = playerbits.FindNextSetBit( index + 1 );
		}
	}

	void RemovePlayersFromBitMask( const CPlayerBitVec& playerbits )
	{
		int index = playerbits.FindNextSetBit( 0 );

		while ( index > -1 )
		{
			RemoveRecipient( index );

			index = playerbits.FindNextSetBit( index + 1 );
		}
	}

public:

	void CopyFrom( const CRecipientFilter& src )
	{
		m_Recipients = src.m_Recipients;
		m_bInitMessage = src.m_bInitMessage;
	}

	void Reset( void )
	{
		m_Recipients.ClearAll();
		m_bInitMessage = false;
	}

	void			MakeInitMessage( void ) { m_bInitMessage = true; }
	void			MakeReliable( void ) {}
	void			AddAllPlayers( void )
	{
		m_Recipients.ClearAll();

		Assert( g_pNetworkServerService );

		CNetworkGameServer *server = g_pNetworkServerService->GetNetworkServer();

		AssertMsg( server, "Server interface unavailable, too early" );

		for( const auto &pClient : server->GetClients() )
		{
			if ( pClient->IsActive() )
			{
				AddRecipient( pClient->GetPlayerSlot() );
			}
		}
	}

	void			AddRecipientsByPVS( const Vector& origin );
	void			RemoveRecipientsByPVS( const Vector& origin );
	void			AddRecipientsByPAS( const Vector& origin );
	void			RemoveAllRecipients( void );
	void			RemoveRecipientByPlayerIndex( int playerindex );
	void			AddRecipientsByTeam( const CTeam *team );
	void			RemoveRecipientsByTeam( const CTeam *team );
	void			RemoveRecipientsNotOnTeam( const CTeam *team );

	void			UsePredictionRules( void );
	bool			IsUsingPredictionRules( void ) const;

	bool			IgnorePredictionCull( void ) const;
	void			SetIgnorePredictionCull( bool ignore );

	void			RemoveSplitScreenPlayers();
	void			ReplaceSplitScreenPlayersWithOwners();

	void			RemoveDuplicateRecipients();

protected:
	CPlayerBitVec m_Recipients;
	NetChannelBufType_t m_nBufType;
	bool m_bInitMessage;
};

//-----------------------------------------------------------------------------
// Purpose: Simple class to create a filter for a single player ( unreliable )
//-----------------------------------------------------------------------------
class CSingleRecipientFilter : public CRecipientFilter
{
public:
	CSingleRecipientFilter( CPlayerSlot nRecipientSlot, NetChannelBufType_t nBufType = BUF_RELIABLE, bool bInitMessage = false ) :
		CRecipientFilter(nBufType, bInitMessage)
	{
		if(nRecipientSlot >= 0 && nRecipientSlot < ABSOLUTE_PLAYER_LIMIT)
		{
			m_Recipients.Set(nRecipientSlot);
		}
	}
};

class CSingleUserAndReplayRecipientFilter : public CRecipientFilter
{
public:
	CSingleUserAndReplayRecipientFilter( CPlayerSlot nRecipientSlot );
};
//-----------------------------------------------------------------------------
// Purpose: Simple class to create a filter for all players on a given team 
//-----------------------------------------------------------------------------
class CTeamRecipientFilter : public CRecipientFilter
{
public:
	CTeamRecipientFilter( int team, bool isReliable = false );
};

//-----------------------------------------------------------------------------
// Purpose: Simple class to create a filter for all players ( unreliable )
//-----------------------------------------------------------------------------
class CBroadcastRecipientFilter : public CRecipientFilter
{
public:
	CBroadcastRecipientFilter( void )
	{
		AddAllPlayers();
	}
};

//-----------------------------------------------------------------------------
// Purpose: Simple class to create a filter for all players ( reliable )
//-----------------------------------------------------------------------------
class CReliableBroadcastRecipientFilter : public CBroadcastRecipientFilter
{
public:
	CReliableBroadcastRecipientFilter( void )
	{
		MakeReliable();
	}
};

//-----------------------------------------------------------------------------
// Purpose: Add players in PAS to recipient list (unreliable)
//-----------------------------------------------------------------------------
class CPASFilter : public CRecipientFilter
{
public:
	CPASFilter( void )
	{
	}

	CPASFilter( const Vector& origin )
	{
		AddRecipientsByPAS( origin );
	}
};

//-----------------------------------------------------------------------------
// Purpose: Simple PVS based filter ( unreliable )
//-----------------------------------------------------------------------------
class CPVSFilter : public CRecipientFilter
{
public:
	CPVSFilter( const Vector& origin )
	{
		AddRecipientsByPVS( origin );
	}
};

#endif // RECIPIENTFILTER_H
