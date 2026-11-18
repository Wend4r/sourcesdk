//====== Copyright (c), Valve Corporation, All rights reserved. =======
//
// Purpose: EconItemSchema: Defines a schema for econ items
//
//=============================================================================

#ifndef ECONITEMSCHEMA_H
#define ECONITEMSCHEMA_H
#ifdef _WIN32
#pragma once
#endif

#include "iecon_item_system.h"
#include "item_selection_criteria.h"

#include "steam/steamtypes.h"
#include "tier0/utldict.h"
#include "tier0/utlmap.h"
#include "tier0/utlstring.h"
#include "tier0/utlstringmap.h"
#include "tier0/utlsymbollarge.h"
#include "tier0/utlvector.h"
#include "tier1/utlhashmaplarge.h"
#include "mathlib/vector.h"
#include "color.h"
#include "string_t.h"

#include <string>
#include <string.h>

class CKeyChain;
class CEconItem;

class CPaintKit;

class CStickerKit;

class CStickerList;

class CProPlayerData;

class CEconQuestDefinition;

class CEconHighlightReelDefinition;

class AlternateIconData_t;

class CEconItemSchema;

class CWeaponEconData;

struct CEntityPrecacheContext;

#define MAX_PAINT_DATA_NAME 128

#define MAX_STICKER_DATA_PATH 128
#define NUM_SHOOT_SOUND_TYPES 24

#define DEFAULT_EMBROIDER_NCOLORS 10
#define DEFAULT_EMBROIDER_GAMMA 0.8

enum CraftFilter_t
{
	CRAFT_FILTER_TRADEUP = -3,
	CRAFT_FILTER_COLLECT = -2,
};

enum EStickerAttributeType
{
	k_EStickerAttribute_ID,
	k_EStickerAttribute_Wear,
	k_EStickerAttribute_Scale,
	k_EStickerAttribute_Rotation,
	k_EStickerAttribute_OffsetX,
	k_EStickerAttribute_OffsetY,
	k_EStickerAttribute_Schema,
	k_EStickerAttribute_Count,
};

enum EQuestVar_t
{
	// The variables a quest expression can name, in the order of the client's name table
	// (113 entries); the comment beside each one is the name the schema uses for it.
	k_EQuestVar_Act_KillHuman = 0, // act_kill_human
	k_EQuestVar_First = k_EQuestVar_Act_KillHuman,

	k_EQuestVar_Act_KillChicken, // act_kill_chicken
	k_EQuestVar_Act_KillTarget, // act_kill_target
	k_EQuestVar_Act_WinMatch, // act_win_match
	k_EQuestVar_Act_FlashbangEnemy, // act_flashbang_enemy
	k_EQuestVar_Act_PickUpHostage, // act_pick_up_hostage
	k_EQuestVar_Act_RescueHostage, // act_rescue_hostage
	k_EQuestVar_Act_DefuseBomb, // act_defuse_bomb
	k_EQuestVar_Act_PlantBomb, // act_plant_bomb
	k_EQuestVar_Act_Damage, // act_damage
	k_EQuestVar_Act_VampiricHeal, // act_vampiric_heal
	k_EQuestVar_Act_WinRound, // act_win_round
	k_EQuestVar_Act_RoundEnd, // act_round_end
	k_EQuestVar_Act_RoundMVP, // act_round_mvp
	k_EQuestVar_Act_DMBonusPoints, // act_dm_bonus_points
	k_EQuestVar_Act_Income, // act_income
	k_EQuestVar_Act_Cash, // act_cash
	k_EQuestVar_Act_Spend, // act_spend
	k_EQuestVar_Act_DZPurchaseItem, // act_dz_purchase_item
	k_EQuestVar_Act_PickUpTrophy, // act_pick_up_trophy
	k_EQuestVar_Act_ArmsraceLevel, // act_armsrace_level
	k_EQuestVar_Act_MotionDetectEnemy, // act_motion_detect_enemy
	k_EQuestVar_Act_AvoidMotionDetection, // act_avoid_motion_detection
	k_EQuestVar_Act_ProtectChicken, // act_protect_chicken
	k_EQuestVar_Act_ArmsraceStealLevel, // act_armsrace_steal_level
	k_EQuestVar_Act_KillOther, // act_kill_other
	k_EQuestVar_Act_DZGameEndPlacement, // act_dz_game_end_placement
	k_EQuestVar_Act_ParachuteLanded, // act_parachute_landed
	k_EQuestVar_Act_BumpmineLaunch, // act_bumpmine_launch
	k_EQuestVar_Act_ApplyGraffiti, // act_apply_graffiti
	k_EQuestVar_Act_DetonateSmokegrenade, // act_detonate_smokegrenade
	k_EQuestVar_Act_DetonateMolotov, // act_detonate_molotov
	k_EQuestVar_Act_UseHealthshot, // act_use_healthshot
	k_EQuestVar_Act_Survivalincome, // act_survivalincome
	k_EQuestVar_Act_GGWeaponUpgrade, // act_gg_weapon_upgrade
	k_EQuestVar_Act_GGUpgradeToKnifegg, // act_gg_upgrade_to_knifegg
	k_EQuestVar_Act_RetakesPickedEnemyCard, // act_retakes_picked_enemy_card
	k_EQuestVar_Act_RetakesPickedMVPCard, // act_retakes_picked_mvp_card
	k_EQuestVar_Cond_DamageHeadshot, // cond_damage_headshot
	k_EQuestVar_Cond_DamageBurn, // cond_damage_burn
	k_EQuestVar_Cond_DamagePenetrate, // cond_damage_penetrate
	k_EQuestVar_Cond_DamageCollateral, // cond_damage_collateral
	k_EQuestVar_Cond_DamageUtility, // cond_damage_utility
	k_EQuestVar_Cond_MatchUniqueWeapon, // cond_match_unique_weapon
	k_EQuestVar_Cond_MatchUniqueWeaponCount, // cond_match_unique_weapon_count
	k_EQuestVar_Cond_MatchCTsUniqueWeapon, // cond_match_cts_unique_weapon
	k_EQuestVar_Cond_MatchCTsUniqueWeaponCount, // cond_match_cts_unique_weapon_count
	k_EQuestVar_Cond_MatchTsUniqueWeapon, // cond_match_ts_unique_weapon
	k_EQuestVar_Cond_MatchTsUniqueWeaponCount, // cond_match_ts_unique_weapon_count
	k_EQuestVar_Cond_RoundstatePistolround, // cond_roundstate_pistolround
	k_EQuestVar_Cond_RoundstateFinalround, // cond_roundstate_finalround
	k_EQuestVar_Cond_RoundstateMatchpoint, // cond_roundstate_matchpoint
	k_EQuestVar_Cond_RoundstateBombPlanted, // cond_roundstate_bomb_planted
	k_EQuestVar_Cond_RoundstateTimeElapsed, // cond_roundstate_time_elapsed
	k_EQuestVar_Cond_ItemOwn, // cond_item_own
	k_EQuestVar_Cond_ItemBorrowed, // cond_item_borrowed
	k_EQuestVar_Cond_ItemBorrowedEnemy, // cond_item_borrowed_enemy
	k_EQuestVar_Cond_ItemBorrowedTeammate, // cond_item_borrowed_teammate
	k_EQuestVar_Cond_ItemBorrowedVictim, // cond_item_borrowed_victim
	k_EQuestVar_Cond_ItemNondefault, // cond_item_nondefault
	k_EQuestVar_Cond_DroneEnemy, // cond_drone_enemy
	k_EQuestVar_Cond_ArmorNone, // cond_armor_none
	k_EQuestVar_Cond_ArmorVestOnly, // cond_armor_vest_only
	k_EQuestVar_Cond_ArmorHelmet, // cond_armor_helmet
	k_EQuestVar_Cond_ArmorHeavyassaultsuit, // cond_armor_heavyassaultsuit
	k_EQuestVar_Cond_BulletSinceSpawn, // cond_bullet_since_spawn
	k_EQuestVar_Cond_PlayerX, // cond_player_x
	k_EQuestVar_Cond_PlayerY, // cond_player_y
	k_EQuestVar_Cond_PlayerZ, // cond_player_z
	k_EQuestVar_Cond_VictimX, // cond_victim_x
	k_EQuestVar_Cond_VictimY, // cond_victim_y
	k_EQuestVar_Cond_VictimZ, // cond_victim_z
	k_EQuestVar_Cond_PlayerRescuing, // cond_player_rescuing
	k_EQuestVar_Cond_PlayerZoomed, // cond_player_zoomed
	k_EQuestVar_Cond_PlayerBlind, // cond_player_blind
	k_EQuestVar_Cond_PlayerTerrorist, // cond_player_terrorist
	k_EQuestVar_Cond_PlayerCT, // cond_player_ct
	k_EQuestVar_Cond_PlayerAirborne, // cond_player_airborne
	k_EQuestVar_Cond_PlayerParachuteAirborneTimeTotal, // cond_player_parachute_airborne_time_total
	k_EQuestVar_Cond_PlayerMoneyCurrent, // cond_player_money_current
	k_EQuestVar_Cond_PlayerMoneyStart, // cond_player_money_start
	k_EQuestVar_Cond_PlayerMoneySpent, // cond_player_money_spent
	k_EQuestVar_Cond_PlayerMoneySpentThisRound, // cond_player_money_spent_this_round
	k_EQuestVar_Cond_LifeKillstreakHuman, // cond_life_killstreak_human
	k_EQuestVar_Cond_LifeKillstreakChicken, // cond_life_killstreak_chicken
	k_EQuestVar_Cond_MatchRoundsWon, // cond_match_rounds_won
	k_EQuestVar_Cond_MatchRoundsPlayed, // cond_match_rounds_played
	k_EQuestVar_Cond_MatchRoundsMVP, // cond_match_rounds_mvp
	k_EQuestVar_Cond_VictimBlind, // cond_victim_blind
	k_EQuestVar_Cond_VictimZoomed, // cond_victim_zoomed
	k_EQuestVar_Cond_VictimRescuing, // cond_victim_rescuing
	k_EQuestVar_Cond_VictimTerrorist, // cond_victim_terrorist
	k_EQuestVar_Cond_VictimCT, // cond_victim_ct
	k_EQuestVar_Cond_VictimReloading, // cond_victim_reloading
	k_EQuestVar_Cond_VictimSecondsSinceFire, // cond_victim_seconds_since_fire
	k_EQuestVar_Cond_VictimAirborne, // cond_victim_airborne
	k_EQuestVar_Cond_VictimArmsraceLeader, // cond_victim_armsrace_leader
	k_EQuestVar_Cond_VictimDistance, // cond_victim_distance
	k_EQuestVar_Cond_VictimThroughSmoke, // cond_victim_through_smoke
	k_EQuestVar_Cond_TabletReceptionIsBlocked, // cond_tablet_reception_is_blocked
	k_EQuestVar_Cond_TabletHasUpgradeHighres, // cond_tablet_has_upgrade_highres
	k_EQuestVar_Cond_TabletHasUpgradeDroneintel, // cond_tablet_has_upgrade_droneintel
	k_EQuestVar_Cond_TabletHasUpgradeZoneintel, // cond_tablet_has_upgrade_zoneintel
	k_EQuestVar_Cond_TabletHasUpgradeDronepilot, // cond_tablet_has_upgrade_dronepilot
	k_EQuestVar_Cond_RoundendCTsEliminateTs, // cond_roundend_cts_eliminate_ts
	k_EQuestVar_Cond_RoundendTsEliminateCTs, // cond_roundend_ts_eliminate_cts
	k_EQuestVar_Cond_RoundendTargetBombed, // cond_roundend_target_bombed
	k_EQuestVar_Cond_RoundendFailedToPlantBomb, // cond_roundend_failed_to_plant_bomb
	k_EQuestVar_Cond_RoundendBombDefused, // cond_roundend_bomb_defused
	k_EQuestVar_Cond_RoundendFailedToRescueHostage, // cond_roundend_failed_to_rescue_hostage
	k_EQuestVar_Cond_RoundendHostageRescued, // cond_roundend_hostage_rescued
	k_EQuestVar_Cond_ScoreFirstHalf, // cond_score_first_half
	k_EQuestVar_Cond_ScoreSecondHalf, // cond_score_second_half

