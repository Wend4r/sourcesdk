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

#endif // MARKUP_VOLUME_H
