//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#ifndef CLIENTFRAME_H
#define CLIENTFRAME_H
#ifdef _WIN32
#pragma once
#endif

#include <bitvec.h>
#include <const.h>
#include <tier0/mempool.h>

class CFrameSnapshot;

class CClientFrame
{
public:
	virtual ~CClientFrame();

public:

	// State of entities this frame from the POV of the client.
	int					tick_count; // server tick of this snapshot
	int					last_entity; // highest entity index
	CClientFrame*		m_pNext;

	// Index of snapshot entry that stores the entities that were active and the serial numbers
	// for the frame number this packed entity corresponds to
	// m_pSnapshot MUST be private to force using SetSnapshot(), see reference counters
	CFrameSnapshot		*m_pSnapshot;

	// Used by server to indicate if the entity was in the player's pvs
	CBitVec<MAX_EDICTS>	transmit_entity; // if bit n is set, entity n will be send to client
	CBitVec<MAX_EDICTS>	unkBitVec2080;
	CBitVec<MAX_EDICTS>	unkBitVec4128;
	CBitVec<MAX_EDICTS>	unkBitVec6176;
	CBitVec<MAX_EDICTS>	*transmit_always; // if bit is set, don't do PVS checks before sending (HLTV only)
};  // sizeof 8232

class CClientFrameManager
{
public:
	virtual ~CClientFrameManager(void) = default;

public:
	/*CUtlLeanVector<CClientFrame*> m_FrameList;
	CUtlMemoryPoolMT<CClientFrame> m_ClientFramePool;
	int m_nOldestTick; // +160
	int m_nNewestTick; // +164*/
	char pad[168];
}; // sizeof 168

#endif // CLIENTFRAME_H