	k_EQuestVar_Last
};

class CEconItemRarityDefinition
{
public:
	int32 m_nValue;
	attrib_colors_t m_iAttribColor;
	CUtlString m_strName;
	CUtlString m_strLocKey;
	CUtlString m_strWepLocKey;
	CUtlString m_strCharacterLocKey;
	CUtlString m_strLootList;
	CUtlString m_strRecycleLootList;
	CUtlString m_strDropSound;
	CUtlString m_strNextRarity;
	int m_iWhiteCount;
	int m_iBlackCount;
	float m_flWeight;
};

typedef int econ_tag_handle_t;
typedef uint16 equipped_class_t;
typedef uint16 equipped_slot_t;
typedef uint8 equipped_preset_t;

#define INVALID_ECON_TAG_HANDLE ( ( econ_tag_handle_t ) - 1 )
#define INVALID_EQUIPPED_SLOT ( ( equipped_slot_t ) - 1 )
#define INVALID_EQUIPPED_SLOT_BITPACKED 0x3F
#define INVALID_STYLE_INDEX ( ( style_index_t ) - 1 )
#define INVALID_PRESET_INDEX ( ( equipped_preset_t ) - 1 )

union attribute_data_union_t
{
	float asFloat;
	uint32 asUint32;
	byte *asBlobPointer;
};

struct static_attrib_t
{
	attrib_definition_index_t iDefIndex;
	attribute_data_union_t m_value;
	bool m_bForceGCToGenerate;
};

enum EWebResourceStatus
{
	kWebResource_InvalidName,
	kWebResource_NotLoaded,
	kWebResource_Loading,
	kWebResource_Loaded,
};

struct WeaponPaintableMaterial_t
{
	char m_szName[ MAX_PAINT_DATA_NAME ];
	char m_szOriginalMaterialName[ MAX_PAINT_DATA_NAME ];
	char m_szFolderName[ MAX_PAINT_DATA_NAME ];
	int m_nViewModelSize;  // texture size
	int m_nWorldModelSize; // texture size
	float m_flWeaponLength;
	float m_flUVScale;
	bool m_bBaseTextureOverride;
	bool m_bMirrorPattern;
};

class LightDesc_t;

struct InventoryImageData_t
{
	QAngle *m_pCameraAngles;
	Vector *m_pCameraOffset;
	float m_cameraFOV;
	LightDesc_t *m_pLightDesc[ 4 ];
	bool m_bOverrideDefaultLight;
};

struct StickerData_t
{
	char m_szStickerModelPath[ MAX_STICKER_DATA_PATH ];
	char m_szStickerMaterialPath[ MAX_STICKER_DATA_PATH ];
	Vector m_vWorldModelProjectionStart;
	Vector m_vWorldModelProjectionEnd;
	char m_szStickerBoneParentName[ 32 ];
};

class CEconItemQualityDefinition
{
public:
	int32 GetDBValue( void ) const
	{
		return m_nValue;
	}

	const char *GetName( void ) const
	{
		return !m_strName.IsEmpty() ? m_strName.Get() : "unknown";
	}

	uint32 GetWeight( void ) const
	{
		return m_unWeight;
	}

	bool GetRequiresExplicitMatches( void ) const
	{
		return m_bExplicitMatchesOnly;
	}

	bool CanSupportSet( void ) const
	{
		return m_bCanSupportSet;
	}

	const char *GetHexColor( void ) const
	{
		return !m_strHexColor.IsEmpty() ? m_strHexColor.Get() : "B2B2B2";
	}

private:
	int32 m_nValue;
	CUtlConstString m_strName;
	uint32 m_unWeight;
	bool m_bExplicitMatchesOnly;
	bool m_bCanSupportSet;
	CUtlConstString m_strHexColor;
};

//-----------------------------------------------------------------------------
// CEconColorDefinition
//-----------------------------------------------------------------------------
class CEconColorDefinition
{
public:
	const char *GetName() const
	{
		return m_strName.Get();
	}

	const char *GetColorName() const
	{
		return m_strColorName.Get();
	} // meant for passing into VGUI styles, etc.
	const char *GetHexColor() const
	{
		return m_strHexColor.Get();
	}

private:
	CUtlConstString m_strName;
	CUtlConstString m_strColorName;
	CUtlConstString m_strHexColor;
};
COMPILE_TIME_ASSERT( sizeof( CEconColorDefinition ) == 0x18 );

//-----------------------------------------------------------------------------
// CEconColorDefinition
//-----------------------------------------------------------------------------
class CEconGraffitiTintDefinition
{
public:
	int GetID( void ) const
	{
		return m_nID;
	}

	const char *GetColorName( void ) const
	{
		return m_strColorName.Get();
	} // meant for passing into VGUI styles, etc.
	const char *GetHexColor( void ) const
	{
		return m_strHexColor.Get();
	}

	uint32 GetHexColorRGB( void ) const
	{
		return m_unHexColorRGB;
	}

private:
	// The ID of the color
	int m_nID;

	// The name of the color in our schema (e.g. "shark_white")
	CUtlConstString m_strColorName;

	// The hex string value of this color. This will be used for Web display. (e.g. "#ff00ff")
	CUtlConstString m_strHexColor;
	uint32 m_unHexColorRGB;
};
COMPILE_TIME_ASSERT( sizeof( CEconGraffitiTintDefinition ) == 0x20 );

//-----------------------------------------------------------------------------
// CEconMusicDefinition
//-----------------------------------------------------------------------------
class CEconMusicDefinition
{
public:
	int nID;
	CUtlConstString m_strName;
	CUtlConstString m_strNameLocToken;
	CUtlConstString m_strLocDescription;
	CUtlConstString m_strPedestalDisplayModel;
	CUtlConstString m_strInventoryImage;

	// Eight more owned strings, freed with the rest. Neither the server nor the client writes
	// them; CS:GO keeps the icon URLs in this place.
	CUtlConstString m_sIconURLs[ 8 ];
};
COMPILE_TIME_ASSERT( sizeof( CEconMusicDefinition ) == 0x70 );

//-----------------------------------------------------------------------------
// An entry of "pet_definitions". The class name is not in the game binaries.
//-----------------------------------------------------------------------------
class CEconPetDefinition
{
public:
	int nID;
	CUtlConstString m_strName;
	CUtlConstString m_strNameLocToken;
	CUtlConstString m_strLocDescription;
	CUtlConstString m_strInventoryImage;
	CUtlConstString m_strPedestalDisplayModel;

	// Same as CEconMusicDefinition::m_sIconURLs.
	CUtlConstString m_sIconURLs[ 8 ];
};
COMPILE_TIME_ASSERT( sizeof( CEconPetDefinition ) == 0x70 );

//-----------------------------------------------------------------------------
// An entry of "highlight_reels"
//-----------------------------------------------------------------------------
class CEconHighlightReelDefinition
{
public:
	int m_nID;                // the block's own name, atoi'd
	CUtlConstString m_strName; // "id"

	int m_nTournamentEventID;      // "tournament event id"
	int m_nTournamentEventStageID; // "tournament event stage id"
	CUtlConstString m_strMap;      // "map"
	int m_nTournamentEventTeam0ID; // "tournament event team0 id"
	int m_nTournamentEventTeam1ID; // "tournament event team1 id"

	// Eight owned strings the server leaves empty; the client builds the video URLs of the reel.
	CUtlConstString m_sURLs[ 8 ];

	KeyValues *m_pKVItem;
};
COMPILE_TIME_ASSERT( sizeof( CEconHighlightReelDefinition ) == 0x70 );

// 56 bytes total
struct quest_event_t
{
	RTime32 m_rtEventStart;
	RTime32 m_rtEventEnd;
	int m_nDurationInMinutes;
	int m_nStartDay;
	CEconQuestDefinition *m_pQuestDef;
	CUtlConstString m_strHHMM;
	CUtlConstString m_strTimeZone;
};

// typedef CUtlVector <quest_event_t *> QuestEvents_t;
typedef CUtlMap< int, quest_event_t *, int > QuestEvents_t;
typedef CUtlMap< RTime32, quest_event_t *, int, CDefLess< RTime32 > > QuestEventsSchedule_t; // a map of quest events that gets resorted by start time every

class CEconQuestDefinition
{
public:
	int nID;
	CUtlConstString m_strName;

	CUtlConstString m_strMapGroup;
	CUtlConstString m_strMap;
	CUtlConstString m_strGameMode;
	CUtlConstString m_strRewardLootList;

	CUtlConstString m_strExpr;
	CUtlConstString m_strBonusExpr;

	CCopyableUtlVector< uint32 > m_vecQuestPoints; // how many times does the player need to do the action.
	bool m_bIsAnEvent;                             // Event quests are treated slightly differently.

	uint32 m_nDifficulty;        // quest difficulty for display purposes
	uint32 m_nOperationalPoints; // points towards coin leveling
	uint32 m_nXPReward;          // xp reward for completing the quest

	uint32 m_nXPBonusPercent; // bonus xp percentage for completing additionally satisfying the Bonus expression
	uint32 m_nTargetTeam;     // specific team this quest applies to (0 if not applicable)

	CUtlConstString m_strQuestConVars; // Cfg file to exec on the server when starting this quest
};

class CEconCampaignDefinition
{
public:
	class CEconCampaignNodeDefinition
	{
	public:
#ifdef CLIENT_DLL
		class CEconCampaignNodeStoryBlockDefinition
		{
		public:
			CUtlConstString m_strContentFile;
			CUtlConstString m_strCharacterName;
			CUtlConstString m_strExpr;
			CUtlConstString m_strDescription;
		};
		CUtlVector< CEconCampaignNodeStoryBlockDefinition * > m_vecStoryBlocks;
#endif
		uint32 m_nID;         // index of this node
		uint32 m_nQuestIndex; // index of the quest
		uint32 m_CampaignID;
		CUtlVector< uint32 > m_vecNextNodes; // list of nodes that completion of this node unlocks
	};

