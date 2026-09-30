//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: How bots talk: radio messages, chatter phrases from botchatter.db, the
//          statements a bot queues and the memes it passes to its teammates.
//
//=============================================================================//

#ifndef CS_BOT_CHATTER_H
#define CS_BOT_CHATTER_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "navlib/cs_nav_area.h"
#include "mathlib/vector.h"
#include "tier1/utlvector.h"
#include "cs_bot_timers.h"

class CCSBot;
class BotChatterInterface;

//-----------------------------------------------------------------------------
// The radio messages. RADIO_START_n separate the three radio menus. The name table
// ends at RADIO_STORM_THE_FRONT, but the bot code accepts and keeps timestamps for
// the values up to RADIO_END.
//-----------------------------------------------------------------------------
enum RadioType
{
	RADIO_INVALID = 0,

	RADIO_START_1,

	RADIO_GO_GO_GO,
	RADIO_TEAM_FALL_BACK,
	RADIO_STICK_TOGETHER_TEAM,
	RADIO_HOLD_THIS_POSITION,
	RADIO_FOLLOW_ME,

	RADIO_START_2,

	RADIO_AFFIRMATIVE,
	RADIO_NEGATIVE,
	RADIO_CHEER,
	RADIO_COMPLIMENT,
	RADIO_THANKS,

	RADIO_START_3,

	RADIO_ENEMY_SPOTTED,
	RADIO_NEED_BACKUP,
	RADIO_YOU_TAKE_THE_POINT,
	RADIO_SECTOR_CLEAR,
	RADIO_IN_POSITION,

	RADIO_COVER_ME,
	RADIO_REGROUP_TEAM,
	RADIO_TAKING_FIRE,
	RADIO_REPORT_IN_TEAM,
	RADIO_REPORTING_IN,
	RADIO_GET_OUT_OF_THERE,
	RADIO_ENEMY_DOWN,
	RADIO_STORM_THE_FRONT,

	// No name table entry and nothing sends them; the names describe how a bot that hears
	// one from a teammate responds. The first three set the desired grenade of its human
	// path follow state (smoke, HE, and molotov for terrorists or incendiary for
	// counter-terrorists), the last two its path name to "_to_BombsiteA" or "_to_BombsiteB";
	// each then tries to start following a human path.
	RADIO_REQUEST_SMOKE,
	RADIO_REQUEST_HE_GRENADE,
	RADIO_REQUEST_FIRE_GRENADE,
	RADIO_GO_TO_BOMBSITE_A,
	RADIO_GO_TO_BOMBSITE_B,

	RADIO_END
};

// The place bot chatter and threat tracking talk about: the nav mesh's place identifier.
typedef CCSPlaceId BotPlace_t;

//-----------------------------------------------------------------------------
// The place a speakable is for, or the place a phrase must be spoken about. botchatter.db
// "Place ANY" leaves m_bAnyPlace set, "Place UNDEFINED" clears it and stores nowhere, and a
// place name clears it and stores that place.
//-----------------------------------------------------------------------------
struct BotPlaceCriteria_t
{
	bool m_bAnyPlace;
	BotPlace_t m_place;
};

//-----------------------------------------------------------------------------
// A unit of information a bot transmits to friendly bots with a statement.
//-----------------------------------------------------------------------------
abstract_class BotMeme
{
public:
	virtual ~BotMeme() {}

	// Makes the receiver act on the meme.
	virtual void Interpret( CCSBot *pSender, CCSBot *pReceiver ) const = 0;
};

abstract_class BotHelpMeme : public BotMeme
{
public:
	BotPlace_t m_place;
};

abstract_class BotBombsiteStatusMeme : public BotMeme
{
public:
	enum StatusType
	{
		CLEAR,
		PLANTED
	};

	int m_zoneIndex;
	StatusType m_status;
};

abstract_class BotBombStatusMeme : public BotMeme
{
public:
	// A CSGameState::BombState.
	int m_state;
	Vector m_pos;
};

abstract_class BotFollowMeme : public BotMeme
{
};

abstract_class BotDefendHereMeme : public BotMeme
{
public:
	Vector m_pos;
};

abstract_class BotWhereBombMeme : public BotMeme
{
};

abstract_class BotRequestReportMeme : public BotMeme
{
};

