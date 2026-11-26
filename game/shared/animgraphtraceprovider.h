//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Traces for the animation graph.
//
//=============================================================================//

#ifndef ANIMGRAPHTRACEPROVIDER_H
#define ANIMGRAPHTRACEPROVIDER_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"

//-----------------------------------------------------------------------------
// Traces for the animation graph (CAnimGraphTraceProvider). The server library of Windows has no
// implementation.
//-----------------------------------------------------------------------------
abstract_class IAnimGraphTraceProvider
{
public:
	// The only method: a trace query taking a ray or shape description, a filter and a result. No virtual
	// destructor.
	virtual void Stub_0( const void *pRay, const void *pFilter, void *pResult ) = 0;
};

#endif // ANIMGRAPHTRACEPROVIDER_H