	typedef CUtlMap< int, CEconCampaignNodeDefinition *, int > CampaignNodeDefinitionsList_t;

	int m_nID;
	CUtlConstString m_strName;
	CUtlConstString m_strNameLocToken;
	CUtlConstString m_strLocDescription;
	CampaignNodeDefinitionsList_t m_mapCampaignNodes;
	CampaignNodeDefinitionsList_t m_mapStartNodes;
	uint32 m_nSeasonNumber; // operation season this campaign applies to
};

class CSkirMishDefinition
{
public:
	uint32 m_nID;
	CUtlConstString m_strName;
	CUtlConstString m_strGameMode;
	uint32 m_nGameMode;
	CUtlConstString m_strServerExec;
	CUtlConstString m_strLocName;
	CUtlConstString m_strLocRules;
	CUtlConstString m_strLocDescription;
	CUtlConstString m_strLocDetails;
	CUtlConstString m_strIcon;
};
COMPILE_TIME_ASSERT( sizeof( CSkirMishDefinition ) == 0x50 );

// 48 bytes in CS2, not the 28 of CS:GO: the item set getters index the entries with that stride.
struct item_list_entry_t
{
	// Item def
	int m_nItemDef;

	// Paint kit applied to it
	int m_nPaintKit;
	int m_nPaintKitSeed;
	float m_flPaintKitWear;

	// Sticker kit applied to it
	uint32 m_nStickerKit;

	// CS2 only: keychain applied to it
	uint32 m_nKeychainKit;

	// music kit applied to it
	uint32 m_nMusicKit;

	// CS2 only: pet definition applied to it
	uint32 m_nPetKit;

	bool m_bIsNestedList;
	bool m_bIsUnusualList;
	mutable bool m_bAlreadyUsedInRecursiveCreation;

	// Zeroed with the rest of a new entry and never read or written by the server.
	uint8 m_Unused[ 13 ];
};
COMPILE_TIME_ASSERT( sizeof( item_list_entry_t ) == 0x30 );

// Stored by value inside CEconItemSchema::m_mapItemSets, so it has to be a concrete type. The
// overrides below mirror what the game's own getters do - each one simply returns the field the
// corresponding vtable slot reads. Dispatch on a game-owned object still goes through the game's
// vtable; these bodies only matter for an instance we construct ourselves.
class CEconItemSetDefinition : public IEconItemSetDefinition
{
public:
	const char *GetName() const override { return m_pszName; }
	const char *GetLocKey() const override { return m_pszLocalizedName; }
	const char *GetUnlocalizedName() const override { return m_pszUnlocalizedName; }
	int GetBundle() const override { return m_iBundleItemDef; }
	int GetItemCount() const override { return m_ItemEntries.Count(); }
	int GetItemDef( int iIndex ) const override { return m_ItemEntries[ iIndex ].m_nItemDef; }
	int GetItemPaintKit( int iIndex ) const override { return m_ItemEntries[ iIndex ].m_nPaintKit; }
	item_definition_index_t GetCraftReward() const override { return m_nCraftReward; }

	// Both of these need the item schema to map a def index to a rarity, so they are left to the
	// game's implementation.
	int GetItemRarity( int iIndex ) const override { return -1; }
	int GetHighestItemRarityValue() const override { return -1; }

	// the three slots CS2 adds past IEconItemSetDefinition's ten
	virtual bool IsHiddenSet() const { return m_bIsHiddenSet; }
	virtual bool HasUnusualsLootList( int nQuality ) const { return false; }
	virtual bool BInitFromKV( KeyValues *pKVItemSet, CEconItemSchema &schema, CUtlVector< CUtlString > *pVecErrors = NULL ) { return false; }

public:
	const char *m_pszName; //	e.g. "set_dust"
	//	const char			*m_pszSetName;	//	unused
	const char *m_pszLocalizedName;
	const char *m_pszUnlocalizedName;
	const char *m_pszLocalizedDescription;
	CUtlVector< item_list_entry_t > m_ItemEntries;
	int m_iBundleItemDef; // Item def of the store bundle for this set, if any
	bool m_bIsCollection;
	bool m_bIsHiddenSet; // If true, this set and any bonuses will only be visible if the whole set is equipped.
	item_definition_index_t m_nCraftReward;

	struct itemset_attrib_t
	{
		int m_iAttribDefIndex;
		attrib_value_t m_valValue;
	};
	CUtlVector< itemset_attrib_t > m_iAttributes;

	// CS2 only: KV "unusuals", quality index -> lootlist name. Only qualities 4 and 9 are accepted.
	// CUtlOrderedMap rather than CUtlMap because the comparator has to be a stateless functor: the
	// game leaves the tree's leading less-func slot untouched, so CUtlMap's function-pointer form
	// would have Find() call through whatever garbage is sitting there.
	CUtlOrderedMap< int, const char *, CDefLess< int >, int > m_mapUnusualsLootLists;
};
COMPILE_TIME_ASSERT( sizeof( CEconItemSetDefinition ) == 0x88 );

// Stored by value inside CEconItemSchema::m_dictLootLists, so it has to be a concrete type. The
// overrides mirror the game's getters; dispatch on a game-owned object still goes through the
// game's vtable, so the bodies only matter for an instance we construct ourselves.
class CEconLootListDefinition : public IEconLootListDefinition
{
public:
	struct loot_list_additional_drop_t
	{
		float m_fChance;
		bool m_bPremiumOnly;
		const char *m_pszLootListDefName;
	};

	struct lootlist_attrib_t
	{
		static_attrib_t m_staticAttrib;
		float m_flWeight;
		float m_flRangeMin;
		float m_flRangeMax;

		CCopyableUtlVector< uint32 > m_vecValues;
	};

	struct random_attrib_t
	{
		float m_flChanceOfRandomAttribute;
		float m_flTotalAttributeWeight;
		bool m_bPickAllAttributes;

		// Constructed empty before m_RandomAttributes; the loot list parser never fills it.
		CUtlVector< lootlist_attrib_t > m_UnusedAttributes;

		CUtlVector< lootlist_attrib_t > m_RandomAttributes;
	};

public:
	const char *GetName() const override { return m_pszName; }
	const CUtlVector< item_list_entry_t > &GetLootListContents() const override { return m_ItemEntries; }
	float GetWeight( int iIdx ) const override { return m_flWeights.IsValidIndex( iIdx ) ? m_flWeights[ iIdx ] : 0.0f; }

	// Rebuilds the block from the members, so it is left to the game's implementation.
	KeyValues *GenerateKeyValues() const override { return NULL; }

	bool IsServerList() const override { return m_bServerList; }

	// Additional Drops
	int GetAdditionalDropCount( void ) const override { return m_AdditionalDrops.Count(); }

	bool GetAdditionalDrop( int iIndex, CUtlString &strLootList, float &flChance ) const override
	{
		if ( !m_AdditionalDrops.IsValidIndex( iIndex ) )
			return false;

		strLootList.Set( m_AdditionalDrops[ iIndex ].m_pszLootListDefName );
		flChance = m_AdditionalDrops[ iIndex ].m_fChance;

		return true;
	}

	// Random Attributes
	int GetRandomAttributeGroupCount( void ) const override { return m_RandomAttribs.Count(); }

	bool GetRandomAttributeGroup( int iIndex, float &flChance, float &flTotalWeight ) const override
	{
		if ( !m_RandomAttribs.IsValidIndex( iIndex ) )
			return false;

		flChance = m_RandomAttribs[ iIndex ]->m_flChanceOfRandomAttribute;
		flTotalWeight = m_RandomAttribs[ iIndex ]->m_flTotalAttributeWeight;

		return true;
	}

	int GetRandomAttributeCount( int iGroup ) const override
	{
		return m_RandomAttribs.IsValidIndex( iGroup ) ? m_RandomAttribs[ iGroup ]->m_RandomAttributes.Count() : 0;
	}

	bool GetRandomAttribute( int iGroup, int iIndex, float &flWeight, int &iValue, int &iDefIndex ) const override
	{
		if ( !m_RandomAttribs.IsValidIndex( iGroup ) || !m_RandomAttribs[ iGroup ]->m_RandomAttributes.IsValidIndex( iIndex ) )
			return false;

		const lootlist_attrib_t &attrib = m_RandomAttribs[ iGroup ]->m_RandomAttributes[ iIndex ];

		flWeight = attrib.m_flWeight;
		iValue = static_cast< int >( attrib.m_staticAttrib.m_value.asFloat );
		iDefIndex = attrib.m_staticAttrib.iDefIndex;

		return true;
	}

	// Editable Interface
	void PurgeItems( void ) override { m_ItemEntries.Purge(); }

	// The one slot the game adds: true when an entry is an unusual list, nested lists included.
	// Walking the nested lists needs the item schema, so it is left to the game's implementation.
	virtual bool HasUnusualLoot() const { return false; }

public:
	CUtlVector< loot_list_additional_drop_t > m_AdditionalDrops;
	const char *m_pszName;
	CUtlVector< item_list_entry_t > m_ItemEntries;
	uint32 m_unHeroID;

	bool m_bPublicListContents;                        // "public_list_contents"
	bool m_bContainsStickersAutographedByProplayers;   // "contains_stickers_autographed_by_proplayers"
	int m_nContainsStickersRepresentingOrganizations;  // "contains_stickers_representing_organizations"
	bool m_bContainsPatchesRepresentingOrganizations;  // "contains_patches_representing_organizations"
	int m_iLimitDescriptionToNumberRound;              // "limit_description_to_number_rnd"

	bool m_bWillProduceStattrak; // "will_produce_stattrak"

	float m_flTotalWeight;
	CUtlVector< float > m_flWeights;

	CUtlVector< random_attrib_t * > m_RandomAttribs;

	bool m_bServerList;
	bool m_bAllEntriesAsAdditionalDrops; // "all_entries_as_additional_drops"

	// CS2 only: the id of the keychain "match_highlight_reel_keychain" names.
	int m_nMatchHighlightReelKeychain;

	// CS2 only. Both are constructed empty and destroyed with the list; the loot list parser
	// never touches either.
	bool m_bUnusedFlag;
	CUtlVector< int > m_UnusedVector;

	// The block the list was parsed from.
	KeyValues *m_pKVLootList;
};
COMPILE_TIME_ASSERT( sizeof( CEconLootListDefinition ) == 0xC0 );

class CEconCraftingRecipeDefinition;

