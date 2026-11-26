//========== Copyright © 2008, Valve Corporation, All rights reserved. ========
//
// Purpose:
//
//=============================================================================

#ifndef VSCRIPT_SHARED_H
#define VSCRIPT_SHARED_H

#include "vscript/ivscript.h"
#include "tier0/convar.h"

#if defined( _WIN32 )
#pragma once
#endif

extern IScriptVM * g_pScriptVM;

HSCRIPT VScriptCompileScript( const char *pszScriptName, bool bWarnMissing = false );
bool VScriptRunScript( const char *pszScriptName, HSCRIPT hScope, bool bWarnMissing = false );
inline bool VScriptRunScript( const char *pszScriptName, bool bWarnMissing = false ) { return VScriptRunScript( pszScriptName, NULL, bWarnMissing ); }

#define DECLARE_ENT_SCRIPTDESC()													ALLOW_SCRIPT_ACCESS(); virtual ScriptClassDesc_t *GetScriptDesc()

#define BEGIN_ENT_SCRIPTDESC( className, baseClass, description )					_IMPLEMENT_ENT_SCRIPTDESC_ACCESSOR( className ); BEGIN_SCRIPTDESC( className, baseClass, description )
#define BEGIN_ENT_SCRIPTDESC_ROOT( className, description )							_IMPLEMENT_ENT_SCRIPTDESC_ACCESSOR( className ); BEGIN_SCRIPTDESC_ROOT( className, description )
#define BEGIN_ENT_SCRIPTDESC_NAMED( className, baseClass, scriptName, description )	_IMPLEMENT_ENT_SCRIPTDESC_ACCESSOR( className ); BEGIN_SCRIPTDESC_NAMED( className, baseClass, scriptName, description )
#define BEGIN_ENT_SCRIPTDESC_ROOT_NAMED( className, scriptName, description )		_IMPLEMENT_ENT_SCRIPTDESC_ACCESSOR( className ); BEGIN_SCRIPTDESC_ROOT_NAMED( className, scriptName, description )

#define _IMPLEMENT_ENT_SCRIPTDESC_ACCESSOR( className )					template <> ScriptClassDesc_t * GetScriptDesc<className>( className * ); ScriptClassDesc_t *className::GetScriptDesc()  { return ::GetScriptDesc( this ); }		

// Only allow scripts to create entities during map initialization
bool IsEntityCreationAllowedInScripts( void );

//-----------------------------------------------------------------------------
// A console command a vscript registers (40 bytes): ICommandCallback, which calls the script function, and the
// destructor, which unregisters the command and releases the function.
//-----------------------------------------------------------------------------
abstract_class CScriptConsoleCommand : public ICommandCallback
{
public:
	virtual ~CScriptConsoleCommand() = 0;

public:
	// The script function the command calls; released through the script VM by the destructor
	HSCRIPT m_hCallbackFn;
	// The registered command (its callback is this object); deleted by the destructor
	ConCommand *m_pConCommand;
	// Both are copies the object owns
	const char *m_pszName;
	const char *m_pszHelpString;
};

#endif // VSCRIPT_SHARED_H
