//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: A text log.
//
//=============================================================================//

#ifndef ITEXTLOG_H
#define ITEXTLOG_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"

//-----------------------------------------------------------------------------
// A text log (CNullTextLogAsyncCS discards everything). The server library of Windows has no table of its own.
//-----------------------------------------------------------------------------
abstract_class ITextLog
{
public:
	virtual ~ITextLog() {}

	// Empty in the null log.
	virtual void Stub_2() = 0;

	// The null log writes the int argument to both output pointers.
	virtual void Stub_3( int, int *, int * ) = 0;

	// Returns true in the null log.
	virtual bool Stub_4() = 0;

	// Empty in the null log.
	virtual void Stub_5() = 0;
};

#endif // ITEXTLOG_H
