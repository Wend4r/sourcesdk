//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The game system that loads and keeps the response rule systems.
//
//=============================================================================//

#ifndef AI_RESPONSESYSTEM_H
#define AI_RESPONSESYSTEM_H
#ifdef _WIN32
#pragma once
#endif

#include "igamesystem.h"

#include <tier0/utlstring.h>
#include <tier0/utldict.h>

class IResponseSystem;
// The game's IResponseSystem implementation: its reference count lives in a second base at +8
class CGameResponseSystem;

//-----------------------------------------------------------------------------
// The interface of the response system manager. No virtual destructor. The method names are
// not in any source: they are named by what the server's implementation does.
//-----------------------------------------------------------------------------
abstract_class IResponseSystemManager
{
public:
	// The response system loaded from pszFile, null when it is not loaded (or pszFile is null)
	virtual IResponseSystem *FindResponseSystem( const char *pszFile ) = 0;
	// Returns the response system of pszFile, loading it when it is not loaded yet (a load
	// failure is fatal)
	virtual IResponseSystem *PrecacheCustomResponseSystem( const char *pszFile ) = 0;
	// The response system of scripts/talker/response_rules.vrr_c
	virtual IResponseSystem *GetDefaultResponseSystem() = 0;
};

//-----------------------------------------------------------------------------
// Registered as "ResponseSystemMgr" (static factory). 96 bytes on both platforms. The field
// names are named by what the code does with them.
//-----------------------------------------------------------------------------
class CResponseSystemManager : public CAutoGameSystem, public IResponseSystemManager
{
public:
	DECLARE_GAME_SYSTEM();

	virtual ~CResponseSystemManager();

	// Releases m_pRetainedResponseSystem
	virtual void Shutdown() override;
	// Loads scripts/talker/response_rules.vrr_c into m_pDefaultResponseSystem, or reuses
	// m_pRetainedResponseSystem when it was loaded from the same file (m_DefaultResponseSystemFile)
	GS_EVENT( GameInit );
	// Releases the custom response systems and moves m_pDefaultResponseSystem to
	// m_pRetainedResponseSystem
	GS_EVENT( GameShutdown );
	// Empty
	GS_EVENT( BuildGameSessionManifest );
	GS_EVENT( GameActivate );
	GS_EVENT( PostSpawnGroupLoad );

	// Forward to the game system factory
	virtual void SetGameSystemGlobalPtrs( void *pValue ) override;
	virtual bool DoesGameSystemReallocate() override;

	// IResponseSystemManager
	virtual IResponseSystem *FindResponseSystem( const char *pszFile ) override;
	virtual IResponseSystem *PrecacheCustomResponseSystem( const char *pszFile ) override;
	virtual IResponseSystem *GetDefaultResponseSystem() override;

public:
	// Returned by GetDefaultResponseSystem
	IResponseSystem *m_pDefaultResponseSystem;
	// The response systems PrecacheCustomResponseSystem loaded, by file name
	CUtlDict< IResponseSystem *, int > m_CustomResponseSystems;
	// The file m_pDefaultResponseSystem was loaded from
	CUtlString m_DefaultResponseSystemFile;
	// The default response system of the previous game, kept (with a reference) for the next GameInit
	CGameResponseSystem *m_pRetainedResponseSystem;
};

#endif // AI_RESPONSESYSTEM_H