//-----------------------------------------------------------------------------
// Purpose: Attribute definition details
//-----------------------------------------------------------------------------
enum
{
	ATTDESCFORM_VALUE_IS_PERCENTAGE,          // Printed as:	((m_flValue*100)-100.0)
	ATTDESCFORM_VALUE_IS_INVERTED_PERCENTAGE, // Printed as:	((m_flValue*100)-100.0) if it's > 1.0, or ((1.0-m_flModifier)*100) if it's < 1.0
	ATTDESCFORM_VALUE_IS_ADDITIVE,            // Printed as:	m_flValue
	ATTDESCFORM_VALUE_IS_ADDITIVE_PERCENTAGE, // Printed as:	(m_flValue*100)
	ATTDESCFORM_VALUE_IS_OR,                  // Printed as:  m_flValue, but results are ORd together instead of added
	ATTDESCFORM_VALUE_IS_DATE,                // Printed as a date
	ATTDESCFORM_VALUE_IS_ACCOUNT_ID,          // Printed as steam user name
	ATTDESCFORM_VALUE_IS_PARTICLE_INDEX,      // Printed as a particle description
	ATTDESCFORM_VALUE_IS_ITEM_DEF,            // Printed as item name
	ATTDESCFORM_VALUE_IS_COLOR,               // Printed as R, G, B
	ATTDESCFORM_VALUE_IS_GAME_TIME,           // Printed as:	00:00
	ATTDESCFORM_VALUE_IS_MINS_AS_HOURS,       // Printed as:  N hours when >2 hours or N minutes otherwise
	ATTDESCFORM_VALUE_IS_REPLACE,             // Printed as:  m_flValue
};

// Coloring for attribute lines
enum attrib_effect_types_t
{
	ATTRIB_EFFECT_NEUTRAL = 0,
	ATTRIB_EFFECT_POSITIVE,
	ATTRIB_EFFECT_NEGATIVE,

	NUM_EFFECT_TYPES,
};

enum EAssetClassAttrExportRule_t
{
	k_EAssetClassAttrExportRule_Default = 0,
	k_EAssetClassAttrExportRule_Bucketed = ( 1 << 0 ), // attribute exports bucketed value to Steam Community
	k_EAssetClassAttrExportRule_Skip = ( 1 << 1 ),     // attribute value is not exported to Steam Community
	k_EAssetClassAttrExportRule_GCOnly = ( 1 << 2 ),   // attribute only lives on GC and not exported to any external request
};

//-----------------------------------------------------------------------------
// CEconItemAttributeDefinition
// Template definition of a randomly created attribute
//-----------------------------------------------------------------------------
class CEconItemAttributeDefinition : public IEconItemAttributeDefinition
{
public:
	uint16 GetDefinitionIndex( void ) const override { return m_nDefIndex; }
	const char *GetDefinitionName( void ) const override { return m_pszDefinitionName; }

	const char *GetDescriptionString( void ) const override { return m_pszDescriptionString; }
	const char *GetAttributeClass( void ) const override { return m_pszAttributeClass; }

	KeyValues *GetRawDefinition( void ) const override { return m_pKVAttribute; }

	const class ISchemaAttributeType *GetAttributeType( void ) const { return m_pAttrType; }
	bool IsStoredAsInteger( void ) const { return ( m_nStoredAsInteger & 1 ) != 0; }

private:
	// The raw keyvalues for this attribute definition.
	KeyValues *m_pKVAttribute;

	// Required valued from m_pKVAttribute:

	// The number used to refer to this definition in the DB
	attrib_definition_index_t m_nDefIndex;

	// ...
	const class ISchemaAttributeType *m_pAttrType;

	// ---------------------------------------------
	// Display related data
	// ---------------------------------------------
	// "hidden": 0, 1, or 2 when the value is a "classdefidx:" list.
	int m_nHidden;

	// The item definition indices of a "classdefidx:" list.
	CUtlVector< int > m_vecHiddenClassDefIndices;

	// If true, this attribute's description is always output in web api calls regardless of the hidden flag.
	bool m_bWebSchemaOutputForced;

	// "stored_as_integer": 0, 1, 2 for "float_floor_to_integer" or 4 for "bytestring".
	int m_nStoredAsInteger;

	// If this is true the attribute is counted as "instance" data for purposes of asset class in the Steam Economy. Non-instance
	// properties are considered things that can differentiate items at a fundamental level (ie., definition index, quality); instance
	// properties are more things like additional customizations -- score for strange items, paint color, etc.
	bool m_bInstanceData;

	// Constructed as 2. Nothing in the server reads or writes it afterwards.
	int m_nUnused;

	EAssetClassAttrExportRule_t m_eAssetClassAttrExportRule; // if this is true the attribute will not be exported for asset class
	uint32 m_unAssetClassBucket;                             // if this is set then attribute value is bucketed when exported for asset class

	// Overall positive/negative effect. Used to color the attribute.
	attrib_effect_types_t m_iEffectType;

	// Contains the description format & string for this attribute
	int m_iDescriptionFormat;
	const char *m_pszDescriptionString;

	const char *m_pszDescriptionTag;

	// Contains information on how to describe items with this attribute in the Armory
	const char *m_pszArmoryDesc;
	int m_iScore;

	// Used to allow unique items to specify attributes by name.
	const char *m_pszDefinitionName;

	// The class name of this attribute. Used in creation, and to hook the attribute into the actual code that uses it.
	const char *m_pszAttributeClass;
	mutable string_t m_iszAttributeClass; // Same as the above, but used for fast lookup when applying attributes.
};
COMPILE_TIME_ASSERT( sizeof( CEconItemAttributeDefinition ) == 0x98 );

class CEconSoundMaterialDefinition
{
public:
	int32 m_nID;
	CUtlString m_strName;
	CUtlString m_strStartDragSound;
	CUtlString m_strEndDragSound;
	CUtlString m_strEquipSound;
};

//-----------------------------------------------------------------------------
// Visual data storage in item definitions
//-----------------------------------------------------------------------------
#define MAX_VISUALS_CUSTOM_SOUNDS 10

enum
{
	kAttachedModelDisplayFlag_WorldModel = 0x01,
	kAttachedModelDisplayFlag_ViewModel = 0x02,

	kAttachedModelDisplayFlag_MaskAll = kAttachedModelDisplayFlag_WorldModel | kAttachedModelDisplayFlag_ViewModel,
};

struct attachedmodel_t
{
	const char *m_pszModelName;
	int m_iModelDisplayFlags;
};

struct attachedparticle_t
{
	int m_iParticleIndex;
	int m_nStyle;
};

enum wearableanimplayback_t
{
	WAP_ON_SPAWN,       // Play this animation immediately on spawning the wearable
	WAP_START_BUILDING, // Game code will start this anim whenever a player wearing this item deploys their builder weapon.
	WAP_STOP_BUILDING,  // Game code will start this anim whenever a player wearing this item holsters their builder weapon.

	NUM_WAP_TYPES,
};

struct animation_on_wearable_t
{
	int iActivity;
	const char *pszActivity;
	wearableanimplayback_t iPlayback;
	const char *pszReplacement;
	int iReplacement; // Replacement activity to play. Might be set to one of kActivityLookup_Unknown/kActivityLookup_Missing.
	const char *pszSequence;
	const char *pszRequiredItem;
	const char *pszScene;
	float flFrequency;
};

struct sound_on_wearable_t
{
	const char *pszSound;
	const char *pszReplacement;
};

struct particle_on_wearable_t
{
	const char *pszParticle;
	const char *pszReplacement;
	bool bFlyingCourierEffect;
};

struct particlesnapshot_on_wearable_t
{
	const char *pszParticleSnapshot;
	const char *pszReplacement;
};

struct particle_control_point_on_wearable_t
{
	const char *pszParticle;
	int nParticleControlPoint;
	Vector vecCPValue;
};

struct codecontrolledbodygroupdata_t
{
	const char *pFuncName;
	void *pFunc;
};

struct style_unlock_info
{
	const char *pszItemName;
	int iPrice;
	int iStylePreReq;
	const char *pszAttrib;
	uint32 iAttribValue;
};

class CEconStyleInfo
{
public:
	virtual ~CEconStyleInfo() = 0;

	virtual void BInitFromKV( KeyValues *pKVItem, CEconItemSchema &pschema, CUtlVector< CUtlString > *pVecErrors ) = 0;

	virtual void GeneratePrecacheModelStringsForStyle( CUtlVector< const char * > *out_pVecModelStrings ) = 0;

protected:
	int m_iIndex;
	int m_iSkin;
	int m_iIcon;
	const char *m_pszName;
	const char *m_pszBasePlayerModel;
	CUtlVector< const char * > m_vecAdditionalHideBodygroups;
	style_unlock_info m_UnlockInfo;
};

struct announcer_preview_t
{
	CUtlString m_strFileName;
	CUtlString m_strCaption;
};

struct ability_icon_replacement_t
{
	CUtlString m_strAbilityName;
	CUtlString m_strReplacement;
};

enum EAssetModifier
{
	AM_Invalid = -1,
	AM_Activity,
	AM_Announcer,
	AM_AnnouncerPreview,
	AM_HudSkin,
	AM_AbilityName,
	AM_Sound,
	AM_Speech,
	AM_Particle,
	AM_ParticleSnapshot,
	AM_ParticleControlPoint,
	AM_EntityModel,
	AM_ViewModel,
	AM_EntityScale,
	AM_Icon,
	AM_AbilityIcon,
	AM_Courier,
	AM_CourierFlying,
	AM_HeroModelChange,
	AM_MAX
};

struct AssetModifier
{
	EAssetModifier m_Type;
	CUtlString m_strAsset;
	CUtlString m_strModifier;
	float m_flModifier;
	float m_flFrequency;
	int m_iStyle;
};

struct AssetInfo
{
	int iHideParentBodyGroup;
	CUtlMap< int, CUtlVector< AssetModifier * > * > m_mapAssetModifiers;
	CUtlMap< const char *, codecontrolledbodygroupdata_t > m_CodeControlledBodyGroupNames;
	int iSkin;
	bool bUsePerClassBodygroups;

	CUtlVector< attachedmodel_t > m_AttachedModels;
	CUtlVector< attachedparticle_t > m_AttachedParticles;

	const char *m_pszCustomSounds[ MAX_VISUALS_CUSTOM_SOUNDS ];
	const char *m_pszMaterialOverride;
	const char *m_pszMuzzleFlash;
	const char *m_pszTracerEffect;
	const char *m_pszParticleEffect;
	const char *m_pszParticleSnapshot;
	const char *m_pszWeaponSoundReplacements[ NUM_SHOOT_SOUND_TYPES ];

	const char *m_pszPrimaryAmmo;
	const char *m_pszSecondaryAmmo;
	const char *m_pszWeaponTypeString;
	const char *m_pszAddonLocation;
	const char *m_pszEjectBrassEffect;
	const char *m_pszMuzzleFlashEffect1stPerson;
	const char *m_pszMuzzleFlashEffect1stPersonAlt;
	const char *m_pszMuzzleFlashEffect3rdPerson;
	const char *m_pszMuzzleFlashEffect3rdPersonAlt;
	const char *m_pszHeatEffect;
	const char *m_pszPlayerAnimationExtension;

	Vector m_vecSmokeColor;

