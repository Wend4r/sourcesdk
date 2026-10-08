//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: Client-side networking interfaces reversed from CS2 engine2.dll.
//          Mirrors iserver.h (INetworkGameServer / CNetworkGameServerBase).
//
//=============================================================================//

#ifndef ICLIENT_H
#define ICLIENT_H
#ifdef _WIN32
#pragma once
#endif

#include <engine/IEngineService.h>
#include <engine/inetworkclientservice.h>
#include <playerslot.h>
#include <ns_address.h>

class INetChannel;
class CNetworkGameClient;
class CClientChangelevelState;
class CGlobalVars;
class IGameSpawnGroupMgr;

//-----------------------------------------------------------------------------
// Purpose: Engine-facing client interface. Implemented by CNetworkGameClientBase.
// Vtable indices are absolute (primary vtable of CNetworkGameClient, engine2.dll).
//-----------------------------------------------------------------------------
abstract_class INetworkGameClient
{
public:
	virtual void *unk000() = 0;
	virtual void SetGameSpawnGroupMgr(IGameSpawnGroupMgr *pMgr) = 0;
	virtual int AddRef() = 0;
	virtual int Release() = 0;

	// Returns &m_GlobalVars (embedded CGlobalVars). Client-side globals getter.
	virtual CGlobalVars *GetGlobals() = 0;

	virtual int GetClientTickCount() = 0;
	virtual int GetServerTickCount() = 0;
	virtual void *unk007() = 0;
	virtual void *unk008() = 0;
	virtual void *unk009() = 0;
	virtual void *unk010() = 0;
	virtual void SetFrameTime(float flFrameTime) = 0;
	virtual void Connect(int nSplitScreenSlot, int nUnk, const void *pAddr) = 0;
	virtual void Disconnect(/*ENetworkDisconnectionReason*/ int reason) = 0;
	virtual void *unk014() = 0;
	virtual void *unk015() = 0;
	virtual void *unk016() = 0;
	virtual void *unk017() = 0;
	virtual void *unk018() = 0;
	virtual void *unk019() = 0;
	virtual void *unk020() = 0;
	virtual void *unk021() = 0;
	virtual void ClientPollNetworking(const void *pEvent) = 0;
	virtual void ClientProcessNetworking(const void *pEvent) = 0;
	virtual void *unk024() = 0;
	virtual void OnClientFrameSimulate(const void *pEvent) = 0;
	virtual void OnClientAdvanceTick(const void *pEvent) = 0;
	virtual void *unk027() = 0;
	virtual void *unk028() = 0;
	virtual void *unk029() = 0;
	virtual void *unk030() = 0;
	virtual void *unk031() = 0;
	virtual void ForceFullUpdate(const char *pszReason) = 0;
	virtual void *unk033() = 0;
	virtual void *unk034() = 0;
	virtual void *unk035() = 0;
	virtual void ServerCmd(int nCommandSrc, const char *pszCmd) = 0;
	virtual void *unk037() = 0;
	virtual void SendStringCmd(int nSplitScreenSlot, const char *pszCmd) = 0;
	virtual void SplitScreenConnect(int nSplitScreenSlot) = 0;
	virtual int GetMaxSplitScreenPlayers() = 0;
	virtual void *unk041() = 0;
	virtual void *unk042() = 0;
	virtual void *unk043() = 0;
	virtual void *unk044() = 0;
	virtual void *unk045() = 0;
	virtual void PrepareSteamConnectResponse(uint64 nUnk, bool bUnk, const void *pAddr, void *pMsg) = 0;
};

