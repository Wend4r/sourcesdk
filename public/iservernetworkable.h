//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#ifndef ISERVERNETWORKABLE_H
#define ISERVERNETWORKABLE_H
#ifdef _WIN32
#pragma once
#endif


#include "entity2/entityinstance.h"
#include "basetypes.h"
#include "tier0/bitvec.h"
#include "const.h"
#include "playerslot.h"
#include "utlleanvector.h"


class CBaseEntity;

struct vis_info_t
{
	uint32 m_uVisBitsBufSize;
	SpawnGroupHandle_t m_SpawnGroupHandle;
	CBitVec<4096> m_VisBits;
};
COMPILE_TIME_ASSERT(sizeof(vis_info_t) == 520);

class CCheckTransmitInfo
{
public:
	CBitVec<MAX_EDICTS>* m_pTransmitEntity; // entities visible/sent to client
	CBitVec<MAX_EDICTS>* m_pNonTransmitEntity; // entity n exists but isn't transmitted; filled from m_pTransmitEntity after the checks, and the client is sent its changes
	CBitVec<MAX_EDICTS>* m_pTransmitOutOfPVS; // entities that left PVS but still need delta update
	CBitVec<MAX_EDICTS>* m_pTransmitAlways; // entity n is always sent even if not in PVS (HLTV and Replay only)
	CUtlVector<SpawnGroupHandle_t> m_LoadedSpawnGroups; // sorted; entities from spawn groups the client hasn't loaded aren't checked
	vis_info_t m_VisInfo; // filled by ISource2GameClients::ClientSetupVisibility
	CPlayerSlot m_nPlayerSlot;
	bool m_bFullUpdate = false; // the client gets a full update instead of a delta
};
COMPILE_TIME_ASSERT(sizeof(CCheckTransmitInfo) == 584);


#endif // ISERVERNETWORKABLE_H
