#ifndef SERVERSIDECLIENT_H
#define SERVERSIDECLIENT_H

#if COMPILER_MSVC
#pragma once
#endif

#include <serversideclientbase.h>

class CHLTVServer;

// AMNOTE: Counted per client and added to the server totals on disconnect
struct HltvReplayStats_t
{
	enum FailEnum_t
	{
		FAILURE_ALREADY_IN_REPLAY = 0,
		FAILURE_TOO_FREQUENT,
		FAILURE_NO_FRAME,
		FAILURE_NO_FRAME2,
		FAILURE_CANNOT_MATCH_DELAY,
		FAILURE_FRAME_NOT_READY,
		NUM_FAILURES
	};

	uint32 m_nClients;
	uint32 m_nStartRequests;
	uint32 m_nSuccessfulStarts;
	uint32 m_nStopRequests;
	uint32 m_nAbortStopRequests;
	uint32 m_nUserCancels;
	uint32 m_nFullReplays;
	uint32 m_nNetAbortReplays;
	uint32 m_nFailedReplays[NUM_FAILURES];
}; // sizeof 56
COMPILE_TIME_ASSERT( sizeof( HltvReplayStats_t ) == 56 );

class CServerSideClient : public CServerSideClientBase
{
public:
	struct SendSnapshotJob_HltvReplay_t
	{
		int m_nFrameTick;
	};

	struct SendSnapshotJob_HltvSource_t
	{
		CClientFrame *m_pCurrentFrame;
	};

	struct SendSnapshotJob_Empty_t
	{
	};

	struct StopHltvReplayCall_t
	{
		bool m_bForce;
	};

	struct FakeClientMarkSpawnGroupsAsLoadedCall_t
	{
	};

	struct StartHltvReplayCall_t
	{
		int m_nReplayStopAt;
		int m_nReplayDelay;
		char m_Request[44];
		double m_flRequestTime;
	};

	using ThisClass = CServerSideClient;
	using BaseClass = CServerSideClientBase;

	DECLARE_DELAYED_CALL( StopHltvReplayDelayedCall_t, StopHltvReplayCall_t, empty_t );
	DECLARE_DELAYED_CALL( FakeClientMarkSpawnGroupsAsLoadedDelayedCall_t, FakeClientMarkSpawnGroupsAsLoadedCall_t, empty_t );
	DECLARE_DELAYED_CALL( StartHltvReplayDelayedCall_t, StartHltvReplayCall_t, empty_t );

	class CSendJob_HltvReplay : public CAsyncCallJob< CServerSideClient, SendSnapshotJob_HltvReplay_t >
	{
	public:
		virtual ~CSendJob_HltvReplay() override = 0;
		virtual void DoExecute() override = 0;
		virtual void BeforeJobRuns() override = 0;
	};

	class CSendJob_HltvSource : public CAsyncCallJob< CServerSideClient, SendSnapshotJob_HltvSource_t >
	{
	public:
		virtual ~CSendJob_HltvSource() override = 0;
		virtual void DoExecute() override = 0;
		virtual void BeforeJobRuns() override = 0;
	};

	class CSendJob_Empty : public CAsyncCallJob< CServerSideClient, SendSnapshotJob_Empty_t >
	{
	public:
		virtual ~CSendJob_Empty() override = 0;
		virtual void DoExecute() override = 0;
		virtual void BeforeJobRuns() override = 0;
	};

	class CSendJob_Regular : public CThreadedJobWithDependencies
	{
	public:
		virtual ~CSendJob_Regular() override = 0;
		virtual void DoExecute() override = 0;
		virtual bool IsJobType( uint32 nJobType ) override = 0;
		virtual void BeforeJobRuns() override = 0;

	public:
		CServerSideClient *m_pClient;
		CClientFrame *m_pDeltaFrame;
		CClientFrame *m_pCurrentFrame;
		int m_nCurrentFrameTick;
		bf_write m_SnapshotBuffer;
		void *m_pSnapshotBufferMemory;
		int m_nMaxAckTick;
		bool m_bSnapshotPrepared;
	};

public:
	virtual ~CServerSideClient() override = 0;

