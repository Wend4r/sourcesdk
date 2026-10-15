#ifndef RENDERMESH_IMP_H
#define RENDERMESH_IMP_H

#ifdef _WIN32
#pragma once
#endif

#include <resourcesystem/iresourcesystem.h>
#include <tier0/platform.h>

//-----------------------------------------------------------------------------
// Resource type manager of the render meshes, g_pRenderMeshManager points at it
//-----------------------------------------------------------------------------
class CRenderMeshManager : public IResourceTypeManager
{
public:
	virtual ~CRenderMeshManager();

public:
	bool m_bTraceLinesEnabled;
};

#endif // RENDERMESH_IMP_H
