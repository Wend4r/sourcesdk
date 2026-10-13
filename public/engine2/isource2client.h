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
#include "tier0/tslist.h"
#include "game_saverestore.h"

#include <functional>

class CBufferString;
class CCommand;
class CGlobalVarsBase;
class CSVCMsg_HltvReplay_t;
class CSVCMsg_UserCommands_t;
class CSteamID;
class CThreadedJob;
class CTransform;
class CUtlBuffer;
class IDemoMessageSink;
class ISceneView;
class InfoForResourceTypeCModel;
class KeyValues;
class QAngle;
class Vector;
struct FlattenedSerializerSpewField_t;
struct RenderViewport_t;
struct vrect_t;

template < class T > class CWeakHandle;
template < class T > class CStrongHandleCopyable;

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
	// Forwards to the input system
	virtual void unk022( int nSlot, void *pUnk, bool bUnk ) = 0;
	virtual bool unk023( int nUnk1, void *pUnk, int nUnk2, int nUnk3 ) = 0;
	virtual void unk024( CSplitScreenSlot nSlot, void *pUnk, int nCommandNumber ) = 0;
	virtual void unk025() = 0;
	virtual double unk026( void *pUnk, int nCommandNumber ) = 0;

	// Forward to the game rules
	virtual void unk027( bool bUnk ) = 0;
	virtual void unk028() = 0;
	// Forward to an object owned by the client entity system
	virtual void unk029() = 0;
	virtual void unk030() = 0;
	virtual void unk031( void *pUnk, bool bUnk ) = 0;
	virtual void *unk032() = 0;
	virtual void unk033( void *pUnk1, int *pUnk2, int nUnk ) = 0;
	// Forwards to an object owned by the client entity system
	virtual void unk034() = 0;
	virtual void unk035( void *pUnk1, void *pUnk2 ) = 0;

	virtual void FrameStageNotify( int nStage ) = 0;

	virtual int HudVidInit() = 0;
	virtual void HudUpdate( bool bActive ) = 0;
	virtual void HudReset() = 0;
	virtual void unk040() = 0;
	virtual void unk041() = 0;
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
	// Writes two vectors to pOut and passes them to unk098
	virtual void unk052( void *pOut, int nUnk ) = 0;
	virtual void unk053() = 0;
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
	virtual void unk068() = 0;
	virtual void unk069( void *pUnk, int nUnk ) = 0;
	// Fires the "demo_skip" game event
	virtual void OnDemoSkip( int nPlaybackTick, int nSkipToTick, bool bInstantReplay ) = 0;
	// Asked by the demo player for each message removed while skipping
	virtual bool ShouldNopCmdDuringDemoSkip( int nCmd ) = 0;
	virtual void InitInstantReplayData( void *pUnk, int nUnk ) = 0;
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
	virtual bool unk087( CEntityIndex nEntityIndex, void *pOut1, void *pOut2 ) = 0;
	virtual bool unk088( CEntityIndex nEntityIndex, int *pOut1, int *pOut2 ) = 0;
	virtual CEntityHandle unk089() = 0;
	virtual void unk090( CEntityIndex nEntityIndex, void *pUnk ) = 0;
	// Falls back to GetEntity2AbsOrigin() when the attachment token is 0
	virtual bool GetEntity2AttachmentAbsOrigin( CEntityIndex nEntityIndex, Vector *pOrigin, CUtlStringToken &attachment ) = 0;
	virtual bool unk092( CEntityIndex nEntityIndex, void *pOut, int *pUnk ) = 0;
	virtual bool unk093( int nUnk ) = 0;
	virtual void unk094( CEntityIndex nEntityIndex, void *pOut1, void *pOut2, void *pOut3 ) = 0;
	virtual bool unk095() = 0;
	virtual bool unk096() = 0;
	virtual CEntityHandle unk097() = 0;
	// Zeroes both outputs
	virtual void unk098( void *pUnk, void *pOut1, void *pOut2 ) = 0;

	virtual bool FormatSerializerFieldValue( CEntityIndex nEntityIndex, FlattenedSerializerSpewField_t &field ) = 0;

	// pView is the "Player %d" view the engine adds through ISceneSystem::AddView()
	virtual void PerformClientSpecificSetupForPlayerViews( ISceneView *pView, RenderViewport_t *pViewport, void *pUnk1, void *pUnk2 ) = 0;
	virtual void *GetPlayerViewSetup( CSplitScreenSlot nSlot ) = 0;
	virtual void GetBugReportLaunchURLParameters( KeyValues *pKV ) = 0;
	// Creates the projected texture when *ppTexture is NULL
	virtual void UpdateProjectedTexture( const void *pInfo, void **ppTexture ) = 0;
	// Calls the deleting destructor of pTexture
	virtual void DeleteProjectedTexture( void *pTexture ) = 0;
	virtual void *UpdateDeferredLight( void *p ) = 0;
	virtual void DeleteDeferredLight( void *p ) = 0;
	virtual void unk107( void *pUnk1, void *pUnk2, void *pUnk3 ) = 0;
	// Forwards to the scene system
	virtual void unk108() = 0;
	virtual void unk109( void *pUnk1, void *pUnk2, void *pUnk3, void *pUnk4 ) = 0;
	// Forwards to the scene system
	virtual void unk110() = 0;
	// Physics shape trace
	virtual bool unk111( int nUnk1, int nUnk2, void *pUnk1, void *pUnk2 ) = 0;
	virtual void unk112( int nUnk1, int nUnk2, int nUnk3, void *pUnk ) = 0;
	virtual void unk113( void *pUnk1, void *pUnk2, void *pUnk3 ) = 0;
	virtual void unk114( void *pUnk1, char *pUnk2, void *pUnk3 ) = 0;
	// Looks the entity up and calls unk114
	virtual void unk115( CEntityIndex nEntityIndex, void *pUnk ) = 0;
	virtual void LaunchApplicationOnExit( const char *pszValue ) = 0;
	// Returns its argument
	virtual void *unk117( void *pUnk ) = 0;
	virtual bool unk118() = 0;
	virtual float unk119() = 0;
	virtual void unk120() = 0;
	virtual void unk121() = 0;
	virtual void unk122() = 0;
	virtual bool unk123() = 0;
	virtual void UpdateWhenNotInGame( float flFrameTime ) = 0;
	virtual void UpdateGlobalLightFromLightInfos( void *p ) = 0;
	virtual bool GetGlobalLightParameters( void *p ) = 0;
	virtual void OnReceivedUpdatedServerPeerList( void *pPeerList, size_t nPeerCount ) = 0;
	virtual void UpdateEventListeners() = 0;
	virtual void *GetEconItemSystem() = 0;
	// Forwards to the game network string tables
	virtual bool GameCreateNetworkStringTables() = 0;
	// Fires the "client_disconnect" game event
	virtual void NotifyDisconnect( int nReason ) = 0;
	// Releases *pCombinedModel and clears *pUnk
	virtual void GetModelCombineInfo( CWeakHandle< InfoForResourceTypeCModel > hModel, CStrongHandleCopyable< InfoForResourceTypeCModel > *pCombinedModel, int *pUnk, const void **ppUnk1, const void **ppUnk2 ) = 0;
	// Save/restore block handling
	virtual void unk133( void *pUnk1, void *pUnk2, int nUnk, void *pUnk3 ) = 0;
	virtual void unk134( void *pUnk1, void *pUnk2, int nUnk, void *pUnk3 ) = 0;
	// Returns "core.global_stack"
	virtual const char *unk135() = 0;
	virtual bool unk136() = 0;
	// Writes NULL and returns 0
	virtual int unk137( void **ppOut ) = 0;
	virtual void unk138( void *pUnk ) = 0;
	virtual bool unk139( const char **ppszUnk, int nUnk, CUtlString *pOut ) = 0;
	virtual void *unk140() = 0;
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
	virtual void unk190() = 0;
	virtual void unk191() = 0;
	virtual void unk192() = 0;
	virtual void unk193() = 0;
	virtual void unk194() = 0;
	virtual void *unk195() = 0;
	virtual float unk196() = 0;
	virtual float unk197() = 0;
	virtual void unk198( void *pUnk1, void *pUnk2, void *pUnk3 ) = 0;
	virtual bool unk199( void *pUnk1, void *pUnk2, void *pUnk3 ) = 0;
	virtual void unk200() = 0;
	virtual void unk201( void *pUnk1, void *pUnk2 ) = 0;
	virtual bool unk202() = 0;
	virtual void unk203() = 0;
	virtual void *unk204() = 0;
	virtual bool unk205( CEntityIndex nEntityIndex ) = 0;
	virtual CEntityHandle unk206( CEntityIndex nEntityIndex ) = 0;
	virtual CEntityHandle unk207( int nUnk ) = 0;
	virtual void unk208( const char *pszUnk, int *pOut ) = 0;
	virtual void unk209( int nUnk, void *pUnk ) = 0;
};