abstract_class BotHostageBeingTakenMeme : public BotMeme
{
};

abstract_class BotHeardNoiseMeme : public BotMeme
{
};

abstract_class BotWarnSniperMeme : public BotMeme
{
};

enum BotStatementType
{
	REPORT_VISIBLE_ENEMIES,
	REPORT_ENEMY_ACTION,
	REPORT_MY_CURRENT_TASK,
	REPORT_MY_INTENTION,
	REPORT_CRITICAL_EVENT,
	REPORT_REQUEST_HELP,
	REPORT_REQUEST_INFORMATION,
	REPORT_ROUND_END,
	REPORT_MY_PLAN,
	REPORT_INFORMATION,
	REPORT_EMOTE,
	// Affirmative or negative.
	REPORT_ACKNOWLEDGE,
	REPORT_ENEMIES_REMAINING,
	REPORT_FRIENDLY_FIRE,
	REPORT_KILLED_FRIEND,
	REPORT_ENEMY_LOST,

	NUM_BOT_STATEMENT_TYPES
};

//-----------------------------------------------------------------------------
// One sound of a phrase and the criteria it is valid for.
//-----------------------------------------------------------------------------
class BotSpeakable
{
public:
	char *m_phrase;
	// Seconds, 1 until the sound is measured.
	float m_duration;
	BotPlaceCriteria_t m_place;
	// The botchatter.db "Count" value, 4 for "Many".
	int m_count;
};

typedef CUtlVector< BotSpeakable * > BotSpeakableVector;
typedef CUtlVector< BotSpeakableVector * > BotVoiceBankVector;

//-----------------------------------------------------------------------------
// A named phrase of botchatter.db ("Chatter" or "Place" blocks) with a set of speakables
// for every voice bank. m_name is also the response rules concept when bot_chatter_use_rr
// is set.
//-----------------------------------------------------------------------------
class BotPhrase
{
public:
	const char *GetName() const { return m_name; }
	RadioType GetRadioEquivalent() const { return m_radioEvent; }
	bool IsImportant() const { return m_isImportant; }
	bool IsPlace() const { return m_isPlace; }

public:
	char *m_name;
	BotPlace_t m_place;
	bool m_isPlace;
	// Sent instead of the phrase when bot_chatter is "radio".
	RadioType m_radioEvent;
	// Part of a mission critical statement.
	bool m_isImportant;
	BotVoiceBankVector m_voiceBank;
	CUtlVector< int > m_count;
	// The next speakable to return per voice bank.
	CUtlVector< int > m_index;
	int m_numVoiceBanks;
	// Any place when the phrase is created; a statement sets its place before picking a
	// speakable.
	BotPlaceCriteria_t m_placeCriteria;
	// 0xFFFF when undefined.
	uint32 m_countCriteria;
	// A CResponseCriteriaSet (a polymorphic class) with the phrase's contexts; a copy
	// merged with "count" and "place" is passed to the response rules.
	alignas( 8 ) uint8 m_contexts[ 0x38 ];
};

typedef CUtlVector< BotPhrase * > BotPhraseList;

enum BotChatterOutputType
{
	BOT_CHATTER_RADIO,
	BOT_CHATTER_VOICE
};

//-----------------------------------------------------------------------------
// The singleton holding every phrase, loaded from botchatter.db and once more per
// custom voice bank.
//-----------------------------------------------------------------------------
class BotPhraseManager
{
public:
	enum
	{
		MAX_PLACES_PER_MAP = 64
	};

	struct PlaceTimeInfo
	{
		BotPlace_t placeID;
		IntervalTimer timer;
	};

public:
	BotPhraseList m_list;
	BotPhraseList m_placeList;
	// Per voice bank.
	CUtlVector< BotChatterOutputType > m_output;
	const BotPhrase *m_painPhrase;
	const BotPhrase *m_agreeWithPlanPhrase;
	// When a teammate last talked about each place.
	PlaceTimeInfo m_placeStatementHistory[ MAX_PLACES_PER_MAP ];
	int m_placeCount;
};

