//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: The Counter-Strike bot manager: scenario zones, bomb knowledge, radio
//          timestamps and the game events it forwards to the bots.
//
//=============================================================================//

#ifndef CS_BOT_MANAGER_H
#define CS_BOT_MANAGER_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "mathlib/vector.h"
#include "tier1/utlvector.h"
#include "igameevents.h"
#include "entityhandle.h"
#include "bot_manager.h"
#include "cs_bot_timers.h"
#include "cs_bot_chatter.h"

class CBaseEntity;
class CNavArea;
class CCSPlayerPawn;

//-----------------------------------------------------------------------------
// A game event listener that also reports the event it listens to.
//-----------------------------------------------------------------------------
abstract_class BotEventInterface : public IGameEventListener2
{
public:
	virtual const char *GetEventName() const = 0;
};

// Every listener registers itself with the game event manager in the bot manager's
// constructor and calls the matching On<Event> of the bot manager from FireGameEvent.
#define DECLARE_CSBOTMANAGER_EVENT_LISTENER( EventClass, EventName ) \
	class EventClass##Event : public BotEventInterface \
	{ \
	public: \
		void FireGameEvent( IGameEvent *event ) override {} \
		const char *GetEventName() const override { return EventName; } \
	\
	public: \
		bool m_enabled; \
	};

