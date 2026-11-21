//======= Copyright (c) 1996-2009, Valve Corporation, All rights reserved. ======
//
// Purpose: Definitions that are shared by the game DLL and the client DLL.
//
//===============================================================================


#ifndef SHAREDDEFS_H
#define SHAREDDEFS_H
#ifdef _WIN32
#pragma once
#endif

#include "basetypes.h"
#include "bittools.h"
#include "const.h"
#include "soundsystem/soundflags.h"
#include "entity2/entityindex.h"
#include "entity2/entityclass.h"

#include "schemacompiler/schemaon.h"

// CS2 runs at a fixed 64 tick rate; CGlobalVarsBase no longer stores the tick interval
#define TICK_INTERVAL			( 1.0f / 64.0f )


#define TIME_TO_TICKS( dt )		( (int)( 0.5f + (float)(dt) / TICK_INTERVAL ) )
#define TICKS_TO_TIME( t )		( TICK_INTERVAL *( t ) )
#define ROUND_TO_TICKS( t )		( TICK_INTERVAL * TIME_TO_TICKS( t ) )
#define TICK_NEVER_THINK		(-1)

#define ANIMATION_CYCLE_BITS		15
#define ANIMATION_CYCLE_MINFRAC		(1.0f / (1<<ANIMATION_CYCLE_BITS))

// Each mod defines these for itself.
class CViewVectors
{
public:
	CViewVectors() {}

	CViewVectors( 
		Vector vView,
		Vector vHullMin,
		Vector vHullMax,
		Vector vDuckHullMin,
		Vector vDuckHullMax,
		Vector vDuckView,
		Vector vObsHullMin,
		Vector vObsHullMax,
		Vector vDeadViewHeight )
	{
		m_vView = vView;
		m_vHullMin = vHullMin;
		m_vHullMax = vHullMax;
		m_vDuckHullMin = vDuckHullMin;
		m_vDuckHullMax = vDuckHullMax;
		m_vDuckView = vDuckView;
		m_vObsHullMin = vObsHullMin;
		m_vObsHullMax = vObsHullMax;
		m_vDeadViewHeight = vDeadViewHeight;
	}

	// Height above entity position where the viewer's eye is.
	Vector m_vView;
	
	Vector m_vHullMin;
	Vector m_vHullMax;
	
	Vector m_vDuckHullMin;
	Vector m_vDuckHullMax;
	Vector m_vDuckView;
	
	Vector m_vObsHullMin;
	Vector m_vObsHullMax;
	
	Vector m_vDeadViewHeight;
};

// Height above entity position where the viewer's eye is.
#define VEC_VIEW			g_pGameRules->GetViewVectors()->m_vView
#define VEC_HULL_MIN		g_pGameRules->GetViewVectors()->m_vHullMin
#define VEC_HULL_MAX		g_pGameRules->GetViewVectors()->m_vHullMax

#define VEC_DUCK_HULL_MIN	g_pGameRules->GetViewVectors()->m_vDuckHullMin
#define VEC_DUCK_HULL_MAX	g_pGameRules->GetViewVectors()->m_vDuckHullMax
#define VEC_DUCK_VIEW		g_pGameRules->GetViewVectors()->m_vDuckView

#define VEC_OBS_HULL_MIN	g_pGameRules->GetViewVectors()->m_vObsHullMin
#define VEC_OBS_HULL_MAX	g_pGameRules->GetViewVectors()->m_vObsHullMax

#define VEC_DEAD_VIEWHEIGHT	g_pGameRules->GetViewVectors()->m_vDeadViewHeight


#define WATERJUMP_HEIGHT			8

#define MAX_CLIMB_SPEED		200

#define TIME_TO_DUCK_MSECS			200
#define TIME_TO_UNDUCK_MSECS		200

inline float FractionDucked( int msecs )
{
	return clamp( (float)msecs / (float)TIME_TO_DUCK_MSECS, 0.0f, 1.0f );
}

inline float FractionUnDucked( int msecs )
{
	return clamp( (float)msecs / (float)TIME_TO_UNDUCK_MSECS, 0.0f, 1.0f );
}

#define MAX_WEAPON_SLOTS		6	// hud item selection slots
#define MAX_WEAPON_POSITIONS	6	// max number of items within a slot
#define MAX_ITEM_TYPES			6	// hud item selection slots
#define MAX_WEAPONS				64	// Max number of weapons available

#define MAX_ITEMS				5	// hard coded item types