//-----------------------------------------------------------------------------
// Up to four phrases spoken one after another. Statements live in the owner's
// list sorted by m_startTime. Update speaks the next phrase once m_nextTime is
// reached and transmits the meme after the last one.
//-----------------------------------------------------------------------------
class BotStatement
{
public:
	enum ConditionType
	{
		IS_IN_COMBAT,
		RADIO_SILENCE,
		ENEMIES_REMAINING,

		NUM_CONDITIONS
	};

	enum ContextType
	{
		CURRENT_ENEMY_COUNT,
		REMAINING_ENEMY_COUNT,
		SHORT_DELAY,
		LONG_DELAY,
		ACCUMULATE_ENEMIES_DELAY
	};

	enum
	{
		MAX_BOT_PHRASES = 4,
		MAX_BOT_CONDITIONS = 4
	};

	struct Element_t
	{
		bool isPhrase;
		union
		{
			const BotPhrase *phrase;
			ContextType context;
		};
	};

	BotStatementType GetType() const { return m_type; }
	bool IsSpeaking() const { return m_isSpeaking; }

public:
	BotChatterInterface *m_chatter;
	BotStatement *m_next;
	BotStatement *m_prev;
	BotStatementType m_type;
	// A player the statement is about, -1 for none.
	int m_subject;
	// Falls back to the place of the first place phrase.
	BotPlace_t m_place;
	// Freed with the statement.
	BotMeme *m_meme;
	// Creation time.
	float m_timestamp;
	// The earliest time to start speaking.
	float m_startTime;
	float m_expireTime;
	float m_speakTimestamp;
	bool m_isSpeaking;
	// When the next phrase may start.
	float m_nextTime;
	Element_t m_statement[ MAX_BOT_PHRASES ];
	ConditionType m_condition[ MAX_BOT_CONDITIONS ];
	int m_conditionCount;
	// The phrase being spoken, -1 before the first.
	int m_index;
	int m_count;
};

//-----------------------------------------------------------------------------
// A bot's voice. Allocated by the bot and owned by it.
//
// A spoken phrase is output one of three ways, chosen per phrase:
//   - bot_chatter "radio": the phrase's radio equivalent through the bot's
//     SendRadioMessage, which plays the Radio.* sound and the #Cstrike_TitlesTXT_*
//     text of the player's radio services;
//   - bot_chatter_use_rr 1: the phrase name as a response rules concept with the
//     phrase contexts plus "count" and "place";
//   - otherwise a speakable of the profile's voice bank, played at the profile's
//     voice pitch. Phrases without a speakable fall back to the radio.
//-----------------------------------------------------------------------------
abstract_class BotChatterInterface
{
public:
	enum VerbosityType
	{
		// Full chatter.
		NORMAL,
		// Scenario critical events only.
		MINIMAL,
		// The standard radio instead.
		RADIO,
		OFF
	};

	enum
	{
		MUST_ADD = 1
	};

	virtual ~BotChatterInterface() = 0;

	virtual void EnemySpotted() = 0;
	virtual void KilledMyEnemy( int nVictimID ) = 0;
	virtual void EnemiesRemaining() = 0;
	// Says "Help" when outnumbered.
	virtual bool NeedBackup() = 0;
	// Says "WonRound", "WonRoundQuickly" or "LastManStanding".
	virtual void CelebrateWin() = 0;

	CCSBot *GetOwner() const { return m_me; }
	BotStatement *GetStatement() const { return m_statementList; }
	bool IsTalking() const { return m_statementList && m_statementList->IsSpeaking(); }
	int GetPitch() const { return m_pitch; }

public:
	BotStatement *m_statementList;
	CCSBot *m_me;
	bool m_seeAtLeastOneEnemy;
	float m_timeWhenSawFirstEnemy;
	bool m_reportedEnemies;
	// Already asked where the planted bomb is.
	bool m_requestedBombLocation;
	// Bots cycle through high (105 to 110), normal (95 to 105) and low (85 to 95).
	int m_pitch;
	IntervalTimer m_needBackupInterval;
	IntervalTimer m_spottedBomberInterval;
	IntervalTimer m_scaredInterval;
	IntervalTimer m_planInterval;
	CountdownTimer m_spottedLooseBombTimer;
	CountdownTimer m_heardNoiseTimer;
	CountdownTimer m_escortingHostageTimer;
	CountdownTimer m_warnSniperTimer;
};

#endif // CS_BOT_CHATTER_H
