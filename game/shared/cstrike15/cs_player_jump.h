#ifndef CSTRIKE15_CS_PLAYER_JUMP_H
#define CSTRIKE15_CS_PLAYER_JUMP_H

#pragma once

#include "tier0/platform.h"

//-----------------------------------------------------------------------------
// Virtual tables of the two jump implementations of the player movement,
// CCSPlayerLegacyJump and CCSPlayerModernJump (members of CCSPlayer_MovementServices). The
// classes carry a VTable suffix and declare virtual methods only: the game classes of the
// same names are schema classes, so a module declares its own class with the members it
// needs and derives it from these. Neither has a base with a virtual table; each has three
// slots, the same on both platforms, and no virtual destructor. Every slot is empty in both
// classes and no call through the table was found, so there is nothing to name them after.
//-----------------------------------------------------------------------------
abstract_class CCSPlayerLegacyJumpVTable
{
public:
	virtual void Stub_0() = 0;
	virtual void Stub_1() = 0;
	virtual void Stub_2() = 0;
};

abstract_class CCSPlayerModernJumpVTable
{
public:
	virtual void Stub_0() = 0;
	virtual void Stub_1() = 0;
	virtual void Stub_2() = 0;
};

#endif // CSTRIKE15_CS_PLAYER_JUMP_H
