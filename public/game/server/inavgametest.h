//===== Copyright (c) 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: Navigation mesh game test app system
//
//===========================================================================//

#ifndef INAVGAMETEST_H
#define INAVGAMETEST_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "appframework/iappsystem.h"

abstract_class INavGameTest : public IAppSystem
{
public:
	// Runs the simple ground, simple air to air and mesh space transition pathfinding tests in that
	// order and stops at the first group that fails; true when all of them pass.
	virtual bool Unk_RunPathfindTests() = 0;
};

#endif // INAVGAMETEST_H