	int m_iViewModelBodyGroupOverride;
	int m_iViewModelBodyGroupStateOverride;
	int m_iWorldModelBodyGroupOverride;
	int m_iWorldModelBodyGroupStateOverride;
	bool m_bSkipModelCombine;

	CUtlVector< const char * > m_vecAnimationModifiers;

	// The effect names above are short names in the schema; these hold the full
	// "particles/weapons/cs_weapon_fx/%s.vpcf" paths the pointers above are set to.
	CUtlString m_strTracerEffect;
	CUtlString m_strEjectBrassEffect;
	CUtlString m_strMuzzleFlashEffect1stPerson;
	CUtlString m_strMuzzleFlashEffect1stPersonAlt;
	CUtlString m_strMuzzleFlashEffect3rdPerson;
	CUtlString m_strMuzzleFlashEffect3rdPersonAlt;
	CUtlString m_strHeatEffect;

	CUtlVector< CEconStyleInfo * > m_Styles;
};
COMPILE_TIME_ASSERT( sizeof( AssetInfo ) == 0x298 );

enum item_capabilities_t
{
	ITEM_CAP_NONE = 0,
	ITEM_CAP_PAINTABLE = 1 << 0,              // some items are tagged in CS:GO schema, but we don't use it
	ITEM_CAP_NAMEABLE = 1 << 1,               // used in CS:GO on all weapons that can get a name tag
	ITEM_CAP_DECODABLE = 1 << 2,              // used in CS:GO on supply crates containers
	ITEM_CAP_CAN_DELETE = 1 << 3,             // used in CS:GO on supply crates containers
	ITEM_CAP_CAN_CUSTOMIZE_TEXTURE = 1 << 4,  // NOT USED
	ITEM_CAP_USABLE = 1 << 5,                 // NOT USED
	ITEM_CAP_USABLE_GC = 1 << 6,              // some items are tagged in CS:GO schema, but we don't use it
	ITEM_CAP_CAN_GIFT_WRAP = 1 << 7,          // NOT USED
	ITEM_CAP_USABLE_OUT_OF_GAME = 1 << 8,     // some items are tagged in CS:GO schema, but we don't use it
	ITEM_CAP_CAN_COLLECT = 1 << 9,            // NOT USED
	ITEM_CAP_CAN_CRAFT_COUNT = 1 << 10,       // NOT USED
	ITEM_CAP_CAN_CRAFT_MARK = 1 << 11,        // NOT USED
	ITEM_CAP_PAINTABLE_TEAM_COLORS = 1 << 12, // NOT USED
	ITEM_CAP_CAN_WRAP_STICKER = 1 << 13,      // "can_wrap_sticker"; CS:GO had "can_be_restored" in this bit
	ITEM_CAP_CAN_USE_STRANGE_PARTS = 1 << 14, // NOT USED
	ITEM_CAP_PAINTABLE_UNUSUAL = 1 << 15,     // NOT USED
	ITEM_CAP_CAN_INCREMENT = 1 << 16,         // NOT USED
	ITEM_CAP_USES_ESSENCE = 1 << 17,          // NOT USED
	ITEM_CAP_AUTOGRAPH = 1 << 18,             // NOT USED
	ITEM_CAP_RECIPE = 1 << 19,                // NOT USED
	ITEM_CAP_CAN_STICKER = 1 << 20,           // used in CS:GO on sticker tools, primary and secondary weapons
	ITEM_CAP_STATTRACK_SWAP = 1 << 21,        // used in CS:GO on stattrack items
	ITEM_CAP_CAN_PATCH = 1 << 22,             // used in CS:GO on customplayers
	ITEM_CAP_CAN_KEYCHAIN = 1 << 23,          // "can_keychain"
	NUM_ITEM_CAPS = 24,
};

enum
{
	ITEM_CAP_DEFAULT = ITEM_CAP_NONE
}; // what are the default capabilities on an item?
enum
{
	ITEM_CAP_TOOL_DEFAULT = ITEM_CAP_NONE
}; // what are the default capabilities of a tool?

struct bundleinfo_t
{
	CUtlVector< item_list_entry_t > vecItemEntries;
};

class CItemSelectionCriteria;

class IEconConsumable;

class IEconItemInterface;

class IEconTool
{
public:
	virtual ~IEconTool() = 0;

	virtual bool CanApplyTo( const IEconItemInterface *pTool, const IEconItemInterface *pToolSubject ) const = 0;

	virtual bool ShouldDisplayQuantity( const IEconItemInterface *pTool ) const = 0;

	virtual bool RequiresToolEscrowPeriod() = 0;

	virtual bool IsValid() = 0;

	virtual IEconConsumable *CreateEconConsumable() = 0;

private:
	const char *m_pszTypeName;
	const char *m_pszUseString;
	const char *m_pszUsageRestriction;
	item_capabilities_t m_unCapabilities;
	CUtlVector< int > m_vecBonusItemDef;
};

enum EItemType
{
	k_EItemTypeNone,
	k_EItemTypeCoupon,
	k_EItemTypeCampaign,
	k_EItemTypeSelfOpeningPurchase,
	k_EItemTypeOperationCoin,
	k_EItemTypePrestigeCoin,
	k_EItemTypePremierSeasonCoin,
	k_EItemTypeTool,
	k_EItemTypeGameLicense,
	k_EItemTypeFanShield,
	k_EItemTypeSubscription,
};

const char *PchNameFromEItemType( EItemType eType );

enum EItemSlot
{
	k_EItemSlotNone,
	k_EItemSlotMelee,
	k_EItemSlotSecondary,
	k_EItemSlotSMG,
	k_EItemSlotRifle,
	k_EItemSlotHeavy,
	k_EItemSlotFlair,
	k_EItemSlotMusicKit,
};

const char *PchNameFromEItemSlot( EItemSlot eSlot );

EItemSlot EItemSlotFromName( const char *pchName );

//-----------------------------------------------------------------------------
// CEconItemDefinition
// Template Definition of a randomly created item
//-----------------------------------------------------------------------------
// This class adds twenty-seven slots to IEconItemDefinition's twenty-one. Only the first four
// are overridden by CCStrike15ItemDefinition -- the destructor, the two parsers and Precache.
class CEconItemDefinition : public IEconItemDefinition
{
public:
	virtual ~CEconItemDefinition() = 0;

	virtual bool BInitFromKV( KeyValues *pKVItem, CEconItemSchema &pschema, CUtlVector< CUtlString > *pVecErrors = nullptr );
	virtual bool BInitFromTestItemKVs( int iNewDefIndex, KeyValues *pKVItem, CEconItemSchema &pschema ) = 0;

	// A no-op here; CCStrike15ItemDefinition adds its four m_pszPlayerDisplayModel entries.
	virtual void Precache( const CEntityPrecacheContext *pContext ) = 0;

	virtual void GeneratePrecacheModelStrings( bool bDynamicLoad, CUtlVector< const char * > *pVecModelStrings ) const = 0;
	virtual void GeneratePrecacheSoundStrings( CUtlVector< const char * > *pVecSoundStrings ) const = 0;
	virtual void GeneratePrecacheEffectStrings( CUtlVector< const char * > *pVecEffectStrings ) const = 0;

	virtual uint32 GetItemTypeID() const = 0;
	virtual bool IsDefaultSlotItem() const = 0; // "flexible_loadout_default"
	// Two stubs, the same in the server and the client: one returns 0, the other true. Nothing
	// calls or overrides either, so their names are not recoverable.
	virtual int ReturnsZero_Slot30() const = 0;
	virtual bool ReturnsTrue_Slot31() const = 0;
	virtual int GetBundleItemPaintKit( int iIndex ) const = 0;
	virtual const char *GetZoomInSound() const = 0;
	virtual const char *GetZoomOutSound() const = 0;

	// Each reads one static string attribute off the definition, named in the comment.
	virtual const char *GetPedestalDisplayModel() const = 0;  // "pedestal display model"
	virtual const char *GetMagazineModel() const = 0;         // "magazine model"
	virtual const char *GetAimsightLensMaskModel() const = 0; // "aimsight lens mask"
	virtual const char *GetUidModel() const = 0;              // "uid model"

	// The attribute name comes from kill_eater_score_types[nScoreType].model_attribute; an
	// unknown score type falls back to type 0.
	virtual const char *GetKillEaterModel( int nScoreType ) const = 0;

	// What a thing takes: the stickers on a weapon, the patches on an agent. This is the count the
	// game itself gates on -- CCSPlayer_ItemServices' wearables pass reads it off the agent's
	// definition before it writes the patch slots.
	virtual int GetNumberOfStickerSlots() const = 0;  // "stickers"
	virtual int GetNumberOfKeychainSlots() const = 0; // "keychains"

	virtual const char *GetIconDefaultImage() const = 0;
	virtual const char *GetParticleFile() const = 0;
	virtual const char *GetParticleSnapshotFile() const = 0;
	// A stub like the two above: returns false, never called or overridden.
	virtual bool ReturnsFalse_Slot45() const = 0;
	virtual bool IsLoadOnDemand() const = 0; // "loadondemand"
	virtual bool IsIgnoredInCollectionView() const = 0;

	const char *GetItemClassname() const { return m_pszItemClassname; }
	const CUtlVector< static_attrib_t > &GetStaticAttributes() const { return m_vecStaticAttributes; }

public:
	// Pointer to the raw KeyValue definition of the item
	KeyValues *m_pKVItem;

	// Required values from m_pKVItem:

	// The number used to refer to this definition in the DB
	item_definition_index_t m_nDefIndex;

	// List of associated items, such as multiple different keys for the same crate.
	CUtlVector< item_definition_index_t > m_nAssociatedItemsDefIndexes;

	// False if this definition has been turned off and we're not using it to generate items
	bool m_bEnabled;

	// The prefab used by the item. NOT VERIFIED against CS2 and it does not read as one: the
	// pointer here is garbage while m_pszItemBaseName two fields down is a valid string, so
	// either the field is somewhere else or the schema drops the name once the prefab has been
	// applied. Do not dereference it; it access-violates. Everything from m_nDefaultDropQuantity
	// on carries a checked offset and is safe.
	const char *m_szPrefab;

	// These values specify the range of item levels that an item based off this definition can be generated within.
	uint8 m_unMinItemLevel;
	uint8 m_unMaxItemLevel;

	// This specifies an item's rarity.
	uint8 m_nItemRarity;

	// This specifies an item quality that items from this definition must be set to. Used mostly to specify unique item definitions.
	uint8 m_nItemQuality;
	uint8 m_nForcedItemQuality;
	uint8 m_nDefaultDropItemQuality;

	// Default drop quantity
	uint8 m_nDefaultDropQuantity;

	// Static attributes (ones that are always on these items)
	CUtlVector< static_attrib_t > m_vecStaticAttributes;

	// Seeds the popular item list with this number of the item when the list is reset.
	uint8 m_nPopularitySeed;

	// Portraits KV
	KeyValues *m_pPortraitsKV;

