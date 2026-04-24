//========= Copyright (c) 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//
#if !defined( FRAMESNAPSHOT_H )
#define FRAMESNAPSHOT_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "tier1/refcount.h"

#include <mempool.h>

class CFrameSnapshot;
class PackedEntity;
class HLTVEntityData;
class ReplayEntityData;
class ServerClass;
class CEventInfo;

class CVariableBitStringBase {
public:
	int		m_numBits;
	int		m_numInts;
	int		*m_pInt;
}; // sizeof 16

class CFrameSnapshotEntry {
public:
	CFrameSnapshot* m_pClass;
	int m_nSerialNumber;
};
static_assert(sizeof(CFrameSnapshotEntry) == 16);

//-----------------------------------------------------------------------------
// Purpose: snapshot manager class
//-----------------------------------------------------------------------------
class CFrameSnapshotManager : public CRefCountServiceMT 
{
	friend class CFrameSnapshot;
public:
	virtual ~CFrameSnapshotManager() = default;

	CAtomicMutex m_FrameSnapshotsWriteMutex;
	CUtlLeanVector<CFrameSnapshot*> m_FrameSnapshots;
	CVariableBitStringBase m_unkBitString;
	CUtlMemoryPoolBase m_FrameSnapshotPool;
	CFrameSnapshotEntry m_EntitySnapshots[MAX_EDICTS];
	void* m_pPendingAsyncJob;
}; // sizeof 262320

#endif // FRAMESNAPSHOT_H