#define WEAPON_NOCLIP			-1	// clip sizes set to this tell the weapon it doesn't use a clip

#define	MAX_AMMO_TYPES	32		// ???
#define MAX_AMMO_SLOTS  32		// not really slots

enum TextMsgDestinations
{
	HUD_PRINTNOTIFY		= 1,
	HUD_PRINTCONSOLE	= 2,
	HUD_PRINTTALK		= 3,
	HUD_PRINTCENTER		= 4,
	HUD_PRINTTALK2		= 5,	// adds 4(EOT) to beginning of msg
	HUD_PRINTALERT		= 6,
};


// Vote creation or processing failure codes
schema enum vote_create_failed_t
{
	VOTE_FAILED_GENERIC = 0,
	VOTE_FAILED_TRANSITIONING_PLAYERS,
	VOTE_FAILED_RATE_EXCEEDED,
	VOTE_FAILED_YES_MUST_EXCEED_NO,
	VOTE_FAILED_QUORUM_FAILURE,
	VOTE_FAILED_ISSUE_DISABLED,
	VOTE_FAILED_MAP_NOT_FOUND,
	VOTE_FAILED_MAP_NAME_REQUIRED,
	VOTE_FAILED_FAILED_RECENTLY,
	VOTE_FAILED_TEAM_CANT_CALL,
	VOTE_FAILED_WAITINGFORPLAYERS,
	VOTE_FAILED_PLAYERNOTFOUND,
	VOTE_FAILED_CANNOT_KICK_ADMIN,
	VOTE_FAILED_SCRAMBLE_IN_PROGRESS,
	VOTE_FAILED_SPECTATOR,
	VOTE_FAILED_FAILED_RECENT_KICK,
	VOTE_FAILED_FAILED_RECENT_CHANGEMAP,
	VOTE_FAILED_FAILED_RECENT_SWAPTEAMS,
	VOTE_FAILED_FAILED_RECENT_SCRAMBLETEAMS,
	VOTE_FAILED_FAILED_RECENT_RESTART,
	VOTE_FAILED_SWAP_IN_PROGRESS,
	VOTE_FAILED_DISABLED,
	VOTE_FAILED_NEXTLEVEL_SET,
	VOTE_FAILED_TOO_EARLY_SURRENDER,
	VOTE_FAILED_MATCH_PAUSED,
	VOTE_FAILED_MATCH_NOT_PAUSED,
	VOTE_FAILED_NOT_IN_WARMUP,
	VOTE_FAILED_NOT_10_PLAYERS,
	VOTE_FAILED_TIMEOUT_ACTIVE,
	VOTE_FAILED_TIMEOUT_INACTIVE,
	VOTE_FAILED_TIMEOUT_EXHAUSTED,
	VOTE_FAILED_CANT_ROUND_END,
	VOTE_FAILED_REMATCH,
	VOTE_FAILED_CONTINUE,
	VOTE_FAILED_MAX,
};

#define MAX_VOTE_DETAILS_LENGTH 64
#define INVALID_ISSUE -1
#define MAX_VOTE_OPTIONS 5

enum
{
	VOTEISSUE_UNDEFINED = -1,
	VOTEISSUE_KICK,
	VOTEISSUE_CHANGELEVEL,
	VOTEISSUE_NEXTLEVEL,
	VOTEISSUE_SWAPTEAMS,
	VOTEISSUE_SCRAMBLE,
	VOTEISSUE_RESTARTGAME,
	VOTEISSUE_SURRENDER,
	VOTEISSUE_REMATCH,
	VOTEISSUE_CONTINUE,
	VOTEISSUE_PAUSEMATCH,
	VOTEISSUE_UNPAUSEMATCH,
	VOTEISSUE_LOADBACKUP,
	VOTEISSUE_READYFORMATCH,
	VOTEISSUE_NOTREADYFORMATCH,
	VOTEISSUE_STARTTIMEOUT,
	VOTEISSUE_LAST
};

enum CastVote
{
	VOTE_OPTION1,  // Use this for Yes
	VOTE_OPTION2,  // Use this for No
	VOTE_OPTION3,
	VOTE_OPTION4,
	VOTE_OPTION5,
	VOTE_UNCAST
};


schema enum AmmoPosition_t
{
	AMMO_POSITION_INVALID = -1,
	AMMO_POSITION_PRIMARY = 0,
	AMMO_POSITION_SECONDARY,

	AMMO_POSITION_COUNT
};

