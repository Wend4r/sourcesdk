//====== Copyright 1996-2005, Valve Corporation, All rights reserved. =======
//
// Purpose:
//
//=============================================================================

#ifndef CSTRIKE15_ITEM_CONSTANTS_H
#define CSTRIKE15_ITEM_CONSTANTS_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "tier0/dbg.h"

//-----------------------------------------------------------------------------
// Purpose: Slots for items within loadouts
// This is CS2's schema enum loadout_slot_t, with the names and the values the server registers
// for it; CS:GO called it loadout_positions_t and its enumerators LOADOUT_POSITION_*.
//-----------------------------------------------------------------------------
enum loadout_slot_t
{
	LOADOUT_SLOT_PROMOTED = -2,
	LOADOUT_SLOT_INVALID = -1,

	LOADOUT_SLOT_MELEE = 0,
	LOADOUT_SLOT_C4 = 1,
	LOADOUT_SLOT_FIRST_AUTO_BUY_WEAPON = LOADOUT_SLOT_MELEE,
	LOADOUT_SLOT_LAST_AUTO_BUY_WEAPON = LOADOUT_SLOT_C4,

	LOADOUT_SLOT_SECONDARY0 = 2,
	LOADOUT_SLOT_SECONDARY1 = 3,
	LOADOUT_SLOT_SECONDARY2 = 4,
	LOADOUT_SLOT_SECONDARY3 = 5,
	LOADOUT_SLOT_SECONDARY4 = 6,
	LOADOUT_SLOT_SECONDARY5 = 7,

	LOADOUT_SLOT_SMG0 = 8,
	LOADOUT_SLOT_SMG1 = 9,
	LOADOUT_SLOT_SMG2 = 10,
	LOADOUT_SLOT_SMG3 = 11,
	LOADOUT_SLOT_SMG4 = 12,
	LOADOUT_SLOT_SMG5 = 13,

	LOADOUT_SLOT_RIFLE0 = 14,
	LOADOUT_SLOT_RIFLE1 = 15,
	LOADOUT_SLOT_RIFLE2 = 16,
	LOADOUT_SLOT_RIFLE3 = 17,
	LOADOUT_SLOT_RIFLE4 = 18,
	LOADOUT_SLOT_RIFLE5 = 19,

	LOADOUT_SLOT_HEAVY0 = 20,
	LOADOUT_SLOT_HEAVY1 = 21,
	LOADOUT_SLOT_HEAVY2 = 22,
	LOADOUT_SLOT_HEAVY3 = 23,
	LOADOUT_SLOT_HEAVY4 = 24,
	LOADOUT_SLOT_HEAVY5 = 25,
	LOADOUT_SLOT_FIRST_WHEEL_WEAPON = LOADOUT_SLOT_SECONDARY0,
	LOADOUT_SLOT_LAST_WHEEL_WEAPON = LOADOUT_SLOT_HEAVY5,
	LOADOUT_SLOT_FIRST_PRIMARY_WEAPON = LOADOUT_SLOT_SMG0,
	LOADOUT_SLOT_LAST_PRIMARY_WEAPON = LOADOUT_SLOT_HEAVY5,

	LOADOUT_SLOT_FIRST_WHEEL_GRENADE = 26,
	LOADOUT_SLOT_GRENADE0 = LOADOUT_SLOT_FIRST_WHEEL_GRENADE,
	LOADOUT_SLOT_GRENADE1 = 27,
	LOADOUT_SLOT_GRENADE2 = 28,
	LOADOUT_SLOT_GRENADE3 = 29,
	LOADOUT_SLOT_GRENADE4 = 30,
	LOADOUT_SLOT_GRENADE5 = 31,
	LOADOUT_SLOT_LAST_WHEEL_GRENADE = LOADOUT_SLOT_GRENADE5,

	LOADOUT_SLOT_EQUIPMENT0 = 32,
	LOADOUT_SLOT_EQUIPMENT1 = 33,
	LOADOUT_SLOT_EQUIPMENT2 = 34,
	LOADOUT_SLOT_EQUIPMENT3 = 35,
	LOADOUT_SLOT_EQUIPMENT4 = 36,
	LOADOUT_SLOT_EQUIPMENT5 = 37,
	LOADOUT_SLOT_FIRST_WHEEL_EQUIPMENT = LOADOUT_SLOT_EQUIPMENT0,
	LOADOUT_SLOT_LAST_WHEEL_EQUIPMENT = LOADOUT_SLOT_EQUIPMENT5,

