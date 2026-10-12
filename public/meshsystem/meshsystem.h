#ifndef MESHSYSTEM_H
#define MESHSYSTEM_H

#ifdef _WIN32
#pragma once
#endif

#include "meshsystem/imeshsystem.h"
#include <tier3/tier3.h>

//-----------------------------------------------------------------------------
// Implementation of IMeshSystem. Its members are kept as padding: the render
// mesh manager (polymorphic, g_pRenderMeshManager points at it) at 0x28, the
// model resource type manager (three vtables) from 0x38, two CUtlVectors and a
// lock from 0xA8, and a std::function at 0xF0.
//-----------------------------------------------------------------------------
class CMeshSystem : public CTier3AppSystem< IMeshSystem >
{
public:
	uint8 m_unk028[ 0x108 ];
};

#endif // MESHSYSTEM_H
