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

#include <tier0/bitvec.h>
#include <const.h>
#include <tier0/mempool.h>
#include <tier0/threadtools.h>
#include <tier0/utllinkedlist.h>

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
	CBitVec<MAX_EDICTS>	non_transmit_entity;
	CBitVec<MAX_EDICTS>	transmit_out_of_pvs;
	CBitVec<MAX_EDICTS>	*transmit_always; // if bit is set, don't do PVS checks before sending (HLTV only)
};

class CClientFrameManager
{
public:
	virtual ~CClientFrameManager(void) = default;

public:
	int m_nUnk008;
	CAtomicMutex m_Mutex;
	CUtlLinkedList< CClientFrame *, unsigned short > m_FrameList;
	uint8 m_unk038[ 8 ];
	CUtlMemoryPoolMT< CClientFrame > m_ClientFramePool;
	int m_nNewestFrameTick;
	int m_nOldestFrameTick;
	// Shared frame store: looked up for ticks older than the oldest frame in the list, and given the frames the list releases
	void *m_pFrameCache;
};
COMPILE_TIME_ASSERT( sizeof( CClientFrameManager ) == 176 );

#endif // CLIENTFRAME_H