	LOADOUT_SLOT_CLOTHING_CUSTOMPLAYER = 38,
	LOADOUT_SLOT_CLOTHING_CUSTOMHEAD = 39,
	LOADOUT_SLOT_CLOTHING_FACEMASK = 40,
	LOADOUT_SLOT_CLOTHING_HANDS = 41,
	LOADOUT_SLOT_FIRST_COSMETIC = LOADOUT_SLOT_CLOTHING_HANDS,
	LOADOUT_SLOT_LAST_COSMETIC = LOADOUT_SLOT_CLOTHING_HANDS,
	LOADOUT_SLOT_CLOTHING_EYEWEAR = 42,
	LOADOUT_SLOT_CLOTHING_HAT = 43,
	LOADOUT_SLOT_CLOTHING_LOWERBODY = 44,
	LOADOUT_SLOT_CLOTHING_TORSO = 45,
	LOADOUT_SLOT_CLOTHING_APPEARANCE = 46,

	LOADOUT_SLOT_MISC0 = 47,
	LOADOUT_SLOT_MISC1 = 48,
	LOADOUT_SLOT_MISC2 = 49,
	LOADOUT_SLOT_MISC3 = 50,
	LOADOUT_SLOT_MISC4 = 51,
	LOADOUT_SLOT_MISC5 = 52,
	LOADOUT_SLOT_MISC6 = 53,

	LOADOUT_SLOT_MUSICKIT = 54,
	LOADOUT_SLOT_FLAIR0 = 55,
	LOADOUT_SLOT_SPRAY0 = 56,
	LOADOUT_SLOT_PET = 57,
	LOADOUT_SLOT_FIRST_ALL_CHARACTER = LOADOUT_SLOT_MUSICKIT,
	LOADOUT_SLOT_LAST_ALL_CHARACTER = LOADOUT_SLOT_PET,

	LOADOUT_SLOT_COUNT = 58,
};

#define LOADOUT_SLOT_NUM_AUTOBUY_WEAPONS ( ( LOADOUT_SLOT_LAST_AUTO_BUY_WEAPON - LOADOUT_SLOT_FIRST_AUTO_BUY_WEAPON ) + 1 )
#define LOADOUT_SLOT_NUM_COSMETICS ( ( LOADOUT_SLOT_LAST_COSMETIC - LOADOUT_SLOT_FIRST_COSMETIC ) + 1 )
#define LOADOUT_SLOT_NUM_WHEEL_WEAPONS ( ( LOADOUT_SLOT_LAST_WHEEL_WEAPON - LOADOUT_SLOT_FIRST_WHEEL_WEAPON ) + 1 )
#define LOADOUT_SLOT_NUM_WHEEL_GRENADE ( ( LOADOUT_SLOT_LAST_WHEEL_GRENADE - LOADOUT_SLOT_FIRST_WHEEL_GRENADE ) + 1 )
#define LOADOUT_SLOT_NUM_WHEEL_EQUIPMENT ( ( LOADOUT_SLOT_LAST_WHEEL_EQUIPMENT - LOADOUT_SLOT_FIRST_WHEEL_EQUIPMENT ) + 1 )
#define LOADOUT_SLOT_NUM_ALL_CHARACTER ( ( LOADOUT_SLOT_LAST_ALL_CHARACTER - LOADOUT_SLOT_FIRST_ALL_CHARACTER ) + 1 )

#define LOADOUT_NUM_PANELS ( LOADOUT_SLOT_NUM_AUTOBUY_WEAPONS + LOADOUT_SLOT_NUM_COSMETICS + LOADOUT_SLOT_NUM_WHEEL_WEAPONS + LOADOUT_SLOT_NUM_ALL_CHARACTER )

// We use this to determine the maximum number of wearable instances we'll send from the server down to connected clients.
// This hardcoded because of the way RecvPropUtlVector works we can't easily change this
// without doing some kludgy work and breaking network/demo compatibility.
// Make sure this is up to date before shipping with enough overhead and don't change it without serious consideration.
#define LOADOUT_MAX_WEARABLES_COUNT ( 1 /*LOADOUT_SLOT_NUM_COSMETICS*/ )

inline bool IsWearableSlot( int iSlot )
{
	return ( iSlot >= LOADOUT_SLOT_FIRST_COSMETIC && iSlot <= LOADOUT_SLOT_LAST_COSMETIC );
}

