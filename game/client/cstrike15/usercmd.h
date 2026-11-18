#ifndef CSTRIKE15_CLIENT_USERCMD_H
#define CSTRIKE15_CLIENT_USERCMD_H

#pragma once

#include "../../shared/cstrike15/usercmd.h"

//-----------------------------------------------------------------------------
// Client side user command, as laid out by the client module.
//
// The client stores per-command prediction bookkeeping where the server keeps
// its list links.
//-----------------------------------------------------------------------------
class C_CSGOUserCmd : public CUserCmdBaseHost< CSGOUserCmdPB >
{
public:
	// Not part of the player message. Named after the server CCSGOUserCmd member at the same
	// offset (120): an int that is -1 by default; the 4 bytes after it align m_flExecutionTime.
	int32 m_nControllerTrackedValue;

	double m_flExecutionTime;
	float m_flCurrentTime;
	bool m_bHasBeenPredicted;
	int m_nCmdType;
	int m_nCmdFlags;
};

#endif // CSTRIKE15_CLIENT_USERCMD_H