	// ---------------------------------------------
	// Display related data
	// ---------------------------------------------
	// The base name of this item. i.e. "The Kritz-Krieg".
	const char *m_pszItemBaseName;
	bool m_bProperName; // If set, the name will have "The" prepended to it, unless it's got a non-unique quality
	// in which case it'll have "A" prepended to the quality. i.e. A Community Kritzkrieg

	// The base type of this item. i.e. "Rocket Launcher" or "Shotgun".
	// This is often the same as the base name, but not always.
	const char *m_pszItemTypeName;

	// A unique identifier for m_pszItemTypeName
	uint32 m_unItemTypeID;

	// The item's non-attribute description.
	const char *m_pszItemDesc;

	// expiration time
	RTime32 m_rtExpiration;

	// item def creation time
	RTime32 m_rtDefCreation;

	// The .mdl file used for this item when it's displayed in inventory-style boxes.
	const char *m_pszInventoryModel;
	// Alternatively, the images used for this item when it's displayed in inventory-style boxes.
	// If specified, they're used over the model; GetInventoryImage(n) answers [0] for an index
	// past the end or an entry that was never filled in.
	const char *m_pszInventoryImages[ 15 ];

	// An optional image that's overlayed over the top of the base inventory image. It'll be RGB colored by the tint color of the item.
	CUtlVector< const char * > m_pszInventoryOverlayImages;

	int m_iInventoryImagePosition[ 2 ];
	int m_iInventoryImageSize[ 2 ];

	CUtlString m_pszBaseDisplayModel;
	bool m_bLoadOnDemand;
	bool m_bHasBeenLoaded;

	bool m_bHideBodyGroupsDeployedOnly;

	// The .mdl file used for the world view.
	// This is inferior to using a c_model, but because the geometry of the sticky bomb launcher's
	// world model is significantly different from the view model the demoman pack requires
	// using two separate models for now.
	CUtlString m_pszWorldDisplayModel;
	const char *m_pszWorldExtraWearableModel; // Some weapons attach an extra wearable item to the player

	// How many of each the item takes, off the "stickers" and "keychains" keys.
	int m_nNumberOfStickerSlots;
	int m_nNumberOfKeychainSlots;

	// Sticker model paths
	CUtlVector< StickerData_t > m_vStickerModels;

	// This is the name of the default image used for the inventory until the generated image is ready
	const char *m_pszIconDefaultImage;

	// If set, we use the base hands model for a viewmodel, and bonemerge the above player model
	bool m_bAttachToHands;
	bool m_bAttachToHandsVMOnly;

	// If set, we will force the view model to render flipped. Good for models built left handed.
	bool m_bFlipViewModel;

	// This is a wearable that sits in a non-wearable loadout slot
	bool m_bActAsWearable;

	// The possible sets this item is a member of
	mutable CUtlVector< int > m_iItemSets;

	// Asset Modifiers & Info
	AssetInfo *m_pAssetInfo;

	// Another way to group econ items with similar properties, separate from item_class.
	// (items of the same class may need to have different types).
	EItemType m_eItemType;

	// Optional override for specifying a custom shell ejection model
	const char *m_pszBrassModelOverride;

	const char *m_pszZoomInSound;
	const char *m_pszZoomOutSound;

	IEconTool *m_pTool;
	bundleinfo_t *m_BundleInfo;
	item_capabilities_t m_iCapabilities;

	uint32 m_unNumConcreteItems; // This is the number of items that will actually end up in a user's inventory - this can be 0 for some items (e.g. map stamps in TF), 1 for a "regular" item, or many for bundles, etc.

	int m_nSoundMaterialID;

	bool m_bDisableStyleSelection;

	// particle file
	const char *m_pszParticleFile;         // Some items have custom particle files attached to them
	const char *m_pszParticleSnapshotFile; // Some weapons override a generic effect with a custom snapshot

	const char *m_pszLootListName; // Optionally specified loot list for a crate item (instead of a supply crate series).

	CUtlVector< int > m_nCharacterPaintKitIndices;

	CUtlVector< WeaponPaintableMaterial_t > m_PaintData;
	InventoryImageData_t *m_pInventoryImageData;

	// ---------------------------------------------
	// Creation related data
	// ---------------------------------------------
	// The entity classname for this item.
	const char *m_pszItemClassname;

	// The entity name that will be displayed in log files.
	const char *m_pszItemLogClassname;

	// The name of the icon used in the death notices.
	const char *m_pszItemIconClassname;

	// This is the script file name of this definition. Used to generate items by script name.
	const char *m_pszDefinitionName;

	bool m_bOwnerOnly;
	bool m_bHidden;
	bool m_bShouldShowInArmory;
	bool m_bBaseItem;
	bool m_bDefaultSlotItem;
	bool m_bImported;
	bool m_bOnePerAccountCDKEY;

	// A pack bundle is a bundle that contains items that are not for sale individually
	bool m_bIsPackBundle;

	// A pack item is an item which is not for sale individually and is only for sale as part of a pack bundle. A 'regular' bundle can only include a pack bundle by explicitly including all of the pack bundle's items individually.
	// If this pointer is non-NULL, this item is considered to be a pack item (see CEconItemDefinition::IsPackItem()).
	CEconItemDefinition *m_pOwningPackBundle;

	// Contains information on how to describe items with this attribute in the Armory
	const char *m_pszArmoryDesc;

	// ---------------------------------------------
	// Remapping data for armory/store
	// ---------------------------------------------
	int m_iArmoryRemap;
	int m_iStoreRemap;
	const char *m_pszArmoryRemap;
	const char *m_pszStoreRemap;

	// ---------------------------------------------
	// Crafting related data
	// ---------------------------------------------
	const char *m_pszClassToken;
	const char *m_pszSlotToken;

	// ---------------------------------------------
	// Gameplay related data
	// ---------------------------------------------
	// How to behave when the player wearing the item dies.
	int m_iDropType;

	// Holiday restriction. Item only has an appearance when the holiday is in effect.
	const char *m_pszHolidayRestriction;

	// Temporary. Revisit this in the engineer update. Enables an additional buildable.
	int m_iSubType;

	// Whitelist support for tournament mode
	bool m_bAllowedInThisMatch;

	// this will hold a pointer to the criteria that should be used to generate the real item
	CItemSelectionCriteria *m_pProxyCriteria;

	equip_region_mask_t m_unEquipRegionMask;         // which equip regions does this item cover directly
	equip_region_mask_t m_unEquipRegionConflictMask; // which equip regions does equipping this item prevent from having something in them

	// Alternate icons.
	CUtlMap< uint32, const char * > *m_pMapAlternateIcons;

	CUtlVector< econ_tag_handle_t > m_vecTags;
	CUtlVector< const CEconItemDefinition * > m_vecContainingBundleItemDefs; // Item definition indices for any bundles which contain this item
	CUtlVector< uint32 > m_vecSteamWorkshopContributors;

	bool m_bPublicItem;
	bool m_bIgnoreInCollectionView;

	// The weapon data cached for this definition index, filled in once the schema is parsed.
	CWeaponEconData *m_pWeaponEconData;
};
COMPILE_TIME_ASSERT( sizeof( CEconItemDefinition ) == 0x330 );

class CTimedItemRewardDefinition
{
public:
	// Frequency of how often the item is awarded
	uint32 m_unMinFreq;
	uint32 m_unMaxFreq;

	RTime32 m_rtForcedBaselineAdjustment;                    // Forces all baselines to be adjusted at least beyond this mark, when tweaking drop rates and reward periods
	RTime32 m_rtForcedLastDropTimeAdjustment;                // Forces last drop time to be adjusted to at least this mark, when tweaking drop rates and reward periods
	uint32 m_unHoursInRewardPeriod;                          // How many hours a reward period lasts before rolling over
	uint32 m_unHoursBetweenDropsRealtime;                    // How many realtime hours before dropping next drop due from this series
	CUtlVector< uint32 > m_arrTotalPointsBasedOnHoursPlayed; // Description table of total points a player can earn in reward period based on hours played
	uint32 m_unPointsPerHourOverplayed;                      // Every hour beyond the points table will reward a player with so many points
	uint32 m_unPointsPerPeriodRollover;                      // Points per period that will rollover

	// The chance, between 0 and 1, that the item is rewarded
	float m_flChance;

	// The criteria to use to select the item to reward
	CItemSelectionCriteria m_criteria;
	// Alternatively, the loot_list to use instead
	CUtlVector< const CEconLootListDefinition * > m_arrLootLists;

	// dynamically allocated loot lists are in both vectors, but we keep this list to delete on destruct
	// loot lists from the schema are in the above vector as well and are stored statically.
	CUtlVector< const CEconLootListDefinition * > m_arrDynamicLootLists;
};

class CItemLevelingDefinition
{
public:
	uint32 m_unLevel;
	uint32 m_unRequiredScore;
	char *m_pszLocalizedName_LocalStorage;
};

struct AchievementAward_t
{
	CUtlString m_sNativeName;
	AppId_t m_unSourceAppId;
	uint32 m_unAuditData;
	CUtlVector< uint16 > m_vecDefIndex;
};

enum eTimedRewardType
{
	kTimedRewards_Reward,
	kTimedRewards_PremiumTimedDrop,
	kTimedRewards_Crate,
	kTimedRewards_Operation,
	kTimedRewards_Capsule,
	kTimedRewards_Quest,
	kTimedRewards_FreeSprayDropsOct2016,
	kNumTimedRewards
};

struct kill_eater_score_type_t
{
	// The value that the game/DB will know this rarity by
	int32 m_nValue;
	const char *m_pszTypeString;
	const char *m_pszModelAttributeString;
	const char *m_pszLevelBlockName;
	bool m_bUseLevelBlock;
};

class CWebResource
{
public:
	CUtlString m_strName;
	CUtlString m_strURL;
	bool m_bOnDemand;
	KeyValues *m_pKeyValues;

	void ( *m_fnLoadCallback )( const char *, KeyValues * );
};

class ISchemaAttributeType
{
public:
	virtual ~ISchemaAttributeType() = 0;

	// Returns a unique integer describing the C++-in-memory-layout type used by this attribute type.
	// For example, something that stores "int" might return 0 and "CSomeFancyWideAttributeType" might
	// return 1. The actual values don't matter and can even differ between different runs of the game/GC.
	// The only important thing is that during a single run the value for a single type is consistent.
	virtual unsigned int GetTypeUniqueIdentifier() = 0;

	// Have this attribute type copy the data out of the value union and type-copy it onto the item. This
	// is accessible on clients as well as the GC.
	virtual void LoadEconAttributeValue( CEconItem *pTargetItem, const CEconItemAttributeDefinition *pAttrDef, const union attribute_data_union_t &value ) = 0;

	// ...
	virtual void ConvertEconAttributeValueToByteStream( const union attribute_data_union_t &value, std::string *out_psBytes ) = 0;

	// ...
	virtual bool BConvertStringToEconAttributeValue( const CEconItemAttributeDefinition *pAttrDef, const char *pszValue, union attribute_data_union_t *out_pValue ) = 0;

