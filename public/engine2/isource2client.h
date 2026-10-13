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
#include "game_saverestore.h"

#include <functional>

class CBufferString;
class CConcreteEntityList;
class ClientClass;
class CCommand;
class CEntityInstance;
class CGameTrace;
class CGlobalVarsBase;
class CSVCMsg_HltvReplay_t;
class CSVCMsg_UserCommands_t;
class CSteamID;
class CThreadedJob;
class CTransform;
class CUtlBuffer;
class IDemoMessageSink;
class InstantReplayMessage_t;
class IEntityReport;
class INetworkFieldChangedEventQueue;
class InfoForResourceTypeCTextureBase;
class IToolObject;
class ISceneView;
class ISceneWorld;
class InfoForResourceTypeCModel;
class KeyValues;
class QAngle;
class Vector;
class Vector4D;
struct AudioState_t;
struct EconItemInfo_t;
struct Entity2Networkable_t;
struct FlattenedSerializerSpewField_t;
struct ParticleProjectedTextureInfo_t;
struct RenderViewport_t;
struct RnQueryAttr_t;
struct vrect_t;

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

abstract_class ISource2Client : public IAppSystem, public IFieldChangeLimitSpew
{
public:
	// The engine passes 0 and 1; the enumerator names are unknown
	enum NotifyShouldTransmitType_t : int
	{
	};

	// NULL restores the module's own globals. Returns the previous globals, or NULL when they were the module's own
	virtual CGlobalVarsBase *SetGlobals( CGlobalVarsBase *pGlobals ) = 0;

	// Called after the engine installed the callbacks of its own string tables, with the same flag
	virtual void InstallStringTableCallback( bool bInvoke ) = 0;
	// Queues a predicted string command event for the client. A non-zero result other than -1 is sent to the server as the prediction sync of the command
	virtual int unk013( int nUnk, const CCommand &args ) = 0;
	virtual void PlayerInfoChanged( int nPlayerIndex ) = 0;

	virtual void CreateMove( int nSlot, bool bActive ) = 0;
	// Forward to the input system
	virtual void unk016() = 0;
	virtual void unk017( void *pUnk ) = 0;

	virtual int unk018( CSplitScreenSlot nSlot ) = 0;
	virtual int unk019( CSplitScreenSlot nSlot ) = 0;
	virtual int unk020( int nUnk ) = 0;
	virtual void unk021() = 0;
	// Forwards to the input system with absolute queries allowed
	virtual void ProcessInput( CSplitScreenSlot nSlot, float flFrameTime, bool bActive ) = 0;
	virtual bool unk023( int nUnk1, void *pUnk, int nUnk2, int nUnk3 ) = 0;
	virtual void unk024( CSplitScreenSlot nSlot, void *pUnk, int nCommandNumber ) = 0;
	virtual void unk025() = 0;
	virtual double unk026( void *pUnk, int nCommandNumber ) = 0;

	// Forward to the game rules
	virtual void unk027( bool bUnk ) = 0;
	virtual void unk028() = 0;
	// Forward to the network classes of the client entity system
	virtual bool GetEntity2Networkable( CEntityIndex nEntityIndex, Entity2Networkable_t &networkable ) = 0;
	virtual void unk030( void *pUnk1, void *pUnk2 ) = 0;
	// Queues the entity handle with a time when bUnk is set and the entity qualifies, otherwise forwards to the network classes
	virtual void unk031( Entity2Networkable_t *pNetworkable, bool bUnk ) = 0;
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

	// Forward to the mouth (lip sync) system
	virtual void unk049( int nUnk ) = 0;
	virtual void unk050( uint16 nUnk ) = 0;
	virtual void unk051( void *pUnk, int nUnk1, int nUnk2 ) = 0;
	// Fills the listener origin and angles, then the two vectors that follow through unk098()
	virtual void UpdateAudioState( AudioState_t *pAudioState, CSplitScreenSlot nSlot ) = 0;
	virtual void OnScreenSizeChanged( int nOldWidth, int nOldHeight ) = 0;
	virtual int IN_KeyEvent( CSplitScreenSlot nSlot, int nEventCode, ButtonCode_t nButtonCode, const char *pszCurrentBinding ) = 0;
	virtual void View_Render( vrect_t *pRect ) = 0;
	virtual void NotifyClientSignon( int nSignonState ) = 0;
	virtual bool CanRecordDemo( CBufferString *pErrorMsg ) const = 0;
	virtual bool unk058() = 0;
	virtual bool unk059( const char *pszCommand ) = 0;
	virtual void OnDemoRecordStart( const char *pszDemoBaseName, IDemoMessageSink *pSink ) = 0;
	virtual void OnDemoRecordStop( IDemoMessageSink *pSink ) = 0;

	// Fires the "demo_start" game event
	virtual void OnDemoPlaybackStart( const char *pszFilename, void *pUnk, KeyValues *pOptions ) = 0;
	// Fires the "demo_stop" game event
	virtual void OnDemoPlaybackStop() = 0;
	virtual void unk064( void *pUnk ) = 0;
	virtual void SetupScrubbingData( bool bSignon, int nUnk, int nMessageId, void *pUnk ) = 0;
	// Logged as the connection info of the scrubbing data
	virtual void unk066( bool bSignon, int nUnk1, int nUnk2, CSteamID steamID, bool bBot, bool bHltv ) = 0;
	// Reads the "AnimAssetData" block of pUnk1
	virtual void unk067( void *pUnk1, void *pUnk2 ) = 0;
	// Frees the game events kept for scrubbing and clears the player connection events
	virtual void ClearScrubbingData() = 0;
	// Fires the game events kept for nTick
	virtual void OnScrubDemo( int nUnk, int nTick ) = 0;
	// Fires the "demo_skip" game event
	virtual void OnDemoSkip( int nPlaybackTick, int nSkipToTick, bool bInstantReplay ) = 0;
	// Asked by the demo player for each message removed while skipping
	virtual bool ShouldNopCmdDuringDemoSkip( int nCmd ) = 0;
	// Serializes the message into a new entry of the instant replay data
	virtual void InitInstantReplayData( InstantReplayMessage_t *pMessage, int nUnk ) = 0;
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
	virtual bool unk081( CEntityIndex nEntityIndex ) = 0;
	// Handle of the first split screen slot's local player pawn
	virtual CEntityHandle unk082() = 0;
	virtual bool unk083( int nUnk ) = 0;

	// Per-entity queries
	virtual bool GetEntity2AbsOrigin( CEntityIndex nEntityIndex, Vector *pOrigin ) = 0;
	virtual float unk085( CEntityIndex nEntityIndex ) = 0;
	virtual void GetEntity2AbsAngles( CEntityIndex nEntityIndex, QAngle *pAngles ) = 0;
	// Zeroes both vectors first
	virtual bool unk087( CEntityIndex nEntityIndex, Vector *pvecUnk1, Vector *pvecUnk2 ) = 0;
	virtual bool unk088( CEntityIndex nEntityIndex, int *pOut1, int *pOut2 ) = 0;
	virtual CEntityHandle unk089() = 0;
	virtual void unk090( CEntityIndex nEntityIndex, void *pUnk ) = 0;
	// Falls back to GetEntity2AbsOrigin() when the attachment token is 0
	virtual bool GetEntity2AttachmentAbsOrigin( CEntityIndex nEntityIndex, Vector *pOrigin, CUtlStringToken &attachment ) = 0;
	// Builds the transform from GetEntity2AbsOrigin() and GetEntity2AbsAngles() when the attachment token is 0
	virtual bool unk092( CEntityIndex nEntityIndex, CTransform *pTransform, CUtlStringToken &attachment ) = 0;
	virtual bool unk093( CEntityIndex nEntityIndex ) = 0;
	virtual void unk094( CEntityIndex nEntityIndex, Vector *pvecUnk1, Vector *pvecUnk2, Vector *pvecUnk3 ) = 0;
	virtual bool unk095() = 0;
	virtual bool unk096() = 0;
	virtual CEntityHandle unk097() = 0;
	// Zeroes both outputs
	virtual void unk098( CSplitScreenSlot nSlot, Vector *pvecUnk1, Vector *pvecUnk2 ) = 0;

	virtual bool FormatSerializerFieldValue( CEntityIndex nEntityIndex, FlattenedSerializerSpewField_t &field ) = 0;

	// pView is the "Player %d" view the engine adds through ISceneSystem::AddView()
	virtual void PerformClientSpecificSetupForPlayerViews( ISceneView *pView, RenderViewport_t *pViewport, void *pUnk1, void *pUnk2 ) = 0;
	virtual void *GetPlayerViewSetup( CSplitScreenSlot nSlot ) = 0;
	virtual void GetBugReportLaunchURLParameters( KeyValues *pKV ) = 0;
	// Creates the projected texture when hTexture is NULL
	virtual void UpdateProjectedTexture( const ParticleProjectedTextureInfo_t &info, HProjectedTexture &hTexture ) = 0;
	// Calls the deleting destructor of hTexture
	virtual void DeleteProjectedTexture( HProjectedTexture hTexture ) = 0;
	virtual void *UpdateDeferredLight( ISceneWorld *pSceneWorld, CWeakHandle< InfoForResourceTypeCTextureBase > hTexture, Vector &vecUnk1, float flUnk1, Vector4D &vecUnk2, float flUnk2, float flUnk3, float flUnk4, Vector vecUnk3, float flUnk5, float flUnk6, float flUnk7, float flUnk8, float flUnk9, HDeferredLight &hLight ) = 0;
	virtual void DeleteDeferredLight( HDeferredLight hLight ) = 0;
	virtual void unk107( void *pUnk1, void *pUnk2, void *pUnk3 ) = 0;
	// Forwards to the scene system
	virtual void unk108() = 0;
	virtual void unk109( void *pUnk1, void *pUnk2, void *pUnk3, void *pUnk4 ) = 0;
	// Forwards to the scene system
	virtual void unk110() = 0;
	// Traces a line through the game physics query with a filter built from attr; resets the trace without one
	virtual bool unk111( const Vector &vecStart, const Vector &vecEnd, const RnQueryAttr_t &attr, CGameTrace *pTrace ) = 0;
	virtual void unk112( int nUnk1, int nUnk2, int nUnk3, void *pUnk ) = 0;
	virtual void unk113( void *pUnk1, void *pUnk2, void *pUnk3 ) = 0;
	// Overloads: the first one looks the entity up and calls the second one. Linux keeps this order, MSVC reverses it
	virtual void unk114( CEntityIndex nEntityIndex, void *pUnk ) = 0;
	virtual void unk114( CEntityInstance *pEntity, uint64 *pUnk1, void *pUnk2 ) = 0;
	virtual void LaunchApplicationOnExit( const char *pszValue ) = 0;
	// Asked through the engine sound services; returns its argument
	virtual const char *DotaEconGetSoundReplacement( const char *pszSoundName ) = 0;
	// Asked through the engine sound services; false
	virtual bool DotaIsGroundUnderWater( float flX, float flY ) = 0;
	virtual float unk119() = 0;
	virtual void unk120() = 0;
	virtual void unk121() = 0;
	virtual void unk122() = 0;
	virtual bool unk123() = 0;
	virtual void UpdateWhenNotInGame( float flFrameTime ) = 0;
	virtual void UpdateGlobalLightFromLightInfos( IToolObject *pUnk, const CUtlVector< IToolObject * > &lightInfos, const Vector &vecUnk, int nUnk, float flUnk1, float flUnk2, float flUnk3 ) = 0;
	virtual bool GetGlobalLightParameters( int *pnUnk, float *pflUnk1, float *pflUnk2 ) = 0;
	virtual void OnReceivedUpdatedServerPeerList( void *pPeerList, size_t nPeerCount ) = 0;
	virtual void UpdateEventListeners() = 0;
	virtual void *GetEconItemSystem() = 0;
	// Forwards to the game network string tables
	virtual bool GameCreateNetworkStringTables() = 0;
	// Fires the "client_disconnect" game event
	virtual void NotifyDisconnect( int nReason ) = 0;
	// Releases *pCombinedModel and clears *pUnk
	virtual void GetModelCombineInfo( CWeakHandle< InfoForResourceTypeCModel > hModel, CStrongHandleCopyable< InfoForResourceTypeCModel > *pCombinedModel, int *pUnk, const CUtlVector< CStrongHandleCopyable< InfoForResourceTypeCModel > > **ppUnk1, const CUtlVector< int > **ppUnk2 ) = 0;
	// Save/restore block handling
	virtual void unk133( void *pUnk1, void *pUnk2, int nUnk, void *pUnk3 ) = 0;
	virtual void unk134( void *pUnk1, void *pUnk2, int nUnk, void *pUnk3 ) = 0;
	// Returns "core.global_stack"
	virtual const char *unk135() = 0;
	virtual bool unk136() = 0;
	// Total and custom game Steam UGC subscription counts for the crash dump comment; zeroes them and returns false
	virtual bool unk137( uint32 *pnSubscriptionCounts ) = 0;
	virtual void unk138( void *pUnk ) = 0;
	virtual bool unk139( const char **ppszUnk, int nUnk, CUtlString *pOut ) = 0;
	// Entity list of the client entity system, summarized in the crash dump comment
	virtual CConcreteEntityList *unk140() = 0;
	// Returns the CToolClientSimulationAPI singleton
	virtual void *GetToolClientSimulationAPI() = 0;
	virtual void unk142() = 0;
	virtual void unk143() = 0;
	virtual bool unk144() = 0;
	virtual void *unk145() = 0;
	virtual void *unk146() = 0;
	virtual void unk147( int nUnk1, int nUnk2, int nUnk3, int nUnk4 ) = 0;
	virtual void unk148() = 0;
	virtual const char *unk149( CEntityIndex nEntityIndex ) = 0;
	virtual CEntityHandle unk150( CEntityHandle hEntity ) = 0;
	virtual void unk151() = 0;
	// Forwards to the game rules
	virtual void unk152() = 0;
	virtual const char *unk153() = 0;
	virtual void unk154( void *pUnk ) = 0;
	virtual void unk155( CEntityIndex nEntityIndex, int *pUnk, int nUnk ) = 0;
	virtual void unk156( CEntityIndex nEntityIndex, int *pUnk ) = 0;

	// Back the "sound_area_register", "sound_area_contains_point" and "sound_area_unregister" sound operators
	virtual SoundAreaGuid_t unk157( const Vector &vecPosition, float flRadius, const char *pszAreaType ) = 0;
	// The "any" area type matches every sound area
	virtual void unk158( const Vector &vecPosition, const char *pszAreaType, bool *pbPointInSoundArea, bool *pbSoundAreasExist ) = 0;
	virtual void unk159( SoundAreaGuid_t guid ) = 0;

	virtual CEntityHandle unk160( int nUnk ) = 0;
	virtual void unk161() = 0;
	virtual void *unk162() = 0;
	virtual bool unk163( void *pUnk ) = 0;
	virtual void OnHltvReplay( const CSVCMsg_HltvReplay_t *pMsg ) = 0;
	virtual void unk165() = 0;
	virtual void unk166() = 0;
	virtual void unk167( void *pMsg ) = 0;
	virtual void unk168() = 0;
	virtual void unk169( void *pUnk1, void *pUnk2 ) = 0;
	virtual void unk170() = 0;
	// Builds a game coordinator protobuf message
	virtual void unk171( void *pUnk ) = 0;
	// Fills matchmaking KeyValues ( "members/machine0/player0" )
	virtual void unk172( KeyValues *pKV, void *pUnk ) = 0;
	virtual CEntityHandle unk173( CEntityIndex nEntityIndex ) = 0;
	// Designer name of the entity, falling back to its class name; "" for an invalid index
	virtual const char *GetEntity2ClassName( CEntityIndex nEntityIndex ) = 0;
	virtual bool unk175( CEntityIndex nEntityIndex, void *pOut ) = 0;

	// Forward to the input system
	virtual bool unk176() = 0;
	virtual void unk177() = 0;
	virtual void unk178() = 0;
	virtual float unk179() = 0;
	// Forwards to an object owned by the client entity system
	virtual void unk180() = 0;
	virtual void unk181( void *pUnk, int nUnk ) = 0;

	virtual void unk182( const CSVCMsg_UserCommands_t *pMsg ) = 0;
	virtual void unk183( const CSVCMsg_UserCommands_t *pMsg ) = 0;

	// Forward to the input system
	virtual bool unk184() = 0;
	virtual void unk185( void *pUnk, int *pUnk2 ) = 0;
	virtual void unk186() = 0;
	virtual bool unk187( void *pUnk1, void *pUnk2, void *pUnk3 ) = 0;
	virtual void unk188( int nUnk, void *pUnk1, void *pUnk2 ) = 0;

	virtual void unk189( void *pUnk, int nUnk1, int nUnk2 ) = 0;
	virtual void GetEconItemNamesForModel( const char *pszModelName, bool bUnk1, bool bUnk2, CUtlVector< CUtlString > &names ) = 0;
	virtual void GetEconItemNamesForCharacter( const char *pszCharacterName, bool bUnk1, bool bUnk2, CUtlVector< CUtlString > &names ) = 0;
	virtual void GetEconItemsInfoForModel( const char *pszModelName, const char *pszUnk, bool bUnk1, bool bUnk2, bool bUnk3, CUtlVector< EconItemInfo_t > &items ) = 0;
	virtual void GetEconItemsInfoForCharacter( const char *pszCharacterName, const char *pszUnk, bool bUnk1, bool bUnk2, bool bUnk3, CUtlVector< EconItemInfo_t > &items ) = 0;
	virtual void unk194() = 0;
	virtual void *unk195() = 0;
	virtual float GetDefaultScaleForModel( const char *pszModelName ) = 0;
	virtual float GetDefaultScaleForCharacter( const char *pszCharacterName ) = 0;
	virtual void unk198( void *pUnk1, void *pUnk2, void *pUnk3 ) = 0;
	virtual bool unk199( void *pUnk1, void *pUnk2, void *pUnk3 ) = 0;
	virtual void unk200() = 0;
	virtual void unk201( void *pUnk1, void *pUnk2 ) = 0;
	virtual bool unk202() = 0;
	virtual void unk203() = 0;
	// Asked before ISource2Server::GetMatchID() for the crash dump comment
	virtual uint64 GetMatchID() = 0;
	virtual bool unk205( CEntityIndex nEntityIndex ) = 0;
	virtual CEntityHandle unk206( CEntityIndex nEntityIndex ) = 0;
	virtual CEntityHandle unk207( int nUnk ) = 0;
	virtual void unk208( const char *pszUnk, int *pOut ) = 0;
	virtual void unk209( int nUnk, void *pUnk ) = 0;
};

