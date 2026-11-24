//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: Interface the client game module exposes to the engine
//
//===========================================================================//

#ifndef ISOURCE2CLIENT_H
#define ISOURCE2CLIENT_H

#ifdef _WIN32
#pragma once
#endif

#include "appframework/iappsystem.h"
#include "entityhandle.h"
#include "entity2/entityindex.h"
#include "splitscreenslot.h"
#include "inputsystem/buttoncode.h"
#include "tier1/utlvector.h"
#include "tier0/utlstring.h"

class CGlobalVarsBase;
class KeyValues;
class IDemoMessageSink;
class CGameNetworkableClass;
class CEngineGotvSyncPacket;
class CCommand;
class CUtlBuffer;
class CTransform;
class Vector;
class QAngle;
class ISceneView;
class ISceneWorld;
class IEntityReport;
class ILoopModePrerequisiteRegistry;
class GameSessionConfiguration_t;
class CRenderAttributes;
class CEconItemSystem;
struct FlattenedSerializerSpewField_t;
struct Entity2Networkable_t;
struct AudioState_t;
struct RenderViewport_t;
struct PipelineRenderTargetInfo_t;
struct GameClientCommandQueueState_t;

// AMNOTE: Slot names come from the CSource2Client vtable of Dota 2 2018 (libclient.dylib symbols), from the
// CEngineSoundServices / CToolService / CNetworkClientService forwarders of the engine that call them (Dota 2018
// libengine2.dylib symbols) and, where no symbol exists, from what the CS2 code does (client.dll, the calls in
// libengine2.so). Those last ones say so in their comment.
abstract_class ISource2Client : public IAppSystem
{
public:
	// NULL restores the module's own globals
	virtual void SetGlobals( CGlobalVarsBase *pGlobals ) = 0;

	// Forwards to the game network string tables
	virtual void InstallStringTableCallback( const char *pszTableName ) = 0;
	// Named by what it does: looks the first argument up in the client command table and runs it;
	// the network game client passes the player slot of the split screen user
	virtual bool ProcessClientCommand( int nPlayerSlot, const CCommand &args ) = 0;
	// Fires the "player_info" game event with the engine's player info of nPlayerIndex
	virtual void OnPlayerInfoChanged( int nPlayerIndex ) = 0;

	virtual void CreateMove( int nSlot, bool bActive ) = 0;
	// Named by what it does: resets the input state of the slot; called for every split screen slot that has no
	// pending prediction
	virtual void ResetInputState( CSplitScreenSlot nSlot ) = 0;
	// Named by what it does: moves the sub-tick input of slot 0 one tick back (subtracts 1.0 from every stored
	// fraction); called for every split screen slot when a tick is dropped
	virtual void AdvanceInputTick( CSplitScreenSlot nSlot ) = 0;

	// Number of the latest user command generated for nSlot
	virtual int GetLatestCommandNumber( CSplitScreenSlot nSlot ) = 0;
	// Named by what it does: same number, minus one while that latest command is still being built (state 2)
	virtual int GetLatestCompletedCommandNumber( CSplitScreenSlot nSlot ) = 0;
	// Named by what it does: number of user commands the controller asks to resend, capped at 150; slot 0 only
	virtual int GetUserCmdSendLimit( CSplitScreenSlot nSlot ) = 0;
	// Named by what it does: empty; called by the packet entities processing with the command the server
	// acknowledged
	virtual void OnServerAcknowledgedCommand( CSplitScreenSlot nSlot, int nCommandNumber, int nTick ) = 0;
	virtual void ProcessInput( CSplitScreenSlot nSlot, float flFrameTime, bool bActive ) = 0;
	virtual bool WriteUsercmdDeltaToBuffer( CSplitScreenSlot nSlot, void *pBuffer, int nFrom, int nTo ) = 0;
	// Serializes the user command nCommandNumber of nSlot into pBuffer as a delta from an empty command
	virtual void EncodeUserCmdToBuffer( CSplitScreenSlot nSlot, void *pBuffer, int nCommandNumber ) = 0;
	// Empty in the client
	virtual void DecodeUserCmdFromBuffer( CSplitScreenSlot nSlot, void *pBuffer, int nCommandNumber ) = 0;
	// Named by what it does: the time stored in user command nCommandNumber (0.0 when it doesn't exist)
	virtual double GetUserCmdTime( CSplitScreenSlot nSlot, int nCommandNumber ) = 0;

	// Named by what they do: the demo player calls them when playback is paused or resumed; both forward to the
	// game rules
	virtual void SetDemoPlaybackPaused( bool bPaused ) = 0;
	virtual void OnDemoPlaybackResumed() = 0;
	virtual bool GetEntity2Networkable( CEntityIndex nEntityIndex, Entity2Networkable_t &networkable ) = 0;
	// Named by what it does: the network game client creates a networked entity through it (CopyNewEntity)
	virtual void CreateEntity2Networkable( const void *pCreateInfo, Entity2Networkable_t &networkable ) = 0;
	// Named by what it does: hands a networked entity the delta reader dropped back to the client entity system
	virtual void ReleaseEntity2Networkable( Entity2Networkable_t &networkable, bool bImmediate ) = 0;
	virtual void *GetNetworkFieldChangeCallbackQueue() = 0;
	virtual void NotifyShouldTransmit( IEntityReport *pReport, void *pTransmitBits, int nNotifyType ) = 0;
	// Networkable classes of the client; the network game client builds its class dictionary from them
	virtual CUtlVector< CGameNetworkableClass * > *GetAllClasses() = 0;
	virtual bool ReceivedServerInfo( const GameSessionConfiguration_t &config, ILoopModePrerequisiteRegistry *pRegistry ) = 0;

	virtual void FrameStageNotify( int nStage ) = 0;

	virtual void HudVidInit() = 0;
	// Runs the client voice manager frame
	virtual void HudUpdate( bool bActive ) = 0;
	// Calls HudVidInit
	virtual void HudReset() = 0;
	// Called by INetworkClientService::OnLevelLoadingStarted, after CenterStringOff
	virtual void OnLevelLoadingStarted() = 0;
	// Empty in this build
	virtual void CenterStringOff() = 0;
	virtual void HudText( const char *pszMessage ) = 0;
	// Called after a split screen client was added or removed
	virtual void OnSplitScreenStateChanged() = 0;
	// Names of the Dota 2018 slots at this place; constants or empty in CS2
	virtual bool SupportsRandomMaps() = 0;
	virtual bool CanVoiceChat() = 0;
	virtual void OnFullEntityUpdate() = 0;
	virtual void RunVGUIFrame() = 0;

	virtual void VoiceStatus( int nEntityIndex, int nSplitScreenSlot, bool bTalking ) = 0;

	// Mouth (lip sync) entries of the client; FreeMouth does nothing for 0
	virtual uint16 AllocateMouth( int nEntityIndex ) = 0;
	virtual void FreeMouth( uint16 nMouth ) = 0;
	// The mouth of the entity when nEntityIndex is set, the mouth entry nMouth otherwise
	virtual void GetClientMouthInfo( void *pMouthInfo, CEntityIndex nEntityIndex, uint16 nMouth ) = 0;
	// Writes the listener origin and angles, then asks GetAudioStateVectors for the rest
	virtual void UpdateAudioState( AudioState_t *pState, CSplitScreenSlot nSlot ) = 0;
	// Empty in this build
	virtual void OnScreenSizeChanged( int nWidth, int nHeight ) = 0;
	virtual int IN_KeyEvent( CSplitScreenSlot nSlot, int nEventCode, ButtonCode_t nKeyNum, const char *pszCurrentBinding ) = 0;
	virtual void View_Render( const void *pRect ) = 0;
	// Called by the network game client when its signon state changes
	virtual void OnSignonStateChanged( int nSignonState ) = 0;
	virtual bool CanRecordDemo( void *pErrorMessage ) = 0;
	// Named by what it does: asked by the in-memory demo buffer for the contents of the demo it is about to load;
	// returns false in this build
	virtual bool LoadDemoFileFromMemory( const char *pszDemoFileName, void *pOut ) = 0;
	// Named by what it does: asked before a console command line is executed; refuses "jump" + "attack" binds and
	// lobby joins within 1.5 s
	virtual bool IsCommandAllowed( const char *pszCommand, int nSource ) = 0;
	// Both are empty in this build
	virtual void OnDemoRecordStart( const char *pszDemoBaseName, IDemoMessageSink *pSink ) = 0;
	virtual void OnDemoRecordStop( IDemoMessageSink *pSink ) = 0;

	// Fires the "demo_start" game event when pOutNames is NULL, otherwise appends a fixed list of names to it
	virtual void OnDemoPlaybackStart( const char *pszDemoBaseName, CUtlVector< CUtlString > *pOutNames, const void *pFullPacketInfo ) = 0;
	// Fires the "demo_stop" game event
	virtual void OnDemoPlaybackStop() = 0;
	// Named by what it does: hands the playback tick of the broadcast source to the demo manager of the client
	virtual void UpdateDemoPlaybackTick( int nTick ) = 0;
	virtual void SetupScrubbingData( bool bSignon, int nTick, int nMessageType, const void *pMessage ) = 0;
	// Named by what it does: logs "SetupScrubbingData[%d]%s: ConnectionInfo" and stores the connection of a player
	virtual void SetupScrubbingConnectionInfo( bool bSignon, int nTick, int nPlayerSlot, const void *pInfo, bool bFakePlayer, bool bHLTV ) = 0;
	// Named by what it does: reads the "AnimAssetData" block of the demo
	virtual void ReadDemoAnimAssetData( const void *pData, void *pOut ) = 0;
	virtual void ClearScrubbingData() = 0;
	// Fires the game event stored for the tick, if there is one
	virtual void OnScrubDemo( int nFromTick, int nToTick ) = 0;
	// Fires the "demo_skip" game event
	virtual void OnDemoSkip( int nPlaybackTick, int nSkipToTick, bool bInstantReplay ) = 0;
	// Asked by the demo player about a net message id while it removes messages for a skip; forwards to
	// IGameConfiguration::IsUserMessage
	virtual bool ShouldNopCmdDuringDemoSkip( int nMessageId ) = 0;
	virtual void InitInstantReplayData( void *pMessages, int nCount ) = 0;
	virtual void ClearInstantReplayData() = 0;
	// Dispatches the "UpdateProgressBar" UI event
	virtual bool UpdateProgressBar( float flProgress, const char *pszDescription ) = 0;
	virtual bool ShouldPauseForToolUI() = 0;
	virtual bool GetBugReportInfo( int nSlot, CUtlBuffer &buf, CUtlString &sTitle, CUtlString &sDescription ) = 0;
	// Notifies the key binding listeners registered for nButton and for pszNewBinding, then fires the
	// "keybind_changed" game event
	virtual void OnKeyBindingChanged( ButtonCode_t nButton, const char *pszOldBinding, const char *pszNewBinding ) = 0;
	virtual bool GetCameraPosition( CEntityIndex nEntityIndex, CSplitScreenSlot nSlot, CTransform *pTransform ) = 0;
	virtual void GetEyeAngles( CSplitScreenSlot nSlot, QAngle *pAngles ) = 0;
	// Forwards to the input system
	virtual void SnapEyeAngles( CSplitScreenSlot nSlot, const QAngle &angles ) = 0;
	// Named by what it does: true when the index has no entity or the entity is dormant
	virtual bool IsEntityDormantOrMissing( CEntityIndex nEntityIndex ) = 0;
	// Named by what it does: handle of the first split screen slot's local player pawn
	virtual CEntityHandle GetLocalPlayerPawnHandle() = 0;
	// Named by what it does: the index is the local controller, the local pawn or the local observer target
	virtual bool IsLocalPlayerEntity( CEntityIndex nEntityIndex ) = 0;

	// Per-entity queries of the sound system (CEngineSoundServices forwards them). For a player controller they
	// answer for its pawn
	virtual bool GetEntity2AbsOrigin( CEntityIndex nEntityIndex, Vector *pOrigin ) = 0;
	// Named by what it does: a float property of the entity, 1.0 without one
	virtual float GetEntityAudioScale( CEntityIndex nEntityIndex ) = 0;
	virtual void GetEntity2AbsAngles( CEntityIndex nEntityIndex, QAngle *pAngles ) = 0;
	// Named by what it does: two vectors of the entity (its eye origin and eye angles), zero without one
	virtual bool GetEntity2EyeOriginAndAngles( CEntityIndex nEntityIndex, Vector *pOrigin, QAngle *pAngles ) = 0;
	// Named by what it does: C_BaseEntity::m_iHealth and the max health virtual
	virtual bool GetEntity2Health( CEntityIndex nEntityIndex, int *pHealth, int *pMaxHealth ) = 0;
	// Named by what it does: handle of the local pawn, of the local controller without one
	virtual CEntityHandle GetLocalPlayerEntityHandle() = 0;
	// Named by what it does: forwards to the entity when it is a C_SoundEventEntity
	virtual void UpdateSoundEventEntity( CEntityIndex nEntityIndex, const void *pParams ) = 0;
	virtual bool GetEntity2AttachmentAbsOrigin( CEntityIndex nEntityIndex, Vector *pOrigin, int *pAttachmentToken ) = 0;
	// Named by what it does: the attachment transform, the entity origin and angles for token 0
	virtual bool GetEntity2AttachmentAbsTransform( CEntityIndex nEntityIndex, void *pTransform, int *pAttachmentToken ) = 0;
	// Named by what it does: C_BaseEntity::m_iTeamNum of the player in nPlayerSlot
	virtual int GetPlayerTeam( int nPlayerSlot ) = 0;
	// Named by what it does: three vectors of the entity filled by three of its accessors
	virtual void GetEntity2SoundVectors( CEntityIndex nEntityIndex, Vector *pFirst, Vector *pSecond, Vector *pThird ) = 0;
	// Named by what it does: C_BaseEntity::m_iTeamNum of the local player
	virtual int GetLocalPlayerTeam() = 0;
	// Named by what it does: the local observer services are in observer mode 4 (roaming)
	virtual bool IsLocalPlayerObserverRoaming() = 0;
	// Named by what it does: handle of the local observer target
	virtual CEntityHandle GetLocalObserverTargetHandle() = 0;
	// Zeroes both outputs; UpdateAudioState calls it for the last two vectors of the audio state
	virtual void GetAudioStateVectors( CSplitScreenSlot nSlot, Vector *pFirst, Vector *pSecond ) = 0;

	virtual bool FormatSerializerFieldValue( CEntityIndex nEntityIndex, FlattenedSerializerSpewField_t &field ) = 0;

	// Empty in this build
	virtual void PerformClientSpecificSetupForPlayerViews( ISceneView *pView, RenderViewport_t *pViewport, const PipelineRenderTargetInfo_t &info, CRenderAttributes *pAttributes ) = 0;
	virtual void *GetPlayerViewSetup( CSplitScreenSlot nSlot ) = 0;
	// Forwards to IGameConfiguration::GetBugReportLaunchURLParameters; the engine merges the KeyValues into its
	// launch options afterwards
	virtual void GetBugReportLaunchURLParameters( KeyValues *pKeyValues ) = 0;
	virtual void UpdateProjectedTexture( const void *pInfo, void **phTexture ) = 0;
	// Calls the deleting destructor of hTexture
	virtual void DeleteProjectedTexture( void *hTexture ) = 0;
	// Returns NULL and does nothing in this build
	virtual void *UpdateDeferredLight( ISceneWorld *pWorld, const void *pDesc, void **phLight ) = 0;
	virtual void DeleteDeferredLight( void *hLight ) = 0;
	virtual void *UpdateStandardLight( ISceneWorld *pWorld, const void *pDesc, void **phLight ) = 0;
	// Forwards to the scene system
	virtual void DeleteStandardLight( void *hLight ) = 0;
	// Named by what it does: the same as UpdateStandardLight for a light desc with a transform
	virtual void *UpdateTransformedLight( ISceneWorld *pWorld, const void *pDesc, const void *pTransform, void **phLight ) = 0;
	// Forwards to the scene system
	virtual void DeleteTransformedLight( void *hLight ) = 0;
	// Physics query with a CTraceFilter built from pFilter
	virtual bool TraceRay( const Vector &vecStart, const Vector &vecEnd, const void *pFilter, void *pTrace ) = 0;
	// Named by what it does: physics line query with a -1.0 radius
	virtual void TraceLine( const Vector &vecStart, const Vector &vecEnd, int nMask, void *pTrace ) = 0;
	virtual int GetPointContents( const Vector &vecPosition, int nMask ) = 0;
	// Writes 0 to pReflectivity and resets the trace
	virtual void GetSurfaceAudioReflectivity( void *pEntity, float *pReflectivity, void *pTrace ) = 0;
	// Looks the entity up and calls GetSurfaceAudioReflectivity
	virtual void GetEntity2SurfaceAudioReflectivity( CEntityIndex nEntityIndex, void *pTrace ) = 0;
	virtual void LaunchApplicationOnExit( const char *pszCommandLine ) = 0;
	// Returns its argument
	virtual const char *EconGetSoundReplacement( const char *pszSoundName ) = 0;
	virtual bool IsGroundUnderWater( float x, float y ) = 0;
	// 1.0 in this build
	virtual float CalculateOcclusion( const Vector &vecListener, const Vector &vecSource, float flRadius, float flLimit, float flOcclusion ) = 0;
	// Empty in this build
	virtual void GetAudioCameraPosition( Vector &vecPosition ) = 0;
	virtual void GetAudioCameraDirection( Vector &vecDirection ) = 0;
	// Increments a frame counter of the client
	virtual void NotifyPreSteamRunCallbacks() = 0;
	virtual bool VoiceReliable() = 0;
	virtual void UpdateWhenNotInGame( float flFrameTime ) = 0;
	// Empty / false in this build
	virtual void UpdateGlobalLightFromLightInfos( void *pGlobalLight, const void *pLightInfos, const Vector &vecDirection, int nLightType, float flBrightness, float flRange, float flAngle ) = 0;
	virtual bool GetGlobalLightParameters( int *pLightType, float *pBrightness, float *pRange ) = 0;
	virtual void OnReceivedUpdatedServerPeerList( const void *pPeers, uint32 nPeers ) = 0;
	// Forwards to the game event manager
	virtual void UpdateEventListeners() = 0;
	virtual CEconItemSystem *GetEconItemSystem() = 0;
	// Forwards to the game network string tables
	virtual void GameCreateNetworkStringTables() = 0;
	// Drops the game's pointers to its network string tables; the tables themselves belong to the engine
	virtual void ResetStringTablePointers() = 0;
	// Fires the "client_disconnect" game event
	virtual void OnDisconnect( int nReasonCode, const char *pszReason, CUtlString *pOutMessage ) = 0;
	virtual void GetModelCombineInfo( const void *hModel, void *pCombinedModel, int *pCombineIndex ) = 0;
	// Named by what they do: hand a save/restore block to the client entity system (restore and save side)
	virtual void RestoreSaveGameBlock( const char *pszBlockName, void *pSaveRestoreData, void *pBlock, int nVersion, void *pReserved ) = 0;
	virtual void SaveSaveGameBlock( const char *pszBlockName, void *pSaveRestoreData, void *pBlock, int nVersion, CUtlString *pError ) = 0;
	// Returns "core.global_stack"
	virtual const char *GetGlobalSoundStackName() = 0;
	// False in this build
	virtual bool ShouldSuppressNonUISounds() = 0;
	// Named by what it does: the crash handler prints "Steam UGC subscriptions: %u total; %u custom games" from it;
	// writes NULL and returns false in this build
	virtual bool GetUGCSubscriptionCounts( void **ppCounts ) = 0;
	// Named by what it does: 1 allows the download of the workshop addon, 2 refuses it (AddonConfig/AllowAddonDownload)
	virtual int GetAddonDownloadPermission( uint64 nPublishedFileId ) = 0;
	// Named by what it does: the demo player asks it for the addons a demo needs
	virtual bool GetAddonsForDemo( const char **ppszDemoFileName, int nCount, CUtlString *pAddons ) = 0;
	// Named by what it does: the entity list the crash handler dumps under "Client"
	virtual void *GetEntityListForCrashDump() = 0;
	// Returns the CToolClientSimulationAPI singleton
	virtual void *GetToolClientSimulationAPI() = 0;
	// Empty in this build; the scene renderer passes the render attributes and the view of a player view
	virtual void SetupPlayerViewRenderAttributes( CRenderAttributes *pAttributes ) = 0;
	virtual void OnPlayerViewCreated( ISceneView *pView ) = 0;
	// AMNOTE: No source names these three and they are constant in this build; named by their shape:
	// false, NULL (IToolService::GetToolClientEntityInterface forwards here) and a NULL handle
	// (IToolService::GetToolClientSelectedEntity forwards here)
	virtual bool IsToolClientActive() = 0;
	virtual void *GetToolClientEntityInterface() = 0;
	virtual CEntityHandle GetToolClientSelectedEntity() = 0;
	// Named by what it does: reconciles the predicted user commands of slot 0 with the command the server executed
	virtual void OnServerExecutedCommand( CSplitScreenSlot nSlot, int nAckedCommand, int nTick, int nCommandNumber ) = 0;
	// AMNOTE: Empty in this build and never called by the engine; no source names it
	virtual void EmptyClientCallback() = 0;
	// Named by what it does: the name of the entity, NULL for an invalid index
	virtual const char *GetEntityName( CEntityIndex nEntityIndex ) = 0;
	// Named by what it does: handle of the entity that owns the scene object of hEntity
	virtual CEntityHandle GetOwnerEntityHandle( CEntityHandle hEntity ) = 0;
	// Named by what it does: forwards to the client entity system; IToolService::UpdateClientEntityList forwards here
	virtual void UpdateClientEntityList() = 0;
	// Forwards to the game rules with the host frame timing of the engine
	virtual void OnHostFrameTiming( const void *pTiming ) = 0;
	// Named by what it does: a string of the game rules, NULL without game rules
	virtual const char *GetGameRulesDescription() = 0;
	// Dispatches the GameClientCommandQueueState_t UI event
	virtual void OnCommandQueueStateChanged( const GameClientCommandQueueState_t *pState ) = 0;
	// Named by what they do: add and remove a per entity record of the networked field change callbacks
	virtual int AddEntityFieldChangeRecord( CEntityIndex nEntityIndex, const int *pField, int nDefault ) = 0;
	virtual void RemoveEntityFieldChangeRecord( CEntityIndex nEntityIndex, const int *pField ) = 0;

	// Forward to the sound event manager of the client (CEngineSoundServices forwards them)
	virtual void *StartClientSoundEvent( int nSoundEvent, int nEntityIndex, void *pParams ) = 0;
	virtual void SetClientSoundEventParam( int nSoundEvent, int nParam, int nValue, void *pParams ) = 0;
	virtual void StopAllClientSoundEvents( int nReason ) = 0;

	// Named by what it does: handle of the pawn (or of the observer target) of the controller in nPlayerSlot
	virtual CEntityHandle GetPlayerViewEntityHandle( int nPlayerSlot ) = 0;
	// Named by what it does: returns a singleton of the client; IToolService::GetClientToolInterface forwards here
	virtual void *GetClientToolInterface() = 0;
	// Returns the singleton loaded from "scripts/replay_compatability_settings.txt"
	virtual void *GetReplayCompatibilitySettings() = 0;
	// Named by what it does: fills the state of the HLTV replay (time scale, position, flags); false without one
	virtual bool GetHltvReplayState( void *pState ) = 0;
	// HLTV replay state ("OnHltvReplay")
	virtual void OnHltvReplay( const void *pHltvReplayMsg ) = 0;
	// Called once from the tick message processing of the network game client after a pending flag was raised;
	// resets a flag of the HLTV replay state
	virtual void ClearHltvReplayPending() = 0;
	virtual void *GetHltvReplayManager() = 0;
	// Named by what it does: stores the message text and flag of the HLTV replay status message
	virtual void OnHltvReplayStatus( const void *pMsg ) = 0;
	// Named by what it does: the list of client side entities the network game client reconciles after a full update
	virtual void *GetClientSideEntityList() = 0;
	// Named by what it does: fills pOut with the default values of a networked class and returns it
	virtual void *GetNetworkedClassDefaults( void *pOut, const void *pClass ) = 0;
	// Named by what it does: the broadcast player calls it when the keyframe of a fragment is ready to play
	virtual void OnBroadcastKeyframeReady() = 0;
	// Called when the engine requests a GOTV broadcast sync; builds a game coordinator protobuf message from the packet
	virtual void RequestGotvSync( const CEngineGotvSyncPacket &packet ) = 0;
	// Fills matchmaking KeyValues ( "members/machine0/player0" )
	virtual void FillMatchmakingKeyValues( KeyValues *pKV, void *pContext ) = 0;
	// Named by what it does: handle of the first owner in the hierarchy of the entity that answers the
	// "is player" virtual
	virtual CEntityHandle GetPlayerOwnerHandle( CEntityIndex nEntityIndex ) = 0;
	// Designer name of the entity, its class name when that is empty, "" for an invalid index
	virtual const char *GetEntityClassname( CEntityIndex nEntityIndex ) = 0;
	// Compares pszClassName with the entity and then with every class of its hierarchy
	virtual bool EntityClassMatches( CEntityIndex nEntityIndex, const char *pszClassName ) = 0;

	// Forward to the input system or are empty
	virtual bool IsInputActive() = 0;
	virtual void OnInputActivated() = 0;
	virtual void OnInputDeactivated() = 0;
	// Named by what it does: the interpolation amount of the local player
	virtual float GetInterpolationAmount() = 0;
	// Forwards to an object owned by the client entity system; the result heads a list the entity delta reader walks
	virtual void *GetEntityDeltaList() = 0;
	// Named by what it does: the in-memory demo buffer hands the loaded demo bytes to it
	virtual void SetMemoryDemoData( const void *pData, int nSize ) = 0;

	// Named by what they do: process the CSVCMsg_UserCommands message of the server (the second also for bots)
	virtual void ProcessServerUserCommands( const void *pMsg ) = 0;
	virtual void ProcessServerUserCommandsForBots( const void *pMsg ) = 0;

	// Forward to the input system; the engine command buffer calls the next three around a batch of commands
	// (__beginseq / __endseq) and the input service asks the last one for every key binding
	virtual bool HasInputCommandOverride() = 0;
	virtual void BeginInputCommandSequence( CSplitScreenSlot nSlot, const int *pCommands ) = 0;
	virtual void EndInputCommandSequence( CSplitScreenSlot nSlot, const int *pCommands, int nCount ) = 0;
	virtual bool ProcessKeyBinding( ButtonCode_t nButton, const void *pBinding, const void *pEvent ) = 0;
	virtual void OnKeyBindingProcessed( int nSlot, const void *pBinding, const void *pEvent ) = 0;

	// Named by what it does: called after a spawn group of the server was loaded or unloaded
	virtual void OnSpawnGroupChanged( void *pSpawnGroup, int nSlot, int nSpawnGroupHandle ) = 0;
	// AMNOTE: The econ / character queries of the tools. Names of the 11 CToolService slots of Dota 2018 at the same
	// place of IToolService, which forwards here; the ones of Dota forward to the server. All are empty or constant
	// in CS2 (the scales return 1.0, the string outputs are cleared). GetDefaultControlPointAutoUpdates sits before
	// the scales here, after them in Dota. EmptyToolCallback, between them, is reached from IToolService slot 81
	// and has no source name
	virtual void GetEconItemNamesForModel( const char *pszModelName, bool bExcludeItemSets, bool bExcludeLockedItems, CUtlVector< CUtlString > &names ) = 0;
	virtual void GetEconItemNamesForCharacter( const char *pszCharacterName, bool bExcludeItemSets, bool bExcludeLockedItems, CUtlVector< CUtlString > &names ) = 0;
	virtual void GetEconItemsInfoForModel( const char *pszModelName, const char *pszSlot, bool bExcludeItemSets, bool bExcludeLockedItems, bool bExcludeDefaultItems, void *pItems ) = 0;
	virtual void GetEconItemsInfoForCharacter( const char *pszCharacterName, const char *pszSlot, bool bExcludeItemSets, bool bExcludeLockedItems, bool bExcludeDefaultItems, void *pItems ) = 0;
	virtual void EmptyToolCallback() = 0;
	virtual bool GetDefaultControlPointAutoUpdates( const char *pszParticleName, void *pAutoUpdates ) = 0;
	virtual float GetDefaultScaleForModel( const char *pszModelName ) = 0;
	virtual float GetDefaultScaleForCharacter( const char *pszCharacterName ) = 0;
	virtual void GetCharacterNameForModel( const char *pszModelName, int nUnused, void *pCharacterName ) = 0;
	virtual bool GetModelNameForCharacter( const char *pszCharacterName, int nIndex, CUtlString *pModelName ) = 0;
	virtual void GetCharacterList( CUtlVector< CUtlString > &characters ) = 0;
	virtual void GetDefaultChoreoDirForModel( const char *pszModelName, CUtlString *pDirectory ) = 0;
	// Named by what it does: true when the engine is not in game, plays a demo or is a dedicated client
	virtual bool ShouldDrawLoadingScreen() = 0;
	// Named by what they do: the network game client asks them for its networked class tables
	virtual void *GetNetworkedClassTable() = 0;
	virtual void *GetNetworkedClassFallback() = 0;
	// Named by what it does: the entity exists and answers the "is player pawn" virtual
	virtual bool IsPlayerPawn( CEntityIndex nEntityIndex ) = 0;
	// Named by what it does: handle of the controller of the pawn at nEntityIndex
	virtual CEntityHandle GetPawnControllerHandle( CEntityIndex nEntityIndex ) = 0;
	// Named by what it does: handle of the entity with the targetname pszName
	virtual CEntityHandle FindEntityHandleByName( const char *pszName ) = 0;
	// Named by what it does: entity indices of the entities that match pszName
	virtual void FindEntitiesByName( const char *pszName, int *pCount ) = 0;
	// Named by what it does: forwards a sound event of the entity to the sound manager of the client
	virtual void EmitEntitySoundEvent( int nEntityIndex, const void *pParams ) = 0;
	// C_CSPlayerPawn::m_flBigHeadScale of the player pawn at nEntityIndex, 1.0 for anything else
	virtual float GetBigHeadScale( CEntityIndex nEntityIndex ) = 0;
};

#endif // ISOURCE2CLIENT_H
