#ifndef HLTVCLIENT_H
#define HLTVCLIENT_H

#if COMPILER_MSVC
#pragma once
#endif

#include <serversideclientbase.h>

class CHLTVClient : public CServerSideClientBase
{
public:
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
	virtual void BuildServerInfoMessage() override = 0;
	virtual void PerformPrespawn() override = 0;
	virtual bool UpdateAcknowledgedFramecount( int tick ) override = 0;
	virtual bool ShouldSendMessages() override = 0;
	virtual void UpdateUserSettings() override = 0;
	virtual void SendSnapshot() override = 0;
	virtual void SendSignonData() override = 0;
	virtual void SpawnPlayer() override = 0;
	virtual void FillServerInfo( void *pInfo ) override = 0;
	virtual void Await() override = 0;
	virtual bool ProcessSignonStateMsg( int state ) override = 0;

public:
	CNetworkGameServerBase *m_pHLTV;
	CUtlString m_szPassword;
	CUtlString m_szChatGroup;
	double m_fLastSendTime = 0.0;
	double m_flLastChatTime = 0.0;
	int m_nLastSendTick = 0;
	int m_nSpawnGroupsSent = 0;
	int m_bNeedsFullFrame = 0;
	int m_nFullFrameTick = 0;
	int m_nUnk0C00 = 0;
	bool m_bNoChat = false;
	bool m_bUnk0C05 = false;
	bool m_bUnk0C06 = false;
	void *m_pJob;
	CUtlVector< INetMessage * > m_QueuedMessages;
};

#endif // HLTVCLIENT_H