	// ...
	virtual void ConvertEconAttributeValueToString( const CEconItemAttributeDefinition *pAttrDef, const attribute_data_union_t &value, std::string *out_ps ) = 0;

	// Used to deserialize a byte stream, probably from an on-wire protobuf message, instead an instance
	// of the attribute in memory. See ConvertByteStreamToTypedValue() for example implementation, or
	// ConvertTypedValueToByteStream() for an example of the byte-stream generator code.
	virtual void LoadByteStreamToEconAttributeValue( CEconItem *pTargetItem, const CEconItemAttributeDefinition *pAttrDef, const std::string &sBytes ) = 0;

	// ...
	virtual void InitializeNewEconAttributeValue( attribute_data_union_t *out_pValue ) = 0;

	// Free any heap-allocated memory from this attribute value. Is not responsible for zeroing out
	// pointers, etc.
	virtual void UnloadEconAttributeValue( union attribute_data_union_t *out_pValue ) = 0;

	virtual bool OnIterateAttributeValue( class IEconItemAttributeIterator *pIterator, const CEconItemAttributeDefinition *pAttrDef, const attribute_data_union_t &value ) = 0;

	// This could also be called "BIsHackyMessyOldAttributeType()". This determines whether the attribute
	// can be set at runtime on a CEconItemView instance, whether the gameserver can replicate the value to
	// game clients, etc. It really only makes sense for value types 32 bits or smaller.
	virtual bool BSupportsGameplayModificationAndNetworking() = 0;
};

typedef CUtlDict< CUtlConstString, int > ArmoryStringDict_t;
typedef CUtlDict< CUtlVector< CItemLevelingDefinition > * > LevelBlockDict_t;
typedef CUtlMap< unsigned int, kill_eater_score_type_t > KillEaterScoreMap_t;

struct attr_type_t
{
	CUtlConstString m_sName;
	const ISchemaAttributeType *m_pAttrType;

	attr_type_t( const char *pszName, const ISchemaAttributeType *pAttrType ) : m_sName( pszName ), m_pAttrType( pAttrType )
	{
	}
};

class IDelayedSchemaData
{
public:
	virtual ~IDelayedSchemaData()
	{
	}

	virtual bool InitializeSchema( CEconItemSchema *pItemSchema ) = 0;
};

class CPipRankData
{
public:
	int32 rank;
	int8 id;
	int8 pips;
	int8 winstreak;
	int8 loss;
};

// size 164
class CSeasonalOperation
{
public:
};

class CEconItemSchema : public IEconItemSchema
{
public:
	virtual ~CEconItemSchema() = 0;

	struct EquipRegion
	{
		CUtlConstString m_sName;
		unsigned int m_unBitIndex;    // which bit are we claiming ownership over? there might be multiple equip regions with the same bit if we're in a "shared" block
		equip_region_mask_t m_unMask; // full region conflict mask
	};

	typedef CUtlHashMapLarge< int, CEconItemDefinition * > ItemDefinitionMap_t;
	typedef CUtlMap< int, CEconItemDefinition *, int > SortedItemDefinitionMap_t;
	typedef CUtlDict< CEconLootListDefinition > LootListDefinitionDict_t;
	typedef CUtlMap< int, const char *, int > RevolvingLootListDefinitionMap_t;
	typedef CUtlMap< const char *, int, unsigned short > BodygroupStateMap_t;
	typedef CUtlVector< CEconColorDefinition * > ColorDefinitionsList_t;
	typedef CUtlMap< int, CEconMusicDefinition *, int > MusicDefinitionsList_t;
	typedef CUtlMap< int, CEconQuestDefinition *, int > QuestDefinitionsList_t;
	typedef CUtlMap< int, CEconCampaignDefinition *, int > CampaignDefinitionsList_t;
	typedef CUtlMap< int, CSeasonalOperation *, int > SeasonalOperations_t;
	typedef CUtlMap< int, CSkirMishDefinition *, int > SkirMishModesMap_t;
	typedef CUtlVector< CEconItemAttributeDefinition * > EconAttrDefsContainer_t;
	typedef CUtlMap< int, CEconCraftingRecipeDefinition *, int > RecipeDefinitionMap_t;
	typedef CUtlVector< EquipRegion > EquipRegionsList_t;
	typedef CUtlDict< econ_tag_handle_t > EconTagDict_t;
	typedef CUtlMap< uint64, AlternateIconData_t *, int, CDefLess< uint64 > > AlternateIconsMap_t;
	typedef CUtlMap< int, CProPlayerData *, int > MapProPlayersByAccountID_t;
	typedef CUtlStringMap< CProPlayerData * > MapProPlayersByName_t;
	typedef CUtlMap< uint64, CUtlVector< const CProPlayerData * > *, int, CDefLess< uint64 > > MapProPlayersByEventIDTeamID_t;

	// The key of m_mapHighlightReelsByMatch, compared as two 64-bit halves.
	struct HighlightReelMatch_t
	{
		int m_nTournamentEventID;
		int m_nTournamentEventStageID;
		int m_nTournamentEventTeam0ID;
		int m_nTournamentEventTeam1ID;
	};

	struct StickerKitEventTeam_t
	{
		int m_nEventID;
		int m_nEventTeamID;

		bool operator<( const StickerKitEventTeam_t &other ) const
		{
			return m_nEventID != other.m_nEventID ? m_nEventID < other.m_nEventID : m_nEventTeamID < other.m_nEventTeamID;
		}
	};

public:
	virtual int GetToolType( const char *pszToolType ) = 0;

	virtual int GetNumPrefabs() = 0;

	virtual const char *GetPrefabName( int idx ) = 0;

	virtual uint32 GetHeroID( const char *pszHeroName ) = 0;

public:
	// Subclass interface.
	virtual CEconItemDefinition *CreateEconItemDefinition() = 0;

	virtual CEconCraftingRecipeDefinition *CreateCraftingRecipeDefinition() = 0;

	virtual CEconStyleInfo *CreateEconStyleInfo() = 0;

	virtual IEconTool *CreateEconToolImpl( const char *pszToolType, const char *pszUseString, const char *pszUsageRestriction, item_capabilities_t unCapabilities, KeyValues *pUsageKV ) = 0;

	virtual void Reset() = 0;

	virtual int CalculateNumberOfConcreteItems( const CEconItemDefinition *pItemDef ) = 0;

	virtual bool BInitItemMappings( CUtlVector< CUtlString > *pVecErrors ) = 0;

	virtual bool BInitBundles( CUtlVector< CUtlString > *pVecErrors ) = 0;

	virtual bool BInitPaymentRules( CUtlVector< CUtlString > *pVecErrors ) = 0;

	virtual bool BInitSchema( KeyValues *pKVRawDefinition, CUtlVector< CUtlString > *pVecErrors ) = 0;

	// Not game functions: lookups over the maps below, defined at the end of this file.
	CPaintKit *GetPaintKitDefinitionByName( const char *pchName ) const;
	CPaintKit *GetPaintKitDefinitionByMapIndex( int iMapIndex ) const;
	AlternateIconData_t *GetAlternateIcon( uint64 ullAlternateIcon );
	CEconItemDefinition *GetItemDefinitionMutable( int iItemIndex, bool bNoDefault = true );

	CUtlMap< const char *, uint16, int > m_mapItemDefRestrictions;

	bool m_bSchemaUpdatesEnabled;

	uint32 m_unResetCount;

	KeyValues *m_pKVRawDefinition;

	// Constructed null and nulled again by Reset() right after the raw definition is deleted;
	// never deleted itself, and nothing in the server reads it.
	void *m_pResetWithRawDefinition;

	uint32 m_unVersion;

	// Class range
	equipped_class_t m_unFirstValidClass;
	equipped_class_t m_unLastValidClass;

	// Item slot range
	equipped_slot_t m_unFirstValidItemSlot;
	equipped_slot_t m_unLastValidItemSlot;

	// Number of allowed presets
	uint32 m_unNumItemPresets;
	uint32 m_unNumMaxStickers;
	uint32 m_unNumMaxPatches;

	// Allowable range of item levels for this app
	uint32 m_unMinLevel;
	uint32 m_unMaxLevel;

	// Total value of all the weights of the qualities
	uint32 m_unSumQualityWeights;

	// Name-to-implementation list of all unique attribute types (ie., "wide strange score").
	CUtlVector< attr_type_t > m_vecAttributeTypes;

	// Contains the list of rarity definitions
	CUtlMap< int, CEconItemRarityDefinition, int > m_mapRarities;

	// Contains the list of quality definitions read in from all data files.
	CUtlMap< int, CEconItemQualityDefinition, int > m_mapQualities;

	// Contains the list of item definitions read in from all data files.
	// This is what IEconItemSchema::GetItemDefinitionInterface looks in, and it looks by KEY:
	// the key is the definition index, and GetItemDefinitionCount answers this map's element
	// count. Walking 0..count therefore visits definition indices 0..count and misses every
	// definition above it -- the agents, the gloves and the pins are all up in the thousands.
	// Enumerate this map instead.
	ItemDefinitionMap_t m_mapItems;

	// A sorted version of the same map, for instances where we really want sorted data
	CUtlOrderedMap< int, CEconItemDefinition * > m_mapItemsSorted;

	// What is the default item definition we'll return to the client code if we can't find the correct one?
	CEconItemDefinition *m_pDefaultItemDefinition;

	// Contains the list of attribute definitions read in from all data files.
	EconAttrDefsContainer_t m_mapAttributesContainer;

	// Contains the list of item recipes read in from all data files.
	RecipeDefinitionMap_t m_mapRecipes;

	// Contains the list of item sets.
	// The definition is stored BY VALUE. Ordered by string content, not by key pointer - CS:GO
	// used DefLessFunc(const char*) here and looked sets up with a linear Q_strcmp scan, CS2
	// sorts with strcmp instead.
	CUtlOrderedMap< const char *, CEconItemSetDefinition, CDefStringLess, int > m_mapItemSets;

	// Revolving loot lists.
	CUtlMap< int, const char *, int > m_mapRevolvingLootLists;

	// Quest reward loot lists.
	CUtlMap< int, const char *, int > m_mapQuestRewardLootLists;

	// Contains the list of loot lists.
	LootListDefinitionDict_t m_dictLootLists;

	// List of events that award items based on time played
	CUtlVector< CTimedItemRewardDefinition > m_vecTimedRewards;

	// List of web resources.
	CUtlDict< CWebResource * > m_dictWebResources;

	// List of alternate icons.
	AlternateIconsMap_t m_mapAlternateIcons;

	// CS:GO's achievement reward dict and map sat here; CS2 has neither.

	// Contains the map of paint kits.
	// GetPaintKitDefinitionByMapIndex() calls Find() on this, so the comparator has to be a
	// stateless functor - the game never writes the tree's leading less-func slot.
	CUtlOrderedMap< int, CPaintKit *, CDefLess< int >, int > m_mapPaintKits;

