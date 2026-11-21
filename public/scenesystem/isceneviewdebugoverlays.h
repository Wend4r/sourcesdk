//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Listener of a debug overlay list (ISceneViewDebugOverlays)
//
//=============================================================================//

#ifndef ISCENEVIEWDEBUGOVERLAYS_H
#define ISCENEVIEWDEBUGOVERLAYS_H
#ifdef _WIN32
#pragma once
#endif

#include <tier0/platform.h>

#include "schemacompiler/schemaon.h"

class ISceneViewDebugOverlays;

// The kind of overlay a listener is told about (a schema enum of the client and the scene system)
schema enum ESceneViewDebugOverlaysListenerDataType_t : uint32
{
	k_ESceneViewDebugOverlaysListenerDataType_Unknown = 0,
	k_ESceneViewDebugOverlaysListenerDataType_Sphere = 1,
	k_ESceneViewDebugOverlaysListenerDataType_Capsule = 2,
	k_ESceneViewDebugOverlaysListenerDataType_BoxAngles = 3,
	k_ESceneViewDebugOverlaysListenerDataType_Line = 4,
	k_ESceneViewDebugOverlaysListenerDataType_SolidBoxAngles = 5,
	k_ESceneViewDebugOverlaysListenerDataType_Text3D = 6,
};

//-----------------------------------------------------------------------------
// Added to an overlay list with CSceneViewDebugOverlays slot 84 (a
// vector of listeners). One method, no destructor in the table. Its body is empty in every CS2
// implementation (server and client CDebugOverlayGameSystem), so its arguments were not identified
// and are kept opaque; the name is descriptive.
//-----------------------------------------------------------------------------
abstract_class ISceneViewDebugOverlaysListener
{
public:
	virtual void OnDebugOverlayData( const void *pData ) = 0;
};

#include "schemacompiler/schemaoff.h"

#endif // ISCENEVIEWDEBUGOVERLAYS_H