class CSource2Client : public CGameAppSystem< ISource2Client >, public IStreamEntitiesFromFileCompletion
{
public:
	uint8 m_unk038[ 0x10 ];
	// Set by LaunchApplicationOnExit()
	CUtlString m_unk048;
	uint8 m_unk050[ 0x8 ];
	GameSessionConfiguration_t m_unk058;
	uint8 m_unk0F0[ 0x58 ];
	// Game event names filled by the constructor
	CUtlVector< const char * > m_unk148;
	uint8 m_unk160[ 0x30 ];
	CUtlVector< const char * > m_unk190;
	uint8 m_unk1A8[ 0x30 ];
	CUtlVector< const char * > m_unk1D8;
	uint8 m_unk1F0[ 0x30 ];
	CUtlVector< const char * > m_unk220;
	uint8 m_unk238[ 0x30 ];
	CUtlVector< const char * > m_unk268;
	uint8 m_unk280[ 0x30 ];
	CUtlVector< const char * > m_unk2B0;
	uint8 m_unk2C8[ 0x30 ];
	CUtlVector< const char * > m_unk2F8;
	uint8 m_unk310[ 0x30 ];
	CUtlVector< const char * > m_unk340;
	uint8 m_unk358[ 0x30 ];
	CUtlVector< const char * > m_unk388;
	uint8 m_unk3A0[ 0x30 ];
	uint8 m_unk3D0[ 0x18 ];
	// CUtlVector of 0x20-byte entries filled by InitInstantReplayData() and purged by ClearInstantReplayData()
	uint8 m_unk3E8[ 0x18 ];
	CTSQueue< std::function< void() > * > m_unk400;
	int m_unk440;
	bool m_unk444;
	uint8 m_unk445[ 0xB ];
	void *m_unk450;
	void *m_unk458;
	// Waited for and released by FrameStageNotify()
	CThreadedJob *m_unk460;
	float m_unk468;
	uint8 m_unk46C[ 0x1C ];
};

#endif // ISOURCE2CLIENT_H
