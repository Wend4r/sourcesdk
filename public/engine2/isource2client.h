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
#include "appframework/gameappsystem.h"
#include "iloopmode.h"
#include "entityhandle.h"
#include "entity2/entityindex.h"
#include "splitscreenslot.h"
#include "inputsystem/buttoncode.h"
#include "tier0/utlstring.h"
#include "tier0/utlstringtoken.h"
#include "tier0/utlvector.h"
#include "tier0/utlsortvector.h"
#include "tier0/utlmap.h"
#include "tier0/tslist.h"
#include "tier0/bitvec.h"
#include "steam/steamclientpublic.h"
#include "tier1/ns_address.h"
#include "resourcefile/resourcetype.h"
#include "spawngrouptypes.h"
#include "game_saverestore.h"
#include "networkbasetypes.pb.h"
#include "network_connection.pb.h"

#include <functional>

class CBufferString;
class CConcreteEntityList;
class CDmElement;
class CFieldPath;
class ConVarRefAbstract;
class ClientClass;
class CCommand;
class CEngineGotvSyncPacket;
class CEntityInstance;
class CGameTrace;
class CGlobalVarsBase;
class CPlayerSlot;
class CRenderAttributes;
class CSVCMsg_HltvReplay_t;
class CSVCMsg_UserCommands_t;
class CSceneObject;
class CSteamID;
class CThreadedJob;
class CTransform;
class CUtlBuffer;
class ICompositeMaterialManager;
class IDemoMessageSink;
class IEconItemSystem;
class InstantReplayMessage_t;
class IEntityReport;
class IGameEvent;
class ISkeletonDemoRecorder;
class ILoadingSpawnGroup;
class INetChannel;
class INetworkFieldChangedEventQueue;
class INetworkStringTableContainer;
class InfoForResourceTypeCTextureBase;
class IToolClientSimulationAPI;
class IToolObject;
class ISceneView;
class ISceneWorld;
class ISceneViewDebugOverlays;
class ISerializerMatchToMetaReplayCompatHelper;
class InfoForResourceTypeCModel;
class KeyValues;
class QAngle;
class Vector;
class Vector4D;
class bf_read;
struct AudioState_t;
struct ClientSideNetworkedEntityDesc_t;
struct EconItemInfo_t;
struct Entity2Networkable_t;
struct FlattenedSerializerSpewField_t;
struct FlattenedSerializerUserData_t;
struct GameClientCommandQueueState_t;
struct LightDesc_t;
struct matrix3x4_t;
class matrix3x4a_t;
struct ParticleProjectedTextureInfo_t;
struct PipelineRenderTargetInfo_t;
struct RenderViewport_t;
struct RnQueryAttr_t;
struct vrect_t;
union CVValue_t;

template < class T > class CWeakHandle;
template < class T > class CStrongHandleCopyable;

FORWARD_DECLARE_HANDLE( HDeferredLight );
FORWARD_DECLARE_HANDLE( HProjectedTexture );

// Stage of the client frame passed to ISource2Client::FrameStageNotify(). The names that the client frame of Source 1
// already had keep them; the others are named after the engine code that notifies them
enum ClientFrameStage_t : int
{
	// The client's current stage before the first notification
	FRAME_UNDEFINED = -1,

	// ProcessInput() enters it before the input of the frame is processed
	FRAME_START,
	// The network game client starts ClientPollNetworking()
	FRAME_CLIENT_POLL_NETWORKING,
	// The network game client starts ClientProcessNetworking(), before it processes the socket messages
	FRAME_CLIENT_PROCESS_NETWORKING_START,
	// ClientProcessNetworking() processed the socket messages of a connected net channel
	FRAME_CLIENT_PROCESS_NETWORKING_END,

	// A non-incremental update is received; the client clears its networked entities
	FRAME_NET_FULL_FRAME_UPDATE_ON_REMOVE,
	// A packet entities message is being processed
	FRAME_NET_UPDATE_START,
	// The entities have been read and PostDataUpdate() calls start
	FRAME_NET_UPDATE_POSTDATAUPDATE_START,
	// PostDataUpdate() has been called on all entities
	FRAME_NET_UPDATE_POSTDATAUPDATE_END,
	// The packet entities message has been processed
	FRAME_NET_UPDATE_END,

	// A client spawn group is forced to load synchronously because of its creation tick
	FRAME_SPAWN_GROUP_FORCE_BLOCKING_LOAD,
	// The network game client's OnServerPostSimulate() of a local server; the client starts restoring the server authoritative state
	FRAME_SERVER_POST_SIMULATE,
	// The network game client's OnServerBeginAsyncPostTickWork()
	FRAME_SERVER_BEGIN_ASYNC_POST_TICK_WORK,
	// The network game client's OnClientFrameSimulate() once the signon state is past SIGNONSTATE_PRESPAWN
	FRAME_CLIENT_FRAME_SIMULATE,
};

// Result of ISource2Client::DownloadWorkshopAddon(), polled by the engine while it loads a workshop map
enum WorkshopAddonDownloadStatus_t : int
{
	// The download is running; the engine asks again
	WORKSHOP_ADDON_DOWNLOAD_IN_PROGRESS = 0,
	// The addon is installed, or there is nothing to download; the engine continues loading
	WORKSHOP_ADDON_DOWNLOAD_COMPLETE,
	// The download failed or timed out ( cl_workshop_map_download_timeout ); the engine goes idle
	WORKSHOP_ADDON_DOWNLOAD_FAILED,
};

// Identifies a sound area registered with the client
class SoundAreaGuid_t
{
public:
	SoundAreaGuid_t() : m_Data( 0 ) {}
	SoundAreaGuid_t( int32 nGuid ) : m_Data( nGuid ) {}

	int32 m_Data;
};

// Installed by the client as the change callback time limit spew of the entity system
abstract_class IFieldChangeLimitSpew
{
public:
	// Called by CNetworkFieldChangedEventQueue::InvokeChangeCallbacks() for a change callback that ran longer than the
	// time limit, with the object id, field name and field path of the queued change; empty in the client
	virtual void OnChangeCallbackTimeLimitExceeded( int nObjectId, const char *pszFieldName, const CFieldPath &path ) = 0;
};

