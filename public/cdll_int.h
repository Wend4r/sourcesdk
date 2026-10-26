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
#include "tier0/utlsortvector.h"
#include "tier4/tier4.h"

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
class IAchievementMgr;
class IVoiceTweak;
class IBroadcastPlayer;
class CEngineGotvSyncPacket;
struct SpawnGroupDesc_t;
struct EngineLoopState_t;
enum ThreadPoolCoreTypeRequest_t : int;

//-----------------------------------------------------------------------------
// Purpose: Interface the engine exposes to the client DLL (Source2EngineToClient001).
//-----------------------------------------------------------------------------
abstract_class IVEngineClient2 : public ISource2Engine
{
public:
	struct ServerTickTime_t;

	// Present timing record of one client/server loop frame (no type name in any binary)
	struct PresentTimingRecord_t
	{
		// Frame number of the presented frame
		int m_nFrame;
		// Tick and tick fraction that were rendered in this frame
		int m_nRenderTick;
		float m_flRenderTickFraction;
		// Tick and tick fraction of the predicted local player in this frame
		int m_nPlayerTick;
		float m_flPlayerTickFraction;
		// Per-frame input data the client attaches to the record of the frame in flight
		void *m_pClientFrameData;
		// Plat_FloatTime() when the present completed
		double m_flPresentCompletedTime;
	};

	virtual EUniverse GetSteamUniverse() const = 0;
	virtual WorldGroupId_t FindWorldGroupByName( const char *pszName ) = 0;
	virtual const char *GetWorldGroupName( WorldGroupId_t hWorldGroupId ) = 0;
	virtual WorldGroupId_t GetFirstWorldGroupId( bool bClientSide ) = 0;
	virtual WorldGroupId_t MaxWorldGroupId( bool bClientSide ) = 0;
	virtual bool IsWorldGroupValid( WorldGroupId_t hWorldGroupId ) = 0;
	virtual float &GetFrameTime() = 0;
	virtual void SetFrameTimeAmnesty( const char *pszReason, int nFrames, float flDuration ) = 0;
	virtual const char *GetFrameTimeAmnesty( bool bCheckCvar ) = 0;
	virtual void SetFramePerformanceTag( const char *pszTag, int nValue, int nFrames, float flDuration ) = 0;
	virtual void PrintVProfLiteReport( void *pReport, bool bDetailed, int nLogChannel ) = 0;
	virtual void DumpNetStats( void *pNetStatData, void ( *pfnOutput )( const char * ) ) = 0;
	virtual bool ShouldForceMaxFrametimeToTickInterval() = 0;
	virtual uint32 GetLongFrameCount() = 0;
	virtual bool GetPlayerInfo( int nPlayerIndex, CMsgPlayerInfo *pInfo ) = 0;
	virtual CPlayerUserId GetPlayerUserId( int nPlayerIndex ) = 0;
	// -1 reads split screen slot 0
	virtual CPlayerSlot GetLocalPlayer( CSplitScreenSlot nSlot ) = 0;
	virtual CSplitScreenSlot GetSplitScreenSlotForPlayer( CPlayerSlot nPlayerSlot ) = 0;
	virtual float GetLastTimeStamp() = 0;
	virtual int GetLastServerTick() = 0;
	virtual int GetMaxClients() = 0;
	virtual bool IsInGame() = 0;
	virtual bool IsConnected() = 0;
	virtual INetChannel *GetNetChannel( CSplitScreenSlot nSlot ) = 0;
	virtual bool IsPlayingDemo() = 0;
	virtual const char *GetDemoFilePath() = 0;
	// Host name from the server info of the demo being played back
	virtual const char *GetDemoHostName() = 0;
	virtual bool IsRecordingDemo() = 0;
	virtual bool IsPlayingTimeDemo() = 0;
	// True when the active loop was started with a "demofile" option
	virtual bool IsDemoFileLoopActive() = 0;
	virtual bool IsPlayingBroadcast() = 0;
	virtual IBroadcastPlayer *GetBroadcastPlayer() = 0;
	// Sends pszCmd to the server over the net channel of nSlot
	virtual void ServerCmd( CSplitScreenSlot nSlot, const char *pszCmd ) = 0;
	virtual void ClientCommand( CSplitScreenSlot nSlot, const char *pszCmd, bool bUnrestricted, double flInputTime, uint64 nInputTimestamp ) = 0;
	virtual void SetRestrictServerCommands( bool bRestrict ) = 0;
	virtual void SetRestrictClientCommands( bool bRestrict ) = 0;
	virtual bool IsLowViolence() = 0;
	// Forwards to ISplitScreenService::GetSplitScreenPlayer
	virtual CPlayerSlot GetSplitScreenPlayer( CSplitScreenSlot nSlot ) = 0;
	virtual bool IsSplitScreenActive() = 0;
	virtual bool IsValidSplitScreenSlot( CSplitScreenSlot nSlot ) = 0;
	virtual CSplitScreenSlot FirstValidSplitScreenSlot() = 0;
	// Returns -1 when there is no further valid slot
	virtual CSplitScreenSlot NextValidSplitScreenSlot( CSplitScreenSlot nPreviousSlot ) = 0;
	// Always returns -1
	virtual int GetAvailableAsyncThread() = 0;
	virtual void GetScreenSize( int &nWidth, int &nHeight ) = 0;
	virtual bool IsLoadingLevel() = 0;
	virtual void OnEngineLevelLoadingFinished() = 0;
	virtual const char *GetLevelName() = 0;
	virtual const char *GetLevelNameShort() = 0;
	// Always returns NULL
	virtual IVoiceTweak *GetVoiceTweakAPI() = 0;
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
	// While set, the demo console commands (demo_goto, demo_pause, demo_resume, ...) do nothing
	virtual void SetDemoCommandsDisabled( bool bDisabled ) = 0;
	virtual bool AreDemoCommandsDisabled() = 0;
	virtual void PauseDemo() = 0;
	virtual void ResumeDemo() = 0;
	virtual void SkipToDemoTick( int nTick ) = 0;
	// While set, the demo player pauses at the end of the demo instead of disconnecting
	virtual void SetPauseAtDemoEnd( bool bPause ) = 0;
	// Also true while an HLTV replay is shown
	virtual bool IsHLTV() = 0;
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
	virtual int GetConnectedServerNetworkVersion() = 0;
	virtual uint32 GetAppID() = 0;
	// Loads the "user_keys" key bindings config of nSlot
	virtual bool ReadKeyBindings( CSplitScreenSlot nSlot ) = 0;
	virtual void SetAchievementMgr( IAchievementMgr *pAchievementMgr ) = 0;
	virtual IAchievementMgr *GetAchievementMgr() = 0;
	virtual CSplitScreenSlot GetActiveSplitScreenPlayerSlot() = 0;
	virtual CSplitScreenSlot SetActiveSplitScreenPlayerSlot( CSplitScreenSlot nSlot ) = 0;
	// Thread-local flag; returns the previous value
	virtual bool SetLocalPlayerIsResolvable( const char *pszContext, int nLine, bool bResolvable ) = 0;
	virtual bool IsLocalPlayerResolvable() = 0;
	virtual void RegisterDemoCustomDataCallback( const CUtlSymbolLarge &szCallbackSaveID, void ( *pfnCallback )( uint8 *pData, size_t nSize ) ) = 0;
	virtual void RecordDemoCustomData( void ( *pfnCallback )( uint8 *pData, size_t nSize ), const void *pData, size_t nDataLength ) = 0;
	// No-op in this build; GetPitchScale always returns 1.0.
	virtual void SetPitchScale( float flPitchScale ) = 0;
	virtual float GetPitchScale() = 0;
	virtual void SetMixGroupOfCurrentMixer( const char *pszGroupName, const char *pszParam, float flValue ) = 0;
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
	// Returns the newest frame number
	virtual uint64 GetServerTickTimes( uint64 nFirstFrame, CUtlVector< ServerTickTime_t > &vecTickTimes ) = 0;
	virtual bool IsInLevelLoadLoop() = 0;
	virtual void SetConnectionPassword( const char *pszPassword ) = 0;
	virtual void ServerCmdKeyValues( KeyValues *pKeyValues ) = 0;
	virtual void TickProgressBar() = 0;
	virtual PlatWindow_t GetEngineWindow() = 0;
	virtual void ChangePVSSpawnGroupHandle( SpawnGroupHandle_t hSpawnGroup ) = 0;
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
	virtual void UnloadSpawnGroup( SpawnGroupHandle_t hSpawnGroup, ESpawnGroupUnloadOption nOption ) = 0;
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
	// SteamID of the server the auth session ticket was requested for
	virtual uint64 GetGameServerSteamID() = 0;
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
	// Sets the game UI frame size from the loop state and resizes Panorama to it; called before RunPanoramaAnimUpdate()
	virtual void PreparePanoramaFrame( const EngineLoopState_t &loopState ) = 0;
	virtual void RunPanoramaAnimUpdate() = 0;
	virtual void WaitForPanoramaAnimUpdate() = 0;
	// The boot config's UILanguage and AudioLanguage, UISubLanguage and AudioSubLanguage
	virtual const char *GetLanguage( LanguageType_t nType ) = 0;
	virtual void SetLanguage( LanguageType_t nType, const char *pszLanguage ) = 0;
	// -language, or -textlanguage for LanguageType_UI.
	virtual bool IsLanguageSetOnCommandLine( LanguageType_t nType ) = 0;
	virtual const char *GetSubLanguage( LanguageType_t nType ) = 0;
	virtual void SetSubLanguage( LanguageType_t nType, const char *pszSubLanguage ) = 0;
	// -textsublanguage, LanguageType_UI only.
	virtual bool IsSubLanguageSetOnCommandLine( LanguageType_t nType ) = 0;
	virtual void WriteMinidumpSystemInfo( CUtlBuffer *pBuffer ) = 0;
	virtual bool GetLowViolence() = 0;
	virtual void SetLowViolence( bool bLowViolence ) = 0;
	virtual float DemoTicksToTime( int nTicks ) = 0;
	virtual const char *GetDemoRecordingFileName() = 0;
	// Returns the constant 0.1.
	virtual double unk171() = 0;
	// Present timing record of the frame in flight, NULL when there is none
	virtual PresentTimingRecord_t *GetCurrentPresentTimingRecord() = 0;
	// Newest completed present timing record
	virtual PresentTimingRecord_t GetLastPresentTimingRecord() = 0;
	// Applies a GOTV sync packet from the GC to the broadcast player
	virtual bool EngineGotvSyncPacket( const CEngineGotvSyncPacket *pPkt ) = 0;
	// No-op in this build; the client passes CUserMessageServerFrameTime::frame_time
	virtual void ReceivedServerFrameTime( float flServerFrameTime ) = 0;
	// No-op in this build; the client passes the number of prediction errors of the tick
	virtual void ReceivedPredictionErrorCount( int nPredictionErrors ) = 0;
	// No-op in this build; the client passes CUserMessageLagCompensationError::distance
	virtual void ReceivedLagCompensationError( float flLagCompensationError ) = 0;
	// Adds or removes a command name (hashed lower-case) from the set of commands that the
	// engine passes to ISource2Client::PredictStringCommand before they are forwarded to the server
	virtual void SetPredictedStringCommand( const char *pszCommand, bool bPredicted ) = 0;
	// Clears the flag that allows connecting to secure servers
	virtual void DisallowSecureServers() = 0;
	virtual void SendVProfLiteReportToServer( void *pReport ) = 0;
	// The boot config's GlobalThreadPoolMode, THREAD_POOL_CORE_TYPE_SPLIT when it is unset
	virtual ThreadPoolCoreTypeRequest_t GetGlobalThreadPoolMode() = 0;
	virtual void SetGlobalThreadPoolMode( ThreadPoolCoreTypeRequest_t nMode ) = 0;
	// Removes the stored mode from the boot config.
	virtual void ResetGlobalThreadPoolMode() = 0;
	virtual void PrintNetworkSummaryReport() = 0;
};

//-----------------------------------------------------------------------------
// Purpose: Engine implementation of IVEngineClient2.
//-----------------------------------------------------------------------------
class CEngineClient : public CTier4AppSystem< IVEngineClient2 >
{
public:
	// Set by SetRestrictClientCommands; ClientCommand applies it unless bUnrestricted is set
	bool m_bRestrictClientCommands;

	// Hashes of lower-case command names, added and removed by SetPredictedStringCommand
	CUtlSortVector< uint32 > m_PredictedStringCommands;
};

typedef IVEngineClient2 IVEngineClient;

#endif // CDLL_INT_H
