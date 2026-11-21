//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Listener of the vdata resources of one file, registered with the
//			resource system's vdata type manager
//			(IResourceSystem::GetVDataTypeManager(), slots 0 and 1: register and
//			unregister a listener for a vdata file name).
//
//=============================================================================//

#ifndef IVDATARESOURCELISTENER_H
#define IVDATARESOURCELISTENER_H
#ifdef _WIN32
#pragma once
#endif

#include "resourcefile/resourcetype.h"

//-----------------------------------------------------------------------------
// The methods are named by what the game does with them: CVDataTypeManager
// calls the first two for each listener of a file
// whose resource finished loading, the first on the initial load and the
// second when the resource was reloaded, and the third from the resource's
// destruction. All three have empty bodies (the manager skips a listener whose
// slot still holds the empty body; the game systems override only the reload,
// on Linux only that override repeats in their main tables). The interface
// has no virtual destructor.
//-----------------------------------------------------------------------------
class IVDataResourceListener
{
public:
	virtual void OnVDataLoaded( ResourceHandle_t hResource ) {}
	virtual void OnVDataReloaded( ResourceHandle_t hResource ) {}
	virtual void OnVDataUnloaded( ResourceHandle_t hResource ) {}
};

#endif // IVDATARESOURCELISTENER_H
