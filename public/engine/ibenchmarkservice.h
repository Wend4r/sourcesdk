//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: Engine service that runs the rendering benchmark and writes results.txt
//
//=============================================================================//

#ifndef IBENCHMARKSERVICE_H
#define IBENCHMARKSERVICE_H
#ifdef _WIN32
#pragma once
#endif

#include <IEngineService.h>

abstract_class IBenchmarkService : public IEngineService
{
public:
	virtual int GetFrameRenderCount() = 0;
};

#endif // IBENCHMARKSERVICE_H
