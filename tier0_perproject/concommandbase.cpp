
//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: 
//
// $NoKeywords: $
//
//===========================================================================//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "basetypes.h"
#include "tier0/characterset.h"
#include "tier0/strtools.h"
#include "tier0/utlbuffer.h"
#include "tier0/convar.h"
#include "tier0/utlvector.h"
#include "tier1/tier1.h"
#include "tier0/icvar.h"
#include "tier0/dbg.h"
#include "color.h"
#if defined( _X360 )
#include "xbox/xbox_console.h"
#endif
#include "tier0/memdbgon.h"

// AMNOTE: Define this if you need cvars/concommands to respect flags sanitization rules
// #define SANITIZE_CVAR_FLAGS 1

//-----------------------------------------------------------------------------
// Statically constructed list of ConVars/ConCommands, 
// used for registering them with the ICVar interface
//-----------------------------------------------------------------------------
static FnConVarRegisterCallback s_ConVarRegCB = nullptr;
static FnConCommandRegisterCallback s_ConCommandRegCB = nullptr;
static uint64 s_nCVarFlag = 0;
static bool s_bRegistered = false;

bool ConCommandRegList::s_bConCommandsRegistered = false;
ConCommandRegList *ConCommandRegList::s_pRoot = nullptr;

void ConCommandRegList::RegisterConCommand( const Entry_t &cmd )
{
	Assert( g_pCVar );
	*cmd.m_Command = g_pCVar->RegisterConCommand( cmd.m_Info, s_nCVarFlag );
	if ( !cmd.m_Command->IsValidRef() )
	{
		Plat_FatalError( "RegisterConCommand: Unknown error registering con command \"%s\"!\n", cmd.m_Info.m_pszName );
		DebuggerBreakIfDebugging();
	}
	else if ( s_ConCommandRegCB )
		s_ConCommandRegCB( cmd.m_Command );
}

void ConCommandRegList::UnregisterConCommand( const Entry_t &cmd )
{
	auto *pConCmd = cmd.m_Command;

	Assert( pConCmd );

	// Could already be unregistered or failed to register
	if ( !pConCmd->IsValidRef() )
	{
		return;
	}

	Assert( g_pCVar );
	g_pCVar->UnregisterConCommandCallbacks( *pConCmd );
	pConCmd->InvalidateRef();
}

void ConCommandRegList::RegisterAll()
{
	if ( s_bConCommandsRegistered )
	{
		return;
	}

	s_bConCommandsRegistered = true;

	for ( auto list = s_pRoot; list; list = list->m_pPrev )
	{
		for ( size_t i = 0; i < list->m_nSize; i++ )
		{
			RegisterConCommand( list->m_Entries[i] );
		}
	}
}

void ConCommandRegList::UnregisterAll()
{
	if ( !s_bConCommandsRegistered )
	{
		return;
	}

	s_bConCommandsRegistered = false;

	ConCommandRegList *pPrev = nullptr;

	for ( auto list = s_pRoot; list; list = pPrev )
	{
		for ( size_t i = 0; i < list->m_nSize; i++ )
		{
			UnregisterConCommand( list->m_Entries[i] );
		}

		pPrev = list->m_pPrev;
		delete list;
	}

	s_pRoot = nullptr;
}

void ConCommandRegList::AddToList( const Entry_t &cmd )
{
	if ( s_bConCommandsRegistered )
	{
		RegisterConCommand( cmd );

		return;
	}

	auto list = s_pRoot;

	if ( !list || list->m_nSize >= ARRAYSIZE( m_Entries ) )
	{
		list = new ConCommandRegList;
		list->m_nSize = 0;
		list->m_pPrev = s_pRoot;

		s_pRoot = list;
	}

	list->m_Entries[list->m_nSize] = cmd;
	list->m_nSize++;
}

void SetupConCommand( ConCommand *cmd, const ConCommandCreation_t& info )
{
	ConCommandRegList::Entry_t entry;
	entry.m_Info = info;
	entry.m_Command = cmd;

	ConCommandRegList::AddToList( entry );
}

void DetroyConCommand( ConCommand *cmd )
{
	if (cmd->IsValidRef())
	{
		if (g_pCVar)
			g_pCVar->UnregisterConCommandCallbacks( *cmd );

		cmd->InvalidateRef();
	}
}

bool ConVarRegList::s_bConVarsRegistered = false;
ConVarRegList *ConVarRegList::s_pRoot = nullptr;

