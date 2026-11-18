#ifndef GAMETYPES_H
#define GAMETYPES_H

#ifdef _WIN32
#pragma once
#endif

#include "igametypes.h"

#include "appframework/gameappsystem.h"
#include "tier0/threadtools.h"
#include "tier0/utlstring.h"
#include "tier0/utlstringlist.h"
#include "tier0/random.h"
#include "tier0/utlvector.h"

class KeyValues;

#define GAMETYPES_MAX_NAME				64
#define GAMETYPES_MAX_ID				64
#define GAMETYPES_MAX_MAP_NAME			256
#define GAMETYPES_MAX_MAP_GROUP_NAME	32
#define GAMETYPES_MAX_IMAGE_NAME		64
#define GAMETYPES_MAX_SCRIPT_NAME		128
#define GAMETYPES_MAX_ATTR_NAME			260

enum CS_GameType
{
	CS_GameType_Min = 0,
	CS_GameType_Classic = CS_GameType_Min,
	CS_GameType_GunGame,
	CS_GameType_Training,
	CS_GameType_Custom,
	CS_GameType_Cooperative,
	CS_GameType_Skirmish,
	CS_GameType_FreeForAll,
	CS_GameType_Max = CS_GameType_FreeForAll
};

class GameTypes : public CGameAppSystem<IGameTypes>
{
public:
	struct GameType;
	struct GameMode;
	struct Map;
	struct MapGroup;
	struct CustomBotDifficulty;

	struct GameType
	{
		int m_Index;
		char m_Name[GAMETYPES_MAX_NAME];
		char m_NameID[GAMETYPES_MAX_ID];
		CUtlVector< GameMode * > m_GameModes;
	};

	struct GameMode
	{
		int m_Index;
		char m_Name[GAMETYPES_MAX_NAME];
		char m_NameID[GAMETYPES_MAX_ID];
		char m_NameID_SP[GAMETYPES_MAX_ID];
		char m_DescID[GAMETYPES_MAX_ID];
		char m_DescID_SP[GAMETYPES_MAX_ID];
		int m_MaxPlayers;
		char m_ScriptName[GAMETYPES_MAX_SCRIPT_NAME];
		KeyValues *m_pExecConfings;
		CUtlStringList m_MapGroupsSP;
		CUtlStringList m_MapGroupsMP;
		CUtlVector< WeaponProgression > m_WeaponProgressionCT;
		CUtlVector< WeaponProgression > m_WeaponProgressionT;
		int m_NoResetVoteThresholdCT;
		int m_NoResetVoteThresholdT;
	};

	struct Map
	{
		int m_Index;
		char m_Name[GAMETYPES_MAX_MAP_NAME];
		char m_NameID[GAMETYPES_MAX_ID];
		char m_ImageName[GAMETYPES_MAX_IMAGE_NAME];
		uint32 m_RichPresence;
		CUtlStringList m_TModels;
		CUtlStringList m_CTModels;
		CUtlString m_TViewModelArms;
		CUtlString m_CTViewModelArms;
		int m_nDefaultGameType;
		int m_nDefaultGameMode;
		char m_RequiresAttr[GAMETYPES_MAX_ATTR_NAME];
		int m_RequiresAttrValue;
		char m_RequiresAttrReward[GAMETYPES_MAX_ATTR_NAME];
		int m_nRewardDropList;
		CUtlString m_Addon;
	};

	struct MapGroup
	{
		char m_Name[GAMETYPES_MAX_MAP_GROUP_NAME];
		char m_NameID[GAMETYPES_MAX_ID];
		char m_ImageName[GAMETYPES_MAX_IMAGE_NAME];
		CUtlStringList m_Maps;
		CUtlStringList m_MapNames;
		bool m_bIsWorkshopMapGroup;
		CUtlString m_Addon;
	};

	struct CustomBotDifficulty
	{
		int m_Index;
		char m_Name[GAMETYPES_MAX_NAME];
		char m_NameID[GAMETYPES_MAX_ID];
		KeyValues *m_pConvars;
		bool m_HasBotQuota;
	};

public:
	CUniformRandomStream m_randomStream;

	bool m_Initialized; // true if the game types interface has been initialized
	CUtlVector< GameType* > m_GameTypes; // list of game types
	CUtlVector< Map* > m_Maps; // list of maps
	CUtlVector< MapGroup* > m_MapGroups; // list of map groups for cycling maps
	CUtlVector< CustomBotDifficulty* > m_CustomBotDifficulties; // list of custom bot difficulty levels for Offline Games

	// These are filled out on the client when connecting to a server
	KeyValues *m_pExtendedServerInfo;
	Map* m_pServerMap; // map on the currently connected server
	MapGroup* m_pServerMapGroup; // map group for cycling maps on the currently connected server
	int m_iCurrentServerNumSlots;

	// if this is true when the Level init happens, we'll run whatever game type and mode that the map defines in its KV file
	// if this is set to false, we know the map was executed via a method that sets the mode and type (like via the menu UI)
	bool m_bRunMapWithDefaultGametype;

	// unless we load through the main menu, we don't trust the data that the loading screen has
	// this keeps track of whehther the data the loading screen has is correct or not
	bool m_bLoadingScreenDataIsCorrect;

	MapGroup *FindMapGroup( const char *pMapGroupName )
	{
		if ( !pMapGroupName || !pMapGroupName[0] )
		{
			return NULL;
		}

		FOR_EACH_VEC( m_MapGroups, i )
		{
			MapGroup *pMapGroup = m_MapGroups[i];
			if ( pMapGroup && V_stricmp( pMapGroup->m_Name, pMapGroupName ) == 0 )
			{
				return pMapGroup;
			}
		}

		return NULL;
	}
};

COMPILE_TIME_ASSERT( sizeof( GameTypes::GameType ) == 0xA0 );
COMPILE_TIME_ASSERT( sizeof( GameTypes::GameMode ) == 0x238 );
COMPILE_TIME_ASSERT( sizeof( GameTypes::Map ) == 0x3E8 );
COMPILE_TIME_ASSERT( sizeof( GameTypes::MapGroup ) == 0xE0 );
COMPILE_TIME_ASSERT( sizeof( GameTypes::CustomBotDifficulty ) == 0x98 );

#endif
