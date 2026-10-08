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

	virtual bool unk013() = 0;
	virtual void unk014() = 0;
	virtual void unk015( void *p1, void *p2, int n ) = 0;
	virtual bool unk016( void *p ) = 0;
	virtual void unk017( void *p ) = 0;
	virtual void unk018( void *p ) = 0;

	virtual void PushPromiseRejectCallback( void *pfnCallback ) = 0;
	virtual void PopPromiseRejectCallback() = 0;
};

#endif // ISOURCE2V8SYSTEM_H
