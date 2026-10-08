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
#include "playerslot.h"
#include "playeruserid.h"
#include "splitscreenslot.h"
#include "tier0/platwindow.h"
#include "spawngrouptypes.h"

//-----------------------------------------------------------------------------
// forward declarations
//-----------------------------------------------------------------------------
class CMovieRecorder;
class CBufferString;
class CMsgPlayerInfo;
class INetChannel;
class IDemoPlayer;
class IDemoRecorder;
class KeyValues;
class CUtlBuffer;
class CUtlSymbolLarge;
class CGamestatsData;
class CGameInfo;
class CDemoSaveGame;
struct SpawnGroupDesc_t;
struct EngineLoopState_t;

//-----------------------------------------------------------------------------
// Purpose: Interface the engine exposes to the client DLL (Source2EngineToClient001).
//-----------------------------------------------------------------------------
abstract_class IVEngineClient2 : public ISource2Engine
{
public:
	virtual EUniverse GetSteamUniverse() const = 0;
	virtual CPlayerSlot GetPlayerSlotByNetworkIDString( const char *pszNetworkID ) = 0;
	virtual const char *GetPlayerNetworkIDString( CPlayerSlot nSlot ) = 0;
	virtual CPlayerSlot GetLocalPlayer( bool bUnk = false ) = 0;
	virtual CPlayerSlot GetLastValidPlayerSlot( bool bUnk = false ) = 0;
	virtual bool IsPlayerSlotActive( CPlayerSlot nSlot ) = 0;
	virtual float &GetFrameTime() = 0;
	virtual void SetFrameTimeAmnesty( const char *pszReason, int nFrames, float flDuration ) = 0;
	virtual const char *GetFrameTimeAmnesty( bool bCheckCvar ) = 0;
	virtual void unk027( const char *pszName, int nAmnesty, int nFrames, float flDuration ) = 0;
	virtual void PrintVProfLiteReport( void *pReport, bool bDetailed, int nLogChannel ) = 0;
	virtual void DumpNetStats( void *pNetStatData, void ( *pfnOutput )( const char * ) ) = 0;
	virtual bool unk030() = 0;
	virtual uint32 GetLongFrameCount() = 0;
	virtual bool GetPlayerInfo( int nPlayerIndex, CMsgPlayerInfo *pInfo ) = 0;
	virtual CPlayerUserId GetPlayerUserId( int nPlayerIndex ) = 0;
	virtual CPlayerSlot GetSplitScreenPlayer( CSplitScreenSlot nSlot ) = 0;
	virtual CSplitScreenSlot GetSplitScreenSlotForPlayer( CPlayerSlot nPlayerSlot ) = 0;
	virtual float GetLastTimeStamp() = 0;
	virtual int GetLastServerTick() = 0;
	virtual int GetMaxClients() = 0;
	virtual bool IsInGame() = 0;
	virtual bool IsConnected() = 0;
	virtual INetChannel *GetNetChannel( CSplitScreenSlot nSlot ) = 0;
	virtual bool IsPlayingDemo() = 0;
	virtual const char *GetDemoFilePath() = 0;
	virtual const char *unk044() = 0;
	virtual bool IsRecordingDemo() = 0;
	virtual bool IsPlayingTimeDemo() = 0;
	virtual bool unk047() = 0;
	virtual bool IsPlayingBroadcast() = 0;
	virtual void *GetBroadcastPlayer() = 0;
	// Sends pszCmd to the server over the net channel of nSlot
	virtual void ServerCmd( CSplitScreenSlot nSlot, const char *pszCmd ) = 0;
	// Callers pass a fixed double and 0 for the last two arguments
	virtual void ClientCommand( CSplitScreenSlot nSlot, const char *pszCmd, bool bUnrestricted, double flUnk, uint64 nUnk ) = 0;
	virtual void SetRestrictServerCommands( bool bRestrict ) = 0;
	virtual void SetRestrictClientCommands( bool bRestrict ) = 0;
	virtual bool unk054() = 0;
	// Player slot the split screen service holds for nSlot
	virtual CPlayerSlot unk055( CSplitScreenSlot nSlot ) = 0;
	virtual bool IsSplitScreenActive() = 0;
	virtual bool IsValidSplitScreenSlot( CSplitScreenSlot nSlot ) = 0;
	virtual CSplitScreenSlot FirstValidSplitScreenSlot() = 0;
	// Returns -1 when there is no further valid slot
	virtual CSplitScreenSlot NextValidSplitScreenSlot( CSplitScreenSlot nPreviousSlot ) = 0;
	// Always returns -1
	virtual int unk060() = 0;
	virtual void GetScreenSize( int &nWidth, int &nHeight ) = 0;
	virtual bool IsLoadingLevel() = 0;
	virtual void OnEngineLevelLoadingFinished() = 0;
	virtual const char *GetLevelName() = 0;
	virtual const char *GetLevelNameShort() = 0;
	// Always returns NULL
	virtual void *unk066() = 0;
	virtual void *unk067() = 0;
	virtual CMovieRecorder *GetMovieRecorder() = 0;
	virtual IDemoPlayer *GetDemoPlayer() = 0;
	virtual IDemoRecorder *GetDemoRecorder() = 0;
	virtual bool IsDemoPlaybackPaused() = 0;
	virtual bool IsSkippingPlayback() = 0;
	virtual int GetDemoRecordingTick() = 0;
	virtual int GetDemoPlaybackTick() = 0;
	virtual int GetDemoPlaybackStartTick() = 0;
	virtual float GetDemoPlaybackTimeScale() = 0;
	virtual int GetDemoPlaybackTotalTicks() = 0;
	virtual const char *GetDemoPlaybackFileName() = 0;
	virtual void PlayDemo( const char *pszFilename ) = 0;
	// Setter and getter of the same demo player flag
	virtual void unk080( bool bValue ) = 0;
	virtual bool unk081() = 0;
	virtual void PauseDemo() = 0;
	virtual void ResumeDemo() = 0;
	virtual void SkipToDemoTick( int nTick ) = 0;
	virtual void unk085( bool bValue ) = 0;
	virtual bool unk086() = 0;
	// Value of cl_language
	virtual void GetUILanguage( CBufferString &sLanguage ) = 0;
	virtual float GetScreenAspectRatio( int nViewportWidth, int nViewportHeight ) = 0;
	// steam.inf PatchVersion with the dots stripped
	virtual int GetPatchVersion() = 0;
	// steam.inf ServerVersion
	virtual int GetServerVersion() = 0;
	// steam.inf VersionDate and VersionTime
	virtual const char *GetVersionDateString() = 0;
	virtual const char *GetVersionTimeString() = 0;
	virtual int unk093() = 0;
	virtual uint32 GetAppID() = 0;
	// Loads the "user_keys" key bindings config of nSlot
	virtual bool ReadKeyBindings( CSplitScreenSlot nSlot ) = 0;
	virtual void unk096( void *p ) = 0;
	virtual void *unk097() = 0;
	virtual CSplitScreenSlot GetActiveSplitScreenPlayerSlot() = 0;
	virtual CSplitScreenSlot SetActiveSplitScreenPlayerSlot( CSplitScreenSlot nSlot ) = 0;
	// Thread-local flag; returns the previous value
	virtual bool SetLocalPlayerIsResolvable( const char *pszContext, int nLine, bool bResolvable ) = 0;
	virtual bool IsLocalPlayerResolvable() = 0;
	virtual void RegisterDemoCustomDataCallback( const CUtlSymbolLarge &szCallbackSaveID, void *pfnCallback ) = 0;
	virtual void RecordDemoCustomData( void *pfnCallback, const void *pData, size_t nDataLength ) = 0;
	// No-op in this build; GetPitchScale always returns 1.0.
	virtual void SetPitchScale( float flPitchScale ) = 0;
	virtual float GetPitchScale() = 0;
	virtual void Unk_SetMixGroupOfCurrentMixer( void *p ) = 0;
	virtual int GetMixLayerIndex( const char *pszMixLayerName ) = 0;
	virtual void SetMixLayerLevel( int nIndex, float flLevel ) = 0;
	virtual bool IsVoiceRecording() = 0;
	virtual void SetTimescale( float flTimescale ) = 0;
	virtual void SetGamestatsData( CGamestatsData *pGamestatsData ) = 0;
	virtual CGamestatsData *GetGamestatsData() = 0;
	// Always 0 / no-op in this build.
	virtual int GetBugSubmissionCount() = 0;
	virtual void ClearBugSubmissionCount() = 0;
	virtual float GetServerSimulationFrameTime() = 0;
	virtual uint64 unk116( uint64 nFirstFrame, void *pOut ) = 0;
	virtual bool IsInLevelLoadLoop() = 0;
	virtual void SetConnectionPassword( const char *pszPassword ) = 0;
	virtual void ServerCmdKeyValues( KeyValues *pKeyValues ) = 0;
	virtual void TickProgressBar() = 0;
	virtual PlatWindow_t GetEngineWindow() = 0;
	virtual void unk122( uint32 nIndex ) = 0;
	virtual void SkipDemoToTime( float flTime ) = 0;
	virtual void FlashWindow() = 0;
	virtual void DesktopNotify( const char *pszTitle, const char *pszMessage ) = 0;
	virtual void AlertUser( const char *pszTitle, const char *pszMessage ) = 0;
	// Game info from the header of the demo being played back.
	virtual bool GetDemoGameInfo( CGameInfo *pGameInfo ) = 0;
	virtual bool GetDemoFileGameInfo( const char *pszFileName, CGameInfo *pGameInfo ) = 0;
	// Both are unported stubs that warn once and return false.
	virtual bool SOSSetOpvarFloat( const char *pszStackName, const char *pszOpvarName, float flValue ) = 0;
	virtual bool SOSGetOpvarFloat( const char *pszStackName, const char *pszOpvarName, float *pflOut ) = 0;
	virtual bool MapLoadFailed() = 0;
	virtual void SetMapLoadFailed( bool bState ) = 0;
	virtual SpawnGroupHandle_t LoadSpawnGroup( const SpawnGroupDesc_t &desc ) = 0;
	virtual void UnloadSpawnGroup( SpawnGroupHandle_t hSpawnGroup, /*ESpawnGroupUnloadOption*/ int nOption ) = 0;
	virtual void SetSpawnGroupDescription( SpawnGroupHandle_t hSpawnGroup, const char *pszDescription ) = 0;
	virtual bool IsSpawnGroupLoaded( SpawnGroupHandle_t hSpawnGroup ) = 0;
	virtual bool IsSpawnGroupLoading( SpawnGroupHandle_t hSpawnGroup ) = 0;
	virtual SpawnGroupHandle_t FindSpawnGroupByName( const char *pszName ) = 0;
	virtual void SynchronouslySpawnGroup( SpawnGroupHandle_t hSpawnGroup ) = 0;
	virtual void SynchronizeAndBlockUntilLoaded( SpawnGroupHandle_t hSpawnGroup ) = 0;
	virtual bool GetDemoSaveGameForTick( int nTick, CDemoSaveGame *pSaveGame ) = 0;
	virtual int GetInstantReplayOldestTick() = 0;
	virtual int GetInstantReplayLiveTick() = 0;
	// 0 when not in an instant replay.
	virtual int GetInstantReplayTicksBehindLive() = 0;
	virtual bool IsInstantReplayPaused() = 0;
	virtual float GetInstantReplayTimescale() = 0;
	virtual bool IsClientLocalToActiveServer() = 0;
	virtual uint64 unk148() = 0;
	virtual const char *GetDefaultRenderSystemOption() = 0;
	virtual void SetDefaultRenderSystemOption( const char *pszOption ) = 0;
	virtual bool IsRenderSystemOptionFromCommandLine() = 0;
	virtual uint64 GetRenderSystemOptionFlags() = 0;
	// Only the low 32 bits of the stored flags survive.
	virtual void SetRenderSystemOptionFlags( uint32 nValue, uint32 nMask ) = 0;
	virtual bool IsRenderSystemOptionRecommendationStale() = 0;
	virtual void MarkRenderSystemOptionRecommended() = 0;
	// Writes cfg/boot.vcfg.
	virtual void SaveBootConfig() = 0;
	virtual void unk157( const EngineLoopState_t &loopState ) = 0;
	virtual void RunPanoramaAnimUpdate() = 0;
	virtual void WaitForPanoramaAnimUpdate() = 0;
	virtual const char *GetLanguage( int nType ) = 0;
	virtual void SetLanguage( int nType, const char *pszLanguage ) = 0;
	// -language, or -textlanguage for nType 0.
	virtual bool IsLanguageSetOnCommandLine( int nType ) = 0;
	virtual const char *GetSubLanguage( int nType ) = 0;
	virtual void SetSubLanguage( int nType, const char *pszSubLanguage ) = 0;
	// -textsublanguage, nType 0 only.
	virtual bool IsSubLanguageSetOnCommandLine( int nType ) = 0;
	virtual void WriteMinidumpSystemInfo( CUtlBuffer *pBuffer ) = 0;
	virtual bool GetLowViolence() = 0;
	virtual void SetLowViolence( bool bLowViolence ) = 0;
	virtual float DemoTicksToTime( int nTicks ) = 0;
	virtual const char *GetDemoRecordingFileName() = 0;
	// Returns the constant 0.1.
	virtual double unk171() = 0;
	virtual void *unk172() = 0;
	virtual void *unk173() = 0;
	virtual bool unk174( const void *pSyncMsg ) = 0;
	// No-op in this build.
	virtual void unk175() = 0;
	// No-op in this build.
	virtual void unk176( int nValue ) = 0;
	// No-op in this build.
	virtual void unk177() = 0;
	virtual void unk178( const char *pszCommand, bool bEnable ) = 0;
	virtual void unk179() = 0;
	virtual void SendVProfLiteReportToServer( void *pReport ) = 0;
	virtual int GetGlobalThreadPoolMode() = 0;
	virtual void SetGlobalThreadPoolMode( int nMode ) = 0;
	// Removes the stored mode from the boot config.
	virtual void ResetGlobalThreadPoolMode() = 0;
	virtual void PrintNetworkSummaryReport() = 0;
};

typedef IVEngineClient2 IVEngineClient;

#endif // CDLL_INT_H