	virtual void Connect( int socket, const char *pszName, int nUserID, INetChannel *pNetChannel, uint8 nConnectionTypeFlags, uint32 uChallengeNumber ) override = 0;
	virtual void Inactivate( const char *pszAddons ) override = 0;
	virtual void Reactivate( CPlayerSlot nSlot ) override = 0;
	virtual void Reconnect() override = 0;
	virtual void Disconnect( ENetworkDisconnectionReason reason, const char *pszInternalReason ) override = 0;
	virtual bool CheckConnect() override = 0;
	virtual void SetRate( int nRate ) override = 0;
	virtual void Clear() override = 0;
	virtual bool ExecuteStringCommand( const CNETMsg_StringCmd_t &msg ) override = 0;
	virtual bool SendNetMessage( const CNetMessage *pData, NetChannelBufType_t bufType = BUF_DEFAULT ) override = 0;
	virtual bool IsHearingClient( CPlayerSlot nSlot ) const override = 0;
	virtual bool IsProximityHearingClient() const override = 0;
	virtual bool ApplyConVars( const CMsg_CVars &list ) override = 0;
	virtual bool ProcessSpawnGroup_LoadCompleted( const CNETMsg_SpawnGroup_LoadCompleted_t &msg ) override = 0;
	virtual bool ProcessClientInfo( const CCLCMsg_ClientInfo_t &msg ) override = 0;
	virtual bool ProcessCmdKeyValues( const CCLCMsg_CmdKeyValues_t &msg ) override = 0;
	virtual bool ProcessDiagnostic( const CCLCMsg_Diagnostic_t &msg ) override = 0;
	virtual bool ProcessMove( const CCLCMsg_Move_t &msg ) override = 0;
	virtual bool ProcessVoiceData( const CCLCMsg_VoiceData_t &msg ) override = 0;
	virtual bool ProcessRespondCvarValue( const CCLCMsg_RespondCvarValue_t &msg ) override = 0;
	virtual bool ProcessPacketStart( const NetMessagePacketStart_t &msg ) override = 0;
	virtual bool ProcessPacketEnd( const NetMessagePacketEnd_t &msg ) override = 0;
	virtual bool ProcessConnectionClosed( const NetMessageConnectionClosed_t &msg ) override = 0;
	virtual bool ProcessConnectionCrashed( const NetMessageConnectionCrashed_t &msg ) override = 0;
	virtual bool UnusedVirtual45() override = 0;
	virtual bool UnusedVirtual46() override = 0;
	virtual bool OnDisconnectForwarding() override = 0;
	virtual void BuildServerInfoMessage() override = 0;
	virtual void PerformPrespawn() override = 0;
	virtual bool UpdateAcknowledgedFramecount( int tick ) override = 0;
	virtual bool ShouldSendMessages() override = 0;
	virtual void UpdateUserSettings() override = 0;
	virtual void SendSnapshot() override = 0;
	virtual void SendSignonData() override = 0;
	virtual void SpawnPlayer() override = 0;
	virtual void ActivatePlayer() override = 0;
	virtual void ShouldReceiveStringTableUserData() override = 0;
	virtual bool StartHltvReplay() override = 0;
	virtual void StopHltvReplay() override = 0;
	virtual int GetHltvLastSendTick() override = 0;
	virtual bool CanStartHltvReplay() override = 0;
	virtual void ResetHltvReplayRequestTime() override = 0;
	virtual const char *GetHltvReplayStats() override = 0;
	virtual void Await() override = 0;
	virtual void OnSendJobFinished() override = 0;
	virtual void OnSendJobStarted() override = 0;
	virtual bool ProcessSignonStateMsg( SignonState_t state, int nSpawnCount ) override = 0;
	virtual void PerformDisconnection( ENetworkDisconnectionReason reason ) override = 0;

	void ExecuteDelayedCall( StopHltvReplayCall_t &call );
	void ExecuteDelayedCall( FakeClientMarkSpawnGroupsAsLoadedCall_t &call );
	void ExecuteDelayedCall( StartHltvReplayCall_t &call );

public:
	CPlayerBitVec m_VoiceStreams;
	CPlayerBitVec m_VoiceProximity;
	CCheckTransmitInfo m_PackInfo;
	CClientFrameManager m_FrameManager;
	void *m_pUnk0EC8;
	CClientFrame *m_pCurrentFrame;

	// AMNOTE: Move messages in the current packet, the client is disconnected when there's more than one
	int m_nMoveMessagesInPacket = 0;

	// AMNOTE: Server time of ActivatePlayer, -1 before it. Snapshots keep more frames for sv_extra_client_connect_time after it
	float m_flTimeClientBecameFullyConnected = -1.0f;
	bool m_bVoiceLoopback = false;

	// AMNOTE: The HLTV replay plays a stash instead of the HLTV server's frames
	bool m_bHltvReplayFromStash = false;

	// AMNOTE: The game sends the client another player's view with a delay (sv_maxreplay)
	bool m_bIsInReplayMode = false;
	int m_nHltvReplayDelay = 0;
	CHLTVServer *m_pHltvReplayServer;

	// AMNOTE: Ticks the HLTV replay stops, starts, and slows down between, sent in CSVCMsg_HltvReplay
	int m_nHltvReplayStopAt;
	int m_nHltvReplayStartAt;
	int m_nHltvReplaySlowdownBeginAt;
	int m_nHltvReplaySlowdownEndAt;
	float m_flHltvReplaySlowdownRate;

	// AMNOTE: Tick of the last HLTV replay frame sent
	int m_nHltvLastSendTick;
	double m_flHltvLastReplayRequestTime;
	CUtlVector< INetMessage * > m_HltvQueuedMessages;
	HltvReplayStats_t m_HltvReplayStats;
	CThreadedJobWithDependencies *m_pSendJob;
};

#endif // SERVERSIDECLIENT_H
