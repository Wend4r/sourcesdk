//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The console completion functors of the entity commands.
//
//=============================================================================//

#ifndef ENTITYDEBUG_H
#define ENTITYDEBUG_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "tier0/convar.h"

//-----------------------------------------------------------------------------
// The console completion functors of the entity commands. The base completes entity names; it has no data of its
// own (8 bytes, its virtual table pointer only).
//-----------------------------------------------------------------------------
abstract_class CEntityAutoCompletionFunctor : public ICommandCompletionCallback
{
public:
	// Empty here; the ent_fire functors complete the input (or output) names of the target given so far.
	virtual void Stub_1() = 0;
};

//-----------------------------------------------------------------------------
// ent_fire, ent_fire_delay and ent_fire_output (CEntFireOutputAutoCompletionFunctor overrides only), 24 bytes:
// one instance per command. The ICommandCallback override has a slot of its own in the primary table on Linux
// only (Itanium), as in the game.
//-----------------------------------------------------------------------------
abstract_class CEntFireAutoCompletionFunctor : public CEntityAutoCompletionFunctor, public ICommandCallback
{
public:
	virtual void CommandCallback( const CCommandContext &context, const CCommand &command ) override = 0;

	// Fires the command on the target given by name or entity index (ent_fire queues the input, ent_fire_output
	// fires the output).
	virtual void Stub_3() = 0;

	// Named by what it does: "ent_fire" / "ent_fire_output".
	virtual const char *GetCommandName() const = 0;

public:
	// Named by what it does: set for ent_fire_delay, whose last argument is the delay of the input rather than
	// an input parameter
	bool m_bLastArgIsDelay;
};

#endif // ENTITYDEBUG_H
