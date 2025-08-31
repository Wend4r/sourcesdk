//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $Workfile:     $
// $Date:         $
//
//-----------------------------------------------------------------------------
// $Log: $
//
// $NoKeywords: $
//=============================================================================//
#if !defined( INETCHANNELINFO_H )
#define INETCHANNELINFO_H
#ifdef _WIN32
#pragma once
#endif

#define FLOW_OUTGOING	0		
#define FLOW_INCOMING	1
#define MAX_FLOWS		2		// in & out

struct netadr_t;
struct SNetChannelLatencyStats;

struct NetChanStat_t
{
	float m_flAvg;
	float m_flMin;
	float m_flMax;
	float m_flStdDev;
};

class INetChannelInfo
{
public:
	virtual const char  *GetName( void ) const = 0;	// get channel name
	virtual const char  *GetAddress( void ) const = 0; // get channel IP address as string

	virtual const netadr_t &GetRemoteAddress( void ) const = 0;

	virtual float		GetTime( void ) const = 0;	// current net time
	virtual float		GetTimeConnected( void ) const = 0;	// get connection time in seconds
	virtual int			GetBufferSize( void ) const = 0;	// netchannel packet history size
	virtual int			GetDataRate( void ) const = 0; // send data rate in byte/sec
	
	virtual bool		IsLocalHost( void ) const = 0;	// true if localhost
	virtual bool		IsLoopback( void ) const = 0;	// true if loopback channel
	virtual bool		IsTimingOut( void ) const = 0;	// true if timing out
	virtual bool		IsPlayback( void ) const = 0;

	virtual float		GetAvgLatency( void ) const = 0;	// average packet latency in seconds
	virtual float		GetEngineLatency( void ) const = 0;	// current latency (RTT), more accurate but jittering
	
	virtual float		GetAvgLoss( int flow ) const = 0;	 // avg packet loss[0..1]
	virtual float		GetAvgChoke( int flow ) const = 0;
	virtual float		GetUnkAvg015( int flow ) const = 0;
	virtual float		GetAvgData( int flow ) const = 0;	 // data flow in bytes/sec
	
	virtual float		GetAvgPackets( int flow ) const = 0; // avg packet bytes/sec
	virtual uint64		GetTotalData( int flow ) const = 0;	 // total flow in/out in bytes
	virtual int			GetTotalPackets( int flow ) const = 0;
	virtual int			GetSequenceNr( int flow ) const = 0;	// last send seq number
	virtual float		GetTimeSinceLastReceived( void ) const = 0;	// get time since last recieved packet in seconds
	virtual void		GetRemoteFramerate( float *pflFrameTime = nullptr, float *pflFrameTimeStdDeviation = nullptr, float *pflFrameStartTimeStdDeviation = nullptr ) const = 0;

	virtual float		GetTimeoutSeconds( void ) const = 0;
	virtual float		GetTimeUntilTimeout( void ) const = 0;
	
	virtual float		GetUnkTime025( void ) const = 0;
	
	virtual void		ResetLatencyStats( int flow ) = 0;
	virtual SNetChannelLatencyStats *GetLatencyStats( int flow ) const = 0;
	virtual void		SetLatencyStats( int flow, const SNetChannelLatencyStats &stats ) = 0;
	
	virtual void		SetInterpolationAmount( float flInterpolationAmount, float flUpdateRate ) = 0;
	virtual void		SetNumPredictionErrors( int num ) = 0;
	virtual void		SetShowNetMessages( bool show ) = 0;

	virtual bool		unk032( int unk1, int unk2, int* unk3 = nullptr ) = 0;

	virtual int			unk033( int unk1, int* unk2, int unk3 ) = 0;
	virtual int			unk034( int unk1, void* unk2 ) = 0;
};

#endif // INETCHANNELINFO_H


