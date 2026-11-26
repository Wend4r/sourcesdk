//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Helpers of the V8 script bindings.
//
//=============================================================================//

#ifndef V8HELPERS_H
#define V8HELPERS_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"

namespace V8HelpersImpl
{

//-----------------------------------------------------------------------------
// The base of the weak V8 handles of game objects (V8WeakWrapper< CCSScriptObject > overrides only), 16 bytes:
// the destructor, which disposes the handle.
//-----------------------------------------------------------------------------
abstract_class V8WeakWrapperBase
{
public:
	virtual ~V8WeakWrapperBase() = 0;

public:
	// Named by what it holds: the global handle of the script value (a v8::Global< v8::External >), made weak
	// with this wrapper as the parameter of the weak callback
	void *m_pHandle;
};

} // namespace V8HelpersImpl

#endif // V8HELPERS_H
