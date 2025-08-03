//========= Copyright 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================//

#ifndef INETCHANNEL_H
#define INETCHANNEL_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "tier0/netadr.h"
#include "inetchannelinfo.h"
#include "steam/steamnetworkingtypes.h"
#include "tier1/bitbuf.h"
#include "tier1/utldelegate.h"
#include <eiface.h>

#define MAX_NETCHANNELL_CATEGORY_ID 32

class	IDemoRecorderBase;
class	IInstantReplayIntercept;
class	CNetMessage;
class	INetChannelHandler;
class	INetChannel;
class	INetChannelInfo;
class	INetMessageBinder;
class	INetworkMessageProcessingPreFilter;
class	INetworkMessageInternal;
class	INetMessageDispatcher;
class	InstantReplayMessage_t;
class	CUtlSlot;

FORWARD_DECLARE_HANDLE(NetMessageHandle_t);

enum SignonGroup_t : int8
{
	SG_INVALID = -1,
	SG_GENERIC = 0,	// must be first and is default group
	SG_LOCALPLAYER,
	SG_OTHERPLAYER,
	SG_ENTITIES,
	SG_SOUNDS,			// game sounds
	SG_EVENTS,			// event messages
	SG_VOICE,
	SG_STRINGTABLE,	// a stringtable update
	SG_MOVE,			// client move cmds
	SG_STRINGCMD,		// string command
	SG_SIGNON,			// various signondata
	SG_SYSTEM,
	SG_USERMSG = 13,
	SG_CLIENTMSG,
	SG_SPAWNGROUPS,
	SG_ENGINE,
	SG_HLTVREPLAY,
	SG_TOTAL,			// must be last and is not a real group
};

enum NetChannelBufType_t : int8
{
	BUF_DEFAULT = -1,
	BUF_UNRELIABLE = 0,
	BUF_RELIABLE,
	BUF_VOICE,
};

#ifndef NET_PACKET_ST_DEFINED
#define NET_PACKET_ST_DEFINED
struct netpacket_s
{
	netadr_t		from;		// sender IP
	int				source;		// received source
	double			received;	// received time
	unsigned char	*data;		// pointer to raw packet data
	bf_read			message;	// easy bitbuf data access
	int				size;		// size in bytes
	int				wiresize;   // size in bytes before decompression
	bool			stream;		// was send as stream
	struct netpacket_s *pNext;	// for internal use, should be NULL in public
};
#endif // NET_PACKET_ST_DEFINED

using netpacket_t = netpacket_s;
using NetPacket_t = netpacket_t;

abstract_class INetworkChannelNotify
{
public:
	virtual void OnShutdownChannel( INetChannel *pChannel ) = 0;
};

abstract_class INetworkMessageProcessingPreFilter
{
public:
	// Filter incoming messages from the netchan, return true to filter out (block) the further processing of the message
	virtual bool FilterMessage( const CNetMessage *pData, INetChannel *pChannel ) = 0;
};

abstract_class INetChannel : public INetChannelInfo
{
public:
	virtual	~INetChannel( void ) {};

	virtual void	Reset( void ) = 0;
	virtual void	Clear( void ) = 0;
	virtual void	Shutdown( ENetworkDisconnectionReason reason ) = 0;
	
	virtual HSteamNetConnection GetSteamNetConnection( void ) const = 0;

	virtual bool	SendNetMessage( const CNetMessage *pData, NetChannelBufType_t bufType ) = 0;
	virtual bool	SendData( bf_write &msg, NetChannelBufType_t bufferType ) = 0;
	virtual int		Transmit( const char *pDebugName, bf_write *data ) = 0;
	virtual void	SetBitsToSend( void ) = 0;
	virtual int		SendMessages( const char *pDebugName, bf_write *data ) = 0;

	virtual const netadr_t &GetRemoteAddress( void ) const = 0;

	virtual void	UpdateMessageStats( int msggroup, int bits, bool ) = 0;
	
	virtual void	unk012() = 0;
	
	virtual bool	CanPacket( void ) const = 0;
	virtual bool	IsOverflowed( void ) const = 0;
	virtual bool	HasPendingReliableData( void ) = 0;

	// For routing messages to a different handler
	virtual bool	SetActiveChannel( INetChannel *pNewChannel ) = 0;
	
	virtual void	AttachSplitPlayer( CSplitScreenSlot nSlot, INetChannel *pChannel ) = 0;
	virtual void	DetachSplitPlayer( CSplitScreenSlot nSlot ) = 0;

	virtual void	SetMinDataRate( int rate ) = 0;
	virtual void	SetMaxDataRate( int rate ) = 0;

	virtual void	SetTimeout( float seconds, bool bForceExact = false ) = 0;
	virtual bool	IsTimedOut( void ) const = 0;
	virtual void	UpdateLastReceivedTime( void ) = 0;

	virtual void	unk111( float ) = 0;
	virtual void	unk112( void *pStat ) = 0;
	
	virtual bool	IsRemoteDisconnected( ENetworkDisconnectionReason &reason ) const = 0;

	virtual void	SetNetMessageDispatcher( INetMessageDispatcher *pDispatcher ) = 0;
	virtual INetMessageDispatcher *GetNetMessageDispatcher( void ) const = 0;
	
	virtual void	StartRegisteringMessageHandlers( void ) = 0;
	virtual void	FinishRegisteringMessageHandlers( void ) = 0;
	
	virtual void	RegisterNetMessageHandlerAbstract( CUtlSlot *nSlot, const CUtlAbstractDelegate &delegate, int nParamCount, INetworkMessageInternal *pNetMessage, int nPriority ) = 0;
	virtual void	UnregisterNetMessageHandlerAbstract( CUtlSlot *nSlot, const CUtlAbstractDelegate &delegate, INetworkMessageInternal *pNetMessage ) = 0;
	
	virtual int		GetNumBitsWritten( NetChannelBufType_t bufferType ) const = 0;
	virtual void	SetDemoRecorder( IDemoRecorderBase *pDemoRecorder ) = 0;
	virtual void	SetInstantReplayIntercept( IInstantReplayIntercept *pInstantReplayIntercept ) = 0;
	virtual bool	IsNull( void ) const = 0;
	virtual bool	ParseMessagesDemo( NetPacket_t *packet ) = 0;
	virtual bool	ProcessMessages() = 0;
	
	virtual void	InstallMessageFilter( INetworkMessageProcessingPreFilter *pFilter ) = 0;
	virtual void	UninstallMessageFilter( INetworkMessageProcessingPreFilter *pFilter ) = 0;

	virtual void	SetActiveNetMessage( CNetMessage *pData ) = 0;
	virtual void	SetActiveNetMessagePB( void *pData ) = 0;
	virtual void	IsActiveNetMessage( int16 id ) = 0;

	virtual void	SetPendingDisconnect( ENetworkDisconnectionReason reason ) = 0;
	virtual ENetworkDisconnectionReason GetPendingDisconnect( void ) const = 0;

	virtual void	SuppressTransmit( bool suppress ) = 0;
	virtual bool	IsSuppressingTransmit( void ) const = 0;

	virtual void	PostReceivedNetMessage( const CNetMessage *pData, int nBits, int nInSequenceNr ) = 0;
	virtual void	InsertReplayMessage( InstantReplayMessage_t &msg ) = 0;
	virtual bool	HasQueuedNetMessages( int nMessageId ) const = 0;

	virtual void	SetUnkWhenEmpty( int ) = 0;

	virtual void	*unk101( void ) = 0;
	virtual void	unk102( void *pStat ) = 0;
	virtual void	unk103( void * ) = 0;
};


#endif // INETCHANNEL_H
