#ifndef GAMERULES_H
#define GAMERULES_H

#pragma once

#include "tier0/platform.h"
#include "tier0/utlstring.h"
#include "mathlib/vector.h"
#include "playerslot.h"
#include "schemasystem/schematypes.h"

class CBaseEntity;
class CBasePlayerController;
class CBasePlayerPawn;
class CCommand;
class CNetworkSerializerClassInfo;
class CTakeDamageInfo;
class CUtlString;
class CViewVectors;
struct GameTick_t;
struct GameTime_t;
class KeyValues;
class CBufferString;
class CEntityIndex;
class INetworkStringTable;
struct EventActiveSpawnGroupChanged_t;
struct EventBuildGameSessionManifest_t;
struct EventGameActivate_t;
struct EventGameDeactivate_t;
struct EventGameInit_t;
struct EventGamePostInit_t;
struct EventGameShutdown_t;
struct EventPostSpawnGroupLoad_t;
struct EventPostSpawnGroupUnload_t;
struct EventPreSpawnGroupLoad_t;
struct EventPreSpawnGroupUnload_t;
struct EventRestoreGame_t;
struct EventSaveGame_t;
struct EventServerPreClientUpdate_t;
struct EventServerPreEntityThink_t;
struct EventSpawnGroupPrecache_t;