//===================================================================================================================
// Close caption flags
#define CLOSE_CAPTION_WARNIFMISSING	( 1<<0 )
#define CLOSE_CAPTION_FROMPLAYER	( 1<<1 )
#define CLOSE_CAPTION_GENDER_MALE	( 1<<2 )
#define CLOSE_CAPTION_GENDER_FEMALE	( 1<<3 )

//===================================================================================================================
// Hud Element hiding flags
#define	HIDEHUD_WEAPONSELECTION		( 1<<0 )	// Hide ammo count & weapon selection
#define	HIDEHUD_FLASHLIGHT			( 1<<1 )
#define	HIDEHUD_ALL					( 1<<2 )
#define HIDEHUD_HEALTH				( 1<<3 )	// Hide health & armor / suit battery
#define HIDEHUD_PLAYERDEAD			( 1<<4 )	// Hide when local player's dead
#define HIDEHUD_NEEDSUIT			( 1<<5 )	// Hide when the local player doesn't have the HEV suit
#define HIDEHUD_MISCSTATUS			( 1<<6 )	// Hide miscellaneous status elements (trains, pickup history, death notices, etc)
#define HIDEHUD_CHAT				( 1<<7 )	// Hide all communication elements (saytext, voice icon, etc)
#define	HIDEHUD_CROSSHAIR			( 1<<8 )	// Hide crosshairs
#define	HIDEHUD_VEHICLE_CROSSHAIR	( 1<<9 )	// Hide vehicle crosshair
#define HIDEHUD_INVEHICLE			( 1<<10 )
#define HIDEHUD_BONUS_PROGRESS		( 1<<11 )	// Hide bonus progress display (for bonus map challenges)
#define HIDEHUD_RADAR				( 1<<12 )   // Hides the radar in CS1.5
#define HIDEHUD_MINISCOREBOARD      ( 1<<13 )   // Hides the miniscoreboard in CS1.5

#define HIDEHUD_BITCOUNT			14

//===================================================================================================================
// Player Defines

// Max number of players in a game ( see const.h for ABSOLUTE_PLAYER_LIMIT (256 ) )
// The Source engine is really designed for 32 or less players.  If you raise this number above 32, you better know what you are doing
//  and have a good answer for a bunch of perf question related to player simulation, thinking logic, tracelines, networking overhead, etc.
// But if you are brave or are doing something interesting, go for it...   ywb 9/22/03

#define MAX_PLAYERS				ABSOLUTE_PLAYER_LIMIT

#define MAX_PLACE_NAME_LENGTH		18

//===================================================================================================================
// Team Defines
enum SharedTeams_t
{
	TEAM_ANY			= -1,	// for some team query methods
	TEAM_INVALID		= -1,
	TEAM_UNASSIGNED		= 0,	// not assigned to a team
	TEAM_SPECTATOR		= 1,	// spectator team

	// Start your team numbers after this
	LAST_SHARED_TEAM	= TEAM_SPECTATOR,

	// CS Specific teams
	CS_TEAM_NONE		= TEAM_UNASSIGNED,
	CS_TEAM_SPECTATOR	= TEAM_SPECTATOR,
	CS_TEAM_T			= 2,
	CS_TEAM_CT			= 3,

	// The first team that's game specific (i.e. not unassigned / spectator)
	FIRST_GAME_TEAM		= LAST_SHARED_TEAM + 1,
};

#define MAX_TEAMS				32	// Max number of teams in a game
#define MAX_TEAM_NAME_LENGTH	32	// Max length of a team's name

#define MAX_TEAM_FLAG_ICON_LENGTH	8	// Max length of a team's flag icon (just the singlular country code)
#define MAX_TEAM_LOGO_ICON_LENGTH	8	// Max length of a team's flag icon (just the singlular country code)

// Weapon m_iState
#define WEAPON_NOT_CARRIED				0	// Weapon is on the ground
#define WEAPON_IS_CARRIED_BY_PLAYER		1	// This client is carrying this weapon.
#define WEAPON_IS_ACTIVE				2	// This client is carrying this weapon and it's the currently held weapon


