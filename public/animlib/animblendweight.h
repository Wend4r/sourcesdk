//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: Weight that is faded in and out over time, used by the IK and
//          lookat nodes of AnimGraph2
//
//===========================================================================//

#ifndef ANIMBLENDWEIGHT_H
#define ANIMBLENDWEIGHT_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "animlib/animlib_types.h"

struct NmBlendWeight_t
{
	enum class State_t : uint8
	{
		Off = 0,
		TurningOff,
		TurningOn,
		On,
	};

	bool IsBlending() const { return m_state == State_t::TurningOn || m_state == State_t::TurningOff; }

	State_t m_state;
	NmEasingOperation_t m_easingOperation;

	// Seconds
	float m_blendTime;
	float m_desiredBlendTime;
};

#endif // ANIMBLENDWEIGHT_H
