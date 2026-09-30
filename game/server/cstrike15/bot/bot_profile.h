//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: Bot personalities: skill, aggression, teamwork, aim, reaction time, weapon
//          preferences and voice, loaded from botprofile.db.
//
//=============================================================================//

#ifndef BOT_PROFILE_H
#define BOT_PROFILE_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "tier1/utlvector.h"
#include "tier1/utllinkedlist.h"

enum BotDifficultyType
{
	BOT_EASY = 0,
	BOT_NORMAL = 1,
	BOT_HARD = 2,
	BOT_EXPERT = 3,

	NUM_DIFFICULTY_LEVELS
};

//-----------------------------------------------------------------------------
// One entry of botprofile.db. Templates are profiles too; a profile copies every
// attribute of its templates that differs from the Default block.
//
// Attribute keys: Skill, Aggression and Teamwork are percentages (stored divided
// by 100), AimFocusInitial, AimFocusDecay, AimFocusOffsetScale, AimFocusInterval,
// WeaponPreference (a weapon name without the weapon_ prefix, up to 16, "none" clears),
// Cost, Skin, VoicePitch, VoiceBank (a chatter database file), ReactionTime,
// AttackDelay, Difficulty, Team (T, CT, anything else for both) and the
// six LookAngle springs.
//-----------------------------------------------------------------------------
class BotProfile
{
public:
	enum
	{
		MAX_WEAPON_PREFS = 16
	};

	const char *GetName() const { return m_name; }
	bool IsDifficulty( BotDifficultyType diff ) const { return ( m_difficultyFlags & ( 1 << diff ) ) != 0; }
	// Team numbers, 0 when the profile is valid for both.
	bool IsValidForTeam( int nTeam ) const { return m_teams == 0 || m_teams == nTeam; }

public:
	char *m_name;
	// 0 is a coward, 1 a berserker.
	float m_aggression;
	// 0 is terrible, 1 an expert.
	float m_skill;
	// 0 is a rogue, 1 always obeys the team and talks a lot.
	float m_teamwork;
	// The least aim error on the first attack.
	float m_aimFocusInitial;
	// How fast the focus error decays, as a scale per second.
	float m_aimFocusDecay;
	// Focus error gained from the angle between the view and the target.
	float m_aimFocusOffsetScale;
	float m_aimFocusInterval;
	// Item definition indices in order of priority.
	uint16 m_weaponPreference[ MAX_WEAPON_PREFS ];
	int m_weaponPreferenceCount;
	int m_cost;
	int m_skin;
	// A bit per BotDifficultyType.
	uint8 m_difficultyFlags;
	// 100 is normal.
	int m_voicePitch;
	float m_reactionTime;
	// From noticing an enemy to opening fire.
	float m_attackDelay;
	int m_teams;
	bool m_prefersSilencer;
	// Index into the profile manager's voice banks, 0 is botchatter.db.
	int m_voiceBank;
	float m_lookAngleMaxAccelNormal;
	float m_lookAngleStiffnessNormal;
	float m_lookAngleDampingNormal;
	float m_lookAngleMaxAccelAttacking;
	float m_lookAngleStiffnessAttacking;
	float m_lookAngleDampingAttacking;
	CUtlVector< const BotProfile * > m_templates;
};

typedef CUtlLinkedList< BotProfile * > BotProfileList;

//-----------------------------------------------------------------------------
// The singleton that owns every profile. ServerActivate resets it and loads each file
// listed in BotPackList.db, or botprofile.db when there is no list.
//-----------------------------------------------------------------------------
class BotProfileManager
{
public:
	enum
	{
		NumCustomSkins = 100
	};

	typedef CUtlVector< char * > VoiceBankList;

public:
	BotProfileList m_profileList;
	BotProfileList m_templateList;
	// Chatter database file names; the first is botchatter.db.
	VoiceBankList m_voiceBanks;
	char *m_skins[ NumCustomSkins ];
	char *m_skinModelnames[ NumCustomSkins ];
	char *m_skinFilenames[ NumCustomSkins ];
	int m_nextSkin;
};

#endif // BOT_PROFILE_H