void ConVarRegList::RegisterConVar( const Entry_t &cvar )
{
	Assert( g_pCVar );

	ConVarRefAbstract hConVar = g_pCVar->FindConVar( cvar.m_Info.m_pszName );
	if ( hConVar.IsValidRef() )
	{
		ConVarData* pConVarData = g_pCVar->GetConVarData( static_cast<ConVarRef>(hConVar) );
		if (pConVarData->GetType() != cvar.m_Info.m_valueInfo.m_eVarType)
		{
			const char* cvarType = cvar.m_pConVar->GetConVarData()->GetDataTypeName();
			const char* validType = pConVarData->GetDataTypeName();
			Plat_FatalError( "RegisterConVar: Convar \"%s\" already exists with different type! (expected: %s, given: %s)\n",
				cvar.m_Info.m_pszName, validType, cvarType );
			DebuggerBreakIfDebugging();
			return;
		}
	}

	g_pCVar->RegisterConVar( cvar.m_Info, s_nCVarFlag, cvar.m_pConVar, cvar.m_pConVarData );

	Assert( cvar.m_pConVar );
	if ( !cvar.m_pConVar->IsValidRef() )
	{
		if ( !hConVar.IsValidRef() )
		{
			Plat_FatalError( "RegisterConVar: Unknown error registering convar \"%s\"!\n", cvar.m_Info.m_pszName );
			DebuggerBreakIfDebugging();
		}
	}

	// Don't let references pass as a newly registered cvar
	if ( s_ConVarRegCB && (cvar.m_Info.m_nFlags & FCVAR_REFERENCE) == 0 )
		s_ConVarRegCB( cvar.m_pConVar );
}

void ConVarRegList::UnregisterConVar( const Entry_t &cvar )
{
	auto *pConVar = cvar.m_pConVar;
	Assert( pConVar );

	// Could already be unregistered or failed to register
	if ( !pConVar->IsValidRef() )
	{
		return;
	}

	Assert( g_pCVar );
	g_pCVar->UnregisterConVarCallbacks( *pConVar );
	pConVar->InvalidateRef();
}

bool ConVarRegList::RegisterAll()
{
	if ( s_bConVarsRegistered )
	{
		return false;
	}

	s_bConVarsRegistered = true;

	for ( auto list = s_pRoot; list; list = list->m_pPrev )
	{
		for(size_t i = 0; i < list->m_nSize; i++)
		{
			RegisterConVar( list->m_Entries[i] );
		}
	}

	return true;
}

bool ConVarRegList::UnregisterAll()
{
	if ( !s_bConVarsRegistered )
	{
		return false;
	}

	s_bConVarsRegistered = false;

	ConVarRegList *pPrev = nullptr;

	for ( auto list = s_pRoot; list; list = pPrev )
	{
		for ( size_t i = 0; i < list->m_nSize; i++ )
		{
			UnregisterConVar( list->m_Entries[i] );
		}

		pPrev = list->m_pPrev;
		delete list;
	}

	s_pRoot = nullptr;

	return true;
}

void ConVarRegList::AddToList( const Entry_t &cvar )
{
	if ( s_bConVarsRegistered )
	{
		RegisterConVar( cvar );

		return;
	}

	auto list = s_pRoot;

	if ( !list || list->m_nSize >= ARRAYSIZE( m_Entries ) )
	{
		list = new ConVarRegList;
		list->m_nSize = 0;
		list->m_pPrev = s_pRoot;

		s_pRoot = list;
	}

	list->m_Entries[list->m_nSize++] = cvar;
}

void SetupConVar( ConVarRefAbstract *cvar, ConVarData **cvar_data, ConVarCreation_t &info )
{
	ConVarRegList::Entry_t entry;
	entry.m_Info = info;
	entry.m_pConVar = cvar;
	entry.m_pConVarData = cvar_data;

	ConVarRegList::AddToList( entry );
}

void DestroyConVar( ConVarRef *cvar )
{
	if ( cvar->IsValidRef() )
	{
		if ( g_pCVar )
			g_pCVar->UnregisterConVarCallbacks( *cvar );

		cvar->InvalidateRef();
	}
}

uint64 SanitiseConVarFlags( uint64 flags )
{
#ifdef SANITIZE_CVAR_FLAGS
	if(!CommandLine()->HasParm( "-tools" )
		&& (flags & (FCVAR_DEVELOPMENTONLY
					| FCVAR_ARCHIVE
					| FCVAR_USERINFO
					| FCVAR_CHEAT
					| FCVAR_RELEASE
					| FCVAR_SERVER_CAN_EXECUTE
					| FCVAR_CLIENT_CAN_EXECUTE
					| FCVAR_CLIENTCMD_CAN_EXECUTE)) == 0)
	{
		flags |= FCVAR_DEFENSIVE | FCVAR_DEVELOPMENTONLY;
	}
#endif
	return flags;
}

//-----------------------------------------------------------------------------
// Called by the framework to register ConCommandBases with the ICVar
//-----------------------------------------------------------------------------
bool ConVar_Register( uint64 nCVarFlag, FnConVarRegisterCallback cvar_reg_cb, FnConCommandRegisterCallback cmd_reg_cb )
{
	if ( !g_pCVar || s_bRegistered )
	{
		return false;
	}

	s_bRegistered = true;
	s_nCVarFlag = nCVarFlag;
	s_ConVarRegCB = cvar_reg_cb;
	s_ConCommandRegCB = cmd_reg_cb;

	ConCommandRegList::RegisterAll();
	ConVarRegList::RegisterAll();

	return true;
}

uint64 ConVar_GetDefaultFlags()
{
	return s_nCVarFlag;
}

bool ConVar_Unregister( )
{
	if ( !g_pCVar || !s_bRegistered )
		return false;

	s_bRegistered = false;
	s_nCVarFlag = FCVAR_NONE;
	s_ConVarRegCB = nullptr;
	s_ConCommandRegCB = nullptr;

	ConCommandRegList::UnregisterAll();
	ConVarRegList::UnregisterAll();

	return true;
}
