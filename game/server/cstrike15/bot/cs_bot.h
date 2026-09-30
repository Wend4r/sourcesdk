//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: The Counter-Strike bot, owned by its pawn.
//
//=============================================================================//

#ifndef CS_BOT_H
#define CS_BOT_H

#ifdef _WIN32
#pragma once
#endif

#include "bot.h"
#include "ehandle.h"
#include "entitytypes.h"
#include "cs_bot_timers.h"
#include "cs_bot_state.h"
#include "cs_bot_chatter.h"
#include "cs_gamestate.h"
#include "navlib/nav_path.h"

class BotChatterInterface;
class CBaseEntity;
class CBtTree;
class CCSPlayerPawn;
class CNavArea;
class CNavLadder;
class HidingSpot;
struct SpotEncounter;

enum PriorityType
{
	PRIORITY_LOW,
	PRIORITY_MEDIUM,
	PRIORITY_HIGH,
	PRIORITY_UNINTERRUPTABLE
};

enum NavRelativeDirType
{
	FORWARD = 0,
	BACKWARD,
	LEFT,
	RIGHT,
	UP,
	DOWN,

	NUM_RELATIVE_DIRECTIONS
};

//-----------------------------------------------------------------------------
// A path maneuver run on top of path following: it owns a small state machine and, while
// active, sets the bot's goal position itself. Stopping presses the opposite movement
// buttons of whatever was held, so the bot counter-strafes to a halt.
//-----------------------------------------------------------------------------
struct CCSBotPathManeuver
{
	CCSBot *m_pBot;
	int m_nState;
	// When m_nState was entered.
	GameTime_t m_stateTimestamp;
	// IN_BACK for IN_FORWARD, IN_FORWARD for IN_BACK, IN_MOVELEFT and IN_MOVERIGHT swapped.
	int m_nStopButtons;
	bool m_bActive;
};

// Mounts, climbs and dismounts the ladder of the current path segment.
struct CCSBotLadderManeuver : public CCSBotPathManeuver
{
	enum LadderStateType
	{
		APPROACH,
		STOP_MOVING,
		FACE_LADDER,
		TELEPORT_TO_MOUNT_POSITION,
		MOUNT_LADDER,
		FACE_TRAVERSAL_DIRECTION,
		TRAVERSE,
		FACE_EXIT,
		DISMOUNT
	};
};

// Jumps across a sharp corner of the path found within 120 units ahead.
struct CCSBotSharpTurnJump : public CCSBotPathManeuver
{
	enum JumpStateType
	{
		APPROACH,
		STOP_MOVING,
		FACE_JUMP,
		AIRBORNE
	};

	// Unit direction of the path leaving the corner.
	Vector m_vecExitDir;
	// Unit direction from the bot to the corner when the jump started.
	Vector m_vecApproachDir;
	// The corner: the goal position while approaching.
	Vector m_vecJumpPos;
	// The next path position, or 50 units past the corner: the goal position while airborne.
	Vector m_vecLandPos;
};

