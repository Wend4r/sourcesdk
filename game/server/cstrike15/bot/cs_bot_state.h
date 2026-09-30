//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: The behavior states of the Counter-Strike bot. The bot owns one instance of each
//          and points its state machine at one of them at a time.
//
//=============================================================================//

#ifndef CS_BOT_STATE_H
#define CS_BOT_STATE_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "mathlib/vector.h"
#include "ehandle.h"
#include "tier0/utlstring.h"
#include "cs_bot_timers.h"

class CBaseEntity;
class CCSBot;
class CEconItemDefinition;
class CCSPlayerPawn;
class CNavArea;

//-----------------------------------------------------------------------------
// The virtuals have bodies because the bot embeds the states by value; the game's own
// vtable is the one in place on every state it constructs.
//-----------------------------------------------------------------------------
class BotState
{
public:
	virtual void OnEnter( CCSBot *pBot ) {}
	virtual void OnUpdate( CCSBot *pBot ) {}
	virtual void OnExit( CCSBot *pBot ) {}
	virtual const char *GetName() const { return ""; }
};

// The main action selection: a bot never stays in it.
class IdleState : public BotState
{
public:
	const char *GetName() const override { return "Idle"; }
};

class HuntState : public BotState
{
public:
	const char *GetName() const override { return "Hunt"; }

public:
	// The far away area being moved to.
	CNavArea *m_huntArea;
};

class AttackState : public BotState
{
public:
	const char *GetName() const override { return "Attack"; }

	enum DodgeStateType
	{
		STEADY_ON,
		SLIDE_LEFT,
		SLIDE_RIGHT,
		JUMP,

		NUM_ATTACK_STATES
	};

public:
	DodgeStateType m_dodgeState;
	float m_nextDodgeStateTimestamp;
	CountdownTimer m_repathTimer;
	float m_scopeTimestamp;
	// False until the enemy is seen during this attack, e.g. when a friend reported it.
	bool m_haveSeenEnemy;
	// Line of sight to the enemy is lost.
	bool m_isEnemyHidden;
	// When firing may resume after the enemy went behind cover.
	float m_reacquireTimestamp;
	float m_shieldToggleTimestamp;
	bool m_shieldForceOpen;
	float m_pinnedDownTimestamp;
	// Cleared on exit.
	bool m_crouchAndHold;
	bool m_didAmbushCheck;
	bool m_shouldDodge;
	bool m_firstDodge;
	// Retreats when outnumbered during this fight.
	bool m_isCoward;
	CountdownTimer m_retreatTimer;
};

class InvestigateNoiseState : public BotState
{
public:
	const char *GetName() const override { return "InvestigateNoise"; }

public:
	Vector m_checkNoisePosition;
	// The least time spent on the current noise.
	CountdownTimer m_minTimer;
};

class BuyState : public BotState
{
public:
	const char *GetName() const override { return "Buy"; }

public:
	bool m_isInitialDelay;
	int m_prefRetries;
	// Position in the profile's list of preferred weapons.
	int m_prefIndex;
	int m_retries;
	bool m_doneBuying;
	bool m_buyDefuseKit;
	bool m_buyGrenade;
	bool m_buyShield;
	bool m_buyPistol;
};

class MoveToState : public BotState
{
public:
	const char *GetName() const override { return "MoveTo"; }

public:
	Vector m_goalPosition;
	// The route type the path is computed with.
	int m_routeType;
	bool m_radioedPlan;
	bool m_askedForCover;
};

//-----------------------------------------------------------------------------
// Follows a recorded human path. The bot_path command ("Tells a specific bot to follow a
// human path, matching the given criteria") and the bomb site and grenade radio requests
// fill in the criteria and then call the state's setup, which always fails in this build,
// so the bot never enters it. Its own OnEnter, OnUpdate and OnExit are empty.
//-----------------------------------------------------------------------------
class HumanPathFollowState : public BotState
{
public:
	const char *GetName() const override { return "HumanPathFollow"; }