// Weapon flags
// -----------------------------------------
//	Flags - NOTE: KEEP g_ItemFlags IN WEAPON_PARSE.CPP UPDATED WITH THESE
// -----------------------------------------
schema enum ItemFlagTypes_t : uint8
{
	ITEM_FLAG_NONE						= 0,
	ITEM_FLAG_CAN_SELECT_WITHOUT_AMMO	= 1 << 0,
	ITEM_FLAG_NOAUTORELOAD				= 1 << 1,
	ITEM_FLAG_NOAUTOSWITCHEMPTY			= 1 << 2,
	ITEM_FLAG_LIMITINWORLD				= 1 << 3,
	ITEM_FLAG_EXHAUSTIBLE				= 1 << 4,	// A player can totally exhaust their ammo supply and lose this weapon
	ITEM_FLAG_DOHITLOCATIONDMG			= 1 << 5,	// This weapon take hit location into account when applying damage
	ITEM_FLAG_NOAMMOPICKUPS				= 1 << 6,	// Don't draw ammo pickup sprites/sounds when ammo is received
	ITEM_FLAG_NOITEMPICKUP				= 1 << 7,	// Don't draw weapon pickup when this weapon is picked up by the player
};
// NOTE: KEEP g_ItemFlags IN WEAPON_PARSE.CPP UPDATED WITH THESE


// Humans only have left and right hands, though we might have aliens with more
//  than two, sigh
#define MAX_VIEWMODELS			2

#define MAX_BEAM_ENTS			10

#define TRACER_TYPE_DEFAULT		0x00000001

// Tracer Flags
#define TRACER_FLAG_WHIZ			0x0001
#define TRACER_FLAG_USEATTACHMENT	0x0002

#define TRACER_DONT_USE_ATTACHMENT	-1

// Entity Dissolve types
schema enum EntityDissolveType_t
{
	ENTITY_DISSOLVE_INVALID = -1,
	ENTITY_DISSOLVE_NORMAL = 0,
	ENTITY_DISSOLVE_ELECTRICAL,
	ENTITY_DISSOLVE_ELECTRICAL_LIGHT,
	ENTITY_DISSOLVE_CORE,
};

#define PLAYER_FATAL_FALL_SPEED		1024 // approx 60 feet
#define PLAYER_MAX_SAFE_FALL_SPEED	580 // approx 20 feet
#define PLAYER_LAND_ON_FLOATING_OBJECT	200 // Can go another 200 units without getting hurt
#define PLAYER_MIN_BOUNCE_SPEED		200
#define PLAYER_FALL_PUNCH_THRESHOLD (float)350 // won't punch player's screen/make scrape noise unless player falling at least this fast.
#define DAMAGE_FOR_FALL_SPEED		100.0f / ( PLAYER_FATAL_FALL_SPEED - PLAYER_MAX_SAFE_FALL_SPEED ) // damage per unit per second.


// instant damage

// For a means of resolving these consts into debug string text, see function
// CTakeDamageInfo::DebugGetDamageTypeString(unsigned int DamageType, char *outbuf, unsigned int outbuflength )
schema enum DamageTypes_t
{
	DMG_GENERIC			= 0,		// generic damage was done
	DMG_CRUSH			= 1 << 0,	// crushed by falling or moving object.
									// NOTE: It's assumed crush damage is occurring as a result of physics collision, so no extra physics force is generated by crush damage.
									// DON'T use DMG_CRUSH when damaging entities unless it's the result of a physics collision. You probably want DMG_CLUB instead.
	DMG_BULLET			= 1 << 1,	// shot
	DMG_SLASH			= 1 << 2,	// cut, clawed, stabbed
	DMG_BURN			= 1 << 3,	// heat burned
	DMG_VEHICLE			= 1 << 4,	// hit by a vehicle
	DMG_FALL			= 1 << 5,	// fell too far
	DMG_BLAST			= 1 << 6,	// explosive blast damage
	DMG_CLUB			= 1 << 7,	// crowbar, punch, headbutt
	DMG_SHOCK			= 1 << 8,	// electric shock
	DMG_SONIC			= 1 << 9,	// sound pulse shockwave
	DMG_ENERGYBEAM		= 1 << 10,	// laser or other high energy beam
	DMG_BUCKSHOT		= 1 << 11,	// not quite a bullet. Little, rounder, different.
	DMG_BLAST_SURFACE	= 1 << 12,	// A blast on the surface of water that cannot harm things underwater
	DMG_DISSOLVE		= 1 << 13,	// Dissolving!
	DMG_DROWN			= 1 << 14,	// Drowning
	DMG_POISON			= 1 << 15,	// blood poisoning - heals over time like drowning damage
	DMG_RADIATION		= 1 << 16,	// radiation exposure
	DMG_DROWNRECOVER	= 1 << 17,	// drowning recovery
	DMG_ACID			= 1 << 18,	// toxic chemicals or acid burns

	// TODO: keep this up to date so all the mod-specific flags don't overlap anything.
	DMG_LASTGENERICFLAG	= DMG_ACID, META( MPropertySuppressEnumerator; MEnumeratorIsNotAFlag )

	DMG_HEADSHOT		= 1 << 19,
};

