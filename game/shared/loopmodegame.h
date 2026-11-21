//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The spawn group entity filter of map loads.
//
//=============================================================================//

#ifndef LOOPMODEGAME_H
#define LOOPMODEGAME_H

#ifdef _WIN32
#pragma once
#endif

#include "gameentitysystem.h"

//-----------------------------------------------------------------------------
// Registered as "mapload" (SPAWN_GROUP_ENTITY_FILTER_FALLBACK); a static object. Passes every
// entity whose "classname" is a non-empty string. 8 bytes.
//-----------------------------------------------------------------------------
class CMapLoadEntityFilter : public ISpawnGroupEntityFilter
{
public:
	void FilterSpawnGroupEntities( SpawnGroupHandle_t hSpawnGroup, bool bIsPrecaching, const CUtlVector< const CEntityKeyValues * > *pInKeyValues, CKV3Arena *pArena, CUtlVector< const CEntityKeyValues * > &outKeyValues ) override;
};

#endif // LOOPMODEGAME_H