	// Contains the map of sticker kits
	CUtlMap< int, CStickerKit *, int > m_mapStickerKits;

	// CS2 only. Rarity 6 kits that name both an event and a team, grouped by (event, team), so
	// the parser can reject "a duplicate gold sticker for event %d team %d player %d with a
	// decreasing ID". The tree compares the event first, then the team; each value is a heap
	// CUtlVector the parser news on first use.
	CUtlMap< StickerKitEventTeam_t, CUtlVector< CStickerKit * > *, int > m_mapGoldStickerKitsByEventTeam;

	// Keyed by a strdup of the kit name, compared with V_stricmp_fast.
	CUtlDict< CStickerKit * > m_dictStickerKits;

	// CS2 only.
	bool m_bHaveNonTradeableStickers;

	// Contains the map of sticker lists
	CUtlDict< CStickerList * > m_dictStickerLists;

	// Contains information for attribute attached particle systems
	CUtlMap< int, attachedparticlesystem_t, int > m_mapAttributeControlledParticleSystems;

	// Contains information on which equip regions conflict with each other regions and how to
	// test for overlap.
	EquipRegionsList_t m_vecEquipRegionsList;

	// Contains information about prefab KeyValues blocks that be can reference elsewhere
	// in the schema.
	CUtlMap< const char *, KeyValues *, int > m_mapDefinitionPrefabs;

	// Contains runtime color information, looked-up by name.
	ColorDefinitionsList_t m_vecColorDefs;
	CUtlVector< CEconGraffitiTintDefinition * > m_vecGraffitiTintDefs;

	CUtlStringMap< CEconGraffitiTintDefinition * > m_mapGraffitiTintByName;
	int m_nMaxValidGraffitiTintDefID;

	// CS2 only: "keychain_definitions", keyed by id.
	CUtlMap< int, CKeyChain *, int > m_mapKeychains;

	// Contains runtime music definitions for music kits.
	MusicDefinitionsList_t m_mapMusicDefs;

	// CS2 only: "highlight_reels", by id, by name and by the match they belong to.
	CUtlMap< int, CEconHighlightReelDefinition *, int > m_mapHighlightReels;
	CUtlDict< CEconHighlightReelDefinition * > m_dictHighlightReels;
	CUtlMap< HighlightReelMatch_t, CUtlVector< CEconHighlightReelDefinition * > *, int > m_mapHighlightReelsByMatch;

	// CS2 only: "pet_definitions", keyed by id.
	CUtlMap< int, CEconPetDefinition *, int > m_mapPetDefs;

	// A loot list attribute that names a quest id is checked against this one.
	QuestDefinitionsList_t m_mapQuestDefs;

	CampaignDefinitionsList_t m_mapCampaignDefs;

	SeasonalOperations_t m_mapSeasonalOperations;

	QuestEventsSchedule_t m_mapQuestEventsSchedule;

	SkirMishModesMap_t m_mapSkirMishModes;

	MapProPlayersByAccountID_t m_mapProPlayersByAccountID;

	MapProPlayersByName_t m_mapProPlayersByCode;

	MapProPlayersByEventIDTeamID_t m_mapProPlayersByEventIDTeamID;

	// Contains the list of sound material definitions
	CUtlMap< int, CEconSoundMaterialDefinition, int > m_mapSoundMaterials;

	// Contains information about: a) every bodygroup that appears anywhere in the schema, and
	// b) whether they default to on or off.
	BodygroupStateMap_t m_mapDefaultBodygroupState;

	// Various definitions can have any number of unique tags associated with them.
	EconTagDict_t m_dictTags;

	// List of item leveling data.
	KillEaterScoreMap_t m_mapKillEaterScoreTypes;

	LevelBlockDict_t m_vecItemLevelingData;

	CUtlVector< CPipRankData > m_vecSkirMishRankData;

	// Contains Armory data key->localization string mappings
	ArmoryStringDict_t m_dictArmoryItemTypesDataStrings;
	ArmoryStringDict_t m_dictArmoryItemClassesDataStrings;
	ArmoryStringDict_t m_dictArmoryAttributeDataStrings;
	ArmoryStringDict_t m_dictArmoryItemDataStrings;

	IDelayedSchemaData *m_pDelayedSchemaData;

	CUtlVector< CEconItemDefinition * > m_vecBundles;

	// Set around each item parse of BInitSchema.
	bool m_bSchemaParsingItems;
};
COMPILE_TIME_ASSERT( sizeof( CEconItemSchema ) == 0x898 );

class CStickerKit
{
public:
	int nID;
	int nRarity;
	CUtlString sName;
	CUtlString sDescriptionString;
	CUtlString sItemName;
	CUtlString sMaterialPath;
	CUtlString sMaterialPathNoDrips;
	CUtlString m_strInventoryImage;
	int m_nEventID;
	int m_nEventTeamID;
	int m_nPlayerID;
	bool bMaterialPathIsAbsolute;
	bool bCannotTrade;
	float flRotateStart;
	float flRotateEnd;
	float flScaleMin;
	float flScaleMax;
	float flWearMin;
	float flWearMax;
	// CS:GO keeps a small and a large icon URL here; CS2 has four strings, and neither the
	// server nor the client writes them.
	CUtlString m_sIconURLs[ 4 ];
	KeyValues *m_pKVItem;
};
COMPILE_TIME_ASSERT( sizeof( CStickerKit ) == 0x88 );

class CStickerList
{
public:
	struct sticker_list_entry_t
	{
		const CStickerList *pList;
		const CStickerKit *pKit;
		float flWeight;
	};
	CUtlVector< sticker_list_entry_t > arrElements;
	float flTotalWeight;

	float flWearMin;
	float flWearMax;
};

// Each field is the keyvalue of a "keychain_definitions" entry the definition parser fills it from.
class CKeyChain
{
public:
	int32 m_nID;                           // the block's own name, atoi'd
	int32 m_nBaseID;                       // "base": the definition this one inherits
	int32 m_nHighlightReelID;              // "highlight_reel", through the schema's dict of reels
	int32 m_nRarity;                       // "item_rarity", default common
	int32 m_nQuality;                      // "item_quality", default normal
	bool m_bIsCommodity;                   // "is commodity"
	CUtlConstString m_strName;             // "name"
	CUtlConstString m_strLocName;          // "loc_name"
	CUtlConstString m_strLocDescription;   // "loc_description"
	CUtlConstString m_strPedestalModel;    // "pedestal_display_model"
	CUtlConstString m_strInventoryImage;   // "image_inventory"
	CUtlConstString m_strKeychainMaterial; // "keychain_material"

	// Same as CEconMusicDefinition::m_sIconURLs.
	CUtlConstString m_sIconURLs[ 8 ];

	KeyValues *m_pKVItem; // the merged copy of the block
};
COMPILE_TIME_ASSERT( sizeof( CKeyChain ) == 0x90 );

class CPaintKit
{
public:
	enum
	{
		NUM_COLORS = 4
	};

	int nID;
	CUtlString sName;
	CUtlString sDescriptionString;
	CUtlString sDescriptionTag;
	CUtlString sSameNameFamilyAggregate;
	CUtlString sPattern;
	CUtlString sNormal;
	CUtlString sLogoMaterial;
	bool bBaseDiffuseOverride;
	int nRarity;
	int nStyle;
	Color rgbaColor[ NUM_COLORS ];
	Color rgbaLogoColor[ NUM_COLORS ];
	float flWearRemapMin;
	float flWearRemapMax;
	unsigned char nFixedSeed;
	unsigned char uchPhongExponent;
	unsigned char uchPhongAlbedoBoost;
	unsigned char uchPhongIntensity;
	float flPatternScale;
	float flPatternOffsetXStart;
	float flPatternOffsetXEnd;
	float flPatternOffsetYStart;
	float flPatternOffsetYEnd;
	float flPatternRotateStart;
	float flPatternRotateEnd;
	float flLogoScale;
	float flLogoOffsetX;
	float flLogoOffsetY;
	float flLogoRotation;
	bool bIgnoreWeaponSizeScale;
	int nViewModelExponentOverrideSize;
	bool bOnlyFirstMaterial;
	bool bUseNormal;
	bool bUseLegacyModel;
	float flPearlescent;
	CUtlString sVmtPath;
	KeyValues *kvVmtOverrides;
	CUtlString sCompositeMaterialPath;
};
COMPILE_TIME_ASSERT( sizeof( CPaintKit ) == 0xD0 );

class AlternateIconData_t
{
public:
	const char *GetInventoryImage() const { return sLargeSimpleName.Get(); }

public:
	CUtlString sSimpleName;
	CUtlString sLargeSimpleName;

	// CS:GO keeps a small and a large icon URL here; CS2 has four strings, and neither the
	// server nor the client writes them.
	CUtlString m_sIconURLs[ 4 ];
};

class CProPlayerData
{
public:
	KeyValues *m_pKVItem;
	uint32 m_nAccountID;
	CUtlString m_sName;
	CUtlString m_sCode;
	RTime32 m_rtDOB;
	CUtlString m_sGeo;
};
COMPILE_TIME_ASSERT( sizeof( CProPlayerData ) == 0x30 );

inline uint64 Helper_GetAlternateIconKeyForWeaponPaintWearItem( item_definition_index_t nDefIdx, uint32 nPaintId, uint32 nWear )
{
	return ( nDefIdx << 16 ) + ( nPaintId << 2 ) + nWear;
}

inline CPaintKit *CEconItemSchema::GetPaintKitDefinitionByName( const char *pchName ) const
{
	FOR_EACH_MAP_FAST( m_mapPaintKits, i )
	{
		CPaintKit *pPaintKit = m_mapPaintKits[ i ];
		if ( strcmp( pPaintKit->sName.Get(), pchName ) == 0 )
		{
			return pPaintKit;
		}
	}

	return NULL;
}

inline CPaintKit *CEconItemSchema::GetPaintKitDefinitionByMapIndex( int iMapIndex ) const
{
	int iIndex = m_mapPaintKits.Find( iMapIndex );
	if ( iIndex != m_mapPaintKits.InvalidIndex() )
	{
		return m_mapPaintKits[ iIndex ];
	}

	return NULL;
}

inline AlternateIconData_t *CEconItemSchema::GetAlternateIcon( uint64 ullAlternateIcon )
{
	int iIndex = m_mapAlternateIcons.Find( ullAlternateIcon );
	if ( iIndex != m_mapAlternateIcons.InvalidIndex() )
	{
		return m_mapAlternateIcons[ iIndex ];
	}

	return NULL;
}

inline CEconItemDefinition *CEconItemSchema::GetItemDefinitionMutable( int iItemIndex, bool bNoDefault )
{
	int iIndex = m_mapItems.Find( iItemIndex );
	if ( iIndex != m_mapItems.InvalidIndex() )
	{
		return m_mapItems[ iIndex ];
	}

	if ( bNoDefault )
	{
		return NULL;
	}

	return m_pDefaultItemDefinition;
}

#endif // ECONITEMSCHEMA_H