//-----------------------------------------------------------------------------
// Purpose: Concrete client base. Adds the network message Process* handlers,
// clock-drift / prediction internals and the sign-on state machine.
// (Split point between the interface and the base impl is approximate.)
//-----------------------------------------------------------------------------
class CNetworkGameClientBase : public INetworkGameClient
{
public:
	virtual void *unk047() = 0;
	virtual void *unk048() = 0;
	virtual void *unk049() = 0;
	virtual void *unk050() = 0;
	virtual void *unk051() = 0;
	virtual void *unk052() = 0;
	virtual void SpewSerializer(const char *pszName) = 0;
	virtual void *unk054() = 0;
	virtual void *unk055() = 0;
	virtual void *unk056() = 0;
	virtual void *StartChangeLevel() = 0; // 57
	virtual void FinishChangeLevel(void *pState) = 0;
	virtual void *unk059() = 0;
	virtual void *unk060() = 0;
	virtual void *unk061() = 0;
	virtual void *unk062() = 0;
	virtual void TransmitNetChannel() = 0;
	virtual void *unk064() = 0;
	virtual void *unk065() = 0;
	virtual void *unk066() = 0;
	virtual void *unk067() = 0;
	virtual void *unk068() = 0;
	virtual void *unk069() = 0;
	virtual void *unk070() = 0;
	virtual void *unk071() = 0;
	virtual void *unk072() = 0;
	virtual void ApplyClockDrift(int nServerTick, int nClientTick) = 0;
	virtual void *unk074() = 0;
	virtual void *unk075() = 0;
	virtual void *unk076() = 0;
	virtual void *unk077() = 0;
	virtual void *unk078() = 0;
	virtual void *unk079() = 0;
	virtual void *unk080() = 0;
	virtual void *unk081() = 0;
	virtual void *unk082() = 0;
	virtual void *unk083() = 0;
	virtual void *unk084() = 0;
	virtual void *unk085() = 0;
	virtual void *unk086() = 0;
	virtual void *unk087() = 0;
	virtual void ProcessTick(const void *msg) = 0;
	virtual void ProcessStringCmd(const void *msg) = 0;
	virtual void ProcessSetConVar(const void *msg) = 0;
	virtual void ProcessSignonState(const void *msg) = 0;
	virtual void *unk092() = 0;
	virtual void ProcessSpawnGroup_Load(const void *msg) = 0;
	virtual void ProcessSpawnGroup_ManifestUpdate(const void *msg) = 0;
	virtual void ProcessSpawnGroup_Unload(const void *msg) = 0;
	virtual void ProcessSpawnGroup_SetCreationTick(const void *msg) = 0;
	virtual void *unk097() = 0;
	virtual void *unk098() = 0;
	virtual void *unk099() = 0;
	virtual void *unk100() = 0;
	virtual void ProcessServerInfo(const void *msg) = 0;
	virtual void ProcessClassInfo(const void *msg) = 0;
	virtual void *unk103() = 0;
	virtual void *unk104() = 0;
	virtual void *unk105() = 0;
	virtual void *unk106() = 0;
	virtual void *unk107() = 0;
	virtual void ProcessCreateStringTable(const void *msg) = 0;
	virtual void ProcessUpdateStringTable(const void *msg) = 0;
	virtual void *unk110() = 0;
	virtual void *unk111() = 0;
	virtual void ProcessSplitScreen(const void *msg) = 0;
	virtual void ProcessCmdKeyValues(const void *msg) = 0;
	virtual void *unk114() = 0;
	virtual void *unk115() = 0;
	virtual void *unk116() = 0;
	virtual void *unk117() = 0;
	virtual void *unk118() = 0;
	virtual void *unk119() = 0;
	virtual void *unk120() = 0;
	virtual void *unk121() = 0;
	virtual void *unk122() = 0;
	virtual void *unk123() = 0;
	virtual void *unk124() = 0;
	virtual void *unk125() = 0;
	virtual void *unk126() = 0;
	virtual void ProcessHltvReplay(const void *msg) = 0;
	virtual void *unk128() = 0;
	virtual void *unk129() = 0;
	virtual void Clear(bool bUnk) = 0;
	virtual void FullConnect(const void *pAddr) = 0;
	virtual void SetSignonState(/*SignonState_t*/ int state, int nUnk, const void *pMsg) = 0;
	virtual void *unk133() = 0;
	virtual void *unk134() = 0;
	virtual void InstallStringTableCallbacks(const char *pszTableName) = 0;
	virtual void *unk136() = 0;
	virtual void ProcessPacketEntities(const void *msg) = 0;
	virtual void OnPreserveEntity(void *pReadInfo) = 0;
	virtual void CopyNewEntity(void *pReport, void *pReadInfo, void *pClientClass, int nUnk, unsigned int nUnk2) = 0;
	virtual void CopyExistingEntity(void *pReport, void *pReadInfo) = 0;
	virtual void OnReceivedUncompressedPacket() = 0;
	virtual void *unk142() = 0;
};

// The most-derived client class. Overrides a handful of base virtuals but
// adds no new vtable slots, so it is layout-compatible with the base.
class CNetworkGameClient : public CNetworkGameClientBase
{
private:
	char pad_0[ 200 ];
public:
	bool m_bInSimulation;

private:
	char pad_1[ 47 ];

public:
	bool m_bAllowClientSidePredict;

private:
	char pad_2[ 331 ];

public:
	int32 m_nDeltaTick;

private:
	char pad_3[ 296 ];

public:
	int32 m_nClientTick;
	int32 m_nServerTick;

};

#endif // ICLIENT_H
