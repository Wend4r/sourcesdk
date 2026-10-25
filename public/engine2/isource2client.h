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
#include "spawngrouptypes.h"
#include "game_saverestore.h"

#include <functional>

class CBufferString;
class CConcreteEntityList;
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
class IDemoMessageSink;
class InstantReplayMessage_t;
class IEntityReport;
class IGameEvent;
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
	virtual void unk000() = 0;
};

// The 8-byte class ISource2Client::unk146 returns (zero in the client), in an integer register on Linux; only its size is known
struct Source2ClientUnk146_t
{
	uint8 m_Data[ 8 ];
};

// The 8-byte tick and fraction value ISource2Client::unk169 returns, in an integer register on Linux and through a hidden
// pointer on Windows; only its size is known
struct Source2ClientUnk169_t
{
	uint8 m_Data[ 8 ];
};

abstract_class ISource2Client : public IAppSystem, public IFieldChangeLimitSpew
{
public:
	// The engine passes 0 and 1; the enumerator names are unknown
	enum NotifyShouldTransmitType_t : int
	{
	};

	// Built by the engine from each CMsgServerPeer of a peer list message
	struct PeerListEntry_t
	{
		// The peer's SteamID with the universe of the local user, and 3 next to it; both zero without a SteamID
		CSteamID m_SteamID;
		int m_nUnk008;
		int m_nUnk00C;

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
	// Forward to the input system; the engine calls them for every split screen slot while it builds the move message
	virtual void unk016( CSplitScreenSlot nSlot ) = 0;
	virtual void unk017( CSplitScreenSlot nSlot ) = 0;

	// Latest user command number the client generated
	virtual int GetLatestCommandNumber( CSplitScreenSlot nSlot ) = 0;
	virtual int unk019( CSplitScreenSlot nSlot ) = 0;
	// Number of commands kept in the command history: the latest command number, at most the history size of 150
	virtual int GetStoredCommandCount( CSplitScreenSlot nSlot ) = 0;
	// Called with the command numbers of a packet entities message; empty
	virtual void unk021( CSplitScreenSlot nSlot, int nLastCommandNumberExecuted, int nUnk ) = 0;
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
	// The engine creates each new entity of a packet through it; pUnk is the engine's creation record
	virtual bool CreateClientSideNetworkedEntity( const void *pUnk, Entity2Networkable_t *pNetworkable ) = 0;
	// Queues the entity handle with a time when bDeferred is set and the entity qualifies, otherwise forwards to the network classes
	virtual void DestroyClientSideNetworkedEntity( const Entity2Networkable_t *pNetworkable, bool bDeferred ) = 0;
	virtual INetworkFieldChangedEventQueue *GetNetworkFieldChangeCallbackQueue() = 0;
	// Called with 0 and then with 1 while the engine processes packet entities
	virtual void NotifyShouldTransmit( IEntityReport *pReport, CBitVec< 16384 > *pTransmitBits, NotifyShouldTransmitType_t nType ) = 0;
	virtual CUtlVector< ClientClass * > *GetAllClasses() = 0;
	// Copies the session configuration and asks the loop mode game shared state for it
	virtual bool ReceivedServerInfo( const GameSessionConfiguration_t &config, ILoopModePrerequisiteRegistry *pRegistry ) = 0;

	virtual void FrameStageNotify( int nStage ) = 0;

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

	// Forward to the mouth (lip sync) system; the first one allocates a mouth and returns its id
	virtual uint16 AllocateMouth( int nUnk ) = 0;
	virtual void FreeMouth( uint16 nMouth ) = 0;
	virtual void GetMouthInfo( void *pInfo, CEntityIndex nEntityIndex, int nUnk ) = 0;
	// Fills the listener origin and angles, then the two vectors that follow through unk098()
	virtual void UpdateAudioState( AudioState_t *pAudioState, CSplitScreenSlot nSlot ) = 0;
	virtual void OnScreenSizeChanged( int nOldWidth, int nOldHeight ) = 0;
	virtual int IN_KeyEvent( CSplitScreenSlot nSlot, int nEventCode, ButtonCode_t nButtonCode, const char *pszCurrentBinding ) = 0;
	virtual void View_Render( vrect_t *pRect ) = 0;
	virtual void NotifyClientSignon( int nSignonState ) = 0;
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
	virtual void unk064( void *pUnk ) = 0;
	virtual void SetupScrubbingData( bool bSignon, int nTick, int nMessageId, bf_read *pBuffer ) = 0;
	// Logged as the connection info of the scrubbing data
	virtual void AddPlayerConnectionEvent( bool bSignon, int nTick, int nConnectionInfo, CSteamID steamID, bool bBot, bool bHltv ) = 0;
	// Reads the "AnimAssetData" string table of the demo's string tables into pAnimAssets
	virtual void ReadAnimAssetData( INetworkStringTableContainer *pStringTables, void *pAnimAssets ) = 0;
	// Frees the game events kept for scrubbing and clears the player connection events
	virtual void ClearScrubbingData() = 0;
	// Fires the game events kept for nTick
	virtual void OnScrubDemo( int nUnk, int nTick ) = 0;
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
	virtual uint64 GetPointContents( const Vector &vecPosition, int nMask ) = 0;
	// Overloads: the first one looks the entity up and calls the second one. Linux keeps this order, MSVC reverses it
	virtual void GetSoundSpatialization( CEntityIndex nEntityIndex, void *pInfo ) = 0;
	virtual void GetSoundSpatialization( CEntityInstance *pEntity, uint64 *pUnk, void *pInfo ) = 0;
	virtual void LaunchApplicationOnExit( const char *pszValue ) = 0;
	// Asked through the engine sound services; returns its argument
	virtual const char *DotaEconGetSoundReplacement( const char *pszSoundName ) = 0;
	// Asked through the engine sound services; false
	virtual bool DotaIsGroundUnderWater( float flX, float flY ) = 0;
	virtual float unk119() = 0;
	virtual void unk120() = 0;
	virtual void unk121() = 0;
	// Increments the Steam callback counter
	virtual void NotifyPreSteamRunCallbacks() = 0;
	// False
	virtual bool VoiceReliable() = 0;
	virtual void UpdateWhenNotInGame( float flFrameTime ) = 0;
	virtual void UpdateGlobalLightFromLightInfos( IToolObject *pUnk, const CUtlVector< IToolObject * > &lightInfos, const Vector &vecUnk, int nUnk, float flUnk1, float flUnk2, float flUnk3 ) = 0;
	virtual bool GetGlobalLightParameters( int *pnUnk, float *pflUnk1, float *pflUnk2 ) = 0;
	virtual void OnReceivedUpdatedServerPeerList( PeerListEntry_t *pPeerList, size_t nPeerCount ) = 0;
	virtual void UpdateEventListeners() = 0;
	virtual void *GetEconItemSystem() = 0;
	// Forwards to the game network string tables
	virtual bool GameCreateNetworkStringTables() = 0;
	// Fires the "client_disconnect" game event
	virtual void NotifyDisconnect( int nReason ) = 0;
	// Releases *phBaseModel and clears *piBaseSkin
	virtual void GetModelCombineInfo( CWeakHandle< InfoForResourceTypeCModel > hCombinedModel, CStrongHandleCopyable< InfoForResourceTypeCModel > *phBaseModel, int *piBaseSkin, const CUtlVector< CStrongHandleCopyable< InfoForResourceTypeCModel > > **ppVecAdditionalModels, const CUtlVector< int > **ppVecSkins ) = 0;
	// Called by the client spawn group's SaveRestoreMap() and SaveRestoreMap2() with its loading spawn group; forward to the
	// entity system save/restore when ILoadingSpawnGroup::ShouldLoadEntitiesFromSave() and ILoadingSpawnGroup::IsLevelTransition() are set
	virtual void unk133( void *pUnk, ILoadingSpawnGroup *pLoading, SpawnGroupHandle_t hSpawnGroup, const matrix3x4a_t &vecSpawnOffset ) = 0;
	virtual void unk134( void *pUnk, ILoadingSpawnGroup *pLoading, SpawnGroupHandle_t hSpawnGroup, const matrix3x4a_t &vecSpawnOffset ) = 0;
	// Returns "core.global_stack"
	virtual const char *GetGlobalSoundStackName() = 0;
	virtual bool unk136() = 0;
	// Total and custom game Steam UGC subscription counts for the crash dump comment; zeroes them and returns false
	virtual bool GetUGCSubscriptionCounts( uint32 *pnSubscriptionCounts ) = 0;
	// Asked for the addon id when "AddonConfig/AllowAddonDownload" is set; the engine handles the results 1 and 2
	virtual int DownloadWorkshopAddon( uint64 nAddonId ) = 0;
	virtual bool unk139( const char **ppszUnk, int nUnk, CUtlString *pOut ) = 0;
	// Entity list of the client entity system, summarized in the crash dump comment
	virtual CConcreteEntityList *GetEntityList() = 0;
	// Returns the CToolClientSimulationAPI singleton
	virtual IToolClientSimulationAPI *GetToolClientSimulationAPI() = 0;
	// The engine render view setup calls them before PerformClientSpecificSetupForPlayerViews(), the first one with the same attributes
	virtual void unk142( CRenderAttributes *pAttributes ) = 0;
	virtual void unk143( void *pUnk ) = 0;
	virtual bool unk144() = 0;
	virtual void *unk145() = 0;
	virtual Source2ClientUnk146_t unk146() = 0;
	// Duplicates the user command nCommandNumber until the newest command reaches nLastExecutedCommand ("Duplicating command %d %dx to catch up")
	virtual void unk147( CSplitScreenSlot nSlot, int nLastExecutedCommand, int nServerTick, int nCommandNumber ) = 0;
	virtual void unk148() = 0;
	virtual const char *GetEntity2UniqueHammerID( CEntityIndex nEntityIndex ) = 0;
	virtual CEntityHandle unk150( CEntityHandle hEntity ) = 0;
	virtual void unk151() = 0;
	// Forwards to the game rules
	virtual void unk152() = 0;
	virtual const char *GetFriendlyMapName() = 0;
	// Fires a panorama event with the state
	virtual void OnCommandQueueStateChanged( const GameClientCommandQueueState_t &state ) = 0;
	virtual void unk155( CEntityIndex nEntityIndex, int *pUnk, int nUnk ) = 0;
	virtual void unk156( CEntityIndex nEntityIndex, int *pUnk ) = 0;

	// Back the "sound_area_register", "sound_area_contains_point" and "sound_area_unregister" sound operators
	virtual SoundAreaGuid_t RegisterSoundArea( const Vector &vecPosition, float flRadius, const char *pszAreaType ) = 0;
	// The "any" area type matches every sound area
	virtual void IsPointInSoundArea( const Vector &vecPosition, const char *pszAreaType, bool *pbPointInSoundArea, bool *pbSoundAreasExist ) = 0;
	virtual void UnregisterSoundArea( SoundAreaGuid_t guid ) = 0;

	virtual CEntityHandle GetPlayerControllerHandle( CPlayerSlot nSlot ) = 0;
	virtual void *GetCompositeMaterialManager() = 0;
	// The engine renames classes through it while it links classes for replay compatibility ("scripts/replay_compatability_settings.txt")
	virtual ISerializerMatchToMetaReplayCompatHelper *GetReplayCompatHelper() = 0;
	virtual bool unk163( void *pUnk ) = 0;
	virtual void OnHltvReplay( const CSVCMsg_HltvReplay_t *pMsg ) = 0;
	// Forward to the HLTV replay system, which OnHltvReplay() feeds
	virtual void OnHltvReplayTick() = 0;
	virtual int GetHltvReplayDelay() = 0;
	virtual void unk167( void *pMsg ) = 0;
	// Scene view debug overlays of the client CDebugOverlayGameSystem; the engine passes received CNETMsg_DebugOverlay messages to it
	virtual ISceneViewDebugOverlays *GetDebugOverlayGameSystem() = 0;
	virtual Source2ClientUnk169_t unk169() = 0;
	virtual void unk170() = 0;
	// Sends the packet to the game coordinator as the data of a CMsgGCCStrike15_GotvSyncPacket
	virtual void EngineGotvSyncPacket( const CEngineGotvSyncPacket *pPkt ) = 0;
	// Fills matchmaking KeyValues ( "members/machine0/player0" )
	virtual void unk172( KeyValues *pKV, void *pUnk ) = 0;
	// Follows the owner entities up to a player pawn or controller; -1 when there is none
	virtual CPlayerSlot GetEntity2OwnerPlayerSlot( CEntityIndex nEntityIndex ) = 0;
	// Designer name of the entity, falling back to its class name; "" for an invalid index
	virtual const char *GetEntity2ClassName( CEntityIndex nEntityIndex ) = 0;
	// True when the class name of the entity or of one of its base classes is pszClassName
	virtual bool IsEntity2OfClass( CEntityIndex nEntityIndex, const char *pszClassName ) = 0;

	// Forwards to the input system; true makes the engine create and send a move every client frame instead of only on new ticks
	virtual bool ShouldCreateMoveEveryFrame() = 0;
	virtual void unk177() = 0;
	virtual void unk178() = 0;
	// cl_interp_ratio clamped to sv_client_min_interp_ratio and sv_client_max_interp_ratio, in seconds
	virtual float GetClientInterpAmount() = 0;
	// Forwards to an object owned by the client entity system; the engine walks the returned table of
	// 112-byte entity entries when it reconciles the entities of a packet
	virtual void *unk180() = 0;
	// The engine passes the contents of a ".evidence" demo file, which the Overwatch UI component decodes in place
	virtual void PrepareSignedEvidenceData( void *pvData, int nNumBytes ) = 0;

	virtual void unk182( const CSVCMsg_UserCommands_t *pMsg ) = 0;
	virtual void unk183( const CSVCMsg_UserCommands_t *pMsg ) = 0;

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

	// Called with the new net channel of the connection
	virtual void OnNetChannelCreated( INetChannel *pNetChannel, int nUnk, bool bUnk ) = 0;
	virtual void GetEconItemNamesForModel( const char *pszModelName, bool bUnk1, bool bUnk2, CUtlVector< CUtlString > &names ) = 0;
	virtual void GetEconItemNamesForCharacter( const char *pszCharacterName, bool bUnk1, bool bUnk2, CUtlVector< CUtlString > &names ) = 0;
	virtual void GetEconItemsInfoForModel( const char *pszModelName, const char *pszUnk, bool bUnk1, bool bUnk2, bool bUnk3, CUtlVector< EconItemInfo_t > &items ) = 0;
	virtual void GetEconItemsInfoForCharacter( const char *pszCharacterName, const char *pszUnk, bool bUnk1, bool bUnk2, bool bUnk3, CUtlVector< EconItemInfo_t > &items ) = 0;
	virtual void unk194() = 0;
	virtual void *unk195() = 0;
	virtual float GetDefaultScaleForModel( const char *pszModelName ) = 0;
	virtual float GetDefaultScaleForCharacter( const char *pszCharacterName ) = 0;
	virtual void unk198( void *pUnk1, bool bUnk, void *pUnk2 ) = 0;
	// Clears the result
	virtual bool unk199( void *pUnk1, void *pUnk2, CBufferString &result ) = 0;
	virtual void unk200() = 0;
	// Clears the result
	virtual void unk201( void *pUnk, CBufferString &result ) = 0;
	// The engine's filter callback of fps_max; the client allows the change while disconnected or on a local server, among others
	virtual bool IsFpsMaxChangeAllowed( ConVarRefAbstract *pConVar, CSplitScreenSlot nSlot, const CVValue_t *pNewValue, const CVValue_t *pOldValue ) = 0;
	// The engine passes it with GetReplayCompatHelper() as the user data of the flattened serializers while it links classes
	virtual FlattenedSerializerUserData_t *GetFlattenedSerializerUserData() = 0;
	// Asked before ISource2Server::GetMatchID() for the crash dump comment
	virtual uint64 GetMatchID() = 0;
	virtual bool unk205( CEntityIndex nEntityIndex ) = 0;
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
	uint8 m_unk038[ 0x10 ];
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
	uint8 m_unk144[ 0x4 ];
	// Nine game event groups filled by the constructor: event names, the ids of those events, and the copies kept for scrubbing,
	// fired by OnScrubDemo(); the camera group holds "hltv_cameraman", "hltv_fixed" and "hltv_chase", the others one event each
	CUtlVector< const char * > m_RoundPrestartEventNames;
	CUtlVector< int > m_RoundPrestartEventIds;
	CUtlVector< SerializedEvent_t > m_RoundPrestartEvents;
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
	CTSQueue< std::function< void() > * > m_unk400;
	// Zeroed by the constructor; no reader found
	int m_unk440;
	bool m_unk444;
	uint8 m_unk445[ 0xB ];
	// 0x20-byte objects starting with a CUtlString that FrameStageNotify() formats as "FrameNetUpdate(%.3f %d)" and
	// "FramePostDataUpdate(%.3f %d)"
	void *m_unk450;
	void *m_unk458;
	// Waited for and released by FrameStageNotify()
	CThreadedJob *m_unk460;
	// -1.0 after the constructor and Connect()
	float m_unk468;
	uint8 m_unk46C[ 0x4 ];
	CUtlVector< DeferredEntityDestruction_t > m_DeferredEntityDestructions;
};

#endif // ISOURCE2CLIENT_H