abstract_class CCSBot : public CBot
{
public:
	~CCSBot() override = 0;

	// While set, the path follower turns the eyes and the look-at logic leaves them alone.
	virtual bool IsEyeAnglesUnderPathFinderControl() const = 0;
	// When stuck or crouching against a breakable that is not a door, looks at it to shoot it.
	virtual void PushawayTouch( CBaseEntity *pOther ) = 0;

public:
	enum TaskType
	{
		SEEK_AND_DESTROY,
		PLANT_BOMB,
		FIND_TICKING_BOMB,
		DEFUSE_BOMB,
		GUARD_TICKING_BOMB,
		GUARD_BOMB_DEFUSER,
		GUARD_LOOSE_BOMB,
		GUARD_BOMB_ZONE,
		GUARD_INITIAL_ENCOUNTER,
		ESCAPE_FROM_BOMB,
		HOLD_POSITION,
		FOLLOW,
		VIP_ESCAPE,
		GUARD_VIP_ESCAPE_ZONE,
		COLLECT_HOSTAGES,
		RESCUE_HOSTAGES,
		GUARD_HOSTAGES,
		GUARD_HOSTAGE_RESCUE_ZONE,
		MOVE_TO_LAST_KNOWN_ENEMY_POSITION,
		MOVE_TO_SNIPER_SPOT,
		SNIPING,
		ESCAPE_FROM_FLAMES,
		ESCAPE_FROM_DANGER_ZONE,
		// Shares one name-table entry with the previous task: the two names run together.
		MOVE_TO_PLAY_AREA,

		NUM_TASKS
	};

	// How the bot reacts to enemies.
	enum DispositionType
	{
		// Engages on sight and investigates noises.
		ENGAGE_AND_INVESTIGATE,
		// Engages on sight, only looks towards noises.
		OPPORTUNITY_FIRE,
		// Engages only when fired on or very close.
		SELF_DEFENSE,
		IGNORE_ENEMIES,

		NUM_DISPOSITIONS
	};

	enum MoraleType
	{
		TERRIBLE = -3,
		BAD = -2,
		NEGATIVE = -1,
		NEUTRAL = 0,
		POSITIVE = 1,
		GOOD = 2,
		EXCELLENT = 3
	};

	// Bits of m_visibleEnemyParts. The sides are from the bot's point of view.
	enum VisiblePartType
	{
		NONE = 0x00,
		GUT = 0x01,
		HEAD = 0x02,
		LEFT_SIDE = 0x04,
		RIGHT_SIDE = 0x08,
		FEET = 0x10
	};

	enum LookAtSpotState
	{
		NOT_LOOKING_AT_SPOT,
		// Turning towards m_lookAtSpot.
		LOOK_TOWARDS_SPOT,
		LOOK_AT_SPOT,

		NUM_LOOK_AT_SPOT_STATES
	};

	enum GrenadeTossState
	{
		NOT_THROWING,
		START_THROW,
		THROW_LINED_UP,
		FINISH_THROW
	};

	enum
	{
		MAX_APPROACH_POINTS = 16,
		MAX_CHECKED_SPOTS = 64,
		MAX_TRACKED_PLAYERS = 64,
		MAX_ENEMY_QUEUE = 20,
		MAX_VEL_SAMPLES = 10
	};

	// Where the enemy can enter the region around the bot.
	struct ApproachPoint
	{
		Vector m_pos;
		CNavArea *m_area;
	};

	struct HidingSpotCheckInfo
	{
		HidingSpot *spot;
		float timestamp;
	};

	struct WatchInfo
	{
		// When the player was last seen, zero if never.
		float timestamp;
		bool isEnemy;
	};

	// One frame of the reaction time queue.
	struct ReactionState
	{
		CHandle< CCSPlayerPawn > player;
		bool isReloading;
		bool isProtectedByShield;
	};

public:
	// A copy of the profile the bot was made with; difficulty changes edit this one.
	BotProfile *m_pLocalProfile;
	Vector m_eyePosition;
	char m_name[ 64 ];

	// The distance kept from the enemy during gunplay.
	float m_combatRange;
	// Listens to no-one; re-rolled when m_rogueTimer elapses.
	bool m_isRogue;
	CountdownTimer m_rogueTimer;
	MoraleType m_morale;
	bool m_diedLastRound;
	// How long into the round the bot feels safe.
	float m_safeTime;
	bool m_wasSafe;
	// Which way to move while blinded.
	NavRelativeDirType m_blindMoveDir;
	// Keeps firing while blinded.
	bool m_blindFire;
	// While running the bot cannot attack.
	CountdownTimer m_surpriseTimer;
	bool m_bAllowActive;

	bool m_isFollowing;
	CHandle< CCSPlayerPawn > m_leader;
	float m_followTimestamp;
	float m_allowAutoFollowTime;

	CountdownTimer m_hurryTimer;
	CountdownTimer m_alertTimer;
	CountdownTimer m_sneakTimer;
	CountdownTimer m_panicTimer;

	IdleState m_idleState;
	HuntState m_huntState;
	AttackState m_attackState;
	InvestigateNoiseState m_investigateNoiseState;
	BuyState m_buyState;
	MoveToState m_moveToState;
	HumanPathFollowState m_humanPathFollowState;
	FetchBombState m_fetchBombState;
	PlantBombState m_plantBombState;
	DefuseBombState m_defuseBombState;
	PickupHostageState m_pickupHostageState;
	HideState m_hideState;
	EscapeFromBombState m_escapeFromBombState;
	FollowState m_followState;
	UseEntityState m_useEntityState;
	OpenDoorState m_openDoorState;
	EscapeFromFlamesState m_escapeFromFlamesState;
	MoveToPlayAreaState m_moveToPlayAreaState;

	// One of the states above, or null.
	BotState *m_state;
	float m_stateTimestamp;
	// The attack state is running on top of m_state.
	bool m_isAttacking;
	// The open door state is running on top of m_state.
	bool m_isOpeningDoor;

	TaskType m_task;
	CHandle< CBaseEntity > m_taskEntity;

	// A behavior tree loaded at spawn replaces the state machine and gets the noises; null
	// runs the regular AI.
	CBtTree *m_pBehaviorTree;
	// The skill level 0 to 7 the behavior tree is given; 8 derives it from the profile.
	int m_nSkillLevelOverride;

	Vector m_goalPosition;
	CHandle< CBaseEntity > m_goalEntity;
	// A higher priority player to make way for.
	CHandle< CBaseEntity > m_avoid;
	float m_avoidTimestamp;
	// Stopping because a 'stop' nav area was entered.
	bool m_isStopping;
	bool m_hasVisitedEnemySpawn;
	// How long the bot has not moved.
	IntervalTimer m_stillTimer;
	bool m_bEyeAnglesUnderPathFinderControl;

	// Storage of a CPathOptimizerNavmesh and a CNavPath, which are declared abstract and so
	// cannot be members by value; reach them through GetPathOptimizer() and GetPath(). The path
	// points at the optimizer.
	alignas( 8 ) uint8 m_pathOptimizer[ sizeof( CPathOptimizerNavmesh ) ];
	alignas( 8 ) uint8 m_path[ sizeof( CNavPath ) ];

	CPathOptimizerNavmesh *GetPathOptimizer() { return reinterpret_cast< CPathOptimizerNavmesh * >( m_pathOptimizer ); }
	CNavPath *GetPath() { return reinterpret_cast< CNavPath * >( m_path ); }
	const CNavPath *GetPath() const { return reinterpret_cast< const CNavPath * >( m_path ); }

	// The next segment of m_path.
	int m_pathIndex;
	GameTime_t m_areaEnteredTimestamp;
	// Must elapse before the bot may compute another path.
	CountdownTimer m_repathTimer;

	// Throttles the check for friends in the path.
	CountdownTimer m_avoidFriendTimer;
	bool m_isFriendInTheWay;
	// How long to wait for a friend to move.
	CountdownTimer m_politeTimer;
	bool m_isWaitingBehindFriend;

	CCSBotLadderManeuver m_ladderManeuver;
	// The ladder of the current segment.
	const CNavLadder *m_pathLadder;
	// Never addressed and not in the datamap; a real member, since m_pathLadderEnd would
	// otherwise sit right after the pointer.
	uint8 m_Unk4F80[ 4 ];
	// The z of the top when ascending, of the bottom when descending.
	float m_pathLadderEnd;
	CCSBotSharpTurnJump m_sharpTurnJump;

	// While running the bot cannot walk.
	CountdownTimer m_mustRunTimer;
	// While running the bot waits where it is.
	CountdownTimer m_waitTimer;

	CountdownTimer m_updateTravelDistanceTimer;
	// Shortest path distance to each player, -1 when unknown.
	float m_playerTravelDistance[ MAX_TRACKED_PLAYERS ];
	uint8 m_travelDistancePhase;

	CSGameState m_gameState;

	uint8 m_hostageEscortCount;
	float m_hostageEscortCountTimestamp;
	int m_desiredTeam;
	bool m_hasJoined;
	bool m_isWaitingForHostage;
	CountdownTimer m_inhibitWaitingForHostageTimer;
	CountdownTimer m_waitForHostageTimer;

	// The last non-friendly noise heard.
	Vector m_noisePosition;
	float m_noiseTravelDistance;
	// When it was heard, zero when forgotten.
	float m_noiseTimestamp;
	// The entity that made it.
	CBaseEntity *m_noiseSource;
	CNavArea *m_noiseArea;
	PriorityType m_noisePriority;
	// Throttles bending the line of sight to the noise.
	CountdownTimer m_noiseBendTimer;
	Vector m_bentNoisePosition;
	bool m_bendNoisePositionValid;

	// When the looking around changes next.
	float m_lookAroundStateTimestamp;
	// The yaw to look ahead at.
	float m_lookAheadAngle;
	float m_lookUpAngle;
	// The yaw the bot faces.
	float m_forwardAngle;
	// Looking around and look-at spots resume at this time.
	float m_inhibitLookAroundTimestamp;

	LookAtSpotState m_lookAtSpotState;
	Vector m_lookAtSpot;
	PriorityType m_lookAtSpotPriority;
	// -1 for forever.
	float m_lookAtSpotDuration;
	// When the bot began looking at the spot.
	float m_lookAtSpotTimestamp;
	// How exactly the spot must be looked at, in degrees.
	float m_lookAtSpotAngleTolerance;
	// Drop the spot when it gets close.
	bool m_lookAtSpotClearIfClose;
	// Shoot the spot once aimed at it.
	bool m_lookAtSpotAttack;
	const char *m_lookAtDesc;
	float m_peripheralTimestamp;

	ApproachPoint m_approachPoint[ MAX_APPROACH_POINTS ];
	uint8 m_approachPointCount;
	// Where the approach points were computed from.
	Vector m_approachPointViewPosition;

	// How long the view has not moved.
	IntervalTimer m_viewSteadyTimer;

	GrenadeTossState m_grenadeTossState;
	// Throwing a grenade times out with it.
	CountdownTimer m_tossGrenadeTimer;
	// Where the enemy is expected to be met first.
	const CNavArea *m_initialEncounterArea;
	// While running the bot is avoiding a grenade.
	CountdownTimer m_isAvoidingGrenade;

	// The spots met while moving through the current area of the path.
	SpotEncounter *m_spotEncounter;
	float m_spotCheckTimestamp;
	HidingSpotCheckInfo m_checkedHidingSpot[ MAX_CHECKED_SPOTS ];
	int m_checkedHidingSpotCount;

	// The desired view angles and how fast the eyes turn towards them.
	float m_lookPitch;
	float m_lookPitchVel;
	float m_lookYaw;
	float m_lookYawVel;

	// The spot to fire at, its velocity and where it is predicted to be.
	Vector m_targetSpot;
	Vector m_targetSpotVelocity;
	Vector m_targetSpotPredicted;
	QAngle m_aimError;
	QAngle m_aimGoal;
	GameTime_t m_targetSpotTime;
	// Radius of the aim focus.
	float m_aimFocus;
	// How long the current aim offset lasts, can be random.
	float m_aimFocusInterval;
	GameTime_t m_aimFocusNextUpdate;

	DispositionType m_disposition;
	CountdownTimer m_ignoreEnemiesTimer;
	CHandle< CCSPlayerPawn > m_enemy;
	// The result of the last visibility test on m_enemy.
	bool m_isEnemyVisible;
	// VisiblePartType bits.
	uint8 m_visibleEnemyParts;
	Vector m_lastEnemyPosition;
	float m_lastSawEnemyTimestamp;
	float m_firstSawEnemyTimestamp;
	float m_currentEnemyAcquireTimestamp;
	// When m_enemy died, if it has.
	float m_enemyDeathTimestamp;
	float m_friendDeathTimestamp;
	// The last enemy was killed or seen dying.
	bool m_isLastEnemyDead;
	// The most enemies seen recently.
	int m_nearbyEnemyCount;
	// Where most enemies were seen.
	BotPlace_t m_enemyPlace;
	WatchInfo m_watchInfo[ MAX_TRACKED_PLAYERS ];
	// The bomb carrier while visible.
	CHandle< CCSPlayerPawn > m_bomber;

	int m_nearbyFriendCount;
	CHandle< CCSPlayerPawn > m_closestVisibleFriend;
	CHandle< CCSPlayerPawn > m_closestVisibleHumanFriend;

	IntervalTimer m_attentionInterval;

	// The last enemy that hurt the bot, not necessarily m_enemy.
	CHandle< CCSPlayerPawn > m_attacker;
	float m_attackedTimestamp;
	IntervalTimer m_burnedByFlamesTimer;

	// Entity index of the last kill, zero if none.
	int m_lastVictimID;
	bool m_isAimingAtEnemy;
	// Upkeep toggles the primary attack as fast as it can.
	bool m_isRapidFiring;
	// How long the current weapon has been equipped.
	IntervalTimer m_equipTimer;
	// Delays firing right after zooming.
	CountdownTimer m_zoomTimer;
	GameTime_t m_fireWeaponTimestamp;
	CountdownTimer m_lookForWeaponsOnGroundTimer;

	bool m_bIsSleeping;
	bool m_isEnemySniperVisible;
	CountdownTimer m_sawEnemySniperTimer;

	// A round-robin queue of the most dangerous threat per frame, read back a reaction time
	// later at m_enemyQueueAttendIndex.
	ReactionState m_enemyQueue[ MAX_ENEMY_QUEUE ];
	uint8 m_enemyQueueIndex;
	uint8 m_enemyQueueCount;
	uint8 m_enemyQueueAttendIndex;

	bool m_isStuck;
	GameTime_t m_stuckTimestamp;
	// Where the bot became stuck.
	Vector m_stuckSpot;
	NavRelativeDirType m_wiggleDirection;
	CountdownTimer m_wiggleTimer;
	// When to jump next while stuck.
	CountdownTimer m_stuckJumpTimer;
	GameTime_t m_nextCleanupCheckTimestamp;

	float m_avgVel[ MAX_VEL_SAMPLES ];
	int m_avgVelIndex;
	int m_avgVelCount;
	Vector m_lastOrigin;

	// The last radio command received, 0 for none.
	int m_lastRadioCommand;
	float m_lastRadioRecievedTimestamp;
	float m_lastRadioSentTimestamp;
	// Who sent it.
	CHandle< CCSPlayerPawn > m_radioSubject;
	// The position it referred to.
	Vector m_radioPosition;
	float m_voiceEndTimestamp;

	BotChatterInterface *m_pChatter;
	int m_lastValidReactionQueueFrame;
};

#endif // CS_BOT_H
