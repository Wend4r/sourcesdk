//===== Copyright (c) 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: V8 JavaScript engine host app system
//
//===========================================================================//

#ifndef ISOURCE2V8SYSTEM_H
#define ISOURCE2V8SYSTEM_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "appframework/iappsystem.h"

abstract_class ISource2V8System : public IAppSystem
{
public:
	// Called by the engine once per host frame.
	virtual void RunFrame() = 0;

	// Forwards to v8::Isolate::LowMemoryNotification.
	virtual void LowMemoryNotification() = 0;

	// Named by what the code does: the shipping v8system implements these six as stubs (false or nothing)
	// and engine2, server and panorama never call them; RunFrame calls ReservedFrameStub( nullptr, nullptr, -1 )
	virtual bool ReservedBoolStub() = 0;
	virtual void ReservedStub() = 0;
	virtual void ReservedFrameStub( void *p1, void *p2, int n ) = 0;
	virtual bool ReservedBoolStubWithArg( void *p ) = 0;
	virtual void ReservedStubWithArg( void *p ) = 0;
	virtual void ReservedStubWithArgB( void *p ) = 0;

	virtual void PushPromiseRejectCallback( void *pfnCallback ) = 0;
	virtual void PopPromiseRejectCallback() = 0;
};

#endif // ISOURCE2V8SYSTEM_H
