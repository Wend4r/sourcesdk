#ifndef SERVERSIDECLIENT_H
#define SERVERSIDECLIENT_H

#if COMPILER_MSVC
#pragma once
#endif

#include "inetchannel.h"
#include "playerslot.h"
#include "playeruserid.h"
#include "protocol.h"
#include "entity2/entityidentity.h"
#include "steam/steamclientpublic.h"
#include "tier0/utlstring.h"

// class CServerSideClientBase: CUtlSlot, INetworkChannelNotify, INetworkMessageProcessingPreFilter;
#pragma pack(push,1)
class CServerSideClientBase
{
	virtual void unk_00() = 0;
public:
	virtual ~CServerSideClientBase() = 0;
	virtual void Connect(uint32_t a2, const char* szName, int nUserID, INetChannel* pNetChannel, bool bFakePlayer, bool bSplitClient, int a8) = 0;
private:
	virtual void unk_03() = 0;
	virtual void unk_04() = 0;
	virtual void unk_05() = 0;
	virtual void unk_06() = 0;
public:
	virtual void Disconnect(/* ENetworkDisconnectionReason */ int reason) = 0;
private:
	virtual void unk_08() = 0;
	virtual void unk_09() = 0;
	virtual void unk_10() = 0;
	virtual void unk_11() = 0;
	virtual void unk_12() = 0;
	virtual void unk_13() = 0;
	virtual void unk_14() = 0;
	virtual void unk_15() = 0;
	virtual void unk_16() = 0;
public:
	virtual void ClientPrintf(const char*, ...) = 0;
	virtual bool IsFakeClient() = 0;

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
	bool IsFullyAuthenticated() { return m_bFullyAuthenticated; }

private:
	[[maybe_unused]] void* m_pVT1; // INetworkMessageProcessingPreFilter
	[[maybe_unused]] char pad1[0x40];
#ifdef __linux__
	[[maybe_unused]] char pad2[0x10];
#endif
	INetChannel* m_NetChannel; // 80 | 96
	[[maybe_unused]] char pad3[0x4];
	int32 m_nSignonState; // 92 | 108
	[[maybe_unused]] char pad4[0x38];
	bool m_bFakePlayer; // 152 | 168
	[[maybe_unused]] char pad5[0x7];
	CPlayerUserId m_UserID; // 160 | 176
	[[maybe_unused]] char pad6[0x1];
	CSteamID m_SteamID; // 163 | 179
	[[maybe_unused]] char pad7[0x25];
	CPlayerSlot m_nClientSlot; // 208 | 224
	CEntityIndex m_nEntityIndex; // 212 | 228
	CUtlString m_Name; // 216 | 232
	[[maybe_unused]] char pad8[0x8F2];
	bool m_bFullyAuthenticated; // 2514 | 2530
};
#pragma pack(pop)

// class CServerSideClient: public CServerSideClientBase, CUtlSlot, INetworkChannelNotify, INetworkMessageProcessingPreFilter
class CServerSideClient : public CServerSideClientBase
{
public:
	virtual ~CServerSideClient() = 0;
};

#endif // SERVERSIDECLIENT_H