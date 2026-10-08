//===== Copyright (c) 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: App system that owns the panorama UI engine instance
//
//===========================================================================//

#ifndef IPANORAMAUIENGINE_H
#define IPANORAMAUIENGINE_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "tier1/utlvector.h"
#include "appframework/iappsystem.h"
#include "inputsystem/InputEnums.h"

namespace panorama
{
	class IUIEngine;
	class IUIWindow;
};

abstract_class IPanoramaUIEngine : public IAppSystem
{
public:
	// Creates the UI engine; returns false when creation failed
	virtual bool SetupUIEngine() = 0;
	virtual void ShutdownUIEngine() = 0;
	virtual panorama::IUIEngine *AccessUIEngine() = 0;

	virtual bool HandleInputEvent( const InputEvent_t &event, const CUtlVector< panorama::IUIWindow * > &vecWindows, bool bRequireFocus ) = 0;

	virtual void SetCustomIMEEnabled( bool bEnabled ) = 0;

	virtual int DeleteClosedWindows() = 0;

	virtual bool unk017( void *p1, void *p2 ) = 0;
};

#endif // IPANORAMAUIENGINE_H
