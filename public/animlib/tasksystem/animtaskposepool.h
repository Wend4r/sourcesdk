//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: Identifiers of the pose buffers the task system keeps between
//          updates
//
//===========================================================================//

#ifndef ANIMTASKPOSEPOOL_H
#define ANIMTASKPOSEPOOL_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"

// Names a pose buffer that outlives the update it was written in, used by transitions
// that start from the pose of a transition in progress
struct NmCachedPoseID_t
{
	static constexpr uint8 s_nMaxNumberOfCachedPoses = 64;

	bool IsValid() const { return m_ID < s_nMaxNumberOfCachedPoses; }

	// s_nMaxNumberOfCachedPoses when invalid
	uint8 m_ID;
};

// Progress of a blend that is faded in and out over time
enum class NmBlendState_t : uint8
{
	None = 0,
	BlendingIn,
	BlendingOut,
	FullyIn,
	FullyOut = None,
};

#endif // ANIMTASKPOSEPOOL_H
