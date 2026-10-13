#ifndef MESHSYSTEM_H
#define MESHSYSTEM_H

#ifdef _WIN32
#pragma once
#endif

#include "meshsystem/imeshsystem.h"
#include <tier0/threadtools.h>
#include <tier0/utlvector.h>
#include <tier3/tier3.h>

//-----------------------------------------------------------------------------
// Implementation of IMeshSystem. Members whose types are not declared in the
// SDK are kept as padding of their size.
//-----------------------------------------------------------------------------
class CMeshSystem : public CTier3AppSystem< IMeshSystem >
{
public:
	// CRenderMeshManager, polymorphic, g_pRenderMeshManager points at it
	void *m_RenderableManager[ 2 ];

	// CModelTypeManager, the "vmdl" resource type manager (three vtables)
	void *m_modelTypeManager[ 14 ];

	// Added by unk037(), applied by unk036()
	CUtlVector< uint32 ( * )( IMaterial2 *pMaterial, uint32 nFlags ) > m_unk0A8;

	// Locks m_unk0D0
	CAtomicMutex m_unk0C0;

	// Copied out by unk038()
	CUtlVector< void * > m_unk0D0;

	// "AnimatableSceneObjectDesc", looked up by the two-argument CreateMeshInstance()
	ISceneObjectDesc *m_pAnimatableDesc;

	// std::function set by unk035()
#ifdef _WIN32
	uint8 m_unk0F0[ 0x40 ];
#else
	uint8 m_unk0F0[ 0x20 ];
#endif
};

#endif // MESHSYSTEM_H
