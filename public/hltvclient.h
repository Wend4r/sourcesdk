#ifndef HLTVCLIENT_H
#define HLTVCLIENT_H

#if COMPILER_MSVC
#pragma once
#endif

#include <serversideclientbase.h>

class CHLTVClient : public CServerSideClientBase
{
public:
	// AMNOTE: Named by what it holds: a message the HLTV client sends once the HLTV frame reaches m_nTick
	struct QueuedMessage_t
	{
		CNetMessage *m_pMessage;
		int m_nTick;
	};

public:
	// AMNOTE: The overrides below are those of the Linux table; on Windows the identical-code folding merges
	// ProcessVoiceData, ProcessRespondCvarValue and ProcessPacketEnd with the base
	virtual ~CHLTVClient() override = 0;

	virtual void Inactivate( const char *pszAddons ) override = 0;
	virtual void SetRate( int nRate ) override = 0;
	virtual void SetUpdateRate( float fUpdateRate ) override = 0;
	virtual void Clear() override = 0;
	virtual bool ExecuteStringCommand( const CNETMsg_StringCmd_t &msg ) override = 0;
	virtual bool ProcessSetConVar( const CNETMsg_SetConVar_t &msg ) override = 0;
	virtual bool ProcessSpawnGroup_LoadCompleted( const CNETMsg_SpawnGroup_LoadCompleted_t &msg ) override = 0;
	virtual bool ProcessClientInfo( const CCLCMsg_ClientInfo_t &msg ) override = 0;
	virtual bool ProcessMove( const CCLCMsg_Move_t &msg ) override = 0;
	virtual bool ProcessVoiceData( const CCLCMsg_VoiceData_t &msg ) override = 0;
	virtual bool ProcessRespondCvarValue( const CCLCMsg_RespondCvarValue_t &msg ) override = 0;
	virtual bool ProcessPacketStart( const NetMessagePacketStart_t &msg ) override = 0;
	virtual bool ProcessPacketEnd( const NetMessagePacketEnd_t &msg ) override = 0;
	virtual bool ProcessConnectionClosed( const NetMessageConnectionClosed_t &msg ) override = 0;
	virtual bool ProcessConnectionCrashed( const NetMessageConnectionCrashed_t &msg ) override = 0;
	virtual void BuildServerInfoMessage( CUtlVector< CNetworkServerSpawnGroup * > &vecInitialSpawnGroups ) override = 0;
	virtual void SendInitialSpawnGroups( const CUtlVector< CNetworkServerSpawnGroup * > &vecSpawnGroups, bool bAsync ) override = 0;
	virtual bool UpdateAcknowledgedFramecount( int tick ) override = 0;
	virtual bool ShouldSendMessages() override = 0;
	virtual void UpdateUserSettings() override = 0;
	virtual CClientFrame *GetDeltaFrame( int nTick ) override = 0;
	virtual void SendSignonData() override = 0;
	virtual void SpawnPlayer() override = 0;
	virtual void FillCommandRecvStatus( void *pStatus ) override = 0;
	virtual void Await() override = 0;
	virtual bool ProcessSignonStateMsg( SignonState_t state, int nSpawnCount ) override = 0;

public:
	CNetworkGameServerBase *m_pHLTV;
	CUtlString m_szPassword;
	CUtlString m_szChatGroup;
	double m_fLastSendTime = 0.0;
	double m_flLastChatTime = 0.0;
	int m_nLastSendTick = 0;
	int m_nSpawnGroupsSent = 0;

	// AMNOTE: Named by what it does: HLTV tick of the next full frame. CHLTVServer::SendClientMessages sets it
	// to the current tick plus a delay when it's 0, UpdateAcknowledgedFramecount sets 1 to rebuild at once
	int m_nNextFullFrameTick = 0;

	// AMNOTE: Tick of the full frame sent, cleared when the client acknowledges it
	int m_nFullFrameTick = 0;

	// AMNOTE: CCLCMsg_Move::last_command_number of the last move message, reported by FillCommandRecvStatus
	int m_nLastCommandNumber = 0;
	bool m_bNoChat = false; // tv_nochat

	// AMNOTE: Named by what it does: the server sets it for the clients with a given SteamID,
	// ExecuteStringCommand drops their chat while it's set
	bool m_bChatBlocked = false;

	// AMNOTE: Named by what it does: set by CHLTVServer::SendClientMessages when the full frame is due,
	// the snapshot then goes out as a full frame
	bool m_bSendFullFrame = false;
	CThreadedJobWithDependencies *m_pJob;
	CUtlVector< QueuedMessage_t > m_QueuedMessages;
};

#endif // HLTVCLIENT_H
