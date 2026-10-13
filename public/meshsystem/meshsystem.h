#ifndef MESHSYSTEM_H
#define MESHSYSTEM_H

#ifdef _WIN32
#pragma once
#endif

#include "meshsystem/imeshsystem.h"
#include "meshsystem/rendermesh_imp.h"
#include <tier0/threadtools.h>
#include <tier0/utlvector.h>
#include <tier3/tier3.h>

#include <functional>

//-----------------------------------------------------------------------------
// Implementation of IMeshSystem. Members whose types are not declared in the
// SDK are kept as padding of their size.
//-----------------------------------------------------------------------------
class CMeshSystem : public CTier3AppSystem< IMeshSystem >
{
public:
	CRenderMeshManager m_RenderableManager;

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

	// Set by unk035(), the model type manager calls it with the id of a legacy model it loads
	std::function< void( ResourceId_t nResourceId ) > m_unk0F0;
};

#endif // MESHSYSTEM_H
