#ifndef SERVERSIDECLIENT_H
#define SERVERSIDECLIENT_H

#if COMPILER_MSVC
#pragma once
#endif

#include <playerslot.h>
#include <playeruserid.h>
// #include <protocol.h> // @Wend4r: use <netmessages.pb.h> instead.
#include <entity2/entityidentity.h>
#include <steam/steamclientpublic.h>
#include <engine/clientframe.h>
#include <tier0/utlstring.h>
#include <tier1/ns_address.h>
#include <networksystem/inetworksystem.h>
#include <netmessages.pb.h>

class CHLTVServer;
class INetMessage;
class CNetworkGameServerBase;

struct HltvReplayStats_t {
	enum FailEnum_t {
		FAILURE_ALREADY_IN_REPLAY,
		FAILURE_TOO_FREQUENT,
		FAILURE_NO_FRAME,
		FAILURE_NO_FRAME2,
		FAILURE_CANNOT_MATCH_DELAY,
		FAILURE_FRAME_NOT_READY,
		NUM_FAILURES
	};

	uint nClients;
	uint nStartRequests;
	uint nSuccessfulStarts;
	uint nStopRequests;
	uint nAbortStopRequests;
	uint nUserCancels;
	uint nFullReplays;
	uint nNetAbortReplays;
	uint nFailedReplays[NUM_FAILURES];
}; // sizeof 56


struct Spike_t {
public:
	CUtlString m_szDesc;
	int m_nBits;
}; // sizeof 16

class CNetworkStatTrace {
public:
	CUtlVector<Spike_t> m_Records;
	int m_nMinWarningBytes;
	int m_nStartBit;
	int m_nCurBit;
}; // sizeof 40

// class CServerSideClientBase: CUtlSlot, INetworkChannelNotify, INetworkMessageProcessingPreFilter;
class CServerSideClientBase
{
	virtual ~CServerSideClientBase() = 0;
public:
	INetChannel* GetNetChannel() const { return m_NetChannel; };
	CPlayerSlot GetPlayerSlot() const { return m_nClientSlot; };
	CEntityIndex GetEntityIndex() const { return m_nEntityIndex; };
	CPlayerUserId GetUserID() const { return m_UserID; };
	int GetSignonState() const { return m_nSignonState; }
	CSteamID GetClientSteamID() const { return m_SteamID; }
	const char* GetClientName() const { return m_Name; };
	bool IsConnected() const { return m_nSignonState >= SIGNONSTATE_CONNECTED; };
	bool IsSpawned() const { return m_nSignonState >= SIGNONSTATE_NEW; };
	bool IsActive() const { return m_nSignonState == SIGNONSTATE_FULL; };
	bool IsFakeClient() const { return m_bFakePlayer; };
	bool IsHLTV() const { return m_bIsHLTV; }
	bool IsFullyAuthenticated() { return m_bFullyAuthenticated; }
	const netadr_t* GetRemoteAddress() const { return m_nAddr.Get<netadr_t>(); }
	void ForceFullUpdate() { m_nForceWaitForTick = m_nDeltaTick = -1; }
public:
	[[maybe_unused]] void* m_pVT1; // INetworkMessageProcessingPreFilter
	CUtlString m_unk16; // 16
	[[maybe_unused]] char pad24[0x16]; // 24
#ifdef __linux__
	[[maybe_unused]] char pad46[0x10]; // 46
#endif
	void (*RebroadcastSource)(int msgID); // 64
	CUtlString m_UserIDString; // 72
	CUtlString m_Name; // 80
	CPlayerSlot m_nClientSlot; // 88
	CEntityIndex m_nEntityIndex; // 92
	CNetworkGameServerBase* m_Server; // 96
	INetChannel* m_NetChannel; // 104
	uint8 m_nUnkVariable; // 112
	bool m_bMarkedToKick; // 113
	int32 m_nSignonState; // 116
	bool m_bSplitScreenUser; // 120
	bool m_bSplitAllowFastDisconnect; // 121
	int m_nSplitScreenPlayerSlot; // 124
	CServerSideClientBase* m_SplitScreenUsers[4]; // 128
	CServerSideClientBase* m_pAttachedTo; // 160
	bool m_bSplitPlayerDisconnecting; // 168
	int m_UnkVariable172; // 172
	bool m_bFakePlayer; // 176
	bool m_bSendingSnapshot; // 177
	[[maybe_unused]] char pad6[0x5];
	CPlayerUserId m_UserID; // 184
	bool m_bReceivedPacket; // 186
	CSteamID m_SteamID; // 187
	CSteamID m_UnkSteamID; // 195
	CSteamID m_UnkSteamID2; // 203 from auth ticket
	CSteamID m_nFriendsID; // 211
	ns_address m_nAddr; // 220
	ns_address m_nAddr2; // 252
	KeyValues* m_ConVars; // 288
	bool m_bConVarsChanged; // 296
	bool m_bConVarsInited; // 297
	bool m_bIsHLTV; // 298
	bool m_bIsReplay; // 299
	[[maybe_unused]] char pad29[0xA];
	uint32 m_nSendtableCRC; // 312
	int m_ClientPlatform; // 316
	int m_nSignonTick; // 320
	int m_nDeltaTick; // 324
	int m_UnkVariable3; // 328
	int m_nStringTableAckTick; // 332
	int m_UnkVariable4; // 336
	CFrameSnapshot* m_pLastSnapshot; // 344
	CUtlVector<void*> m_vecLoadedSpawnGroups; // 352
	CMsgPlayerInfo m_playerInfo; // 376
	CFrameSnapshot* m_pBaseline; // 432
	int m_nBaselineUpdateTick; // 440
	CBitVec<MAX_EDICTS>	m_BaselinesSent; // 444
	int m_nBaselineUsed; // 2492
	int m_nLoadingProgress; // 2496
	int m_nForceWaitForTick; // 2500
	bool m_bLowViolence; // 2504
	bool m_bSomethingWithAddressType; // 2505
	bool m_bFullyAuthenticated; // 2506
	bool m_bUnkBool2507; // 2507
	float m_fNextMessageTime; // 2508
	float m_fSnapshotInterval; // 2512
	float m_fAuthenticatedTime; // 2516
	[[maybe_unused]] char pad168[0x124]; // 2520
	[[maybe_unused]] char pad1658[0x24]; // 2816 something in CServerSideClientBase::ExecuteStringCommand
	CNetworkStatTrace m_Trace; // 2848
	int m_spamCommandsCount; // 2888 if the value is greater than 16, the player will be kicked with reason 39
	double m_lastExecutedCommand; // 2896 if command executed more than once per second, ++m_spamCommandCount
};