class CSource2Client : public CGameAppSystem< ISource2Client >, public IStreamEntitiesFromFileCompletion
{
public:
	// Recorded for the demo scrubbing data by unk066()
	struct PlayerConnectionEvent_t
	{
		int m_nTick;
		int m_nUnk004;
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
	CUtlString m_unk048;
	// The factory passed to Connect(), read by Init()
	CreateInterfaceFn m_unk050;
	// Copied by ReceivedServerInfo()
	GameSessionConfiguration_t m_unk058;
	CUtlSortVector< PlayerConnectionEvent_t, PlayerConnectionTickOrderingFunc > m_unk0F0;
	// Connection info number to the index of its event in m_unk0F0
	CUtlOrderedMap< int, int > m_unk118;
	// Universe of the last player SteamID recorded by unk066()
	EUniverse m_unk140;
	uint8 m_unk144[ 0x4 ];
	// Nine game event groups filled by the constructor: event names, the ids of those events, and a CUtlVector of the 16-byte
	// ( int tick, IGameEvent * ) copies kept for scrubbing, sorted by tick and fired by OnScrubDemo()
	CUtlVector< const char * > m_unk148;
	CUtlVector< int > m_unk160;
	uint8 m_unk178[ 0x18 ];
	CUtlVector< const char * > m_unk190;
	CUtlVector< int > m_unk1A8;
	uint8 m_unk1C0[ 0x18 ];
	CUtlVector< const char * > m_unk1D8;
	CUtlVector< int > m_unk1F0;
	uint8 m_unk208[ 0x18 ];
	CUtlVector< const char * > m_unk220;
	CUtlVector< int > m_unk238;
	uint8 m_unk250[ 0x18 ];
	CUtlVector< const char * > m_unk268;
	CUtlVector< int > m_unk280;
	uint8 m_unk298[ 0x18 ];
	CUtlVector< const char * > m_unk2B0;
	CUtlVector< int > m_unk2C8;
	uint8 m_unk2E0[ 0x18 ];
	CUtlVector< const char * > m_unk2F8;
	CUtlVector< int > m_unk310;
	uint8 m_unk328[ 0x18 ];
	CUtlVector< const char * > m_unk340;
	CUtlVector< int > m_unk358;
	uint8 m_unk370[ 0x18 ];
	CUtlVector< const char * > m_unk388;
	CUtlVector< int > m_unk3A0;
	uint8 m_unk3B8[ 0x18 ];
	// CUtlVector of the 0x20-byte message entries built by SetupScrubbingData() and purged by ClearScrubbingData()
	uint8 m_unk3D0[ 0x18 ];
	// CUtlVector of the same 0x20-byte entries, filled by InitInstantReplayData() and purged by ClearInstantReplayData()
	uint8 m_unk3E8[ 0x18 ];
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
	// CUtlVector of 8-byte ( CEntityHandle, float time ) entries appended by unk031()
	uint8 m_unk470[ 0x18 ];
};

#endif // ISOURCE2CLIENT_H
