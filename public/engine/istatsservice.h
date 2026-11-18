//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: Engine service behind the stats_display / stats_print performance statistics
//
//=============================================================================//

#ifndef ISTATSSERVICE_H
#define ISTATSSERVICE_H
#ifdef _WIN32
#pragma once
#endif

#include <IEngineService.h>

abstract_class IStatsService : public IEngineService
{
public:
	// stats_display != 0
	virtual bool IsStatsDisplayEnabled() = 0;

	// Clears the accumulated frame and GPU timing samples
	virtual void ResetStats() = 0;

	// Frame rate of the last frame (1 / frame time)
	virtual double GetFrameRate() = 0;
	// Frame rate from the averaged frame time
	virtual double GetAverageFrameRate() = 0;
};

#endif // ISTATSSERVICE_H