schema enum TakeDamageFlags_t : uint64
{
	DFLAG_NONE								= 0,
	DFLAG_SUPPRESS_HEALTH_CHANGES			= 1ull << 0,
	DFLAG_SUPPRESS_PHYSICS_FORCE			= 1ull << 1,
	DFLAG_SUPPRESS_EFFECTS					= 1ull << 2,
	DFLAG_PREVENT_DEATH						= 1ull << 3,
	DFLAG_FORCE_DEATH						= 1ull << 4,
	DFLAG_ALWAYS_GIB						= 1ull << 5,
	DFLAG_NEVER_GIB							= 1ull << 6,
	DFLAG_REMOVE_NO_RAGDOLL					= 1ull << 7,
	DFLAG_SUPPRESS_DAMAGE_MODIFICATION		= 1ull << 8,
	DFLAG_ALWAYS_FIRE_DAMAGE_EVENTS			= 1ull << 9,
	DFLAG_RADIUS_DMG						= 1ull << 10,
	DFLAG_FORCEREDUCEARMOR_DMG				= 1ull << 11,
	DFLAG_SUPPRESS_INTERRUPT_FLINCH			= 1ull << 12,
	DFLAG_IGNORE_DESTRUCTIBLE_PARTS			= 1ull << 13,
	DFLAG_SUPPRESS_BREAKABLES				= 1ull << 14,
	DFLAG_FORCE_PHYSICS_FORCE				= 1ull << 15,
	DFLAG_SUPPRESS_SCREENSPACE_DAMAGE_FX	= 1ull << 16,
	DFLAG_ALLOW_NON_AUTHORITATIVE			= 1ull << 17,

	DMG_LASTDFLAG							= DFLAG_ALLOW_NON_AUTHORITATIVE, META( MPropertySuppressEnumerator; MEnumeratorIsNotAFlag )

#if defined( CS2 )
	DFLAG_IGNORE_ARMOR						= 1ull << 18,
	DFLAG_SUPPRESS_UTILREMOVE				= 1ull << 19,
#endif // CS2
};


// settings for m_takedamage
#define	DAMAGE_NO				0
#define DAMAGE_EVENTS_ONLY		1		// Call damage functions, but don't modify health
#define	DAMAGE_YES				2
#define	DAMAGE_AIM				3


// Spectator Movement modes
schema enum ObserverMode_t : uint32
{
	OBS_MODE_NONE = 0,		// not in spectator mode
	OBS_MODE_FIXED,			// view from a fixed camera position
	OBS_MODE_IN_EYE,		// follow a player in first person view
	OBS_MODE_CHASE,			// follow a player in third person view
	OBS_MODE_ROAMING,		// free roaming

	NUM_OBSERVER_MODES,
};

#define LAST_PLAYER_OBSERVERMODE	OBS_MODE_ROAMING

// Force Camera Restrictions with mp_forcecamera
enum {
	OBS_ALLOW_ALL = 0,	// allow all modes, all targets
	OBS_ALLOW_TEAM,		// allow only own team & first person, no PIP
	OBS_ALLOW_NONE,		// don't allow any spectating after death (fixed & fade to black)

	OBS_ALLOW_NUM_MODES,
};

enum
{
	TYPE_TEXT = 0,	// just display this plain text
	TYPE_INDEX,		// lookup text & title in stringtable
	TYPE_URL,		// show this URL
	TYPE_FILE,		// show this local file
} ;

// basic team colors
#define COLOR_RED		Color(255, 64, 64, 255)
#define COLOR_BLUE		Color(153, 204, 255, 255)
#define COLOR_YELLOW	Color(255, 178, 0, 255)
#define COLOR_GREEN		Color(153, 255, 153, 255)
#define COLOR_GREY		Color(204, 204, 204, 255)
#define COLOR_WHITE		Color(255, 255, 255, 255)
#define COLOR_BLACK		Color(0, 0, 0, 255)

// Shared think context stuff
#define	MAX_CONTEXT_LENGTH		32
#define NO_THINK_CONTEXT	-1

