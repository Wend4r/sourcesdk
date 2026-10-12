#ifndef TIER0_TIER0_UTILS_MATH_H
#define TIER0_TIER0_UTILS_MATH_H

#ifdef _WIN32
#pragma once
#endif

enum MatrixAxisType_t
{
#ifdef YUP_ACTIVE
	FORWARD_AXIS = 2,
	LEFT_AXIS = 0,
	UP_AXIS = 1,
#else
	FORWARD_AXIS = 0,
	LEFT_AXIS = 1,
	UP_AXIS = 2,
#endif

	X_AXIS = 0,
	Y_AXIS = 1,
	Z_AXIS = 2,
	ORIGIN = 3,
	PROJECTIVE = 3,
};

typedef int SideType;

#endif // TIER0_TIER0_UTILS_MATH_H
