//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: A channel of a choreo actor: a named row of events.
//
//=============================================================================//

#ifndef CHOREOCHANNEL_H
#define CHOREOCHANNEL_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/utlstring.h"
#include "choreoobjects/choreoobjectbase.h"

class CChoreoActor;
class CChoreoEvent;

//-----------------------------------------------------------------------------
// Owned and destroyed by its scene (no virtual destructor). 64 bytes on both platforms.
//-----------------------------------------------------------------------------
abstract_class CChoreoChannel : public CChoreoObjectBase
{
public:
	CChoreoActor *m_pActor;
	CUtlString m_Name;
	CUtlVector< CChoreoEvent * > m_Events;
	// True from the constructor on; an inactive channel's events do not start
	bool m_bActive;
};

#endif // CHOREOCHANNEL_H
