//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Loads the soundscape scripts and picks the soundscape of every player.
//
//=============================================================================//

#ifndef SOUNDSCAPE_SYSTEM_H
#define SOUNDSCAPE_SYSTEM_H
#ifdef _WIN32
#pragma once
#endif

#include "igamesystem.h"
#include "spawngrouptypes.h"

#include <tier0/utlmap.h>
#include <tier0/utldict.h>
#include <tier0/utlstring.h>
#include <tier0/utlvector.h>

class CEnvSoundscape;

struct clusterSoundscapeList_t
{
	unsigned short soundscapeCount;
	unsigned short firstSoundscape;
};

//-----------------------------------------------------------------------------
// Registered as "SoundscapeSystem" (reallocating factory). 224 bytes on both platforms.
//-----------------------------------------------------------------------------
class CSoundscapeSystem : public CAutoGameSystem
{
public:
	DECLARE_GAME_SYSTEM();

	// The soundscapes of one set of scripts (the global ones or those of a spawn group)
	struct SoundscapeInfo_t
	{
		// The dictionary is inline: the
		// soundscape names (the strings are duplicated and freed with it) and their index
		CUtlDict< int, int > m_soundscapes;
		// The sound names of each soundscape
		CUtlVector< CUtlVector< CUtlString > > m_sounds;
	};

	virtual ~CSoundscapeSystem();

	GS_EVENT( GameInit );
	GS_EVENT( GameShutdown );
	GS_EVENT( PreSpawnGroupLoad );
	GS_EVENT( PostSpawnGroupUnload );
	GS_EVENT( ActiveSpawnGroupChanged );
	GS_EVENT( ServerPreEntityThink );

	// Forward to the game system factory
	virtual void SetGameSystemGlobalPtrs( void *pValue ) override;
	virtual bool DoesGameSystemReallocate() override;

public:
	SoundscapeInfo_t m_globalSoundscapes;
	CUtlMap< SpawnGroupHandle_t, SoundscapeInfo_t *, unsigned short, bool ( * )( const SpawnGroupHandle_t &, const SpawnGroupHandle_t & ) > m_spawnGroupSoundscapes;
	SoundscapeInfo_t *m_pActiveSpawnGroupSoundscapes;
	int m_nGlobalSoundscapeCount;
	int m_nSpawnGroupSoundscapeCount;
	CUtlVector< CEnvSoundscape * > m_soundscapeEntities;
	CUtlVector< clusterSoundscapeList_t > m_soundscapesInCluster;
	CUtlVector< unsigned short > m_soundscapeIndexList;
	SpawnGroupHandle_t m_nPVSClusterSpawnGroup;
	int m_activeIndex;
	bool m_bCacheDirty;
};

#endif // SOUNDSCAPE_SYSTEM_H
