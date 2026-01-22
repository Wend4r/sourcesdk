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


class PackedEntity;
class HLTVEntityData;
class ReplayEntityData;
class ServerClass;
class CEventInfo;
class CFrameSnapshot;

class CVariableBitStringBase {
public:
	int		m_numBits;
	int		m_numInts;
	int		m_iBitStringStorage;
	int		*m_pInt;
}; // sizeof 16

//-----------------------------------------------------------------------------
// Purpose: snapshot manager class
//-----------------------------------------------------------------------------
class CFrameSnapshotManager : public CRefCountServiceMT 
{
	friend class CFrameSnapshot;
public:
	virtual ~CFrameSnapshotManager() = default;

	CAtomicMutex m_FrameSnapshotsWriteMutex;
	CUtlVector<CFrameSnapshot*> m_FrameSnapshots;
	CVariableBitStringBase m_unkBitString;
}; // sizeof unknown

#endif // FRAMESNAPSHOT_H