inline bool SlotContainsBaseItems( int iSlot )
{
	// Primary wheel 2 has 6 base item slots... all the rest have 5 and one empty
	if ( iSlot < LOADOUT_SLOT_MELEE )
	{
		return false;
	}

	if ( iSlot >= LOADOUT_SLOT_FIRST_WHEEL_WEAPON && iSlot <= LOADOUT_SLOT_LAST_WHEEL_WEAPON )
	{
		switch ( iSlot )
		{
		case LOADOUT_SLOT_SECONDARY5:
		case LOADOUT_SLOT_SMG5:
		case LOADOUT_SLOT_HEAVY5:
			return false;
		}

		return true;
	}

	if ( iSlot >= LOADOUT_SLOT_FIRST_WHEEL_GRENADE && iSlot <= LOADOUT_SLOT_LAST_WHEEL_GRENADE )
	{
		// 		switch( iSlot )
		// 		{
		// 		case LOADOUT_SLOT_GRENADE5:
		// 			return false;
		// 		}

		return true;
	}

	if ( iSlot >= LOADOUT_SLOT_FIRST_WHEEL_EQUIPMENT && iSlot <= LOADOUT_SLOT_LAST_WHEEL_EQUIPMENT )
	{
		switch ( iSlot )
		{
		case LOADOUT_SLOT_EQUIPMENT0:
		case LOADOUT_SLOT_EQUIPMENT1:
		case LOADOUT_SLOT_EQUIPMENT3:
		case LOADOUT_SLOT_EQUIPMENT4:
		case LOADOUT_SLOT_EQUIPMENT5:
			return false;
		}

		return true;
	}

	if ( iSlot >= LOADOUT_SLOT_FIRST_COSMETIC && iSlot <= LOADOUT_SLOT_LAST_COSMETIC )
	{
		switch ( iSlot )
		{
		case LOADOUT_SLOT_CLOTHING_CUSTOMPLAYER:
		case LOADOUT_SLOT_CLOTHING_CUSTOMHEAD:
		case LOADOUT_SLOT_CLOTHING_FACEMASK:
		case LOADOUT_SLOT_CLOTHING_HANDS:
		case LOADOUT_SLOT_CLOTHING_EYEWEAR:
		case LOADOUT_SLOT_CLOTHING_HAT:
		case LOADOUT_SLOT_CLOTHING_LOWERBODY:
		case LOADOUT_SLOT_CLOTHING_TORSO:
		case LOADOUT_SLOT_CLOTHING_APPEARANCE:
			return true;
		}

		return false;
	}

	if ( iSlot == LOADOUT_SLOT_MUSICKIT )
		return true;

	if ( iSlot >= LOADOUT_SLOT_FIRST_ALL_CHARACTER && iSlot <= LOADOUT_SLOT_LAST_ALL_CHARACTER )
	{
		return false;
	}

	return true;
}

// The total number of loadouts to track for each player.
#define LOADOUT_COUNT ( 2 + 2 ) // these are the number of skins (2 valid + 2 invalid)

//-----------------------------------------------------------------------------
// The maximum number of presets per class - CS doesn't actually use all 10
// right now, though.
//-----------------------------------------------------------------------------
#define MAX_LOADOUT_PRESET_COUNT 10

enum xp_category_t
{
	kXPCategory_None = 0,
	kXPCategory_ContributionScore = 1,    // Server can send ContributionScore (mutually exclusive with CompetitiveRoundWins)
	kXPCategory_CompetitiveRoundWins = 2, // Server can send CompetitiveRoundWins (mutually exclusive with ContributionScore)
	kXPCategory_BonusBoost = 3,           // GC rewards this weekly boost
	kXPCategory_Overwatch = 4,            // GC rewards Overwatch XP in post SQL-transaction
	kXPCategory_OverwatchBonusBoost = 5,  // GC rewards Overwatch Bonus XP in post SQL-transaction
	kXPCategory_QuestReward = 6,          // Server sends evaluated quest points, and GC rewards Quest XP from coin progress
	kXPCategory_QuestBonus = 7,           // Server sends evaluated quest bonus points, and GC rewards Quest XP from coin progress
	kXPCategory_QuestEvent = 8,           // Server sends quest event XP, GC relays it to client
	// kXPCategory_Headshots, etc

	// GC can also use reduced XP reasons -
	kXPCategory_ContributionScoreReduced = 51,    // ContributionScore remapped by GC for client display
	kXPCategory_CompetitiveRoundWinsReduced = 52, // CompetitiveRoundWins remapped by GC for client display
	kXPCategory_OverwatchReduced = 54,            // Overwatch XP reason remapped by GC for client display
	kXPCategory_QuestEventReduced = 58,           // QuestEvent XP reason remapped by GC for client display

