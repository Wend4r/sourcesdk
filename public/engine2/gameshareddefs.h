#ifndef ENGINE2_GAMESHAREDDEFS_H
#define ENGINE2_GAMESHAREDDEFS_H

#ifdef _WIN32
#pragma once
#endif

#include "globalvars_base.h"
#include "iservernetworkable.h"

//-----------------------------------------------------------------------------
// Purpose: Defines the ways that a map can be loaded.
//-----------------------------------------------------------------------------
enum MapLoadType_t
{
	MapLoad_NewGame = 0,
	MapLoad_LoadGame,
	MapLoad_Transition,
	MapLoad_Background,
};

//-----------------------------------------------------------------------------
// Purpose: Global variables shared between the engine and the game .dll
//-----------------------------------------------------------------------------
class CGlobalVars : public CGlobalVarsBase
{	
public:

	CGlobalVars();

public:
	// Current map
	string_t		mapname;
	string_t		startspot;
	MapLoadType_t	eLoadType;		// How the current map was loaded
	bool mp_teamplay;

	// current maxentities
	int				maxEntities;

	int				serverCount;
};

#endif // ENGINE2_GAMESHAREDDEFS_H
