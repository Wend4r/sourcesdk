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

	// Added by AddMaterialFlagsCallback(), applied by GetMaterialFlags()
	CUtlVector< uint32 ( * )( IMaterial2 *pMaterial, uint32 nFlags ) > m_MaterialFlagsCallbacks;

	CAtomicMutex m_GroomMeshInstancesMutex;

	// Added by the nine-argument CreateMeshInstance() when the render mesh has groom data, removed by the mesh
	// instance destructor and when the model is unloaded
	CUtlVector< CMeshInstance * > m_GroomMeshInstances;

	// "AnimatableSceneObjectDesc", looked up by the two-argument CreateMeshInstance()
	ISceneObjectDesc *m_pAnimatableDesc;

	// The model type manager calls it with the id of a legacy model it loads when "legacy_models_supported" is set
	std::function< void( ResourceId_t nResourceId ) > m_LegacyModelLoadCallback;
};

#endif // MESHSYSTEM_H