	// GC can also use introductory XP reasons -
	kXPCategory_ContributionScoreIntroductory = 81,    // ContributionScore remapped by GC for client display
	kXPCategory_CompetitiveRoundWinsIntroductory = 82, // CompetitiveRoundWins remapped by GC for client display
	kXPCategory_QuestEventIntroductory = 88,           // QuestEvent XP reason remapped by GC for client display

	// NEW ENTRIES MUST BE ADDED AT THE BOTTOM
};

const uint32 k_nCSGOXpPerLevel = 5000;
const uint32 k_nCSGOXpMaxLevel = 40;

// Prime status
const uint32 k_nCSGOMinLevelForPrimeStatus = 21;

inline uint32 CSGOXpPointsToLevel( uint32 unExperiencePoints ) { return Min( k_nCSGOXpMaxLevel, 1 + ( unExperiencePoints / k_nCSGOXpPerLevel ) ); }
inline uint32 CSGOXpPointsToXpPoints( uint32 unExperiencePoints ) { return unExperiencePoints % k_nCSGOXpPerLevel; }
inline uint32 CSGOXpPointsToXpWirePoints( uint32 unExperiencePoints )
{
	COMPILE_TIME_ASSERT( k_nCSGOXpPerLevel < 0xFFFF );
	Assert( unExperiencePoints <= 0xFFFF );
	return ( ( unExperiencePoints ) & 0xFFFF ) | ( ( k_nCSGOXpPerLevel & 0xFFFF ) << 16 );
}
inline uint32 CSGOXpPointsFromXpWirePoints( uint32 unXpWirePoints )
{
	COMPILE_TIME_ASSERT( k_nCSGOXpPerLevel < 0xFFFF );
	uint32 const unWireBase = ( unXpWirePoints >> 16 ) & 0xFFFF;
	if ( !unWireBase || ( unWireBase == k_nCSGOXpPerLevel ) )
		return unXpWirePoints & 0xFFFF;
	else
		return uint32( ( double( unXpWirePoints & 0xFFFF ) / double( unWireBase ) ) * double( k_nCSGOXpPerLevel ) );
}

// STORED IN DATABASE! Do not renumber
enum
{
	kCSXpBonusFlags_EarnedXpThisPeriod = 1 << 0,
	kCSXpBonusFlags_FirstReward = 1 << 1,
	kCSXpBonusFlags_Msg_YourReportGotConvicted = 1 << 2,
	kCSXpBonusFlags_Msg_YouPartiedWithCheaters = 1 << 3,
	kCSXpBonusFlags_PrestigeEarned = 1 << 4,
	// ===$CHINAGOVERNMENTCERT$===
	kCSXpBonusFlags_ChinaGovernmentCert = 1 << 5,
	// ===$CHINAGOVERNMENTCERT$===

	// Client-facing bits not backed by SQL
	kCSXpBonusFlags_OverwatchBonus = 1 << 28,
	kCSXpBonusFlags_BonusBoostConsumed = 1 << 29,
	kCSXpBonusFlags_ReducedGain = 1 << 30,

	kCSXpBonusFlagsMask_SQLBacked_TimeBased = ( kCSXpBonusFlags_EarnedXpThisPeriod | kCSXpBonusFlags_FirstReward ),
	kCSXpBonusFlagsMask_SQLBacked_Notifications = ( kCSXpBonusFlags_Msg_YourReportGotConvicted | kCSXpBonusFlags_Msg_YouPartiedWithCheaters ),
	kCSXpBonusFlagsMask_SQLBacked_Permanent = ( kCSXpBonusFlagsMask_SQLBacked_Notifications | kCSXpBonusFlags_PrestigeEarned | kCSXpBonusFlags_ChinaGovernmentCert // ===$CHINAGOVERNMENTCERT$===
	                                            ),
	kCSXpBonusFlagsMask_SQLBacked = ( kCSXpBonusFlagsMask_SQLBacked_TimeBased | kCSXpBonusFlagsMask_SQLBacked_Permanent ),
	kCSXpBonusFlagsMask_Client_Permanent = ( kCSXpBonusFlags_OverwatchBonus ),
	kCSXpBonusFlagsMask_Client_TimeBased = ( kCSXpBonusFlags_BonusBoostConsumed | kCSXpBonusFlags_ReducedGain ),
};

// Used in scoreboard to show other players the reason for the drop. Standard weekly drops are of type 'None'.
enum
{
	kCSGODropReason_None = 0,
	kCSGODropReason_Quest = 1,
	kCSGODropReason_LevelUp = 2,
};

#endif // CSTRIKE15_ITEM_CONSTANTS_H
