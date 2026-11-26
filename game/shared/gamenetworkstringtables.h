//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The network string tables of the game module.
//
//=============================================================================//

#ifndef GAMENETWORKSTRINGTABLES_H
#define GAMENETWORKSTRINGTABLES_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"

//-----------------------------------------------------------------------------
// Creates and finds the network string tables of the game module. No virtual destructor.
//-----------------------------------------------------------------------------
abstract_class IGameNetworkStringTables
{
public:
	virtual void InitStringTableDefinitions() = 0;
	virtual void ClearStringTablePointers() = 0;
	virtual void SV_CreateNetworkStringTables() = 0;
	virtual void CL_InstallStringTableCallback( const char *pszTableName ) = 0;
};

#endif // GAMENETWORKSTRINGTABLES_H
