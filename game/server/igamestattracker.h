//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Interface of the game stat trackers that submit Steamworks stats.
//
//=============================================================================//

#ifndef IGAMESTATTRACKER_H
#define IGAMESTATTRACKER_H
#ifdef _WIN32
#pragma once
#endif

#include <tier0/platform.h>
#include <tier0/utlvector.h>

class KeyValues;

//-----------------------------------------------------------------------------
// Two methods, no destructor in the table
//-----------------------------------------------------------------------------
abstract_class IGameStatTracker
{
public:
	// A container of one kind of tracked stat
	abstract_class IStatContainer
	{
	public:
		virtual void SendData( KeyValues *pKV ) = 0;
		virtual void Clear() = 0;
		virtual void PrintMemoryUsage() = 0;
	};

	typedef CUtlVector< IStatContainer * > StatContainerList_t;

	virtual void SubmitGameStats( KeyValues *pKV ) = 0;
	virtual StatContainerList_t *GetStatContainerList() = 0;
};

#endif // IGAMESTATTRACKER_H