// Sequence index of a model, -1 for none
class HSequence
{
public:
	HSequence() : m_Value( -1 ) {}
	HSequence( int nValue ) : m_Value( nValue ) {}

	int m_Value;
};

// A character with one of its personas, as the tools list them through ISource2Client::GetCharacterList() and pass them
// to the methods for characters; sorted by name and then by persona
struct CharacterInfo_t
{
	CUtlString m_sName;
	int8 m_nPersona;
};

// Filled by ISource2Client::GetMouthInfo() from the mouth (lip sync) system
struct MouthInfo_t
{
	// Mouth id, 0 for none
	uint16 m_nMouth;
	// Mouth openness the mouth system keeps for it
	uint8 m_nMouthOpen;
};

// A tick with the fraction of the tick that has passed, normalized to [0, 1)
struct GameTickFraction_t
{
	int m_nTick;
	float m_flFraction;
};

// Given by the client to the engine for demos recorded before the serializers carried their own encoding parameters
struct LegacyNetworkEncoding_t
{
	// World extent for Worldsize_SetSessionMaxCoord(); 0 keeps the game info one
	float m_flMaxCoord;

	// Pairs of a quantized float encoder alias name and its QuantizedFloatEncoderDef_t (32 bytes each), applied
	// through OverrideQuantizedFloatEncoderAlias()
	const void *m_pEncoderAliases;
	int m_nEncoderAliasCount;

	// Whether the coord size parameters below are set; they mirror ProtoCoordSizeParams_t
	bool m_bHasCoordSizeParams;
	int m_nCoordIntegerBits;
	int m_nCoordFractionalBits;
	int m_nCoordIntegerBitsMP;
	int m_nCoordFractionalBitsMP;
	int m_nNormalFractionalBits;
	int m_nAngleBits;
};

