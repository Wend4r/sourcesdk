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

	// Entities of the frame that are not sent; WriteNonTransmitted reads it
	CBitVec<MAX_EDICTS>	non_transmitted_entity;

	// Entities outside the PVS of the client; WriteOOPVSDeltaEntities reads it, ReadPacketEntities
	// of the client sets it
	CBitVec<MAX_EDICTS>	out_of_pvs_entity;
	CBitVec<MAX_EDICTS>	*transmit_always; // if bit is set, don't do PVS checks before sending (HLTV only)
};

class CClientFrameManager
{
public:
	virtual ~CClientFrameManager(void) = default;

public:
	// Named by what it does: neither the constructor nor a method of the manager touches it. It puts
	// the mutex at +20 on Windows (MSVC starts the data of this 16-byte aligned class at +16) and at
	// +16 on Linux
	int m_nUnused;
	CAtomicMutex m_FramesMutex;

	// The frames from the oldest (head) to the newest (tail)
	CUtlLinkedList< CClientFrame *, unsigned short > m_Frames;

	// Blocks of 6184 bytes, 128 per blob. Its CTSListBase makes the class 16-byte aligned, which puts
	// CServerSideClient::m_FrameManager at 3632 on Windows (3616 on Linux)
	CUtlMemoryPoolBase m_ClientFramePool;

	// tick_count of the tail and of the head frame of m_Frames
	int m_nNewestTick;
	int m_nOldestTick;

	// Named by what it does: when it's set, frames older than its tick go to its locked queue
	// instead of back to m_ClientFramePool
	void *m_pFrameReleaseQueue;
};

#endif // CLIENTFRAME_H