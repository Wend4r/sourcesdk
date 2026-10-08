//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: Host-owned utilities shared by the client and the server
//
//=============================================================================//

#ifndef IHOSTUTILS_H
#define IHOSTUTILS_H
#ifdef _WIN32
#pragma once
#endif

#include <appframework/iappsystem.h>

struct footstep_sounds_t
{
	uint32 m_nWalkLeft;
	uint32 m_nWalkRight;
	uint32 m_nRunLeft;
	uint32 m_nRunRight;
	uint32 m_nFootstepModifier;
};
COMPILE_TIME_ASSERT( sizeof( footstep_sounds_t ) == 0x14 );

abstract_class IFootstepSounds
{
public:
	virtual bool GetFootstepSounds( footstep_sounds_t *pSounds, int nSurfacePropIndex ) = 0;

	// Returns the entry name, or an empty string
	virtual const char *GetName() = 0;
};

abstract_class IHostUtils : public IAppSystem
{
public:
	virtual void *GetSymbolTable() = 0;

	virtual void LoadFootstepSounds( void *pSurfacePropertyManager ) = 0;

	virtual IFootstepSounds *GetFootstepSounds( const char *pszName ) = 0;
};

#endif // IHOSTUTILS_H
