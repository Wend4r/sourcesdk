//========= Copyright � 1996-2005, Valve Corporation, All rights reserved. ============//
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
	enum
	{
		TYPE_UNK = 0,
		TYPE_MODEL,
		TYPE_SOUND,
		TYPE_GENERIC,
		TYPE_DECAL
	};

	unsigned int	m_nType : 3;
	unsigned int	m_uiRefcount : 29;
	union precacheptr
	{
		model_t		*model;
		char const	*generic;
		CSfxTable	*sound;
		char const	*name;
	} u;
};

#endif // PRECACHE_H
