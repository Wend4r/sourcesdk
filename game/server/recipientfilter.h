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
#include "serversideclient.h"
#include "iserver.h"
#include "const.h"
#include "tier0/bitvec.h"
#include <tier0/utlvector.h>
#include <kv3lib/kv3transfer.h>
#include <schemasystem/schematypes.h>

#include "schemacompiler/schemaon.h"

class CBaseEntity;
class CBasePlayer;
class CBasePlayerController;
class CTeam;

//-----------------------------------------------------------------------------
// Purpose: A generic filter for determining whom to send message/sounds etc. to and
//  providing a bit of additional state information
//
// 24 bytes on both platforms.
//-----------------------------------------------------------------------------
class CRecipientFilter : public IRecipientFilter
{
public:
	// The game's own default constructor leaves the buffer type at BUF_DEFAULT; this one keeps
	// BUF_RELIABLE as its default argument.
	CRecipientFilter( NetChannelBufType_t nBufType = BUF_RELIABLE, bool bInitMessage = false ) :
		m_nPredictedByPlayerSlot(), m_nBufType( nBufType ), m_bInitMessage( bInitMessage ), m_bUsingPredictionRules( false ), m_bIgnorePredictionCull( false ) {}

	CRecipientFilter( IRecipientFilter *source, CPlayerSlot exceptSlot = INVALID_PLAYER_SLOT ) :
		m_nPredictedByPlayerSlot(), m_bUsingPredictionRules( false ), m_bIgnorePredictionCull( false )
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
	CPlayerSlot GetPredictedByPlayerSlot() const override { return m_nPredictedByPlayerSlot; }

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

	// Adds the player's slot; under prediction rules the predicting player goes to
	// m_nPredictedByPlayerSlot instead
	void AddRecipient( const CBasePlayerController *pPlayer );

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

	// Copies everything but m_nPredictedByPlayerSlot
	void CopyFrom( const CRecipientFilter& src )
	{
		m_Recipients = src.m_Recipients;
		m_nBufType = src.GetNetworkBufType();
		m_bInitMessage = src.IsInitMessage();
		m_bUsingPredictionRules = src.m_bUsingPredictionRules;
		m_bIgnorePredictionCull = src.m_bIgnorePredictionCull;
	}

	void Reset( void )
	{
		m_Recipients.ClearAll();
		m_nBufType = BUF_DEFAULT;
		m_bInitMessage = false;
		m_bUsingPredictionRules = false;
		m_bIgnorePredictionCull = false;
	}

	void			MakeInitMessage( void ) { m_bInitMessage = true; }
	void			MakeReliable( void ) { m_nBufType = BUF_RELIABLE; }
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

	// Sets m_bUsingPredictionRules once and takes the predicting player (and the players it
	// controls) out of the recipients
	void			UsePredictionRules( void );
	bool			IsUsingPredictionRules( void ) const { return m_bUsingPredictionRules; }

	bool			IgnorePredictionCull( void ) const { return m_bIgnorePredictionCull; }
	void			SetIgnorePredictionCull( bool ignore ) { m_bIgnorePredictionCull = ignore; }

	void			RemoveSplitScreenPlayers();
	void			ReplaceSplitScreenPlayersWithOwners();

	void			RemoveDuplicateRecipients();

protected:
	CPlayerBitVec m_Recipients;
	CPlayerSlot m_nPredictedByPlayerSlot;
	NetChannelBufType_t m_nBufType;
	bool m_bInitMessage;
	bool m_bUsingPredictionRules;
	bool m_bIgnorePredictionCull;
};

//-----------------------------------------------------------------------------
// Purpose: Simple class to create a filter for a single player. A helper of this SDK, the game
//  uses CSingleUserRecipientFilter.
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

//-----------------------------------------------------------------------------
// Purpose: Simple class to create a filter for a single player ( unreliable )
//-----------------------------------------------------------------------------
class CSingleUserRecipientFilter : public CRecipientFilter
{
public:
	CSingleUserRecipientFilter( const CBasePlayerController *pPlayer );
};

//-----------------------------------------------------------------------------
// Purpose: Simple class to create a filter for a single player ( reliable )
//-----------------------------------------------------------------------------
class CReliableSingleUserRecipientFilter : public CSingleUserRecipientFilter
{
public:
	CReliableSingleUserRecipientFilter( const CBasePlayerController *pPlayer ) :
		CSingleUserRecipientFilter( pPlayer )
	{
		MakeReliable();
	}
};

//-----------------------------------------------------------------------------
// Purpose: The player and every player observing it
//-----------------------------------------------------------------------------
class CSingleUserPlusObserversFilter : public CRecipientFilter
{
public:
	CSingleUserPlusObserversFilter( CBaseEntity *pPlayer, bool bReliable = false );
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
// Purpose: The players in the PAS of a sound that can still hear it at the given attenuation
//-----------------------------------------------------------------------------
class CPASAttenuationFilter : public CPASFilter
{
public:
	CPASAttenuationFilter( void )
	{
	}

	// A player entity adds only its controller; any other entity filters from its origin
	CPASAttenuationFilter( CBaseEntity *pEntity, float flAttenuation );

