//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The CS2 VScript game system.
//
//=============================================================================//

#ifndef CSTRIKE15_CS_VSCRIPTGAMESYSTEM_H
#define CSTRIKE15_CS_VSCRIPTGAMESYSTEM_H
#ifdef _WIN32
#pragma once
#endif

#include "vscript_server.h"

//-----------------------------------------------------------------------------
// Registered as "CSGOVScriptGameSystem" (reallocating factory), 152 bytes on both platforms:
// CVScriptGameSystem and the DECLARE_GAME_SYSTEM byte.
//-----------------------------------------------------------------------------
class CCSGOVScriptGameSystem : public CVScriptGameSystem
{
public:
	DECLARE_GAME_SYSTEM();

	// Feeds both the abstract factory of CVScriptGameSystem and its own factory
	virtual void SetGameSystemGlobalPtrs( void *pValue ) override;
	virtual bool DoesGameSystemReallocate() override;

	virtual ~CCSGOVScriptGameSystem();

	// Returns 2 (SL_NOP in public/vscript/ivscript.h): CS2 starts no script language
	virtual ScriptLanguage_t GetScriptLanguage() override;
	// The base bindings, then the CS ones ("Initializing CSGO VScript Game System")
	virtual void InstallScriptBindings() override;
};

#endif // CSTRIKE15_CS_VSCRIPTGAMESYSTEM_H
