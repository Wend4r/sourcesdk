//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: An actor of a choreographed scene: its model, moods and channels.
//
//=============================================================================//

#ifndef CHOREOACTOR_H
#define CHOREOACTOR_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/utlstring.h"
#include "resourcesystem/resourcehandle.h"
#include "resourcesystem/resourcename.h"
#include "choreoobjects/choreoobjectbase.h"

class CChoreoChannel;
class InfoForResourceTypeCModel;

//-----------------------------------------------------------------------------
// Owned and destroyed by its scene (no virtual destructor). 528 bytes on both platforms.
//-----------------------------------------------------------------------------
abstract_class CChoreoActor : public CChoreoObjectBase
{
public:
	// "name"
	CUtlString m_Name;
	// "model_name": the model the tools show the actor with
	CResourceNameTyped< CWeakHandle< InfoForResourceTypeCModel > > m_ModelName;
	// "moods_file": the vdata file of the actor's moods (empty for the default moods file)
	CResourceName m_MoodsFile;
	CUtlVector< CChoreoChannel * > m_Channels;
	// The element type is not established: only the destructor touches this vector (it frees its
	// memory)
	CUtlVector< void * > m_ReservedVector;
	// True from the constructor on; an inactive actor's events do not start
	bool m_bActive;
	// "dynamic_actor"
	bool m_bDynamicActor;
};

#endif // CHOREOACTOR_H