abstract_class ISource2Client : public IAppSystem, public IFieldChangeLimitSpew
{
public:
	// Like ShouldTransmitState_t: START makes the entities whose transmit bits were set active, END makes the entities
	// whose bits were cleared dormant ( cl_dormant_spew "Entity %d: %s became %s on client" )
	enum NotifyShouldTransmitType_t : int
	{
		SHOULDTRANSMIT_START = 0,
		SHOULDTRANSMIT_END
	};

	// Built by the engine from each CMsgServerPeer of a peer list message
	struct PeerListEntry_t
	{
		// The peer's SteamID with the universe of the local user on Steam channel 3; zero without a SteamID. The client
		// skips addresses that are not P2P_STEAMID and hands the others to the network P2P service as peers
		CPeerToPeerAddress m_Address;

		int m_nPlayerSlot;
		bool m_bYouHearThem;
		bool m_bTheyHearYou;
		bool m_bIsListenServerHost;
	};

	// NULL restores the module's own globals. Returns the previous globals, or NULL when they were the module's own
	virtual CGlobalVarsBase *SetGlobals( CGlobalVarsBase *pGlobals ) = 0;

	// Called after the engine installed the callbacks of its own string tables, with the same flag
	virtual void InstallStringTableCallback( bool bInvoke ) = 0;
	// Queues a predicted string command event for the client. A non-zero result other than -1 is sent to the server as the prediction sync of the command
	virtual int PredictStringCommand( CPlayerSlot nSlot, const CCommand &args ) = 0;
	virtual void PlayerInfoChanged( int nPlayerIndex ) = 0;

	virtual void CreateMove( int nSlot, bool bActive ) = 0;
	// Forward to the input system; the engine calls them for every split screen slot while it builds the move message.
	// The first one starts the move of the next tick from the latest user command, the second one applies the queued
	// subtick moves of the current tick; both shift the remaining subtick moves back by one tick
	virtual void BeginNextTickMove( CSplitScreenSlot nSlot ) = 0;
	virtual void ApplyQueuedSubtickMoves( CSplitScreenSlot nSlot ) = 0;

	// Latest user command number the client generated
	virtual int GetLatestCommandNumber( CSplitScreenSlot nSlot ) = 0;
	// The latest command number, minus one while the state of that command in the history is 2
	virtual int GetLatestFinishedCommandNumber( CSplitScreenSlot nSlot ) = 0;
	// Number of commands kept in the command history: the latest command number, at most the history size of 150
	virtual int GetStoredCommandCount( CSplitScreenSlot nSlot ) = 0;
	// Called with the executed and received command numbers of a packet entities message; empty
	virtual void OnCommandNumbersAcknowledged( CSplitScreenSlot nSlot, int nLastCommandNumberExecuted, int nLastCommandNumberReceived ) = 0;
	// Forwards to the input system with absolute queries allowed
	virtual void ProcessInput( CSplitScreenSlot nSlot, float flFrameTime, bool bActive ) = 0;
	// Writes the user commands nFrom through nTo, each one delta encoded against the previous one
	virtual bool WriteUsercmdDeltaToBuffer( CSplitScreenSlot nSlot, CUtlBuffer *pBuffer, int nFrom, int nTo ) = 0;
	virtual void EncodeUserCmdToBuffer( CSplitScreenSlot nSlot, CUtlBuffer *pBuffer, int nCommandNumber ) = 0;
	virtual void unk025() = 0;
	// Time stored with the command in the command history, 0.0 when it is no longer kept
	virtual double GetCommandTime( CSplitScreenSlot nSlot, int nCommandNumber ) = 0;

	// Forward to the game rules: the first one logs "paused on tick" and "unpaused on tick", the second one clears the pause ticks.
	// The engine calls them for the demo player's pause state
	virtual void SetPaused( bool bPaused ) = 0;
	virtual void ClearPauseTicks() = 0;
	// Forward to the network classes of the client entity system
	virtual bool GetEntity2Networkable( CEntityIndex nEntityIndex, Entity2Networkable_t &networkable ) = 0;
	// The engine creates each new entity of a packet through it
	virtual bool CreateClientSideNetworkedEntity( const ClientSideNetworkedEntityDesc_t *pDesc, Entity2Networkable_t *pNetworkable ) = 0;
	// Queues the entity handle with a time when bDeferred is set and the entity qualifies, otherwise forwards to the network classes
	virtual void DestroyClientSideNetworkedEntity( const Entity2Networkable_t *pNetworkable, bool bDeferred ) = 0;
	virtual INetworkFieldChangedEventQueue *GetNetworkFieldChangeCallbackQueue() = 0;
	// Called with SHOULDTRANSMIT_START and then with SHOULDTRANSMIT_END while the engine processes packet entities
	virtual void NotifyShouldTransmit( IEntityReport *pReport, CBitVec< 16384 > *pTransmitBits, NotifyShouldTransmitType_t nType ) = 0;
	virtual CUtlVector< ClientClass * > *GetAllClasses() = 0;
	// Copies the session configuration and asks the loop mode game shared state for it
	virtual bool ReceivedServerInfo( const GameSessionConfiguration_t &config, ILoopModePrerequisiteRegistry *pRegistry ) = 0;

	virtual void FrameStageNotify( ClientFrameStage_t nStage ) = 0;

	virtual int HudVidInit() = 0;
	virtual void HudUpdate( bool bActive ) = 0;
	virtual void HudReset() = 0;
	// Called by the network client service after CenterStringOff()
	virtual void OnLevelLoadingStarted() = 0;
	virtual void CenterStringOff() = 0;
	virtual void HudText( const char *pszMessage ) = 0;
	virtual void OnSplitScreenStateChanged() = 0;
	virtual bool unk044() = 0;
	virtual bool unk045() = 0;
	virtual void unk046() = 0;
	virtual void unk047() = 0;

	virtual void VoiceStatus( int nEntityIndex, int nSplitScreenSlot, bool bTalking ) = 0;

	// Forward to the mouth (lip sync) system; the first one allocates a mouth for a playing sound event and returns its id.
	// The last one looks the mouth up through the entity when nEntityIndex is set, otherwise through the sound event
	virtual uint16 AllocateMouth( SoundEventGuid_t guid ) = 0;
	virtual void FreeMouth( uint16 nMouth ) = 0;
	virtual void GetMouthInfo( MouthInfo_t *pInfo, CEntityIndex nEntityIndex, SoundEventGuid_t guid ) = 0;
	// Fills the listener origin and angles, then the two vectors that follow through unk098()
	virtual void UpdateAudioState( AudioState_t *pAudioState, CSplitScreenSlot nSlot ) = 0;
	virtual void OnScreenSizeChanged( int nOldWidth, int nOldHeight ) = 0;
	// nEventCode is 1 for a pressed and 0 for a released button; returns 0 when the client swallowed the key
	virtual int IN_KeyEvent( CSplitScreenSlot nSlot, int nEventCode, ButtonCode_t nButtonCode, const char *pszCurrentBinding ) = 0;
	virtual void View_Render( vrect_t *pRect ) = 0;
	// Called when the signon state of the network game client changes; the client acts on SIGNONSTATE_FULL
	virtual void NotifyClientSignon( SignonState_t nSignonState ) = 0;
	virtual bool CanRecordDemo( CBufferString *pErrorMsg ) const = 0;
	// Asked by the engine's in-memory demo buffer for an 8-byte key; on true the engine decrypts the demo file in 8-byte blocks with it.
	// Returns false
	virtual bool GetDemoEncryptionKey( const char *pszDemoFileName, uint8 *pKey ) = 0;
	// Filters console commands; the engine passes 0 as the second argument
	virtual bool FilterCommand( const char *pszCommand, int nUnk ) = 0;
	virtual void OnDemoRecordStart( const char *pszDemoBaseName, IDemoMessageSink *pSink ) = 0;
	virtual void OnDemoRecordStop( IDemoMessageSink *pSink ) = 0;

	// With pStringTableNames, fills it with the names of the string tables the engine looks up while skipping
	// ("userinfo", "instancebaseline", ...), applies pOptions and clears the scrubbing data; without, fires the "demo_start" game event
	virtual void OnDemoPlaybackStart( const char *pszFilename, CUtlVector< CUtlString > *pStringTableNames, KeyValues *pOptions ) = 0;
	// Fires the "demo_stop" game event
	virtual void OnDemoPlaybackStop() = 0;
	// Called by the broadcast player once it plays the first keyframe of a broadcast, with NULL; sets up the demo controller
	virtual void OnBroadcastPlaybackStarted( void *pUnk ) = 0;
	virtual void SetupScrubbingData( bool bSignon, int nTick, int nMessageId, bf_read *pBuffer ) = 0;
	// Logged as the connection info of the scrubbing data
	virtual void AddPlayerConnectionEvent( bool bSignon, int nTick, int nConnectionInfo, CSteamID steamID, bool bBot, bool bHltv ) = 0;
	// Reads the "AnimAssetData" string table of the demo's string tables into pAnimAssets
	virtual void ReadAnimAssetData( INetworkStringTableContainer *pStringTables, void *pAnimAssets ) = 0;
	// Frees the game events kept for scrubbing and clears the player connection events
	virtual void ClearScrubbingData() = 0;
	// Fires the game events kept for nTick; the engine passes the tick the skip started from first
	virtual void OnScrubDemo( int nSkipStartTick, int nTick ) = 0;
	// Fires the "demo_skip" game event
	virtual void OnDemoSkip( int nPlaybackTick, int nSkipToTick, bool bInstantReplay ) = 0;
	// Asked by the demo player for each message removed while skipping
	virtual bool ShouldNopCmdDuringDemoSkip( int nCmd ) = 0;
	// Serializes the message into a new entry of the instant replay data
	virtual void InitInstantReplayData( InstantReplayMessage_t *pMessage, int nTick ) = 0;
	virtual void ClearInstantReplayData() = 0;
	virtual bool UpdateProgressBar( float flProgress, const char *pszDescription ) = 0;
	virtual bool unk075() = 0;
	// Enumerated by the engine while gathering state for a bug report; returns false past the last attachment
	virtual bool GetBugReportInfo( int nIndex, CUtlBuffer &buf, CUtlString &sName, CUtlString &sDescription ) = 0;
	// Fires the "keybind_changed" game event
	virtual void OnKeyBindingChanged( ButtonCode_t nButtonCode, const char *pszUnk1, const char *pszUnk2 ) = 0;
	virtual bool GetCameraPosition( CEntityIndex nEntityIndex, CSplitScreenSlot nSlot, CTransform *pTransform ) = 0;
	virtual void GetEyeAngles( CSplitScreenSlot nSlot, QAngle *pAngles ) = 0;
	virtual void SnapEyeAngles( CSplitScreenSlot nSlot, const QAngle &angles ) = 0;
	// True when the index has no entity or the entity is dormant
	virtual bool IsEntityDormant( CEntityIndex nEntityIndex ) = 0;
	// Handle of the first split screen slot's local player pawn
	virtual CEntityHandle GetLocalPlayerPawnHandle() = 0;
	// True for the local player controller and pawn and for the player the local pawn observes
	virtual bool IsLocalOrObservedPlayer( int nEntityIndex ) = 0;

	// Per-entity queries
	virtual bool GetEntity2AbsOrigin( CEntityIndex nEntityIndex, Vector *pOrigin ) = 0;
	virtual float GetEntity2AbsScale( CEntityIndex nEntityIndex ) = 0;
	virtual void GetEntity2AbsAngles( CEntityIndex nEntityIndex, QAngle *pAngles ) = 0;
	// Zeroes both vectors first
	virtual bool GetEntity2WorldBounds( CEntityIndex nEntityIndex, Vector *pvecMins, Vector *pvecMaxs ) = 0;
	virtual bool GetEntity2Health( CEntityIndex nEntityIndex, int *pHealth, int *pMaxHealth ) = 0;
	// Player slot of the pawn the local player observes in the in-eye or chase mode, otherwise of the local player pawn
	virtual CPlayerSlot GetLocalOrObservedPlayerSlot() = 0;
	// Only for sound event entities
	virtual void GetSoundEventEntityAbsOrigin( CEntityIndex nEntityIndex, Vector *pOrigin ) = 0;
	// Falls back to GetEntity2AbsOrigin() when the attachment token is 0
	virtual bool GetEntity2AttachmentAbsOrigin( CEntityIndex nEntityIndex, Vector *pOrigin, CUtlStringToken &attachment ) = 0;
	// Builds the transform from GetEntity2AbsOrigin() and GetEntity2AbsAngles() when the attachment token is 0
	virtual bool GetEntity2AttachmentTransform( CEntityIndex nEntityIndex, CTransform *pTransform, CUtlStringToken &attachment ) = 0;
	virtual uint8 GetEntity2TeamNum( CEntityIndex nEntityIndex ) = 0;
	virtual void GetEntity2Vectors( CEntityIndex nEntityIndex, Vector *pvecForward, Vector *pvecLeft, Vector *pvecUp ) = 0;
	virtual uint8 GetLocalPlayerTeamNum() = 0;
	virtual bool IsLocalPlayerObserverRoaming() = 0;
	virtual CEntityHandle GetLocalPlayerObserverTarget() = 0;
	// Zeroes both outputs
	virtual void unk098( CSplitScreenSlot nSlot, Vector *pvecUnk1, Vector *pvecUnk2 ) = 0;

	virtual bool FormatSerializerFieldValue( CEntityIndex nEntityIndex, FlattenedSerializerSpewField_t &field ) = 0;

	// pView is the "Player %d" view the engine adds through ISceneSystem::AddView()
	virtual void PerformClientSpecificSetupForPlayerViews( ISceneView *pView, RenderViewport_t *pViewport, const PipelineRenderTargetInfo_t &info, CRenderAttributes *pAttributes ) = 0;
	virtual void *GetPlayerViewSetup( CSplitScreenSlot nSlot ) = 0;
	virtual void GetBugReportLaunchURLParameters( KeyValues *pKV ) = 0;
	// Creates the projected texture when hTexture is NULL
	virtual void UpdateProjectedTexture( const ParticleProjectedTextureInfo_t &info, HProjectedTexture &hTexture ) = 0;
	// Calls the deleting destructor of hTexture
	virtual void DeleteProjectedTexture( HProjectedTexture hTexture ) = 0;
	virtual void *UpdateDeferredLight( ISceneWorld *pSceneWorld, CWeakHandle< InfoForResourceTypeCTextureBase > hTexture, Vector &vecUnk1, float flUnk1, Vector4D &vecUnk2, float flUnk2, float flUnk3, float flUnk4, Vector vecUnk3, float flUnk5, float flUnk6, float flUnk7, float flUnk8, float flUnk9, HDeferredLight &hLight ) = 0;
	virtual void DeleteDeferredLight( HDeferredLight hLight ) = 0;
	// Creates the light through ISceneSystem::CreateLight() when hLight is NULL, otherwise copies desc into it
	virtual CSceneObject *UpdateStandardLight( ISceneWorld *pSceneWorld, const LightDesc_t &desc, HDeferredLight &hLight ) = 0;
	// Deletes the scene object of hLight
	virtual void DeleteStandardLight( HDeferredLight hLight ) = 0;
	// Same as UpdateStandardLight() for a barn light, which also takes the transform of the scene object
	virtual CSceneObject *UpdateBarnLight( ISceneWorld *pSceneWorld, const void *pBarnLightDesc, const matrix3x4_t &transform, HDeferredLight &hLight ) = 0;
	// Deletes the scene object of hLight
	virtual void DeleteBarnLight( HDeferredLight hLight ) = 0;
	// Traces a line through the game physics query with a filter built from attr; resets the trace without one
	virtual bool TraceLine( const Vector &vecStart, const Vector &vecEnd, const RnQueryAttr_t &attr, CGameTrace *pTrace ) = 0;
	// Forward to the game physics query methods of the same names, the first one with a -1.0 default
	virtual void TraceLineIntervals( void *pIntervals, const Vector &vecStart, const Vector &vecEnd, const RnQueryAttr_t &attr ) = 0;
	// The CONTENTS_* bits of the game physics at the position; nMask holds the CONTENTS_* bits to query ( MASK_* )
	virtual uint64 GetPointContents( const Vector &vecPosition, int nMask ) = 0;
	// Overloads: the first one looks the entity up and calls the second one. Linux keeps this order, MSVC reverses it
	virtual void GetSoundSpatialization( CEntityIndex nEntityIndex, void *pInfo ) = 0;
	virtual void GetSoundSpatialization( CEntityInstance *pEntity, uint64 *pUnk, void *pInfo ) = 0;
	virtual void LaunchApplicationOnExit( const char *pszValue ) = 0;
	// Asked through the engine sound services; returns its argument
	virtual const char *DotaEconGetSoundReplacement( const char *pszSoundName ) = 0;
	// Asked through the engine sound services; false
	virtual bool DotaIsGroundUnderWater( float flX, float flY ) = 0;
	// Asked by the sound system through the engine sound services: the first one by the "calc_audibility" operator
	// (returns 1.0), the others by the "citadel_entity_info" and "citadel_point_effect_info" operators (empty)
	virtual float CalculateAudibility( const Vector &vecSource, const Vector &vecListener, const char *pszCaller, float flRadius ) = 0;
	virtual void GetCitadelEntityInfo( int nEntityIndex, float *pflRelevance, bool *pbEnemyIsTargetingLocalPlayer, bool *pbEnemyIsPrimaryTargetingLocalPlayer, float *pflEnclosure, bool *pbHasSkyExposure, bool *pbIsNPC, bool *pbIsShrunken ) = 0;
	virtual void GetCitadelPointEffectInfo( const Vector &vecPosition, void *pUnk, int *pnEffect, bool *pbEffectMatchesListener ) = 0;
	// Increments the Steam callback counter
	virtual void NotifyPreSteamRunCallbacks() = 0;
	// False
	virtual bool VoiceReliable() = 0;
	virtual void UpdateWhenNotInGame( float flFrameTime ) = 0;
	virtual void UpdateGlobalLightFromLightInfos( IToolObject *pUnk, const CUtlVector< IToolObject * > &lightInfos, const Vector &vecUnk, int nUnk, float flUnk1, float flUnk2, float flUnk3 ) = 0;
	// False in the client; SFM records the results as "weatherType", "timeOfDayAmount" and "smoothedTimeOfDayAmount"
	virtual bool GetGlobalLightParameters( int *pnWeatherType, float *pflTimeOfDayAmount, float *pflSmoothedTimeOfDayAmount ) = 0;
	virtual void OnReceivedUpdatedServerPeerList( PeerListEntry_t *pPeerList, size_t nPeerCount ) = 0;
	virtual void UpdateEventListeners() = 0;
	// The CCStrike15ItemSystem singleton
	virtual IEconItemSystem *GetEconItemSystem() = 0;
	// Forwards to the game network string tables
	virtual bool GameCreateNetworkStringTables() = 0;
	// Fires the "client_disconnect" game event with the reason as "reason_code"
	virtual void NotifyDisconnect( ENetworkDisconnectionReason reason ) = 0;
	// Releases *phBaseModel and clears *piBaseSkin
	virtual void GetModelCombineInfo( CWeakHandle< InfoForResourceTypeCModel > hCombinedModel, CStrongHandleCopyable< InfoForResourceTypeCModel > *phBaseModel, int *piBaseSkin, const CUtlVector< CStrongHandleCopyable< InfoForResourceTypeCModel > > **ppVecAdditionalModels, const CUtlVector< int > **ppVecSkins ) = 0;
	// Called by the client spawn group's SaveRestoreMap() and SaveRestoreMap2() with its loading spawn group; forward to the
	// entity system save/restore when ILoadingSpawnGroup::ShouldLoadEntitiesFromSave() and ILoadingSpawnGroup::IsLevelTransition() are set
	// The save/restore keeps hManifest locked through IGameResourceService::LockGameResourceManifest() while it restores
	virtual void SaveRestoreMap( HGameResourceManifest hManifest, ILoadingSpawnGroup *pLoading, SpawnGroupHandle_t hSpawnGroup, const matrix3x4a_t &vecSpawnOffset ) = 0;
	virtual void SaveRestoreMap2( HGameResourceManifest hManifest, ILoadingSpawnGroup *pLoading, SpawnGroupHandle_t hSpawnGroup, const matrix3x4a_t &vecSpawnOffset ) = 0;
	// Returns "core.global_stack"
	virtual const char *GetGlobalSoundStackName() = 0;
	// The sound system starts the GetGlobalSoundStackName() sound event only while this returns true; false
	virtual bool ShouldStartGlobalSoundStack() = 0;
	// Total and custom game Steam UGC subscription counts for the crash dump comment; zeroes them and returns false
	virtual bool GetUGCSubscriptionCounts( uint32 *pnSubscriptionCounts ) = 0;
	// Asked for the addon id when "AddonConfig/AllowAddonDownload" is set
	virtual WorkshopAddonDownloadStatus_t DownloadWorkshopAddon( uint64 nAddonId ) = 0;
	// Looks the map name up in the workshop map table and formats the file id kept for nPatchVersion; false for the
	// running patch version or an unknown map
	virtual bool GetWorkshopMapFileIdForVersion( const char **ppszMapName, int nPatchVersion, CUtlString *pFileId ) = 0;
	// Entity list of the client entity system, summarized in the crash dump comment
	virtual CConcreteEntityList *GetEntityList() = 0;
	// Returns the CToolClientSimulationAPI singleton
	virtual IToolClientSimulationAPI *GetToolClientSimulationAPI() = 0;
	// The engine render view setup calls them before PerformClientSpecificSetupForPlayerViews(), the first one with the same attributes
	virtual void SetupPlayerViewRenderAttributes( CRenderAttributes *pAttributes ) = 0;
	// The second one gets the render attributes of the "Player %d" scene view (ISceneView slot 55); empty in the client
	virtual void SetupPlayerSceneViewRenderAttributes( CRenderAttributes *pAttributes ) = 0;
	virtual bool unk144() = 0;
	// SFM reads the "modelID" attribute of the element the first one returns for an entity and builds the model from it with
	// the second one; NULL and an empty handle in the client
	virtual CDmElement *GetEntityModelElement( CEntityHandle hEntity ) = 0;
	virtual CWeakHandle< InfoForResourceTypeCModel > GetModelFromElement( CDmElement *pElement ) = 0;
	// Duplicates the user command nCommandNumber until the newest command reaches nLastExecutedCommand ("Duplicating command %d %dx to catch up")
	virtual void DuplicateCommandToCatchUp( CSplitScreenSlot nSlot, int nLastExecutedCommand, int nServerTick, int nCommandNumber ) = 0;
	virtual void unk148() = 0;
	virtual const char *GetEntity2UniqueHammerID( CEntityIndex nEntityIndex ) = 0;
	// Current sequence of the animation controller of the entity's skeleton, -1 without one
	virtual HSequence GetEntitySequence( CEntityHandle hEntity ) = 0;
	// Appends the values of CEntitySystem::m_EntityMaterialAttributes; the material editor lists them as entity render attributes
	virtual void GetEntityMaterialAttributes( CUtlVector< void * > *pAttributes ) = 0;
	// Forwards to the game rules; the engine passes its host frame timings, more of them than ISource2Server::OnHostFrameTiming() gets
	virtual void OnHostFrameTiming( const double *pFrameTimes ) = 0;
	virtual const char *GetFriendlyMapName() = 0;
	// Fires a panorama event with the state
	virtual void OnCommandQueueStateChanged( const GameClientCommandQueueState_t &state ) = 0;
	// Read the attribute table of the entity identity like CEntityIdentity::Attribute_GetIntValue() and Attribute_GetFloatValue()
	virtual int GetEntity2AttributeIntValue( CEntityIndex nEntityIndex, const CUtlStringToken &key, int nDefault ) = 0;
	virtual float GetEntity2AttributeFloatValue( CEntityIndex nEntityIndex, const CUtlStringToken &key, float flDefault ) = 0;

	// Back the "sound_area_register", "sound_area_contains_point" and "sound_area_unregister" sound operators
	virtual SoundAreaGuid_t RegisterSoundArea( const Vector &vecPosition, float flRadius, const char *pszAreaType ) = 0;
	// The "any" area type matches every sound area
	virtual void IsPointInSoundArea( const Vector &vecPosition, const char *pszAreaType, bool *pbPointInSoundArea, bool *pbSoundAreasExist ) = 0;
	virtual void UnregisterSoundArea( SoundAreaGuid_t guid ) = 0;

	virtual CEntityHandle GetPlayerControllerHandle( CPlayerSlot nSlot ) = 0;
	// The CCompositeMaterialManager singleton
	virtual ICompositeMaterialManager *GetCompositeMaterialManager() = 0;
	// The engine renames classes through it while it links classes for replay compatibility ("scripts/replay_compatability_settings.txt")
	virtual ISerializerMatchToMetaReplayCompatHelper *GetReplayCompatHelper() = 0;
	// The engine asks for it when a demo's server info or serializers carry no encoding parameters
	virtual bool GetLegacyNetworkEncoding( LegacyNetworkEncoding_t *pEncoding ) = 0;
	virtual void OnHltvReplay( const CSVCMsg_HltvReplay_t *pMsg ) = 0;
	// Forward to the HLTV replay system, which OnHltvReplay() feeds
	virtual void OnHltvReplayTick() = 0;
	virtual int GetHltvReplayDelay() = 0;
	virtual void unk167( void *pMsg ) = 0;
	// Scene view debug overlays of the client CDebugOverlayGameSystem; the engine passes received CNETMsg_DebugOverlay messages to it
	virtual ISceneViewDebugOverlays *GetDebugOverlayGameSystem() = 0;
	// CPrediction::m_nPredictedTickBase as a tick and fraction
	virtual GameTickFraction_t GetPredictionTickFraction() = 0;
	// Called by the broadcast player each time it has played a keyframe fragment; resets the spectator target
	virtual void OnBroadcastKeyframePlayed() = 0;
	// Sends the packet to the game coordinator as the data of a CMsgGCCStrike15_GotvSyncPacket
	virtual void EngineGotvSyncPacket( const CEngineGotvSyncPacket *pPkt ) = 0;
	// Fills the local player's matchmaking settings ( "members/machine0/player0", "game/ranking", ... ); the "client" context skips the ranking
	virtual void FillMatchmakingPlayerSettings( KeyValues *pSettings, const char *pszContext ) = 0;
	// Follows the owner entities up to a player pawn or controller; -1 when there is none
	virtual CPlayerSlot GetEntity2OwnerPlayerSlot( CEntityIndex nEntityIndex ) = 0;
	// Designer name of the entity, falling back to its class name; "" for an invalid index
	virtual const char *GetEntity2ClassName( CEntityIndex nEntityIndex ) = 0;
	// True when the class name of the entity or of one of its base classes is pszClassName
	virtual bool IsEntity2OfClass( CEntityIndex nEntityIndex, const char *pszClassName ) = 0;

	// Forwards to the input system; true makes the engine create and send a move every client frame instead of only on new ticks
	virtual bool ShouldCreateMoveEveryFrame() = 0;
	// The model editor's animation capture toolbar registers itself while it records and unregisters afterwards
	virtual void AddSkeletonDemoRecorder( ISkeletonDemoRecorder *pRecorder ) = 0;
	virtual void RemoveSkeletonDemoRecorder( ISkeletonDemoRecorder *pRecorder ) = 0;
	// cl_interp_ratio clamped to sv_client_min_interp_ratio and sv_client_max_interp_ratio, in seconds
	virtual float GetClientInterpAmount() = 0;
	// Forwards to IEntity2Networkables::GetEntity2Networkables() of the client entity system; the engine walks it when it
	// reconciles the entities of a packet
	virtual CUtlMap< int, Entity2Networkable_t > &GetEntity2Networkables() = 0;
	// The engine passes the contents of a ".evidence" demo file, which the Overwatch UI component decodes in place
	virtual void PrepareSignedEvidenceData( void *pvData, int nNumBytes ) = 0;

	// The second one decodes the command baselines of the message as soon as the engine receives it, the first one
	// applies the server execution ticks to the cached commands when the engine processes it
	virtual void ProcessUserCommands( const CSVCMsg_UserCommands_t *pMsg ) = 0;
	virtual void OnUserCommandsReceived( const CSVCMsg_UserCommands_t *pMsg ) = 0;

	// Forward to the input system. True makes the engine wrap the key binding commands of one input event in "__beginseq" and "__endseq"
	virtual bool ShouldSequenceInputEvents() = 0;
	// Called with the commands of one input event sequence before and after the engine runs them; the first one
	// drops the whole sequence when it combines several movement commands
	virtual void BeginInputEvent( CSplitScreenSlot nSlot, CUtlVector< CCommand * > *pCommands ) = 0;
	virtual void EndInputEvent( CSplitScreenSlot nSlot, CUtlVector< CCommand * > *pCommands ) = 0;
	// The engine asks the first one before it runs a key binding of an input button and calls the second one after it.
	// True ignores a movement command that follows another one in the same input event
	virtual bool ShouldIgnoreButtonCommand( CSplitScreenSlot nSlot, void *pButton, const CCommand &args ) = 0;
	virtual void OnButtonCommandExecuted( CSplitScreenSlot nSlot, void *pButton, const CCommand &args ) = 0;

	// Called with the new net channel of the connection; the engine passes -1 as nUnk, which the client does not read.
	// The HLTV client state passes true for bHLTV, and the client then removes its CBidirMsg_PredictionEvent handler from
	// the channel instead of installing it
	virtual void OnNetChannelCreated( INetChannel *pNetChannel, int nUnk, bool bHLTV ) = 0;
	virtual void GetEconItemNamesForModel( const char *pszModelName, bool bUnk1, bool bUnk2, CUtlVector< CUtlString > &names ) = 0;
	virtual void GetEconItemNamesForCharacter( const char *pszCharacterName, bool bUnk1, bool bUnk2, CUtlVector< CUtlString > &names ) = 0;
	virtual void GetEconItemsInfoForModel( const char *pszModelName, const char *pszUnk, bool bUnk1, bool bUnk2, bool bUnk3, CUtlVector< EconItemInfo_t > &items ) = 0;
	virtual void GetEconItemsInfoForCharacter( const CharacterInfo_t *pCharacter, const char *pszUnk, bool bUnk1, bool bUnk2, bool bUnk3, CUtlVector< EconItemInfo_t > &items ) = 0;
	virtual void unk194() = 0;
	// SFM's particle system query forwards IParticleSystemQuery slot 50 here, which the model renderer asks for the index of
	// C_OP_RenderModels::m_EconSlotName (-1 when the name is empty); 0 in the client
	virtual int GetEconSlotIndex( const char *pszEconSlotName ) = 0;
	virtual float GetDefaultScaleForModel( const char *pszModelName ) = 0;
	virtual float GetDefaultScaleForCharacter( const CharacterInfo_t *pCharacter ) = 0;
	// Only clears the persona in the client
	virtual void GetCharacterForModel( const char *pszModelName, bool bUnk, CharacterInfo_t *pCharacter ) = 0;
	// The model names of a character by index; false past the last one. Clears the result in the client
	virtual bool GetCharacterModelName( const CharacterInfo_t *pCharacter, int nModelIndex, CBufferString &modelName ) = 0;
	// Empty in the client
	virtual void GetCharacterList( CUtlVector< CharacterInfo_t > *pCharacters ) = 0;
	// Directory of the scenes of a model; SFM falls back to "scenes/<hero name>" when it is empty. Clears the result in the client
	virtual void GetSceneDirectoryForModel( const char *pszModelName, CBufferString &sceneDirectory ) = 0;
	// The engine's filter callback of fps_max; the client allows the change while disconnected or on a local server, among others
	virtual bool IsFpsMaxChangeAllowed( ConVarRefAbstract *pConVar, CSplitScreenSlot nSlot, const CVValue_t *pNewValue, const CVValue_t *pOldValue ) = 0;
	// The engine passes it with GetReplayCompatHelper() as the user data of the flattened serializers while it links classes
	virtual FlattenedSerializerUserData_t *GetFlattenedSerializerUserData() = 0;
	// Asked before ISource2Server::GetMatchID() for the crash dump comment
	virtual uint64 GetMatchID() = 0;
	// Asked by the "game_get_source_player_info" sound operator for its is_stealthy output
	virtual bool IsPlayerStealthy( CEntityIndex nEntityIndex ) = 0;
	// Entity index of the pawn of a player controller
	virtual CEntityIndex GetControllerPawnIndex( CEntityIndex nControllerIndex ) = 0;
	virtual CEntityIndex FindEntityByName( const char *pszName ) = 0;
	// Clears pEntities first
	virtual void FindEntitiesByName( const char *pszName, CUtlVector< CEntityIndex > *pEntities ) = 0;
	// One of the 8 positions of the current soundscape; false when it is not set
	virtual bool GetSoundscapePosition( int nIndex, Vector *pPosition ) = 0;
};

