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

#include "inetchannelinfo.h"
#include "tier0/platform.h"
#include "tier0/netadr.h"
#include "tier1/bitbuf.h"
#include "tier1/utldelegate.h"
#include <eiface.h>

class	IDemoRecorderBase;
class IInstantReplayIntercept;
class	IInstantReplayIntercept;
class	INetMessage;
class	INetChannelHandler;
class	INetChannelInfo;
class	INetMessageBinder;
class	INetworkMessageProcessingPreFilter;
class	INetMessageDispatcher;
class	InstantReplayMessage_t;
class	CUtlSlot;

DECLARE_HANDLE_32BIT(CSplitScreenPlayerSlot);
DECLARE_POINTER_HANDLE(NetMessageHandle_t);


#ifndef NET_PACKET_ST_DEFINED
#define NET_PACKET_ST_DEFINED
struct NetPacket_t
{
	ns_address		from;		// sender address
	int				source;		// received source 
	double			received;	// received time
	unsigned char	*data;		// pointer to raw packet data
	bf_read			message;	// easy bitbuf data access
	int				size;		// size in bytes
	int				wiresize;   // size in bytes before decompression
	bool			stream;		// was send as stream
	struct NetPacket_t *pNext;	// for internal use, should be NULL in public
};
#endif // NET_PACKET_ST_DEFINED

enum NetChannelBufType_t
{
	BUF_RELIABLE = 0,
	BUF_UNRELIABLE,
	BUF_VOICE,
};

abstract_class INetChannel : public INetChannelInfo
{
public:
	virtual	~INetChannel( void ) {};

	virtual size_t 	GetTotalPacketBytes( int, int ) const = 0;
	virtual size_t 	GetTotalPacketReliableBytes( int, int ) const = 0;
	virtual void	SetTimeout(float seconds, bool bForceExact = false) = 0;
	virtual void	SetDemoRecorder(IDemoRecorder *recorder) = 0;
	virtual void	SetChallengeNr(unsigned int chnr) = 0;
	
	virtual void	Reset( void ) = 0;
	virtual void	Clear( void ) = 0;
	virtual void	Shutdown(const char *reason) = 0;
	
	virtual void	ProcessPlayback( void ) = 0;
    virtual bool    ProcessStream( void ) = 0;
	virtual void	ProcessPacket( struct netpacket_s* packet, bool bHasHeader ) = 0;
			
	virtual bool	SendNetMsg(INetMessage &msg, bool bForceReliable = false, bool bVoice = false ) = 0;
#ifdef POSIX
	FORCEINLINE bool SendNetMsg(INetMessage const &msg, bool bForceReliable = false, bool bVoice = false ) { return SendNetMsg( *( (INetMessage *) &msg ), bForceReliable, bVoice ); }
#endif
	virtual bool	SendData(bf_write &msg, bool bReliable = true) = 0;
	virtual bool	SendFile(const char *filename, unsigned int transferID, bool bIsReplayDemoFile ) = 0;
	virtual void	DenyFile(const char *filename, unsigned int transferID, bool bIsReplayDemoFile ) = 0;
	virtual void	RequestFile_OLD(const char *filename, unsigned int transferID) = 0;	// get rid of this function when we version the 
	virtual void	SetChoked( void ) = 0;
	virtual bool	Transmit( const char *, bf_write * ) = 0;

	virtual const netadr_t	&GetRemoteAddress( void ) const = 0;
	virtual int				GetDropNumber( void ) const = 0;
	virtual int				GetSocket( void ) const = 0;
	virtual unsigned int	GetChallengeNr( void ) const = 0;
	virtual void			GetSequenceData( int &nOutSequenceNr, int &nInSequenceNr, int &nOutSequenceNrAck ) = 0;
	virtual void			SetSequenceData( int nOutSequenceNr, int nInSequenceNr, int nOutSequenceNrAck ) = 0;
	virtual int		GetNumBitsWritten(NetChannelBufType_t bufferType) const = 0;
	virtual void	SetCompressionMode( bool ) = 0;
	// Max # of payload bytes before we must split/fragment the packet
	virtual void	SetMaxRoutablePayloadSize( int nSplitSize ) = 0;
	virtual int		GetMaxRoutablePayloadSize() = 0;

	// For routing messages to a different handler
	virtual bool	SetActiveChannel( INetChannel *pNewChannel ) = 0;
	virtual void	AttachSplitPlayer( int nSplitPlayerSlot, INetChannel *pChannel ) = 0;
	virtual void	DetachSplitPlayer( int nSplitPlayerSlot ) = 0;

	virtual bool	IsRemoteDisconnected() const = 0;

	virtual bool	WasLastMessageReliable() const = 0; // True if the last (or currently processing) message was sent via the reliable channel

	virtual const unsigned char * GetChannelEncryptionKey() const = 0;	// Returns a buffer with channel encryption key data (network layer determines the buffer size)

	virtual bool	EnqueueVeryLargeAsyncTransfer( INetMessage &msg ) = 0;	// Enqueues a message for a large async transfer
};


#endif // INETCHANNEL_H
