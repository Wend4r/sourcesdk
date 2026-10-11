#ifndef MATHLIB_OBB_H
#define MATHLIB_OBB_H

#ifdef _WIN32
#pragma once
#endif

#include "mathlib/vector.h"

/// Oriented 3d bounding box: m_vExtent is the half size along the axes of the orientation, around m_vCenter.
struct OBB_t
{
	FORCEINLINE Quaternion GetOrientation() const { return Quaternion( m_qOrientation[ 0 ], m_qOrientation[ 1 ], m_qOrientation[ 2 ], m_qOrientation[ 3 ] ); }
	FORCEINLINE void SetOrientation( const Quaternion &q ) { m_qOrientation[ 0 ] = q.x; m_qOrientation[ 1 ] = q.y; m_qOrientation[ 2 ] = q.z; m_qOrientation[ 3 ] = q.w; }

	Vector				m_vCenter;
	// x, y, z, w of a Quaternion, stored unaligned right after m_vCenter as in the engine, where the SDK's Quaternion is 16-byte aligned
	float				m_qOrientation[ 4 ];
	Vector				m_vExtent;
};

#endif // MATHLIB_OBB_H
