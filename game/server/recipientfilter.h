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
	CRecipientFilter()
	{
		m_nBufType = BUF_RELIABLE;
		m_bInitMessage = false;
	}

	CRecipientFilter(IRecipientFilter *source, int iExcept = -1)
	{
		m_nBufType = source->GetNetworkBufType();
		m_bInitMessage = source->IsInitMessage();
		m_Recipients.RemoveAll();

		for (int i = 0; i < source->GetRecipientCount(); i++)
		{
			if (source->GetRecipientIndex(i).Get() != iExcept)
				m_Recipients.AddToTail(source->GetRecipientIndex(i));
		}
	}

	~CRecipientFilter() override {}

	NetChannelBufType_t GetNetworkBufType(void) const override { return m_nBufType; }
	bool IsInitMessage(void) const override { return m_bInitMessage; }
	int GetRecipientCount(void) const override { return m_Recipients.Count(); }

	CPlayerSlot GetRecipientIndex(int slot) const override
	{
		if (slot < 0 || slot >= GetRecipientCount())
			return CPlayerSlot(-1);

		return m_Recipients[slot];
	}

	void AddRecipient(CPlayerSlot slot)
	{
		// Don't add if it already exists
		if (m_Recipients.Find(slot) != m_Recipients.InvalidIndex())
			return;

		m_Recipients.AddToTail(slot);
	}

	void RemoveRecipient( CPlayerSlot slot )
	{
		auto iFound = m_Recipients.Find(slot);

		if (iFound != m_Recipients.InvalidIndex())
		{
			m_Recipients.Remove(iFound);
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

	void			CopyFrom( const CRecipientFilter& src );

	void			Reset( void );

	void			MakeInitMessage( void );

	void			MakeReliable( void );
	
	void			AddAllPlayers( void );
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

private:
	NetChannelBufType_t m_nBufType;
	bool m_bInitMessage;
	CUtlVectorFixed<CPlayerSlot, ABSOLUTE_PLAYER_LIMIT> m_Recipients;
};

//-----------------------------------------------------------------------------
// Purpose: Simple class to create a filter for a single player ( unreliable )
//-----------------------------------------------------------------------------
class CSingleRecipientFilter : public CRecipientFilter
{
public:
	CSingleRecipientFilter(CPlayerSlot nRecipientSlot, NetChannelBufType_t nBufType = BUF_RELIABLE, bool bInitMessage = false) :
		m_nRecipientSlot(nRecipientSlot), m_nBufType(nBufType), m_bInitMessage(bInitMessage) {}

	~CSingleRecipientFilter() override {}

	NetChannelBufType_t GetNetworkBufType(void) const override { return m_nBufType; }
	bool IsInitMessage(void) const override { return m_bInitMessage; }
	int GetRecipientCount(void) const override { return 1; }
	CPlayerSlot GetRecipientIndex(int slot) const override { return m_nRecipientSlot; }

private:
	CPlayerSlot m_nRecipientSlot;
	NetChannelBufType_t m_nBufType;
	bool m_bInitMessage;
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
