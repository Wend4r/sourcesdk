#ifndef IANIMTAGLISTENER_H
#define IANIMTAGLISTENER_H

#pragma once

#include "tier0/platform.h"

//-----------------------------------------------------------------------------
// Receives the tags the animation graph of an entity fires. The cursor state of
// CPulseCell_Outflow_ListenForAnimgraphTag implements it. A virtual destructor (two slots on
// Linux, one on Windows) and one method.
//-----------------------------------------------------------------------------
abstract_class IAnimTagListener
{
public:
	virtual ~IAnimTagListener() {}

	// A tag fired: pTag is the tag (an object whose virtual method gives its name), pEvent starts
	// with the phase (0: the tag starts and ends at once, 1: it starts, 2: it ends). The pulse
	// cursor state compares the name with its tag and resumes its start or end outflow. Named by
	// what it does
	virtual void OnAnimTagEvent( const void *pTag, const void *pEvent ) = 0;
};

#endif // IANIMTAGLISTENER_H
