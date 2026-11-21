//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The entity debug game system: entity overlays, the "entityreport" /
//          "entitysummary" commands and the debug snapshot history.
//
//=============================================================================//

#ifndef ENTITYDEBUGGAMESYSTEM_H
#define ENTITYDEBUGGAMESYSTEM_H
#ifdef _WIN32
#pragma once
#endif

#include "igamesystem.h"
#include "entityhandle.h"

#include <mathlib/vector.h>
#include <tier0/circularbuffer.h>
#include <tier0/convar.h>
#include <tier0/utlhashtable.h>
#include <tier0/utlleanvector.h>
#include <tier0/utlmap.h>
#include <tier0/utlsignalslot.h>
#include <tier0/utlstring.h>
#include <tier0/utlvector.h>

struct OverlayLine_t;
// The element type of DebugSnapshotNode_t::m_Values (freed without destructing elements) is not
// identified yet
struct DebugSnapshotValue_t;

//-----------------------------------------------------------------------------
// One node of a debug snapshot, 304 bytes, 16-byte aligned (the constructor runs on the node of
// every frame; the destructor recurses into m_Children). No source names the
// members; they are named by their types and the values the constructor gives them.
//-----------------------------------------------------------------------------
struct DebugSnapshotNode_t
{
	// A 48-byte member whose type is not identified: the constructor clears its first
	// 16 bytes and the flag at 40
	struct UnidentifiedMember_t
	{
		uint64 m_nClearedOnConstruction[ 2 ];
		uint64 m_nNotConstructed[ 3 ];
		bool m_bClearedOnConstruction;
	};

	CUtlString m_sName;
	uint64 m_nId;
	int m_nFlags;
	Vector m_vecOrigin;
	// NaN until set
	QAngle m_angAngles;
	// 1 after construction
	float m_flScale;
	// NaN until set
	QuaternionAligned m_qOrientation;
	UnidentifiedMember_t m_Unidentified;
	CUtlLeanVectorFixedGrowable< Vector, 8 > m_Points;
	int m_nIndex1;
	CUtlVector< DebugSnapshotValue_t > m_Values;
	uint64 m_nClearedOnConstruction;
	int m_nIndex2;
	CUtlString m_sText;
	int m_nIndex3;
	uint64 m_nClearedOnConstruction2;
	// The children, a CUtlLeanVector< DebugSnapshotNode_t > written out member by member:
	// CUtlLeanVectorBase::MIN_ALLOCATED needs a complete element type under MSVC, which a node
	// cannot be inside its own declaration
	struct ChildList_t
	{
		int m_nCount;
		int m_nAllocated;
		DebugSnapshotNode_t *m_pElements;
	};
	ChildList_t m_Children;
};

//-----------------------------------------------------------------------------
// One frame of the debug snapshot history, 336 bytes, 16-byte aligned (RTTI names CDebugSnapshotFrame_t
// and its nested MapStructedDataByEntity_t)
//-----------------------------------------------------------------------------
struct CDebugSnapshotFrame_t
{
	// Polymorphic and reference counted (48 bytes: vtable, count at 8, a hashtable of 24-byte entries at
	// 16); the frame holds one reference
	class MapStructedDataByEntity_t;

	uint64 m_nFrame;
	DebugSnapshotNode_t m_Root;
	MapStructedDataByEntity_t *m_pStructedDataByEntity;
};

//-----------------------------------------------------------------------------
// The snapshot history of one channel, 672384 bytes (RTTI: CDebugSnapshotCache derives from
// CFixedSizeCircularBuffer< CDebugSnapshotFrame_t, 2000, int >). Its own members are named by what
// the constructor gives them.
//-----------------------------------------------------------------------------
class CDebugSnapshotCache : public CFixedSizeCircularBuffer< CDebugSnapshotFrame_t, 2000, int >
{
public:
	virtual void ElementAlloc( CDebugSnapshotFrame_t &element ) override;
	virtual void ElementRelease( CDebugSnapshotFrame_t &element ) override;

public:
	// The frame being recorded
	CDebugSnapshotFrame_t m_CurrentFrame;
	// true after construction
	bool m_bEnabled;
	int m_nClearedOnConstruction;
	// -1 after construction
	int m_nSelectedFrame;
};

#define ENTITYDEBUG_SNAPSHOT_CACHE_COUNT 8

//-----------------------------------------------------------------------------
// Reallocating factory, 5,379,584 bytes on both platforms.
// It adds no virtual methods to IGameSystem.
//-----------------------------------------------------------------------------
class CEntityDebugGameSystem : public CAutoGameSystem, public CUtlSlot
{
public:
	DECLARE_GAME_SYSTEM();

	// 24 bytes; the two members after m_nDebugOverlays are not identified
	struct EntityDebugInfo_t
	{
		uint64 m_nDebugOverlays;
		uint64 m_nNotIdentified[ 2 ];
	};

	struct DebugScrollingMessage_t;

	CEntityDebugGameSystem();
	virtual ~CEntityDebugGameSystem();

	GS_EVENT( GameInit );
	GS_EVENT( BuildGameSessionManifest );
	GS_EVENT( GameActivate );
	GS_EVENT( GameDeactivate );
	GS_EVENT( ClientPreRender );
	GS_EVENT( ServerPreEntityThink );
	GS_EVENT( ServerPostSimulate );
	GS_EVENT( ClientPostSimulate );

	// Forward to the game system factory
	virtual void SetGameSystemGlobalPtrs( void *pValue ) override;
	virtual bool DoesGameSystemReallocate() override;


public:
	int m_nDebugOverlayIndex;
	OverlayLine_t *m_debugOverlayLine[ 20 ];
	CON_COMMAND_MEMBER_F( CEntityDebugGameSystem, "entityreport", OnEntityReport, "Reports all extant entities. Optional 2nd arg is a substring of a classname that the list will be filtered by.", 0 );
	CON_COMMAND_MEMBER_F( CEntityDebugGameSystem, "entitysummary", OnEntitySummary, "Summarizes (by class) all extant entities. Optional 2nd arg is a substring of a classname that the list will be filtered by.", 0 );
	CUtlHashtable< uint32, EntityDebugInfo_t > m_simDebugEntities;
	CUtlHashtable< uint32, EntityDebugInfo_t > m_renderDebugEntities;
	CUtlMap< int, CUtlVector< DebugScrollingMessage_t > *, int > m_ScrollingMessageLists;
	// Named by what they hold
	CDebugSnapshotCache m_SnapshotCaches[ ENTITYDEBUG_SNAPSHOT_CACHE_COUNT ];
	CUtlVector< CUtlString > m_StickyEntTextNames;
	bool m_bInDebugSelect;
	// Cleared by the constructor
	bool m_bClearedOnConstruction;
	// Named by what the constructor does: set from two engine queries (dedicated / text mode)
	bool m_bNoGraphics;
	CEntityIndex m_nDebugPlayer;
	// 1 after construction
	int m_nOneOnConstruction;
};

#endif // ENTITYDEBUGGAMESYSTEM_H
