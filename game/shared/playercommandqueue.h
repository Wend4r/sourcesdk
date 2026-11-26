//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The user command queue of a player.
//
//=============================================================================//

#ifndef PLAYERCOMMANDQUEUE_H
#define PLAYERCOMMANDQUEUE_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"

//-----------------------------------------------------------------------------
// The queue of user commands of one player (CPlayerCommandQueue). No virtual destructor.
//-----------------------------------------------------------------------------
abstract_class IPlayerCommandQueue
{
public:
	// Frees the queue state.
	virtual void Stub_0() = 0;

	// Adds parsed user commands of a tick.
	virtual void Stub_1() = 0;

	// Runs the queued commands of the tick.
	virtual void Stub_2() = 0;

	// Takes a pointer and a bool.
	virtual void Stub_3( void *, bool ) = 0;

	// Returns a clamped count.
	virtual int Stub_4() = 0;

	// Updates the queue from a record.
	virtual void Stub_5( const void * ) = 0;

	// Zeroes a member.
	virtual void Stub_6() = 0;

	// Formats the queue state (tick, depth, ping, loss).
	virtual void Stub_7() = 0;

	// Returns a member array when it is not empty.
	virtual void *Stub_8() = 0;
};

#endif // PLAYERCOMMANDQUEUE_H
