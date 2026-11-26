//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The systems that create break pieces.
//
//=============================================================================//

#ifndef BREAKABLEPROP_H
#define BREAKABLEPROP_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"

//-----------------------------------------------------------------------------
// A system that creates break pieces in batches (CPropsBreakableSystem, CShatterGlassBreakableSystem).
//-----------------------------------------------------------------------------
abstract_class IBreakableSystem
{
public:
	virtual ~IBreakableSystem() {}

	// The only method: CPropsBreakableSystem creates the queued break pieces ("break_list", "BreakPiece");
	// CShatterGlassBreakableSystem the queued glass shards.
	virtual void Stub_2() = 0;
};

#endif // BREAKABLEPROP_H
