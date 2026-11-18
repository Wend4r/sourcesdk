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
#include "engine2/isource2engine.h"
#include <steam/steamuniverse.h>
#include "playerslot.h"
#include "playeruserid.h"
#include "splitscreenslot.h"
#include "tier0/platwindow.h"
#include "spawngrouptypes.h"

#include <cmath>

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

// Frame record the engine hands to the render device present callback; the engine keeps the last 10 presented ones
struct EnginePresentedFrame_t
{
	int m_nFrame;
	int m_nRenderTick;
	float m_flRenderTickFraction;
	int m_nPlayerTick;
	float m_flPlayerTickFraction;
	// Stored by the client while the frame is pending, see IVEngineClient2::GetPendingPresentedFrame
	void *m_pClientData;
	// Plat_FloatTime() of the present callback
	double m_flPresentTime;
};

//-----------------------------------------------------------------------------
// Purpose: Interface the engine exposes to the client DLL (Source2EngineToClient001).
//-----------------------------------------------------------------------------
abstract_class IVEngineClient2 : public ISource2Engine
{
public:
	virtual EUniverse GetSteamUniverse() const = 0;
	virtual CPlayerSlot GetPlayerSlotByNetworkIDString( const char *pszNetworkID ) = 0;
	virtual const char *GetPlayerNetworkIDString( CPlayerSlot nSlot ) = 0;
	// Without a network game client both return 0x80000000 when bInvalidWhenMissing is set, 0 otherwise
	virtual CPlayerSlot GetLocalPlayer( bool bInvalidWhenMissing = false ) = 0;
	virtual CPlayerSlot GetLastValidPlayerSlot( bool bInvalidWhenMissing = false ) = 0;
	virtual bool IsPlayerSlotActive( CPlayerSlot nSlot ) = 0;
	virtual float &GetFrameTime() = 0;
	virtual void SetFrameTimeAmnesty( const char *pszReason, int nFrames, float flDuration ) = 0;
	virtual const char *GetFrameTimeAmnesty( bool bCheckCvar ) = 0;
	// Forwards to the same function as IEngineServiceMgr::SetFramePerformanceTag
	virtual void SetFramePerformanceTag( const char *pszTag, int nValue, int nFrames, float flDuration ) = 0;
	virtual void PrintVProfLiteReport( void *pReport, bool bDetailed, int nLogChannel ) = 0;
	virtual void DumpNetStats( void *pNetStatData, void ( *pfnOutput )( const char * ) ) = 0;
	// Named by what it does: asks virtual 14 of the active engine loop entry (false by default)
	virtual bool QueryActiveLoop() = 0;
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
	// Named by what it does: string of the CDemoFile being played back (its virtual 5, the field at +368), NULL
	// without playback
	virtual const char *GetDemoFileIdentifier() = 0;
	virtual bool IsRecordingDemo() = 0;
	virtual bool IsPlayingTimeDemo() = 0;
	// Named by what it does: byte 10 of the active engine loop entry
	virtual bool IsActiveLoopFlagSet() = 0;
	virtual bool IsPlayingBroadcast() = 0;
	virtual void *GetBroadcastPlayer() = 0;
	// Sends pszCmd to the server over the net channel of nSlot
	virtual void ServerCmd( CSplitScreenSlot nSlot, const char *pszCmd ) = 0;
	// Callers pass a fixed double and 0 for the last two arguments
	// flInputTime and nInputTimestamp go to CCommandBuffer::AddText, a flInputTime other than NAN can wrap the command in __beginseq and __endseq
	virtual void ClientCommand( CSplitScreenSlot nSlot, const char *pszCmd, bool bUnrestricted, double flInputTime = NAN, uint64 nInputTimestamp = 0 ) = 0;
	virtual void SetRestrictServerCommands( bool bRestrict ) = 0;
	virtual void SetRestrictClientCommands( bool bRestrict ) = 0;
	// Forwards to IApplication::IsLowViolence
	virtual bool IsLowViolence() = 0;
	// Player slot the split screen service holds for nSlot; GetSplitScreenPlayer asks the network game client instead
	virtual CPlayerSlot GetSplitScreenServicePlayer( CSplitScreenSlot nSlot ) = 0;
	virtual bool IsSplitScreenActive() = 0;
	virtual bool IsValidSplitScreenSlot( CSplitScreenSlot nSlot ) = 0;
	virtual CSplitScreenSlot FirstValidSplitScreenSlot() = 0;
	// Returns -1 when there is no further valid slot
	virtual CSplitScreenSlot NextValidSplitScreenSlot( CSplitScreenSlot nPreviousSlot ) = 0;
	// Always returns -1 (Dota 2 2018 name)
	virtual int GetAvailableAsyncThread() = 0;
	virtual void GetScreenSize( int &nWidth, int &nHeight ) = 0;
	virtual bool IsLoadingLevel() = 0;
	virtual void OnEngineLevelLoadingFinished() = 0;
	virtual const char *GetLevelName() = 0;
	virtual const char *GetLevelNameShort() = 0;
	// Always returns NULL (Dota 2 2018 name)
	virtual void *GetVoiceTweakAPI() = 0;
	// Forwards to ITestScriptMgr::CheckPoint
	virtual void CheckPoint( const char *pszName ) = 0;
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
	// IDemoPlayer::SetDemoCommandsLocked / AreDemoCommandsLocked
	virtual void SetDemoCommandsLocked( bool bLocked ) = 0;
	virtual bool AreDemoCommandsLocked() = 0;
	virtual void PauseDemo() = 0;
	virtual void ResumeDemo() = 0;
	virtual void SkipToDemoTick( int nTick ) = 0;
	// IDemoPlayer::SetPauseAtEnd
	virtual void SetDemoPauseAtEnd( bool bPause ) = 0;
	// Named by what it does: the network game client is in an HLTV replay (its replay flag or replay ticks left)
	virtual bool IsInHltvReplay() = 0;
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
	// Dota 2 2018 name; 0 without a network game client
	virtual int GetConnectedServerNetworkVersion() = 0;
	virtual uint32 GetAppID() = 0;
	// Loads the "user_keys" key bindings config of nSlot
	virtual bool ReadKeyBindings( CSplitScreenSlot nSlot ) = 0;
	// Dota 2 2018 names; the engine only stores the pointer
	virtual void SetAchievementMgr( void *pAchievementMgr ) = 0;
	virtual void *GetAchievementMgr() = 0;
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
	// Dota 2 2018 name and argument types
	virtual void SetMixGroupOfCurrentMixer( const char *pszMixGroupName, const char *pszParameter, float flValue, int nLayer ) = 0;
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
	// Copies the server tick timing records from nFirstIndex on (the engine keeps the last 128) into the vector at pOutRecords and returns the index after the newest record
	virtual uint64 GetServerTickStats( uint64 nFirstIndex, void *pOutRecords ) = 0;
	virtual bool IsInLevelLoadLoop() = 0;
	virtual void SetConnectionPassword( const char *pszPassword ) = 0;
	virtual void ServerCmdKeyValues( KeyValues *pKeyValues ) = 0;
	virtual void TickProgressBar() = 0;
	virtual PlatWindow_t GetEngineWindow() = 0;
	// Named by what it does: forwards nIndex to virtual 25 of the view object of the network game client
	virtual void SetClientViewIndex( uint32 nIndex ) = 0;
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
	// Named by what it does: the uint64 of the network game client that INetSupport::GetBaseServerInfo reports
	// first and the SourceTV status prints formatted
	virtual uint64 GetConnectedServerSteamID() = 0;
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
	// Named by what it does: puts the game UI service into its loading progress state (progress 0 of 1.0)
	virtual void BeginGameUILoadingProgress( const EngineLoopState_t &loopState ) = 0;
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
	// AMNOTE: Returns the constant 0.1; no source names it and the engine does not call it
	virtual double ConstantDoubleQuery() = 0;
	// Record of the frame being submitted, NULL outside of the engine's frame submission
	virtual EnginePresentedFrame_t *GetPendingPresentedFrame() = 0;
	// Copy of the most recently presented frame record
	virtual EnginePresentedFrame_t GetLastPresentedFrame() = 0;
	// Hands a GOTV sync answer of the game coordinator to the broadcast player ("Ignoring unexpected sync from gc")
	virtual bool OnGCGotvSyncReceived( const void *pSyncMsg ) = 0;
	// AMNOTE: The next three are no-ops in this build; no source names them
	virtual void EmptyEngineCallback1() = 0;
	virtual void EmptyEngineCallback2( int nValue ) = 0;
	virtual void EmptyEngineCallback3() = 0;
	// Named by what it does: stores the lowercased name with the flag in a list of the engine client
	virtual void AddCommandFilter( const char *pszCommand, bool bEnable ) = 0;
	// Named by what it does: clears a global flag of the engine client and returns its address
	virtual void ClearCommandFilterFlag() = 0;
	virtual void SendVProfLiteReportToServer( void *pReport ) = 0;
	virtual int GetGlobalThreadPoolMode() = 0;
	virtual void SetGlobalThreadPoolMode( int nMode ) = 0;
	// Removes the stored mode from the boot config.
	virtual void ResetGlobalThreadPoolMode() = 0;
	virtual void PrintNetworkSummaryReport() = 0;
};

typedef IVEngineClient2 IVEngineClient;

#endif // CDLL_INT_H
