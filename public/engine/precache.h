//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#ifndef PRECACHE_H
#define PRECACHE_H
#ifdef _WIN32
#pragma once
#endif

class CSfxTable;
struct model_t;

class CPrecacheItem
{
private:
	union precacheptr
	{
		model_t		*model;
		char const	*generic;
		CSfxTable	*sound;
		char const	*name;
	} u;

	// Reset to 0 when the pointer is set
	unsigned int	m_uiRefcount;
};

#endif // PRECACHE_H
