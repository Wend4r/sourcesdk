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
#include "tier0/globalsymbol.h"
#include "tier0/utlvector.h"
#include "appframework/iappsystem.h"
#include "inputsystem/inputenums.h"

struct ResourceBindingBase_t;

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

	// Element of a layout file collected by unk017
	struct unk017_t
	{
		// Value of the element's "id" attribute, NULL when it has none
		CGlobalSymbol m_ID;
		// Element name
		CGlobalSymbol m_Name;

	private:
		// Copied from the layout node, meaning not established
		uint8 m_nUnknown0010[8];
	};

	// Appends every element of the loaded layout file (a "vxml" resource) to pElements;
	// returns false when the resource is not loaded
	virtual bool unk017( const ResourceBindingBase_t *hLayout, CUtlVector< unk017_t > *pElements ) = 0;
};

#endif // IPANORAMAUIENGINE_H
