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

abstract_class IVScriptGameSystem
{
public:
	virtual void VScriptInit( bool bUnk ) = 0;
	virtual void VScriptTerm() = 0;

	virtual bool IsVScriptInitialized() = 0;
	virtual bool IsEntityCreationAllowedInScripts() = 0;
	virtual IScriptVM *GetVM() = 0;

	virtual HSCRIPT CompileScript( const char *pszScriptName, bool bWarnMissing = false ) = 0;

	virtual bool RunScript( const char *pszScriptName, HSCRIPT hScope, bool bWarnMissing = false ) = 0;
	virtual bool RunScript( const char *pszScriptName, bool bWarnMissing ) = 0;
	virtual bool RunScript( const char *pszScript ) = 0;

	virtual HSCRIPT FetchCachedScriptScope( const char *pszScriptName ) = 0;
	virtual void ClearCachedScripts() = 0;
	
	virtual void ReloadCachedScripts() = 0;
};

// Base of the per-game VScript game systems (CCSGOVScriptGameSystem, CDOTAVScriptGameSystem, CCitadelVScriptGameSystem).
class CVScriptGameSystem : public CBaseGameSystem, public IVScriptGameSystem
{
};

#endif // VSCRIPT_SERVER_H