class CServerSideClient : public CServerSideClientBase
{
public:
	virtual ~CServerSideClient() = 0;
public:
	CPlayerBitVec m_VoiceStreams; // 2904
	CPlayerBitVec m_VoiceProximity; // 2912
	CCheckTransmitInfo m_PackInfo; // 2920
	CClientFrameManager m_FrameManager; // 3520
	CClientFrame* m_pCurrentFrame; // 3808
	float m_flLastClientCommandQuotaStart; // 3816
	float m_flTimeClientBecameFullyConnected; // 3820
	bool m_bVoiceLoopback; // 3824
	int m_nHltvReplayDelay; // 3828
	CHLTVServer* m_pHltvReplayServer; // 3832
	int m_nHltvReplayStopAt; // 3840
	int m_nHltvReplayStartAt; // 3844
	int m_nHltvReplaySlowdownBeginAt; // 3848
	int m_nHltvReplaySlowdownEndAt; // 3852
	float m_flHltvReplaySlowdownRate; // 3856
	int m_nHltvLastSendTick; // 3860
	float m_flHltvLastReplayRequestTime; // 3864
	CUtlVector<INetMessage*> m_HltvQueuedMessages; // 3872
	HltvReplayStats_t m_HltvReplayStats; // 3896
	void* m_pLastJob; // 3952
};

// not full class reversed
class CHLTVClient : public CServerSideClientBase
{
public:
	virtual ~CHLTVClient() = 0;

	CNetworkGameServerBase* m_pHLTV; // 2904
	CUtlString m_szPassword; // 2912
	CUtlString m_szChatGroup; // 2920
	double m_fLastSendTime; // 2928
	double m_flLastChatTime; // 2936
	int m_nLastSendTick; // 2944
	[[maybe_unused]] char pad2948[0x4]; // 2948
	int m_nFullFrameTime; // 2952
	[[maybe_unused]] char pad2956[0x4]; // 2956
	[[maybe_unused]] char pad2960[0x4]; // 2960
	bool m_bNoChat; // 2964
	bool m_bUnkBool; // 2965
	bool m_bUnkBool2; // 2966
	bool m_bUnkBool3; // 2967
}; // sizeof 3008

#endif // SERVERSIDECLIENT_H