// A game event kept for demo scrubbing, sorted by tick
struct SerializedEvent_t
{
	int m_nTick;
	IGameEvent *m_pEvent;
};

// A user message serialized for demo scrubbing or for the instant replay
struct SerializedUserMessage_t
{
	int m_nTick;
	int m_nMessageId;
	CUtlVector< uint8 > m_Data;
};

class CSource2Client : public CGameAppSystem< ISource2Client >, public IStreamEntitiesFromFileCompletion
{
public:
	// Queued by DestroyClientSideNetworkedEntity() when bDeferred is set
	struct DeferredEntityDestruction_t
	{
		CEntityHandle m_hEntity;

		// The current time plus the delay the entity returns
		float m_flDestroyTime;
	};

	// Recorded for the demo scrubbing data by AddPlayerConnectionEvent()
	struct PlayerConnectionEvent_t
	{
		int m_nTick;
		int m_nConnectionInfo;
		bool m_bUnk008;
		CSteamID m_SteamID;
		bool m_bBot;
		bool m_bHltv;
	};

	struct PlayerConnectionTickOrderingFunc
	{
		bool Less( const PlayerConnectionEvent_t &lhs, const PlayerConnectionEvent_t &rhs, void *pContext )
		{
			return lhs.m_nTick < rhs.m_nTick;
		}
	};

public:
	// Zeroed by the constructor; no reader found
	uint64 m_unk038[ 2 ];
	// Set by LaunchApplicationOnExit()
	CUtlString m_sApplicationOnExit;
	// The factory passed to Connect(), read by Init()
	CreateInterfaceFn m_pFactory;
	// Copied by ReceivedServerInfo()
	GameSessionConfiguration_t m_GameSessionConfiguration;
	CUtlSortVector< PlayerConnectionEvent_t, PlayerConnectionTickOrderingFunc > m_PlayerConnectionEvents;
	// Connection info number to the index of its event in m_PlayerConnectionEvents
	CUtlOrderedMap< int, int > m_PlayerConnectionEventIndices;
	// Universe of the last player SteamID recorded by AddPlayerConnectionEvent()
	EUniverse m_eUniverse;
	uint8 m_pad144[ 0x4 ];
	// Ten game event groups filled by the constructor: event names, the ids of those events, and the copies kept for scrubbing,
	// fired by OnScrubDemo(); the camera group holds "hltv_cameraman", "hltv_fixed" and "hltv_chase", the others one event each
	CUtlVector< const char * > m_RoundPrestartEventNames;
	CUtlVector< int > m_RoundPrestartEventIds;
	CUtlVector< SerializedEvent_t > m_RoundPrestartEvents;
	CUtlVector< const char * > m_RoundFreezeEndEventNames;
	CUtlVector< int > m_RoundFreezeEndEventIds;
	CUtlVector< SerializedEvent_t > m_RoundFreezeEndEvents;
	CUtlVector< const char * > m_RoundOfficiallyEndedEventNames;
	CUtlVector< int > m_RoundOfficiallyEndedEventIds;
	CUtlVector< SerializedEvent_t > m_RoundOfficiallyEndedEvents;
	CUtlVector< const char * > m_PlayerDisconnectEventNames;
	CUtlVector< int > m_PlayerDisconnectEventIds;
	CUtlVector< SerializedEvent_t > m_PlayerDisconnectEvents;
	CUtlVector< const char * > m_PlayerDeathEventNames;
	CUtlVector< int > m_PlayerDeathEventIds;
	CUtlVector< SerializedEvent_t > m_PlayerDeathEvents;
	CUtlVector< const char * > m_PlayerHurtEventNames;
	CUtlVector< int > m_PlayerHurtEventIds;
	CUtlVector< SerializedEvent_t > m_PlayerHurtEvents;
	CUtlVector< const char * > m_HltvCameraEventNames;
	CUtlVector< int > m_HltvCameraEventIds;
	CUtlVector< SerializedEvent_t > m_HltvCameraEvents;
	CUtlVector< const char * > m_PlayerConnectEventNames;
	CUtlVector< int > m_PlayerConnectEventIds;
	CUtlVector< SerializedEvent_t > m_PlayerConnectEvents;
	CUtlVector< const char * > m_BulletDamageEventNames;
	CUtlVector< int > m_BulletDamageEventIds;
	CUtlVector< SerializedEvent_t > m_BulletDamageEvents;
	CUtlVector< const char * > m_PlayerSpawnEventNames;
	CUtlVector< int > m_PlayerSpawnEventIds;
	CUtlVector< SerializedEvent_t > m_PlayerSpawnEvents;
	// Built by SetupScrubbingData() and purged by ClearScrubbingData()
	CUtlVector< SerializedUserMessage_t > m_ScrubbingUserMessages;
	// Filled by InitInstantReplayData() and purged by ClearInstantReplayData()
	CUtlVector< SerializedUserMessage_t > m_InstantReplayUserMessages;
	// Handlers of hooked user messages (CUserMessageCameraTransition) queued by their delegate hooks and run by the
	// game loop mode once per frame
	CTSQueue< std::function< void() > * > m_QueuedMessageHandlers;
	// Zeroed by the constructor; no reader found
	bool m_unk490[ 2 ];
	// Read by the delegate hook and the queue drain, never set: the first one runs a handler at once instead of
	// queueing it, the second one destroys handlers without running them
	bool m_bRunMessageHandlersImmediately;
	bool m_bDiscardMessageHandlers;
	// Set while handlers run
	bool m_bRunningMessageHandlers;
	uint8 m_pad495[ 0xB ];
	// 0x20-byte profiling records starting with a CUtlString that FrameStageNotify() formats as "FrameNetUpdate(%.3f %d)"
	// and "FramePostDataUpdate(%.3f %d)" and opens and closes around those stages
	void *m_pFrameNetUpdateRecord;
	void *m_pFramePostDataUpdateRecord;
	// The "Client Restore Server State" job (cl_async_restore_server_authoritative_state) FrameStageNotify() starts;
	// waited for and released by the next FrameStageNotify()
	CThreadedJob *m_pRestoreServerStateJob;
	// -1.0 after the constructor and Connect()
	float m_unk4B8;
	uint8 m_pad4BC[ 0x4 ];
	CUtlVector< DeferredEntityDestruction_t > m_DeferredEntityDestructions;
};

#endif // ISOURCE2CLIENT_H
