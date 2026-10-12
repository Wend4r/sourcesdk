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
#include "tier0/refcount.h"
#include "tier0/utllinkedlist.h"

#include "const.h"

#include <mempool.h>

class CFrameSnapshot;
class PackedEntity;
class HLTVEntityData;
class ReplayEntityData;
class ServerClass;
class CEventInfo;
class CJob;

class CFrameSnapshotEntry {
public:
	PackedEntity* m_pPackedData;
	int m_nSerialNumber;
};
static_assert(sizeof(CFrameSnapshotEntry) == 16);

//-----------------------------------------------------------------------------
// Purpose: snapshot manager class
//-----------------------------------------------------------------------------
class CFrameSnapshotManager : public CRefCounted<CRefCountServiceMT>
{
	friend class CFrameSnapshot;
public:
	virtual ~CFrameSnapshotManager() = default;

	CAtomicMutex m_FrameSnapshotsWriteMutex;
	CUtlLinkedList<CFrameSnapshot*> m_FrameSnapshots;
	CUtlMemoryPoolBase m_PackedEntitiesPool;
	CFrameSnapshotEntry m_EntitySnapshots[MAX_EDICTS];
	CJob* m_pPendingAsyncJob;
}; // sizeof 262320

#endif // FRAMESNAPSHOT_H
