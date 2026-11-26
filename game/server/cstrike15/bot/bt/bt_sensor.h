#ifndef BT_SENSOR_H
#define BT_SENSOR_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"

class IBtAgent;
class Vector;

//-----------------------------------------------------------------------------
// CBtSensorShape, the shape a sensor decorator tests the sensed entities against (KeyValues3
// types sensor_shape_sphere and sensor_shape_fov). Not a behaviour tree node. A pure abstract
// base without data of its own: the field of view shape is the virtual table pointer alone
// (8 bytes), the sphere adds its radius and squared radius after it. Linux 3 slots, Windows 2:
// the destructor takes two slots on Linux.
//-----------------------------------------------------------------------------
abstract_class CBtSensorShape
{
public:
	virtual ~CBtSensorShape() = 0;

	// True when vecPoint is outside the shape placed at vecOrigin: the sphere compares the
	// squared distance with its squared radius, the field of view asks the agent whether the
	// point is outside its view cone. Named by what it does.
	virtual bool IsOutside( const Vector &vecOrigin, IBtAgent *pAgent, const Vector &vecPoint ) const = 0;
};

#endif // BT_SENSOR_H
