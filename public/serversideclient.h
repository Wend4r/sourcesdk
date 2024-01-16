#ifndef SERVERSIDECLIENT_H
#define SERVERSIDECLIENT_H

#if COMPILER_MSVC
#pragma once
#endif

class CServerSideClient
{
public:
	virtual ~CServerSideClient() = 0;

	INetChannel* GetNetChannel() const { return m_NetChannel; };
	CPlayerSlot GetPlayerSlot() const { return m_nClientSlot; };
	CPlayerUserId GetUserID() const { return m_UserID; };
	int GetSignonState() const { return m_nSignonState; }
	CSteamID GetClientSteamID() const { return m_SteamID; }
	const char* GetClientName() const { return m_Name; };

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
	CPlayerUserId m_UserID; // 152 | 168
	[[maybe_unused]] char pad5[0x1];
	CSteamID m_SteamID; // 155 | 171
	[[maybe_unused]] char pad6[0x15];
	CPlayerSlot m_nClientSlot; // 184 | 200
	[[maybe_unused]] char pad7[0x4];
	CUtlString m_Name; // 192 | 208
};

#endif // SERVERSIDECLIENT_H