	// A weapon looked up by name in the item schema, cached until the schema changes.
	struct CachedItemDefinition_t
	{
		// A weapon class name such as "weapon_smokegrenade".
		const char *m_pszName;
		const CEconItemDefinition *m_pItemDef;
		// The item schema version m_pItemDef was looked up for. The constructor sets one
		// less than the current version, forcing a lookup.
		int m_nSchemaVersion;
	};

public:
	// Left alone by the constructor; nothing found reads it.
	uint8 m_Unk08[ 0x44 ];
	// Cleared by the constructor.
	bool m_bUnk4C;
	// Set together with m_desiredGrenade.
	bool m_hasDesiredGrenade;
	// The grenade to throw along the path: smoke, HE, molotov, incendiary, flashbang or decoy.
	CachedItemDefinition_t m_desiredGrenade;
	// The path name criterion: the first bot_path argument, or "_to_BombsiteA" and
	// "_to_BombsiteB" from the radio requests.
	CUtlString m_pathName;
	// Left alone by the constructor; nothing found reads it.
	uint8 m_Unk70[ 0x20 ];

	// Zeroed by the constructor; nothing found reads them.
	uint64 m_nUnk90;
	uint64 m_nUnk98;
	uint64 m_nUnkA0;
	int m_nUnkA8;
	int16 m_nUnkAC;
	uint64 m_nUnkB0;
	int m_nUnkB8;
	// Freed by the bot's destructor.
	CUtlString m_strUnkC0[ 6 ];
	bool m_bUnkF0;
	int m_nUnkF4;
};

class FetchBombState : public BotState
{
public:
	const char *GetName() const override { return "FetchBomb"; }
};

class PlantBombState : public BotState
{
public:
	const char *GetName() const override { return "PlantBomb"; }
};

class DefuseBombState : public BotState
{
public:
	const char *GetName() const override { return "DefuseBomb"; }
};

class PickupHostageState : public BotState
{
public:
	const char *GetName() const override { return "PickupHostage"; }

public:
	CHandle< CBaseEntity > m_entity;
};

// Includes moving to the hiding spot, which may be across the map.
class HideState : public BotState
{
public:
	const char *GetName() const override { return "Hide"; }

public:
	CNavArea *m_searchFromArea;
	float m_range;
	Vector m_hidingSpot;
	bool m_isLookingOutward;
	bool m_isAtSpot;
	float m_duration;
	CountdownTimer m_hideTimer;
	bool m_isHoldingPosition;
	// How long to hold after hearing a nearby enemy.
	float m_holdPositionTime;
	bool m_heardEnemy;
	float m_firstHeardEnemyTime;
	int m_retry;
	// The follow leader's position when hiding was decided.
	Vector m_leaderAnchorPos;
	// Paused for a moment while retreating.
	bool m_isPaused;
	CountdownTimer m_pauseTimer;
};

class EscapeFromBombState : public BotState
{
public:
	const char *GetName() const override { return "EscapeFromBomb"; }
};

class FollowState : public BotState
{
public:
	const char *GetName() const override { return "Follow"; }

	enum LeaderMotionStateType
	{
		INVALID,
		STOPPED,
		WALKING,
		RUNNING
	};

public:
	CHandle< CCSPlayerPawn > m_leader;
	// Where the leader was when the follow path was computed.
	Vector m_lastLeaderPos;
	bool m_isStopped;
	float m_stoppedTimestamp;
	LeaderMotionStateType m_leaderMotionState;
	IntervalTimer m_leaderMotionStateTime;
	bool m_isSneaking;
	float m_lastSawLeaderTime;
	CountdownTimer m_repathInterval;
	IntervalTimer m_walkTime;
	bool m_isAtWalkSpeed;
	float m_waitTime;
	CountdownTimer m_idleTimer;
};

// Faces an entity and presses use on it.
class UseEntityState : public BotState
{
public:
	const char *GetName() const override { return "UseEntity"; }

public:
	CHandle< CBaseEntity > m_entity;
};

class OpenDoorState : public BotState
{
public:
	const char *GetName() const override { return "OpenDoor"; }

public:
	CHandle< CBaseEntity > m_funcDoor;
	CHandle< CBaseEntity > m_propDoor;
	bool m_isDone;
	CountdownTimer m_timeout;
};

// Leaves a field of flames, for example from a molotov.
class EscapeFromFlamesState : public BotState
{
public:
	const char *GetName() const override { return "EscapeFromFlames"; }

public:
	CountdownTimer m_searchTimer;
	CNavArea *m_safeArea;
};

// Moves back into the playable area: on entry the bot stands up, runs, drops its path and
// clears the area; each update moves towards the area's centre while it has one.
class MoveToPlayAreaState : public BotState
{
public:
	const char *GetName() const override { return "MoveToPlayArea"; }

public:
	// Invalidated on entry.
	CountdownTimer m_Timer;
	CNavArea *m_pArea;
};

#endif // CS_BOT_STATE_H
