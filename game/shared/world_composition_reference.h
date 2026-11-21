//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Loading of another map's entities around a landmark (CS2
//			game/shared/world_composition_reference.cpp, the path of
//			CEntityLumpRequest::ComputeWorldOrigin's assert).
//
//=============================================================================//

#ifndef WORLD_COMPOSITION_REFERENCE_H
#define WORLD_COMPOSITION_REFERENCE_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/utlstring.h"
#include "mathlib/vector.h"
#include "worldrenderer/iworldrenderermgr.h"

//-----------------------------------------------------------------------------
// A request to load a map as a spawn group placed so that its landmark of the
// same name lands on this map's landmark. 80 bytes on both platforms; the fields
// are named by what Start and ComputeWorldOrigin do with them.
//-----------------------------------------------------------------------------
class CEntityLumpRequest : public IComputeWorldOriginCallback
{
public:
	// Finds the landmark of the loaded world ("origin" and "angles" keys) and returns the
	// transform that moves it onto m_vecOldLandmarkOrigin / m_angOldLandmarkAngles
	virtual matrix3x4_t ComputeWorldOrigin( const char *pWorldName, SpawnGroupHandle_t hSpawnGroup, IWorld *pWorld ) override;

public:
	CUtlString m_MapName;
	CUtlString m_LandmarkName;
	// The spawn group the engine created for the map
	SpawnGroupHandle_t m_hSpawnGroup;
	// Old landmark minus new landmark, computed by ComputeWorldOrigin
	Vector m_vecLandmarkOffset;
	// The landmark entity of this map
	Vector m_vecOldLandmarkOrigin;
	QAngle m_angLandmarkOffset;
	QAngle m_angOldLandmarkAngles;
	// Set by Start, cleared by ComputeWorldOrigin
	bool m_bWaitingForWorldOrigin;
	// Set by ComputeWorldOrigin once the offsets are known
	bool m_bOffsetComputed;
	// Set when Start got the landmark entity instead of looking it up by name
	bool m_bLandmarkEntityGiven;
};

#endif // WORLD_COMPOSITION_REFERENCE_H
