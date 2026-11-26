//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The temporary entity system.
//
//=============================================================================//

#ifndef ITEMPENTS_H
#define ITEMPENTS_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"

class IRecipientFilter;

//-----------------------------------------------------------------------------
// The temporary entity system (CTempEntsSystem). No virtual destructor.
//-----------------------------------------------------------------------------
abstract_class ITempEntsSystem
{
public:
	virtual bool SuppressTE( IRecipientFilter &filter ) = 0;

	// Empty in the server.
	virtual void Stub_1() = 0;

	// Returns false in the server.
	virtual bool Stub_2() const = 0;
};

#endif // ITEMPENTS_H