// entity flags, CBaseEntity::m_iEFlags
enum
{
	EFL_KILLME	=				(1<<0),	// This entity is marked for death -- This allows the game to actually delete ents at a safe time
	EFL_DORMANT	=				(1<<1),	// Entity is dormant, no updates to client
	EFL_NOCLIP_ACTIVE =			(1<<2),	// Lets us know when the noclip command is active.
	EFL_SETTING_UP_BONES =		(1<<3),	// Set while a model is setting up its bones.
	EFL_KEEP_ON_RECREATE_ENTITIES = (1<<4), // This is a special entity that should not be deleted when we restart entities only

	EFL_DIRTY_SHADOWUPDATE =	(1<<5),	// Client only- need shadow manager to update the shadow...
	EFL_NOTIFY =				(1<<6),	// Another entity is watching events on this entity (used by teleport)

	// The default behavior in ShouldTransmit is to not send an entity if it doesn't
	// have a model. Certain entities want to be sent anyway because all the drawing logic
	// is in the client DLL. They can set this flag and the engine will transmit them even
	// if they don't have a model.
	EFL_FORCE_CHECK_TRANSMIT =	(1<<7),

	EFL_BOT_FROZEN =			(1<<8),	// This is set on bots that are frozen.
	EFL_SERVER_ONLY =			(1<<9),	// Non-networked entity.
	EFL_NO_AUTO_EDICT_ATTACH =	(1<<10), // Don't attach the edict; we're doing it explicitly
	
	// Some dirty bits with respect to abs computations
	EFL_DIRTY_ABSTRANSFORM =	(1<<11),
	EFL_DIRTY_ABSVELOCITY =		(1<<12),
	EFL_DIRTY_ABSANGVELOCITY =	(1<<13),
	EFL_DIRTY_SURROUNDING_COLLISION_BOUNDS	= (1<<14),
	EFL_DIRTY_SPATIAL_PARTITION = (1<<15),
	EFL_HAS_PLAYER_CHILD=		(1<<16),	// One of the child entities is a player.

	EFL_IN_SKYBOX =				(1<<17),	// This is set if the entity detects that it's in the skybox.
											// This forces it to pass the "in PVS" for transmission.
	EFL_USE_PARTITION_WHEN_NOT_SOLID = (1<<18),	// Entities with this flag set show up in the partition even when not solid
	EFL_TOUCHING_FLUID =		(1<<19),	// Used to determine if an entity is floating

	// FIXME: Not really sure where I should add this...
	EFL_IS_BEING_LIFTED_BY_BARNACLE = (1<<20),
	EFL_NO_ROTORWASH_PUSH =		(1<<21),		// I shouldn't be pushed by the rotorwash
	EFL_NO_THINK_FUNCTION =		(1<<22),
	EFL_NO_GAME_PHYSICS_SIMULATION = (1<<23),

	EFL_CHECK_UNTOUCH =			(1<<24),
	EFL_DONTBLOCKLOS =			(1<<25),		// I shouldn't block NPC line-of-sight
	EFL_DONTWALKON =			(1<<26),		// NPC;s should not walk on this entity
	EFL_NO_DISSOLVE =			(1<<27),		// These guys shouldn't dissolve
	EFL_NO_MEGAPHYSCANNON_RAGDOLL = (1<<28),	// Mega physcannon can't ragdoll these guys.
	EFL_NO_WATER_VELOCITY_CHANGE  =	(1<<29),	// Don't adjust this entity's velocity when transitioning into water
	EFL_NO_PHYSCANNON_INTERACTION =	(1<<30),	// Physcannon can't pick these up or punt them
	EFL_NO_DAMAGE_FORCES =		(1<<31),	// Doesn't accept forces from physics damage
};

//-----------------------------------------------------------------------------
// Purpose: Data for making the MOVETYPE_STEP entities appear to simulate every frame
//  We precompute the simulation and then meter it out each tick during networking of the 
//  entities origin and orientation.  Uses a bit more bandwidth, but it solves the NPCs interacting
//  with elevators/lifts bugs.
//-----------------------------------------------------------------------------
struct StepSimulationStep
{
	int			nTickCount;
	Vector		vecOrigin;
	Quaternion	qRotation;
};

struct StepSimulationData
{
	// Are we using the Step Simulation Data
	bool		m_bOriginActive;
	bool		m_bAnglesActive;

	// This is the pre-pre-Think position, orientation (Quaternion) and tick count
	StepSimulationStep	m_Previous2;

	// This is the pre-Think position, orientation (Quaternion) and tick count
	StepSimulationStep	m_Previous;

