//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The spawn group entity filter of round restarts.
//
//=============================================================================//

#ifndef CS_MAPENTITYFILTER_H
#define CS_MAPENTITYFILTER_H

#ifdef _WIN32
#pragma once
#endif

#include "gameentitysystem.h"

//-----------------------------------------------------------------------------
// Registered as "cs_respawn" (SPAWN_GROUP_ENTITY_FILTER_FALLBACK); a static object. Passes every
// entity whose "classname" is a non-empty string and not one of the classes that survive a round
// restart (ai_network, cs_gamerules, cs_team_manager, env_sky, func_buyzone, info_player_*,
// light_environment, player, cs_player_controller, cs_bot, point_camera and others). 8 bytes.
//-----------------------------------------------------------------------------
class CCSMapEntityFilter : public ISpawnGroupEntityFilter
{
public:
	void FilterSpawnGroupEntities( SpawnGroupHandle_t hSpawnGroup, bool bIsPrecaching, const CUtlVector< const CEntityKeyValues * > *pInKeyValues, CKV3Arena *pArena, CUtlVector< const CEntityKeyValues * > &outKeyValues ) override;
};

#endif // CS_MAPENTITYFILTER_H