//-----------------------------------------------------------------------------
// CGameRules declares 119 slots: Linux 0-118, Windows 0-117.
//
// Virtual methods only, in the order of the Linux table; the class has no members.
// Unk_N is a method without a name, Stub_N one that does nothing in every class;
// N is the Windows slot.
//-----------------------------------------------------------------------------
abstract_class CGameRulesVTable
{
public:
	virtual SchemaMetaInfoHandle_t< CSchemaClassInfo > Schema_DynamicBinding() = 0;
	// Named by what it does. Returns a static object every class registers the way CBaseEntity registers the one of its method of this name
	virtual const CNetworkSerializerClassInfo *GetSerializerClassInfo() = 0;
	// CGameRulesGameSystem::PostInit calls it
	virtual void PostInit() = 0;
	// CGameRulesGameSystem::GameInit calls Init and then this
	virtual void GameInit( const EventGameInit_t &msg ) = 0;
	// Called by the method of the same name of CGameRulesGameSystem
	virtual void GamePostInit( const EventGamePostInit_t &msg ) = 0;
	// CGameRulesGameSystem::GameActivate calls it and then CreateStandardEntities
	virtual void GameActivate( const EventGameActivate_t &msg ) = 0;
	// Called by the method of the same name of CGameRulesGameSystem
	virtual void SpawnGroupPrecache( const EventSpawnGroupPrecache_t &msg ) = 0;
	// Called by the method of the same name of CGameRulesGameSystem
	virtual void PreSpawnGroupLoad( const EventPreSpawnGroupLoad_t &msg ) = 0;
	// Called by the method of the same name of CGameRulesGameSystem
	virtual void PostSpawnGroupLoad( const EventPostSpawnGroupLoad_t &msg ) = 0;
	// Called by the method of the same name of CGameRulesGameSystem
	virtual void PreSpawnGroupUnload( const EventPreSpawnGroupUnload_t &msg ) = 0;
	// Called by the method of the same name of CGameRulesGameSystem
	virtual void PostSpawnGroupUnload( const EventPostSpawnGroupUnload_t &msg ) = 0;
	// Named after the event of IGameSystem whose handler in CGameRulesGameSystem calls it
	virtual void ActiveSpawnGroupChanged( const EventActiveSpawnGroupChanged_t &msg ) = 0;
	// CGameRulesGameSystem::GameDeactivate calls DestroyStandardEntities and then this
	virtual void GameDeactivate( const EventGameDeactivate_t &msg ) = 0;
	// Between GameDeactivate and SaveGame
	virtual void GameShutdown( const EventGameShutdown_t &msg ) = 0;
	// Called by the method of the same name of CGameRulesGameSystem
	virtual void SaveGame( const EventSaveGame_t &msg ) = 0;
	// Called by the method of the same name of CGameRulesGameSystem
	virtual void RestoreGame( const EventRestoreGame_t &msg ) = 0;
	// Named by its strings. CCSGameRules adds ParticleEffect and ParticleEffectStop to the manifest
	virtual void BuildGameSessionManifest( const EventBuildGameSessionManifest_t &msg ) = 0;
	// Named after the event of IGameSystem (slot 52) whose handler in CGameRulesGameSystem calls it
	virtual void NewLevelPlayerConnect( const void *pMsg ) = 0;
	// Named by its strings. The profiler name of the CCSGameRules method
	virtual void FrameUpdatePreEntityThink( const EventServerPreEntityThink_t &msg ) = 0;
	// The ServerPreClientUpdate handler of CGameRulesGameSystem calls it
	virtual void PreClientUpdate( const EventServerPreClientUpdate_t &msg ) = 0;
	// Two slots on Linux, one on Windows
	virtual ~CGameRulesVTable() = 0;
	// The first method after the destructor, replaced by CMultiplayRules
	virtual bool Init() = 0;
	virtual void Shutdown() = 0;
	// Empty in every class, on the client too. The first of a pair: FindPickerEntity calls it first and Stub_24 at the end, and a scope object does the same around ent_remove, ent_gib, ent_kill and create_explosion_damage
	virtual void Stub_23() = 0;
	// Empty in every class, on the client too. The second of the pair that Stub_23 opens
	virtual void Stub_24() = 0;
	virtual CBaseEntity *FindPickerEntity( CBasePlayerController *pPlayer ) = 0;
	// Named by what it does. CMultiplayRules tests the mask 0x2E1, which is crush, fall, blast, club and sonic
	virtual bool Damage_ShouldGibCorpse( int nDamageType ) = 0;
	// Named by what it does. CMultiplayRules tests the mask 0x7C029: fall, burn, drown, crush and the time based types
	virtual bool Damage_NoPhysicsForce( int nDamageType ) = 0;
	// Named by what it does. CMultiplayRules tests the mask 0x48000, poison and acid
	virtual bool Damage_ShouldNotBleed( int nDamageType ) = 0;
	// Named by what it does. CCSGameRules registers csgo_team1, csgo_grenadeclip and others
	virtual void RegisterGameSpecificInteractionLayers( void *pUnknown ) = 0;
	// Named by what it does. CCSGameRules sets pairs of collision groups through the controller it is given
	virtual void InitializeGameSpecificCollisionGroups( void *pController ) = 0;
	// A constant, replaced by CCSGameRules
	virtual int DefaultFOV() = 0;
	// A static table, replaced by CCSGameRules
	virtual const CViewVectors *GetViewVectors() const = 0;
	// A constant between GetViewVectors and IsMultiplayer
	virtual float GetDamageMultiplier() = 0;
	// CMultiplayRules returns true
	virtual bool IsMultiplayer() = 0;
	// The slot after IsMultiplayer; CLogicAuto::Think asks it. Returns false in every class
	virtual bool InRoundRestart() = 0;
	// Named by its strings. CCSGameRules registers the script functions of the game rules ("Returns whether the player is playing with a controller or not" and others)
	virtual void RegisterScriptFunctions() = 0;
	// Named by what it does. C_BaseEntity::CheckInitPredictable of the client makes an entity predictable only when this answers true. False here, true in CMultiplayRules, and CCSGameRules says false in the game phases 4 and 5 (halftime, game over)
	virtual bool IsEntityPredictionAllowed() = 0;
	// Returns an empty value in every class: one pointer set to null through the hidden result, a string or a symbol
	virtual CUtlString Stub_38() = 0;
	// Named by its strings. CCSGameRules handles CrosshairCode. ISource2GameClients::ClientCommandKeyValues finds the controller of the slot and passes it with the key values
	virtual void ClientCommandKeyValues( CBasePlayerController *pPlayer, KeyValues *pKeyValues ) = 0;
	// Returns true in every class
	virtual bool Stub_40() = 0;
	// Named by what it does. A weapon adds the use capability to ObjectCaps and sets up its pickup touch only when this answers true; the same in client.dll. Returns true in every class
	virtual bool AllowWeaponPickup() = 0;
	// Returns true in every class (one function on Linux for all three classes); ObjectCaps of a weapon adds capability bit 1 when it does and CBreakableProp::Spawn asks it, on the server and in client.dll
	virtual bool Stub_42() = 0;
	// CEnvSoundscape::IsEnabled asks it after its own m_bDisabled; in client.dll the callers are the CEnvSoundscape script functions. Returns true in every class
	virtual bool AllowSoundscapes() = 0;
	// Returns false in every class, on the client too; no caller found
	virtual bool Stub_44() = 0;
	// The slot before GetTaggedConVarList; ISource2Server::PrintStatus forwards here when the engine runs the status command, with -1 from the server console or the entity index of the client and an empty CBufferString. Empty in every class
	virtual void PrintStatus( CEntityIndex nPlayerEntityIndex, CBufferString *pOutput ) = 0;
	// Named by its strings. CMultiplayRules lists sv_gravity
	virtual void GetTaggedConVarList( KeyValues *pCvarTagList ) = 0;
	// Named by what it does. Called by the HTTP uploader of the log (X-LogBytes-BeginOffset, X-Tick-Start, X-SteamID); CCSGameRules adds Map, State (Warmup, Active or Finished), the team names and the scores
	virtual void AddLogUploadFields( void *pFields ) = 0;
	// Empty; CCSGameRules replaces it with an empty method. No caller found
	virtual void LevelShutdown() = 0;
	// CGameRulesGameSystem::GameInit calls it after Init with true
	virtual void RefreshSkillData( bool bForceUpdate ) = 0;
	// Named by its strings. The profiler name
	virtual void Think() = 0;
	// The slot after Think; CSource2Server::GameFrame calls it every frame. Empty in every class
	virtual void EndGameFrame() = 0;
	// Named by what it does. Compares GetSkillLevel with the argument
	virtual bool IsSkillLevel( int nLevel ) = 0;
	// Named by what it does. Returns a global
	virtual int GetSkillLevel() = 0;
	// Between GetSkillLevel and SetSkillLevel
	virtual void OnSkillLevelChanged( int nLevel ) = 0;
	// Named by what it does. Stores the argument, clamped, in the global GetSkillLevel returns
	virtual void SetSkillLevel( int nLevel ) = 0;
	// Pure here, CMultiplayRules returns true
	virtual bool IsDeathmatch() = 0;
	// Pure here, CMultiplayRules returns false
	virtual bool IsCoOp() = 0;
	// Named by its strings. Fires client_connected. ISource2GameClients::ClientConnect passes its slot, name, network id and reject reason, not the xuid and the load game flag
	virtual bool ClientConnected( CPlayerSlot slot, const char *pszName, const char *pszNetworkID, CBufferString *pRejectReason ) = 0;
	// Named by its strings. Fires client_disconnected. ISource2GameClients::ClientDisconnect passes its slot and its ENetworkDisconnectionReason
	virtual void ClientDisconnected( CPlayerSlot slot, int nReason ) = 0;
	// And ISource2Server::ShouldTimeoutClient, which forwards here. Returns false in every class
	virtual bool ShouldTimeoutClient( int nUserID, float flTimeSinceLastReceived ) = 0;
	// Named by what it does. CCSGameRules refuses the damage of a teammate unless friendly fire allows it
	virtual bool FPlayerCanTakeDamage( CBasePlayerPawn *pPlayer, CBaseEntity *pAttacker ) = 0;
	// Returns 1; CSingleplayRules returns a convar. Asked by CBaseEntity::GetAutoAimRadius and CBreakableProp::Spawn
	virtual int GetAutoAimMode() = 0;
	// Between FPlayerCanTakeDamage and FlPlayerFallDeathDoesScreenFade; replaced by CCSGameRules. The callers pass a damage info, whose damage position (+20) the debug overlay draws and whose attacker handle (+60) is read, and null
	virtual void RadiusDamage( const CTakeDamageInfo &info, void *pUnknown ) = 0;
	// Returns true, CCSGameRules false
	virtual bool FlPlayerFallDeathDoesScreenFade( CBasePlayerPawn *pPlayer ) = 0;
	// Named by what it does. CCSGameRules asks the spawn point about the player
	virtual bool IsSpawnPointValid( CBaseEntity *pSpot, CBasePlayerPawn *pPlayer ) = 0;
	// Named by its strings. Changeteam and single_player_pause
	virtual bool ClientCommand( CBaseEntity *pEntity, const CCommand &args ) = 0;
	// CCSGameRules reads cl_autohelp
	virtual void ClientSettingsChanged( CBasePlayerController *pPlayer ) = 0;
	// Returns null or false in every class
	virtual void *Stub_68() = 0;
	// Named by what it does. The killer when it is a player, else the owner found through the inflictor
	virtual CBasePlayerPawn *GetDeathScorer( CBaseEntity *pKiller, CBaseEntity *pInflictor ) = 0;
	// Named by what it does. Calls the two argument one. The name differs because the game keeps the pair in this order on Windows too
	virtual CBasePlayerPawn *GetDeathScorerForVictim( CBaseEntity *pKiller, CBaseEntity *pInflictor, CBaseEntity *pVictim ) = 0;
	// Named by what it does. CMultiplayRules answers by a spawn flag of the item
	virtual int ItemShouldRespawn( CBaseEntity *pItem ) = 0;
	// Named by what it does. CMultiplayRules returns the current time plus 30
	virtual float FlItemRespawnTime( CBaseEntity *pItem ) = 0;
	// Named by what it does. CMultiplayRules returns the origin of the item
	virtual Vector VecItemRespawnSpot( CBaseEntity *pItem ) = 0;
	// Named by what it does. CMultiplayRules returns the angles of the item
	virtual QAngle VecItemRespawnAngles( CBaseEntity *pItem ) = 0;
	// Named by what it does. CCSGameRules fills the 13 by 13 table of relationships; CGameRulesGameSystem::GameActivate calls it
	virtual void InitDefaultAIRelationships() = 0;
	// Pure here, replaced by CMultiplayRules and CCSGameRules, the first of two such slots before CheckChatText
	virtual int PlayerRelationship( CBaseEntity *pPlayer, CBaseEntity *pTarget ) = 0;
	// The second of the two. CMultiplayRules and CCSGameRules read two arguments, the controllers, and CMultiplayRules answers PlayerRelationship of their pawns == GR_TEAMMATE; no call site found (Host_Say no longer calls it)
	virtual bool PlayerCanHearChat( CBasePlayerController *pListener, CBasePlayerController *pSpeaker ) = 0;
	// Host_Say passes the speaker and the text before it tests the text for empty. Empty in every class
	virtual void CheckChatText( CBasePlayerController *pPlayer, char *pszText ) = 0;
	// Named by its strings. CCSGameRules writes the say line of the log. Host_Say passes the controller, its team (0 for the console), its name ("Console" without one) and the text
	virtual void OnPlayerChatted( CBasePlayerController *pPlayer, int nTeam, const char *pszName, const char *pszText ) = 0;
	// Empty; CSingleplayRules sets the flag IsSinglePlayerGameEnding returns
	virtual void NotifySinglePlayerGameEnding() = 0;
	// CSource2Server::GameFrame sends autosavedangerousissafe only when it answers false, and the level change trigger then only slows the player down. Returns false; CSingleplayRules returns its flag
	virtual bool IsSinglePlayerGameEnding() = 0;
	// Named by what it does. CCSGameRules returns m_bForceTeamChangeSilent. No class reads an argument and ChangeTeam passes none
	virtual bool IsTeamChangeSilent() = 0;
	// Pure here, the last such slot before CreateStandardEntities; CCSGameRules returns true
	virtual bool FAllowNPCs() = 0;
	// Two slots before GetChatPrefix; the largest method CCSGameRules replaces here
	virtual void CreateStandardEntities() = 0;
	// The slot before GetChatPrefix
	virtual void DestroyStandardEntities() = 0;
	// Named by its strings. *DEAD*(TEAM)
	virtual const char *GetChatPrefix( bool bTeamOnly, CBasePlayerController *pPlayer ) = 0;
	// The slot after GetChatPrefix
	virtual const char *GetChatLocation( bool bTeamOnly, CBasePlayerController *pPlayer ) = 0;
	// Named by its strings. Cstrike_Chat_All
	virtual const char *GetChatFormat( bool bTeamOnly, CBasePlayerController *pPlayer, int nUnknown ) = 0;
	// CGameRulesGameSystem::GameInit calls it. Empty in every class
	virtual void CreateCustomNetworkStringTables() = 0;
	// Named by what it does. Counts the statistic and tells the achievement manager of the engine
	virtual void MarkAchievement( void *pFilter, const char *pszAchievementName ) = 0;
	// And ISource2Server::PreFatalShutdown, which forwards here. Empty in every class
	virtual void PreFatalShutdown() = 0;
	// Named by what it does. TakeDamage of an entity drops the damage when this answers 1, after RejectDamage of the entity itself, on the server and in client.dll; pure here, CCSGameRules returns 0
	virtual int RejectDamage( CBaseEntity *pVictim, const CTakeDamageInfo &info ) = 0;
	// Returns true, CCSGameRules a convar; the slot before GetMaxHumanPlayers
	virtual bool ForceSplitScreenPlayersOnToSameTeam() = 0;
	// Returns -1, replaced by CCSGameRules
	virtual int GetMaxHumanPlayers() = 0;
	// The slot after GetMaxHumanPlayers; compares the argument with a handle
	virtual bool CanUnloadSpawnGroup( uint32 hSpawnGroup ) = 0;
	// ISource2GameEntities::ShouldClientReceiveStringTableUserData asks it with the slot of its transmit info, the table and the string number. Returns true in every class
	virtual bool ShouldPlayerReceiveStringTableUserData( CPlayerSlot slot, const INetworkStringTable *pTable, int nStringNumber ) = 0;
	// Named by what it does. The entity cast to a player pawn, null for any other
	virtual CBasePlayerPawn *ToBasePlayerPawn( CBaseEntity *pEntity ) = 0;
	// The search of entities by name passes its five arguments here when its own lookup finds nothing, on the server and in client.dll. Returns null in every class
	virtual CBaseEntity *FindEntityByUnitType( CBaseEntity *pStartEntity, const char *pszName, CBaseEntity *pSearchingEntity, CBaseEntity *pActivator, CBaseEntity *pCaller ) = 0;
	// The slot after FindEntityByUnitType. Returns null in every class. The arguments are not checked
	virtual CBaseEntity *FindEntityByUnitTypeWithin( CBaseEntity *pStartEntity, const char *pszName, const Vector &vecSrc, float flRadius, CBaseEntity *pSearchingEntity, CBaseEntity *pActivator, CBaseEntity *pCaller ) = 0;
	// The last of the three. Returns null in every class. The arguments are not checked
	virtual CBaseEntity *FindEntityByUnitTypeNearest( const char *pszName, const Vector &vecSrc, float flRadius, CBaseEntity *pSearchingEntity, CBaseEntity *pActivator, CBaseEntity *pCaller ) = 0;
	// Named by its strings. The map name, or SOURCE2 SAVE COMMENT; returned by value, the callers pass the hidden result
	virtual CUtlString GetSaveGameComment() = 0;
	// Named by its strings and what it does. Game_vars, gc_status and the frame time percentiles, written once when status_json ran in the traced game; replaced by CCSGameRules
	virtual void WriteStatusJson( void *pWriter ) = 0;
	// Named by its strings. CGameRules - paused on tick. The body reads one argument
	virtual void SetGamePaused( bool bPaused ) = 0;
	// Named by what it does. Returns m_bGamePaused
	virtual bool IsGamePaused() = 0;
	// Named by what it does. True when the engine is paused, else IsGamePaused unless the argument is set; asked every frame
	virtual bool IsPaused( bool bIgnoreGamePause ) = 0;
	// Named by what it does. The frame time of the globals, 0 while IsPaused says so
	virtual float GetFrameTime( bool bIgnoreGamePause ) = 0;
	// Named by what it does. The current time of the globals through SubtractPausedTime; the argument picks another clock of the globals
	virtual GameTime_t GetGameTime( bool bUnknown ) = 0;
	// Named by what it does. The current tick of the globals through SubtractPausedTicks; the argument picks another counter of the globals
	virtual GameTick_t GetGameTick( bool bUnknown ) = 0;
	// Named by what it does. The tick plus m_nTotalPausedTicks, unchanged when the second argument is set
	virtual GameTick_t AddPausedTicks( GameTick_t nTick, bool bKeep ) = 0;
	// Named by what it does. The tick minus m_nTotalPausedTicks, and not past m_nPauseStartTick while the game is paused; unchanged when the second argument is set. Called for every tick the game reads
	virtual GameTick_t SubtractPausedTicks( GameTick_t nTick, bool bKeep ) = 0;
	// Named by what it does. The time plus m_nTotalPausedTicks ticks, unchanged when the first argument is set. The order of the arguments is the one the Linux build shows
	virtual GameTime_t AddPausedTime( bool bKeep, GameTime_t flTime ) = 0;
	// Named by what it does. The counterpart of SubtractPausedTicks for a time; warns when the game is paused without a pause start tick
	virtual GameTime_t SubtractPausedTime( GameTime_t flTime, bool bKeep ) = 0;
	// Named by what it does. Clears m_nTotalPausedTicks and m_nPauseStartTick
	virtual void ResetPausedTicks() = 0;
	// Named by ISource2Server::OnHostFrameTiming, which forwards here every frame. Empty in every class
	virtual void OnHostFrameTiming( const double *pFrameTimes ) = 0;
	// Returns null in every class: CCSGameRules has its own copy with the same body (xor eax, eax). Called nowhere: no devirtualised compare, no call through g_pGameRules on the server, the trace never saw it, and the client table has no such slot. The last pass (data/proposals/last_unk_sdk.md) found nothing new
	virtual void *Stub_115() = 0;
	// Named by what it does. Returns Plat_GetTime
	virtual double GetRealTime() = 0;
	// Named by what it does. True when the second team is a playing one and is not the first
	virtual bool IsOtherPlayingTeam( int nTeam, int nOtherTeam ) = 0;
};

#endif // GAMERULES_H