	// This is a potential mid-think position, orientation (Quaternion) and tick count
	// Used to mark motion discontinuities that happen between thinks
	StepSimulationStep	m_Discontinuity;

	// This is the goal or post-Think position and orientation (and Quaternion for blending) and next think time tick
	StepSimulationStep	m_Next;
	QAngle		m_angNextRotation;

	// This variable is used so that we only compute networked origin/angles once per tick
	int			m_nLastProcessTickCount;
	// The computed/interpolated network origin/angles to use
	Vector		m_vecNetworkOrigin;
	int			m_networkCell[3];
	QAngle		m_angNetworkAngles;
};

struct CSoundParameters;

// Bits of EmitSound_t::m_nFlags
enum EmitSoundFlags_t : uint8
{
	EMITSOUND_FLAG_NONE = 0,
	EMITSOUND_FLAG_EMIT_CLOSE_CAPTION = ( 1 << 0 ),
	EMITSOUND_FLAG_WARN_ON_MISSING_CLOSE_CAPTION = ( 1 << 1 ),
	EMITSOUND_FLAG_WARN_ON_DIRECT_WAVE_REFERENCE = ( 1 << 2 ), // Set by the entity's own EmitSound helpers
	EMITSOUND_FLAG_FIND_SOURCE_SOUNDSCAPE = ( 1 << 3 ),
	EMITSOUND_FLAG_USE_ORIGIN = ( 1 << 4 ), // Emit at m_veSoundOrigin instead of the entity's origin

	EMITSOUND_FLAGS_DEFAULT = EMITSOUND_FLAG_EMIT_CLOSE_CAPTION,
};
DEFINE_ENUM_BITWISE_OPERATORS( EmitSoundFlags_t );

//-----------------------------------------------------------------------------
// Purpose: Aggregates and sets default parameters for EmitSound function calls
//-----------------------------------------------------------------------------
struct EmitSound_t
{
	EmitSound_t() :
		m_pSoundName( 0 ),
		m_veSoundOrigin( 0.0f, 0.0f, 0.0f ),
		m_flVolume( VOL_NORM ),
		m_flSoundTime( 0.0f ),
		m_nSpeakerEntity( INVALID_ENTITY_INDEX ),
		m_nForceGuid( 0 ),
		m_nSourceSoundscape( 0 ),
		m_nPitch( PITCH_NORM ),
		m_nFlags( EMITSOUND_FLAGS_DEFAULT )
	{
	}

	EmitSound_t( const char *pSoundName, const Vector &vecSoundOrigin = Vector( 0.0f, 0.0f, 0.0f ), float flVolume = VOL_NORM, float flSoundTime = 0.0f, int16 nPitch = PITCH_NORM, EmitSoundFlags_t nFlags = EMITSOUND_FLAGS_DEFAULT ) :
		m_pSoundName( pSoundName ),
		m_veSoundOrigin( vecSoundOrigin ),
		m_flVolume( flVolume ),
		m_flSoundTime( flSoundTime ),
		m_nSpeakerEntity( INVALID_ENTITY_INDEX ),
		m_nForceGuid( 0 ),
		m_nSourceSoundscape( 0 ),
		m_nPitch( nPitch ),
		m_nFlags( nFlags )
	{
	}

	const char *m_pSoundName;
	Vector m_veSoundOrigin;
	float m_flVolume;
	float m_flSoundTime;
	CEntityIndex m_nSpeakerEntity;
	uint32 m_nForceGuid;
	uint32 m_nSourceSoundscape;
	int16 m_nPitch;
	EmitSoundFlags_t m_nFlags;

	const char *GetSoundName() const { return m_pSoundName; }
	void SetSoundName( const char *pSoundName ) { m_pSoundName = pSoundName; }

	const Vector &GetSoundOrigin() const { return m_veSoundOrigin; }
	Vector &GetSoundOrigin() { return m_veSoundOrigin; }
	void SetSoundOrigin( const Vector &vecSoundOrigin ) { m_veSoundOrigin = vecSoundOrigin; }

	float GetVolume() const { return m_flVolume; }
	void SetVolume( float flVolume ) { m_flVolume = flVolume; }

	float GetSoundTime() const { return m_flSoundTime; }
	void SetSoundTime( float flSoundTime ) { m_flSoundTime = flSoundTime; }

