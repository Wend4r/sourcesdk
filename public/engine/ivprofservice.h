//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: Engine service that drives the VProf profiler
//
//=============================================================================//

#ifndef IVPROFSERVICE_H
#define IVPROFSERVICE_H
#ifdef _WIN32
#pragma once
#endif

#include <IEngineService.h>

abstract_class IVProfService : public IEngineService
{
public:
	virtual void ForceVProfEnabled( bool bEnabled ) = 0;
	virtual bool IsVProfEnabled() = 0;

	virtual void SetVProfAllThreads( bool bAllThreads ) = 0;
};

#endif // IVPROFSERVICE_H
