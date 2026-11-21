//========== Copyright © 2008, Valve Corporation, All rights reserved. ========
//
// Purpose:
//
//=============================================================================

#ifndef VSCRIPT_SERVER_H
#define VSCRIPT_SERVER_H

#include "vscript/ivscript.h"
#include "tier0/keyvalues.h"
#include "vscript_shared.h"
#include "entity2/scriptkeyvalues.h"
#include "entity2/scriptprecachecontext.h"
#include "igamesystem.h"
#include "tier0/utlmap.h"
#include "tier0/utlsymbol.h"

#if defined( _WIN32 )
#pragma once
#endif

class ISaveRestoreBlockHandler;

bool VScriptServerReplaceClosures( const char *pszScriptName, HSCRIPT hScope, bool bWarnMissing = false );
ISaveRestoreBlockHandler *GetVScriptSaveRestoreBlockHandler();


class CBaseEntityScriptInstanceHelper : public IScriptInstanceHelper
{
	bool ToString( void *p, char *pBuf, int bufSize );
	void *BindOnRead( HSCRIPT hInstance, void *pOld, const char *pszId );
};

extern CBaseEntityScriptInstanceHelper g_BaseEntityScriptInstanceHelper;

// Only allow scripts to create entities during map initialization
bool IsEntityCreationAllowedInScripts( void );

//-----------------------------------------------------------------------------
// The interface of the VScript game system. 17 methods, no destructor. MSVC orders the
// RunScript overloads in reverse.
//-----------------------------------------------------------------------------
abstract_class IVScriptGameSystem
{
public:
	// Creates the VM (language from GetScriptLanguage), connects the debugger, installs the
	// bindings and runs "addon_init". bIsBackgroundMap is is_background_map of the session
	// configuration: a background map ignores the second debugger switch of the command line
	virtual void VScriptInit( bool bIsBackgroundMap ) = 0;
	virtual void VScriptTerm() = 0;

	virtual bool IsVScriptInitialized() const = 0;
	virtual bool IsEntityCreationAllowedInScripts() const = 0;
	virtual IScriptVM *GetVM() = 0;

	virtual HSCRIPT CompileScript( const char *pszScriptName, bool bWarnMissing = false ) = 0;

	virtual bool RunScript( const char *pszScriptName, HSCRIPT hScope, bool bWarnMissing = false ) = 0;
	virtual bool RunScript( const char *pszScriptName, bool bWarnMissing ) = 0;
	virtual bool RunScript( const char *pszScript ) = 0;

	virtual HSCRIPT FetchCachedScriptScope( const char *pszScriptName ) = 0;
	virtual void ClearCachedScripts() = 0;
	virtual void ReloadCachedScripts() = 0;

	// Call "__RegisterGameEventListeners" / remove the listeners of the scope
	virtual void RegisterGameEventListenersInScope( HSCRIPT hScope ) = 0;
	virtual void UnregisterGameEventListenersInScope( HSCRIPT hScope ) = 0;
	virtual void ClearScriptGameEventListeners() = 0;

	// Named by what they do: CScriptConsoleCommand sets the player slot of the client that ran the
	// command for the duration of the script call (returning the previous one); script bindings
	// read it to find the calling player
	virtual int SetCommandClientSlot( int nSlot ) = 0;
	virtual int GetCommandClientSlot() = 0;
};

//-----------------------------------------------------------------------------
// Base of the per-game VScript game systems (CCSGOVScriptGameSystem here). 144 bytes on both
// platforms; not registered itself: its SetGameSystemGlobalPtrs feeds
// CGameSystemAbstractFactory< CVScriptGameSystem, IVScriptGameSystem >
// (game/shared/igamesystemfactory.h).
//-----------------------------------------------------------------------------
class CVScriptGameSystem : public IGameSystem, public IVScriptGameSystem
{
public:
	DECLARE_GAME_SYSTEM();

	// Block of the game's ultra-conservative vector of the root table's function handles: the
	// count followed by the elements (as CScriptComponent::FuncHandles_t)
	struct FuncHandles_t
	{
		int m_Size;
		HSCRIPT *m_Elements[ 1 ];
	};

	// Allows entity creation in scripts and calls VScriptInit
	GS_EVENT( GameInit );
	// Forbids entity creation in scripts and calls VScriptTerm
	GS_EVENT( GameShutdown );
	// Empty
	GS_EVENT( BuildGameSessionManifest );
	// Runs the VM frame and "ScriptDebugDraw"
	GS_EVENT( ServerPostSimulate );
	// Lets the script debugger process its frame
	GS_EVENT( GameFrameBoundary );

	// Forward to the game system factory
	virtual void SetGameSystemGlobalPtrs( void *pValue ) override;
	virtual bool DoesGameSystemReallocate() override;

	virtual ~CVScriptGameSystem();

	// IVScriptGameSystem
	virtual void VScriptInit( bool bIsBackgroundMap ) override;
	virtual void VScriptTerm() override;
	virtual bool IsVScriptInitialized() const override;
	virtual bool IsEntityCreationAllowedInScripts() const override;
	virtual IScriptVM *GetVM() override;
	virtual HSCRIPT CompileScript( const char *pszScriptName, bool bWarnMissing = false ) override;
	virtual bool RunScript( const char *pszScriptName, HSCRIPT hScope, bool bWarnMissing = false ) override;
	virtual bool RunScript( const char *pszScriptName, bool bWarnMissing ) override;
	virtual bool RunScript( const char *pszScript ) override;
	virtual HSCRIPT FetchCachedScriptScope( const char *pszScriptName ) override;
	virtual void ClearCachedScripts() override;
	virtual void ReloadCachedScripts() override;
	virtual void RegisterGameEventListenersInScope( HSCRIPT hScope ) override;
	virtual void UnregisterGameEventListenersInScope( HSCRIPT hScope ) override;
	virtual void ClearScriptGameEventListeners() override;
	virtual int SetCommandClientSlot( int nSlot ) override;
	virtual int GetCommandClientSlot() override;

	// The language CreateVM gets: SL_LUA here, CCSGOVScriptGameSystem returns 2 (SL_NOP)
	virtual ScriptLanguage_t GetScriptLanguage();
	// Named by what it does: the bEnableDebugLibrary argument of IScriptManager::CreateVM (false)
	virtual bool ShouldEnableDebugLibrary();
	// Registers the script functions and classes after the VM starts / removes them before it stops
	virtual void InstallScriptBindings();
	virtual void RemoveScriptBindings();

public:
	// The root table scope (a CScriptScopeT< CVScriptGameSystem::CScriptScopeBase, true >; the SDK
	// template's layout differs, so its members are spelled out)
	HSCRIPT m_hRootTable;
	FuncHandles_t *m_pRootTableFuncHandles;
	int m_nRootTableFlags;

	// Script name symbol -> scope / compiled script
	CUtlMap< CUtlSymbol, HSCRIPT, int > m_ScriptScopeCache;
	CUtlMap< CUtlSymbol, HSCRIPT, int > m_CompiledScriptCache;

	bool m_bAllowEntityCreationInScripts;
	bool m_bVscriptServerInitialized;
	// See SetCommandClientSlot; -1 outside a client's script command
	int m_nCommandClientSlot;
};

#endif // VSCRIPT_SERVER_H