	CEntityIndex GetSpeakerEntity() const { return m_nSpeakerEntity; }
	void SetSpeakerEntity( CEntityIndex nSpeakerEntity ) { m_nSpeakerEntity = nSpeakerEntity; }

	bool HasForceGuid() const { return m_nForceGuid != 0; }
	uint32 GetForceGuid() const { return m_nForceGuid; }
	void SetForceGuid( uint32 nForceGuid ) { m_nForceGuid = nForceGuid; }
	void ClearForceGuid() { m_nForceGuid = 0; }

	bool HasSourceSoundscape() const { return m_nSourceSoundscape != 0; }
	uint32 GetSourceSoundscape() const { return m_nSourceSoundscape; }
	void SetSourceSoundscape( uint32 nSourceSoundscape ) { m_nSourceSoundscape = nSourceSoundscape; }
	void ClearSourceSoundscape() { m_nSourceSoundscape = 0; }

	int16 GetPitch() const { return m_nPitch; }
	void SetPitch( int16 nPitch ) { m_nPitch = nPitch; }

	EmitSoundFlags_t GetFlags() const { return m_nFlags; }
	void SetFlags( EmitSoundFlags_t nFlags ) { m_nFlags = nFlags; }
	void AddFlags( EmitSoundFlags_t nFlags ) { m_nFlags |= nFlags; }
	void RemoveFlags( EmitSoundFlags_t nFlags ) { m_nFlags &= ~nFlags; }
	bool HasFlags( EmitSoundFlags_t nFlags ) const { return ( m_nFlags & nFlags ) == nFlags; }
};
COMPILE_TIME_ASSERT( sizeof( EmitSound_t ) == 48 );


#define MAX_ACTORS_IN_SCENE 16

enum
{
	SIMULATION_TIME_WINDOW_BITS = 8,
};

//-----------------------------------------------------------------------------
// Cell origin values
//-----------------------------------------------------------------------------
#define CELL_COUNT( bits ) ( (MAX_COORD_INTEGER*2) / (1 << (bits)) ) // How many cells on an axis based on the bit size of the cell
#define CELL_COUNT_BITS( bits ) MINIMUM_BITS_NEEDED( CELL_COUNT( bits ) ) // How many bits are necessary to respresent that cell
#define CELL_BASEENTITY_ORIGIN_CELL_BITS 5 // default amount of entropy bits for base entity


// The player's method of starting / stopping commentary
#define COMMENTARY_BUTTONS	(IN_USE)

#define MAX_SPLITSCREEN_PLAYERS 1

inline bool IsSplitScreenSupported()
{
	return ( MAX_SPLITSCREEN_PLAYERS > 1 ) ? true : false;
}

//-----------------------------------------------------------------------------
// For invalidate physics recursive
//-----------------------------------------------------------------------------
enum InvalidatePhysicsBits_t
{
	POSITION_CHANGED	= 0x1,
	ANGLES_CHANGED		= 0x2,
	VELOCITY_CHANGED	= 0x4,
	ANIMATION_CHANGED	= 0x8,		// Means cycle has changed, or any other event which would cause render-to-texture shadows to need to be rerendeded
	BOUNDS_CHANGED		= 0x10,		// Means render bounds have changed, so shadow decal projection is required, etc.
	SEQUENCE_CHANGED	= 0x20,		// Means sequence has changed, only interesting when surrounding bounds depends on sequence																				
};

schema enum Class_T
{
	CLASS_NONE = 0,
	CLASS_PLAYER,
	CLASS_PLAYER_ALLY,
	CLASS_C4_FOR_RADAR,
	CLASS_FOOT_CONTACT_SHADOW,
	CLASS_WEAPON,
	CLASS_WATER_SPLASHER,
	CLASS_HUDMODEL_WEAPON,
	CLASS_HUDMODEL_ARMS,
	CLASS_HUDMODEL_ADDON,
	CLASS_WORLDMODEL_GLOVES,
	CLASS_DOOR,
	CLASS_PLANTED_C4,

	NUM_CLASSIFY_CLASSES,
};


//-----------------------------------------------------------------------------
// Generic activity lookup support
//-----------------------------------------------------------------------------
enum
{
	kActivityLookup_Unknown = -2,			// hasn't been searched for
	kActivityLookup_Missing = -1,			// has been searched for but wasn't found
};

// Used by base entity for spotted by masks
static int const kNumSpottedByMask = ALIGN_VALUE( MAX_PLAYERS, 32 ) / 32;


#include "schemacompiler/schemaoff.h"

#endif // SHAREDDEFS_H