	CPASAttenuationFilter( const Vector &origin, float flAttenuation )
	{
		Filter( origin, flAttenuation );
	}

	// Adds the players of the PAS of origin that are close enough to hear a sound of this
	// attenuation (all of them when the attenuation is not positive)
	void Filter( const Vector &origin, float flAttenuation );
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

//-----------------------------------------------------------------------------
// Bits of m_Flags of CCopyRecipientFilter and CSosCopyRecipientFilter
//-----------------------------------------------------------------------------
enum CopyRecipientFilterFlags_t
{
	COPY_RECIPIENT_FILTER_ACTIVE = 0x1,
	COPY_RECIPIENT_FILTER_RELIABLE = 0x2,
	COPY_RECIPIENT_FILTER_INIT_MESSAGE = 0x4,
	// Named by what it does: GetNetworkBufType of CSosCopyRecipientFilter returns BUF_UNRELIABLE
	// for it and BUF_DEFAULT without it (the reliable bit wins)
	COPY_RECIPIENT_FILTER_UNRELIABLE = 0x8,
};

//-----------------------------------------------------------------------------
// A recipient filter kept as a list of player slots, saved with the sound patches (schema class,
// 56 bytes on both platforms). It has its own schema and KV3 transfer virtuals after the
// IRecipientFilter ones.
//-----------------------------------------------------------------------------
schema class CCopyRecipientFilter : public IRecipientFilter
{
public:
	TYPEMETA( MGetKV3ClassDefaults );

	CCopyRecipientFilter() : m_Flags( 0 ), m_slotPlayerExcludedDueToPrediction( -1 ) {}
	~CCopyRecipientFilter() override {}

	// BUF_RELIABLE or BUF_UNRELIABLE, from COPY_RECIPIENT_FILTER_RELIABLE
	NetChannelBufType_t GetNetworkBufType() const override { return ( m_Flags & COPY_RECIPIENT_FILTER_RELIABLE ) ? BUF_RELIABLE : BUF_UNRELIABLE; }
	bool IsInitMessage() const override { return ( m_Flags & COPY_RECIPIENT_FILTER_INIT_MESSAGE ) != 0; }
	// Rebuilds m_RecipientBits from m_Recipients
	const CPlayerBitVec &GetRecipients() const override;
	CPlayerSlot GetPredictedByPlayerSlot() const override { return m_slotPlayerExcludedDueToPrediction; }

	virtual SchemaMetaInfoHandle_t< CSchemaClassInfo > Schema_DynamicBinding();
	CLASS_USES_KV3TRANSFER_VIRTUAL( CCopyRecipientFilter );
	virtual const char *GetClassName() const { return "CCopyRecipientFilter"; }
	// Adds the slot unless it is already in the list; false for an invalid or a listed slot
	virtual bool AddRecipient( CPlayerSlot slot );
	// Adds the slot of the player controller the entity belongs to
	virtual bool AddRecipient( CBaseEntity *pPlayer );

	bool IsActive() const { return ( m_Flags & COPY_RECIPIENT_FILTER_ACTIVE ) != 0; }

public:
	int32 m_Flags; // CopyRecipientFilterFlags_t
	CUtlVector< CPlayerSlot > m_Recipients;
	// Named by what it does: the bits GetRecipients builds from m_Recipients
	noschema mutable CPlayerBitVec m_RecipientBits;
	CPlayerSlot m_slotPlayerExcludedDueToPrediction;
};

//-----------------------------------------------------------------------------
// A recipient filter copied into a sound operator event: the recipients as bits (24 bytes on both
// platforms)
//-----------------------------------------------------------------------------
class CSosCopyRecipientFilter : public IRecipientFilter
{
public:
	CSosCopyRecipientFilter() : m_Flags( 0 ), m_slotPlayerExcludedDueToPrediction( -1 ) {}
	~CSosCopyRecipientFilter() override {}

	NetChannelBufType_t GetNetworkBufType() const override
	{
		if ( m_Flags & COPY_RECIPIENT_FILTER_RELIABLE )
			return BUF_RELIABLE;

		return ( m_Flags & COPY_RECIPIENT_FILTER_UNRELIABLE ) ? BUF_UNRELIABLE : BUF_DEFAULT;
	}
	bool IsInitMessage() const override { return ( m_Flags & COPY_RECIPIENT_FILTER_INIT_MESSAGE ) != 0; }
	const CPlayerBitVec &GetRecipients() const override { return m_Recipients; }
	CPlayerSlot GetPredictedByPlayerSlot() const override { return m_slotPlayerExcludedDueToPrediction; }

	// Sets the bit of the player controller the entity belongs to; false when it was set already
	virtual bool AddRecipient( CBaseEntity *pPlayer );

public:
	int32 m_Flags; // CopyRecipientFilterFlags_t
	CPlayerBitVec m_Recipients;
	CPlayerSlot m_slotPlayerExcludedDueToPrediction;
};

#include "schemacompiler/schemaoff.h"

#endif // RECIPIENTFILTER_H
