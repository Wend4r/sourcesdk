//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: Engine service that lists the maps available in the GAME search path
//
//=============================================================================//

#ifndef IMAPLISTSERVICE_H
#define IMAPLISTSERVICE_H
#ifdef _WIN32
#pragma once
#endif

#include <IEngineService.h>
#include <tier0/utlstring.h>
#include <tier1/utlvector.h>

class KeyValues;

abstract_class IMapListService : public IEngineService
{
public:
	virtual void GetMapList( const char *pszFilter, CUtlVector< CUtlString > &maps ) = 0;

	virtual KeyValues *ParseMapOptions( const char *pszOptions ) = 0;

	virtual bool IsMapValid( const char *pszMapName ) = 0;
};

#endif // IMAPLISTSERVICE_H