abstract_class CCSBotManager : public CBotManager
{
public:
	~CCSBotManager() override = 0;

	// Empty.
	void ClientDisconnect( CBaseEntity *pEntity ) override = 0;
	// Returns false.
	bool ClientCommand( CBasePlayerController *pController, const CCommand &args ) override = 0;
	// Reloads botchatter.db and the voice banks, BotPackList.db or botprofile.db,
	// reads the scenario zones and restarts the round.
	void ServerActivate() override = 0;
	void ServerDeactivate() override = 0;
	// Returns false.
	bool ServerCommand( const char *pszCommand ) override = 0;
	void RestartRound() override = 0;
	void StartFrame() override = 0;
	// 0 for the bomb carrier, or a CT escorting hostages, else the bot ID plus one.
	unsigned int GetPlayerPriority( CCSPlayerPawn *pPlayer ) const override = 0;

	// True for the bomb carrier on the defuse scenario.
	virtual bool IsImportantPlayer( CCSPlayerPawn *pPlayer ) const = 0;

	virtual void OnPlayerFootstep( IGameEvent *event ) = 0;
	// A player_radio event with slot RADIO_ENEMY_SPOTTED also refreshes m_lastSeenEnemyTimestamp.
	virtual void OnPlayerRadio( IGameEvent *event ) = 0;
	virtual void OnPlayerDeath( IGameEvent *event ) = 0;
	virtual void OnPlayerFallDamage( IGameEvent *event ) = 0;
	virtual void OnBombPickedUp( IGameEvent *event ) = 0;
	virtual void OnBombPlanted( IGameEvent *event ) = 0;
	virtual void OnBombBeep( IGameEvent *event ) = 0;
	virtual void OnBombDefuseBegin( IGameEvent *event ) = 0;
	virtual void OnBombDefused( IGameEvent *event ) = 0;
	virtual void OnBombDefuseAbort( IGameEvent *event ) = 0;
	virtual void OnBombExploded( IGameEvent *event ) = 0;
	virtual void OnRoundEnd( IGameEvent *event ) = 0;
	virtual void OnRoundPreStart( IGameEvent *event ) = 0;
	virtual void OnRoundFreezeEnd( IGameEvent *event ) = 0;
	virtual void OnDoorMoving( IGameEvent *event ) = 0;
	virtual void OnBreakProp( IGameEvent *event ) = 0;
	virtual void OnBreakBreakable( IGameEvent *event ) = 0;
	virtual void OnHostageFollows( IGameEvent *event ) = 0;
	virtual void OnHostageRescuedAll( IGameEvent *event ) = 0;
	virtual void OnWeaponFire( IGameEvent *event ) = 0;
	virtual void OnWeaponFireOnEmpty( IGameEvent *event ) = 0;
	virtual void OnWeaponReload( IGameEvent *event ) = 0;
	virtual void OnWeaponZoom( IGameEvent *event ) = 0;
	virtual void OnBulletImpact( IGameEvent *event ) = 0;
	virtual void OnHEGrenadeDetonate( IGameEvent *event ) = 0;
	virtual void OnFlashbangDetonate( IGameEvent *event ) = 0;
	virtual void OnSmokeGrenadeDetonate( IGameEvent *event ) = 0;
	virtual void OnMolotovDetonate( IGameEvent *event ) = 0;
	virtual void OnDecoyDetonate( IGameEvent *event ) = 0;
	virtual void OnDecoyFiring( IGameEvent *event ) = 0;
	virtual void OnGrenadeBounce( IGameEvent *event ) = 0;
	virtual void OnNavBlocked( IGameEvent *event ) = 0;
	virtual void OnServerShutdown( IGameEvent *event ) = 0;

public:
	enum GameScenarioType
	{
		SCENARIO_DEATHMATCH,
		SCENARIO_DEFUSE_BOMB,
		SCENARIO_RESCUE_HOSTAGES
	};

	enum
	{
		MAX_ZONES = 4,
		MAX_ZONE_NAV_AREAS = 64
	};

	// A bomb site or a hostage rescue zone.
	struct Zone
	{
		CBaseEntity *m_entity;
		CNavArea *m_area[ MAX_ZONE_NAV_AREAS ];
		int m_areaCount;
		Vector m_center;
		// The zone is an info_ point entity: m_center with a 256 unit box.
		bool m_isLegacy;
		int m_index;
		bool m_isBlocked;
		Vector m_extentLo;
		Vector m_extentHi;
	};

	DECLARE_CSBOTMANAGER_EVENT_LISTENER( PlayerFootstep, "player_footstep" )
	DECLARE_CSBOTMANAGER_EVENT_LISTENER( PlayerRadio, "player_radio" )
	DECLARE_CSBOTMANAGER_EVENT_LISTENER( PlayerDeath, "player_death" )
	DECLARE_CSBOTMANAGER_EVENT_LISTENER( PlayerFallDamage, "player_falldamage" )
	DECLARE_CSBOTMANAGER_EVENT_LISTENER( BombPickedUp, "bomb_pickup" )
	DECLARE_CSBOTMANAGER_EVENT_LISTENER( BombPlanted, "bomb_planted" )
	DECLARE_CSBOTMANAGER_EVENT_LISTENER( BombBeep, "bomb_beep" )
	DECLARE_CSBOTMANAGER_EVENT_LISTENER( BombDefuseBegin, "bomb_begindefuse" )
	DECLARE_CSBOTMANAGER_EVENT_LISTENER( BombDefused, "bomb_defused" )
	DECLARE_CSBOTMANAGER_EVENT_LISTENER( BombDefuseAbort, "bomb_abortdefuse" )
	DECLARE_CSBOTMANAGER_EVENT_LISTENER( BombExploded, "bomb_exploded" )
	DECLARE_CSBOTMANAGER_EVENT_LISTENER( RoundEnd, "round_end" )
	DECLARE_CSBOTMANAGER_EVENT_LISTENER( RoundPreStart, "round_prestart" )
	DECLARE_CSBOTMANAGER_EVENT_LISTENER( RoundFreezeEnd, "round_freeze_end" )
	DECLARE_CSBOTMANAGER_EVENT_LISTENER( DoorMoving, "door_moving" )
	DECLARE_CSBOTMANAGER_EVENT_LISTENER( BreakProp, "break_prop" )
	DECLARE_CSBOTMANAGER_EVENT_LISTENER( BreakBreakable, "break_breakable" )
	DECLARE_CSBOTMANAGER_EVENT_LISTENER( HostageFollows, "hostage_follows" )
	DECLARE_CSBOTMANAGER_EVENT_LISTENER( HostageRescuedAll, "hostage_rescued_all" )
	DECLARE_CSBOTMANAGER_EVENT_LISTENER( WeaponFire, "weapon_fire" )
	DECLARE_CSBOTMANAGER_EVENT_LISTENER( WeaponFireOnEmpty, "weapon_fire_on_empty" )
	DECLARE_CSBOTMANAGER_EVENT_LISTENER( WeaponReload, "weapon_reload" )
	DECLARE_CSBOTMANAGER_EVENT_LISTENER( WeaponZoom, "weapon_zoom" )
	DECLARE_CSBOTMANAGER_EVENT_LISTENER( BulletImpact, "bullet_impact" )
	DECLARE_CSBOTMANAGER_EVENT_LISTENER( HEGrenadeDetonate, "hegrenade_detonate" )
	DECLARE_CSBOTMANAGER_EVENT_LISTENER( FlashbangDetonate, "flashbang_detonate" )
	DECLARE_CSBOTMANAGER_EVENT_LISTENER( SmokeGrenadeDetonate, "smokegrenade_detonate" )
	DECLARE_CSBOTMANAGER_EVENT_LISTENER( MolotovDetonate, "molotov_detonate" )
	DECLARE_CSBOTMANAGER_EVENT_LISTENER( DecoyDetonate, "decoy_detonate" )
	DECLARE_CSBOTMANAGER_EVENT_LISTENER( DecoyFiring, "decoy_firing" )
	DECLARE_CSBOTMANAGER_EVENT_LISTENER( GrenadeBounce, "grenade_bounce" )
	DECLARE_CSBOTMANAGER_EVENT_LISTENER( NavBlocked, "nav_blocked" )
	DECLARE_CSBOTMANAGER_EVENT_LISTENER( ServerShutdown, "server_shutdown" )

	// Radio timestamp index for a team: 0 for terrorists, 1 for everyone else.
	static int RadioTeamIndex( int nTeam ) { return ( nTeam == 2 ) ? 0 : 1; }

	float GetRadioMessageTimestamp( RadioType event, int nTeam ) const
	{
		if ( event <= RADIO_START_1 || event >= RADIO_END )
			return 0.0f;

		return m_radioMsgTimestamp[ event - RADIO_START_1 ][ RadioTeamIndex( nTeam ) ];
	}

public:
	// True between ServerActivate and ServerDeactivate.
	bool m_serverActive;
	GameScenarioType m_gameScenario;
	Zone m_zone[ MAX_ZONES ];
	int m_zoneCount;
	bool m_isBombPlanted;
	float m_bombPlantTimestamp;
	// Set to a random 10 to 30 seconds after the round restart.
	float m_earliestBombPlantTimestamp;
	CEntityHandle m_bombDefuser;
	CEntityHandle m_looseBomb;
	CNavArea *m_looseBombArea;
	bool m_isRoundOver;
	CountdownTimer m_checkTransientAreasTimer;
	// The last time each radio message was sent, per team. See RadioTeamIndex.
	float m_radioMsgTimestamp[ RADIO_END - RADIO_START_1 ][ 2 ];
	// -9999.9 after the round restart.
	float m_lastSeenEnemyTimestamp;
	// The round restart time plus mp_freezetime.
	float m_roundStartTimestamp;
	bool m_isDefenseRushing;
	// Counted up by path computations; with throttle_expensive_ai one is allowed per frame.
	int m_nNumExpensiveOperationsThisFrame;

	// The round strategy: the terrorist plan and target bomb site, the counter-terrorist plan
	// and the site they expect. Nothing reads or writes them any more, so the names come from
	// their position alone.
	int m_eTStrat;
	int m_iTerroristTargetSite;
	int m_eCTStrat;
	int m_iCTPrioritySite;

	// Bots removed by a passed kick vote, per team: index 0 for terrorists, 1 for
	// counter-terrorists. bot_quota_mode "nokick" subtracts them from the quota. Cleared by
	// the constructor and by ServerActivate.
	int m_nVoteKickedBotCount[ 2 ];

	PlayerFootstepEvent m_PlayerFootstepEvent;
	PlayerRadioEvent m_PlayerRadioEvent;
	PlayerDeathEvent m_PlayerDeathEvent;
	PlayerFallDamageEvent m_PlayerFallDamageEvent;
	BombPickedUpEvent m_BombPickedUpEvent;
	BombPlantedEvent m_BombPlantedEvent;
	BombBeepEvent m_BombBeepEvent;
	BombDefuseBeginEvent m_BombDefuseBeginEvent;
	BombDefusedEvent m_BombDefusedEvent;
	BombDefuseAbortEvent m_BombDefuseAbortEvent;
	BombExplodedEvent m_BombExplodedEvent;
	RoundEndEvent m_RoundEndEvent;
	RoundPreStartEvent m_RoundPreStartEvent;
	RoundFreezeEndEvent m_RoundFreezeEndEvent;
	DoorMovingEvent m_DoorMovingEvent;
	BreakPropEvent m_BreakPropEvent;
	BreakBreakableEvent m_BreakBreakableEvent;
	HostageFollowsEvent m_HostageFollowsEvent;
	HostageRescuedAllEvent m_HostageRescuedAllEvent;
	WeaponFireEvent m_WeaponFireEvent;
	WeaponFireOnEmptyEvent m_WeaponFireOnEmptyEvent;
	WeaponReloadEvent m_WeaponReloadEvent;
	WeaponZoomEvent m_WeaponZoomEvent;
	BulletImpactEvent m_BulletImpactEvent;
	HEGrenadeDetonateEvent m_HEGrenadeDetonateEvent;
	FlashbangDetonateEvent m_FlashbangDetonateEvent;
	SmokeGrenadeDetonateEvent m_SmokeGrenadeDetonateEvent;
	MolotovDetonateEvent m_MolotovDetonateEvent;
	DecoyDetonateEvent m_DecoyDetonateEvent;
	DecoyFiringEvent m_DecoyFiringEvent;
	GrenadeBounceEvent m_GrenadeBounceEvent;
	NavBlockedEvent m_NavBlockedEvent;
	ServerShutdownEvent m_ServerShutdownEvent;

	// The frequently fired listeners, registered in the constructor: footstep, radio,
	// fall damage, bomb beep, door, both breaks, the four weapon events, bullet impact,
	// grenade bounce and nav blocked.
	CUtlVector< BotEventInterface * > m_commonEventListeners;
	bool m_eventListenersEnabled;
};

#undef DECLARE_CSBOTMANAGER_EVENT_LISTENER

#endif // CS_BOT_MANAGER_H
