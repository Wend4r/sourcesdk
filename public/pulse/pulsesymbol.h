//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The name type of the Pulse runtime (clocks, blackboards, hooks, domains).
//
//=============================================================================//

#ifndef PULSESYMBOL_H
#define PULSESYMBOL_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/globalsymbol.h"

// 16 bytes. Built from a string by interning it twice (e.g.
// "ServerCurTime"): the case-sensitive symbol first, the case-insensitive one second; the string
// is read back from the first. Member names are descriptive.
struct PulseSymbol_t
{
	PulseSymbol_t() {}
	explicit PulseSymbol_t( const char *pszName )
	 : m_Name( MakeGlobalSymbolCaseSensitive( pszName ) ), m_NameCaseless( MakeGlobalSymbol( pszName ) )
	{
	}

	CGlobalSymbolCaseSensitive m_Name;
	CGlobalSymbol m_NameCaseless;
};

#endif // PULSESYMBOL_H
