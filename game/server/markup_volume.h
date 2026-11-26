//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The game system that groups the tagged markup volumes. Factory
//			"MarkupManager".
//
//=============================================================================//

#ifndef MARKUP_VOLUME_H
#define MARKUP_VOLUME_H
#ifdef _WIN32
#pragma once
#endif

#include "igamesystem.h"
#include "tier0/utlhashtable.h"
#include "tier0/utlstring.h"
#include "tier0/utlvector.h"

class CMarkupVolumeTagged;

//-----------------------------------------------------------------------------
// Reallocating game system, 112 bytes on both platforms.
//-----------------------------------------------------------------------------
class CMarkupManager : public CAutoGameSystem
{
public:
	DECLARE_GAME_SYSTEM();

	// Counts the loaded spawn groups
	GS_EVENT( SpawnGroupPrecache );
	// Uncounts one; when the last is gone, deletes every group and empties the table and
	// m_AllMarkupEntities
	GS_EVENT( PostSpawnGroupUnload );

	// Forward to the game system factory
	virtual void SetGameSystemGlobalPtrs( void *pValue ) override;
	virtual bool DoesGameSystemReallocate() override;

public:
	// 40 bytes, allocated with new
	struct MarkupGroup_t
	{
		CUtlString m_GroupName;
		const CMarkupVolumeTagged *m_pGroupEntity;
		CUtlVector< CMarkupVolumeTagged * > m_Members;
	};

	// Groups by name
	typedef CUtlStableHashtable< const char *, MarkupGroup_t *, DefaultHashFunctor< const char * >, DefaultEqualFunctor< const char * >, uint16 > CMarkupGroupTable;

	CMarkupGroupTable m_MarkupGroups;
	CUtlVector< CMarkupVolumeTagged * > m_AllMarkupEntities;
	int m_NumLoadedSpawnGroups;
};

// BEGIN ENTITY VTABLES: generated, do not edit

#include "../shared/basemodelentity.h"
#include "../shared/positionwatcher.h"

class Color;

//-----------------------------------------------------------------------------
// CMarkupVolume adds 2 methods to CBaseModelEntity: Linux slots 272-273, Windows 271-272.
//-----------------------------------------------------------------------------
template < class TBase = CBaseModelEntityVTable<> >
abstract_class CMarkupVolumeVTable : public TBase
{
public:
	// Only caller is the Enable input handler; empty in the base. Named by what it does
	virtual void Enable() = 0;
	// Only caller is the Disable input handler; empty in the base. Named by what it does
	virtual void Disable() = 0;
};

//-----------------------------------------------------------------------------
// CMarkupVolumeTagged adds 3 methods to CMarkupVolume: Linux slots 274-276, Windows 273-275.
//-----------------------------------------------------------------------------
template < class TBase = CMarkupVolumeVTable<> >
abstract_class CMarkupVolumeTaggedVTable : public TBase
{
public:
	// Draws the volume and its tags ("DISABLED") as debug overlay. Named by what it does
	virtual void DrawTagsDebugOverlay( const Color &color, float flDuration ) = 0;
	// Empty; called twice by the nav markup code, overridden by the _Nav classes
	virtual void Unk_274() = 0;
	// Empty in every class
	virtual void Stub_275() = 0;
};

//-----------------------------------------------------------------------------
// CMarkupVolumeWithRef adds no method to CMarkupVolumeTagged. The Linux table has 1 more slot, 277: the override of a method of IPositionWatcher, which the
// Itanium ABI also enters into the primary table and MSVC does not.
// The second bases sit at offsets 8, 16, ... here, not where the game has them: do not convert a pointer to them.
//-----------------------------------------------------------------------------
template < class TBase = CMarkupVolumeTaggedVTable<> >
abstract_class CMarkupVolumeWithRefVTable : public TBase, public IPositionWatcher
{
public:
	// Recomputes m_vRefPosEntitySpace / m_vRefPos. Overrides IPositionWatcher: a slot of this table on Linux only
	virtual void NotifyPositionChanged( CBaseEntity *pEntity ) override = 0;
};

// END ENTITY VTABLES

#endif // MARKUP_VOLUME_H
