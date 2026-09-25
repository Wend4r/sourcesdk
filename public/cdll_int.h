//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: Interfaces between the client.dll and engine
//
//===========================================================================//

#ifndef CDLL_INT_H
#define CDLL_INT_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "isource2engine.h"
#include <steam/steamuniverse.h>

//-----------------------------------------------------------------------------
// forward declarations
//-----------------------------------------------------------------------------
class CMovieRecorder;
class IDemoFile;

//-----------------------------------------------------------------------------
// Purpose: Interface the engine exposes to the client DLL (Source2EngineToClient001).
// Backed by CEngineClient in engine2.dll; own methods occupy vtable slots 18-184.
// Unnamed slots (unkNNN) are not yet reversed for this build.
//-----------------------------------------------------------------------------
abstract_class IVEngineClient2 : public ISource2Engine
{
public:
	virtual EUniverse GetSteamUniverse() const = 0;
	virtual int &GetPlayerSlotByNetworkIDString(int &nSlot, const char *pszNetworkID) = 0;
	virtual const char *GetPlayerNetworkIDString(int nSlot) = 0;
	virtual int &GetLocalPlayer(int &nSlot, bool bUnk = false) = 0;
	virtual int &GetLastValidPlayerSlot(int &nSlot, bool bUnk = false) = 0;
	virtual bool IsPlayerSlotActive(int nSlot) = 0;
	virtual float &GetFrameTime() = 0;
	virtual void SetFrameTimeAmnesty(const char *pszReason, int nFrames, float flDuration) = 0;
	virtual const char *GetFrameTimeAmnesty(bool bCheckCvar) = 0;
	virtual void *unk027() = 0;
	virtual void PrintVProfLiteReport(void *pReport, bool bDetailed, int nLogChannel) = 0;
	virtual void DumpNetStats(void *pNetStatData, void (*pfnOutput)(const char *)) = 0;
	virtual void *GetNetChannelInfo() = 0;
	virtual uint32 GetLongFrameCount() = 0;
	virtual bool GetPlayerInfo(int nPlayerIndex, void *pInfo) = 0;
	virtual unsigned short &GetPlayerUserId(unsigned short &userid, int nPlayerIndex) = 0;
	virtual int &GetSplitScreenPlayer(int &nPlayerSlot, int nSplitScreenSlot) = 0;
	virtual int &GetSplitScreenSlotForPlayer(int &nSplitScreenSlot, int nPlayerSlot) = 0;
	virtual float GetLastTimeStamp() = 0;
	virtual int GetLastServerTick() = 0;
	virtual int GetMaxClients() = 0;
	virtual bool IsInGame() = 0;
	virtual bool IsConnected() = 0;
	virtual void *GetNetChannel(int nSplitScreenSlot) = 0;
	virtual bool IsPlayingDemo() = 0;
	virtual const char * GetDemoFilePath() = 0;
	virtual bool IsRecordingDemo() = 0;
	virtual bool IsPlayingTimeDemo() = 0;
	virtual void *unk046() = 0;
	virtual void *unk047() = 0;
	virtual void *unk048() = 0;
	virtual void *unk049() = 0;
	virtual void *unk050() = 0;
	virtual void ClientCommand(int iUnk0MaybeSplitScreenSlotSetTo0, const char * pszCommands, bool bUnrestricted) = 0;
	virtual void *unk052() = 0;
	virtual void *unk053() = 0;
	virtual void *unk054() = 0;
	virtual bool IsSplitScreenActive() = 0;
	virtual bool IsValidSplitScreenSlot(int nSplitScreenSlot) = 0;
	virtual int &FirstValidSplitScreenSlot(int &nSplitScreenSlot) = 0;
	virtual int &NextValidSplitScreenSlot(int &nSplitScreenSlot, int nPreviousSlot) = 0;
	virtual void GetScreenSize(int& width, int& height) = 0;
	virtual void *unk060() = 0;
	virtual void *unk061() = 0;
	virtual void *unk062() = 0;
	virtual void OnEngineLevelLoadingFinished() = 0;
	virtual const char *GetLevelName() = 0;
	virtual const char *GetLevelNameShort() = 0;
	virtual void *unk066() = 0;
	virtual void *GetBroadcastRecorder() = 0;
	virtual CMovieRecorder* GetMovieRecorder() = 0;
	virtual IDemoFile* GetDemoFile() = 0;
	virtual void *unk070() = 0;
	virtual void *unk071() = 0;
	virtual void *unk072() = 0;
	virtual void *unk073() = 0;
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
	virtual void *unk088() = 0;
	virtual void *unk089() = 0;
	virtual void *unk090() = 0;
	virtual void *unk091() = 0;
	virtual void *unk092() = 0;
	virtual void *unk093() = 0;
	virtual void *unk094() = 0;
	virtual void *unk095() = 0;
	virtual void *unk096() = 0;
	virtual void *unk097() = 0;
	virtual void *unk098() = 0;
	virtual void *unk099() = 0;
	virtual void *unk100() = 0;
	virtual void *unk101() = 0;
	virtual int RegisterDemoCustomDataCallback(const char *pszName, void *pfnCallback) = 0;
	virtual void RecordDemoCustomData(int nCallbackID, const void *pData, int nSize) = 0;
	virtual void *unk104() = 0;
	virtual void *unk105() = 0;
	virtual void *unk106() = 0;
	virtual void *unk107() = 0;
	virtual void *unk108() = 0;
	virtual void *unk109() = 0;
	virtual void *unk110() = 0;
	virtual void *unk111() = 0;
	virtual void *unk112() = 0;
	virtual void *unk113() = 0;
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
	virtual void *unk127() = 0;
	virtual void *unk128() = 0;
	virtual int SOSSetOpvarFloat(const char *pszStackName, const char *pszOpvarName, float flValue) = 0;
	virtual int SOSGetOpvarFloat(const char *pszStackName, const char *pszOpvarName, float *pflOut) = 0;
	virtual void *unk131() = 0;
	virtual void *unk132() = 0;
	virtual void *unk133() = 0;
	virtual void *unk134() = 0;
	virtual void *unk135() = 0;
	virtual void *unk136() = 0;
	virtual void *unk137() = 0;
	virtual void *unk138() = 0;
	virtual void *unk139() = 0;
	virtual void *unk140() = 0;
	virtual void *unk141() = 0;
	virtual void *unk142() = 0;
	virtual void *unk143() = 0;
	virtual void *unk144() = 0;
	virtual void *unk145() = 0;
	virtual void *unk146() = 0;
	virtual void *unk147() = 0;
	virtual void *unk148() = 0;
	virtual const char *GetDefaultRenderSystemOption() = 0;
	virtual void SetDefaultRenderSystemOption(const char *pszOption) = 0;
	virtual uint64 GetRenderSystemOptionFlags() = 0;
	virtual void SetRenderSystemOptionFlags(uint64 nValue, uint64 nMask) = 0;
	virtual bool IsRenderSystemOptionRecommendationStale() = 0;
	virtual void MarkRenderSystemOptionRecommended() = 0;
	virtual void *unk155() = 0;
	virtual void *unk156() = 0;
	virtual void *unk157() = 0;
	virtual void RunPanoramaAnimUpdate() = 0;
	virtual void *unk159() = 0;
	virtual void *unk160() = 0;
	virtual void *unk161() = 0;
	virtual void *unk162() = 0;
	virtual void *unk163() = 0;
	virtual void *unk164() = 0;
	virtual void *unk165() = 0;
	virtual void WriteMinidumpSystemInfo(void *pBuffer) = 0;
	virtual bool GetLowViolence() = 0;
	virtual void SetLowViolence(int nValue) = 0;
	virtual void *unk169() = 0;
	virtual void *unk170() = 0;
	virtual void *unk171() = 0;
	virtual void *unk172() = 0;
	virtual void *unk173() = 0;
	virtual void *unk174() = 0;
	virtual void *unk175() = 0;
	virtual void *unk176() = 0;
	virtual void *unk177() = 0;
	virtual void *unk178() = 0;
	virtual void *unk179() = 0;
	virtual void *unk180() = 0;
	virtual int GetGlobalThreadPoolMode() = 0;
	virtual const char *GetThreadPoolModeName(int nMode) = 0;
	virtual void SetGlobalThreadPoolMode(int nMode) = 0;
	virtual void *unk184() = 0;
};

typedef IVEngineClient2 IVEngineClient;

#endif // CDLL_INT_H
