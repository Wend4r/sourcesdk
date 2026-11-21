//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Collects the FCVAR_USERINFO convars as they are created.
//
//=============================================================================//

#ifndef USERINFOCONVARLIST_H
#define USERINFOCONVARLIST_H
#ifdef _WIN32
#pragma once
#endif

#include "igamesystem.h"

#include <tier0/icvar.h>
#include <tier0/convarref.h>
#include <tier0/utlvector.h>

//-----------------------------------------------------------------------------
// Registered as "CUserInfoConvarList" (static factory). 56 bytes on both platforms. IConVarListener
// is the primary base. No source has the class; the names are named by what the code does.
// The declaration order of the IGameSystem overrides and of the destructor is the order of the
// entries libserver.so adds to the IConVarListener table.
//-----------------------------------------------------------------------------
class CUserInfoConvarList : public IConVarListener, public CAutoGameSystem
{
public:
	DECLARE_GAME_SYSTEM();

	// Forward to the game system factory
	virtual bool DoesGameSystemReallocate() override;
	virtual void SetGameSystemGlobalPtrs( void *pValue ) override;

	// Installs this listener in ICvar (returns true) / removes it
	virtual bool Init() override;
	virtual void Shutdown() override;

	virtual ~CUserInfoConvarList();

	// IConVarListener: adds the convar to m_UserInfoConVars when it has FCVAR_USERINFO
	virtual void OnConVarCreated( ConVarRefAbstract *pNewCvar ) override;

public:
	CUtlVector< ConVarRef > m_UserInfoConVars;
};

#endif // USERINFOCONVARLIST_H
