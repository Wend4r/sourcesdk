//===== Copyright (c) 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: Game side of the navigation mesh generation, used by the tools
//
//===========================================================================//

#ifndef ICUSTOMNAVGEN_H
#define ICUSTOMNAVGEN_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "appframework/iappsystem.h"

class CNavMesh;
class IVPhysics2World;

abstract_class ICustomNavGen : public IAppSystem
{
public:
	// Named by what it does: allocates the game's navigation mesh class
	virtual CNavMesh *CreateNavMesh() = 0;

	// Named by what it does: runs the game's post-processing of a generated mesh ("Compute AI Data": approach areas,
	// play area markup from the "trigger_survival_playarea" entities). pUnused is not read; pPhysicsQuery is wrapped
	// in the navigation tools physics interface for the traces; pEntityQuery answers entity lookups by class name;
	// *ppNavMesh is the mesh to process
	virtual void ComputeAIData( void *pUnused, void *pPhysicsQuery, void *pEntityQuery, CNavMesh **ppNavMesh ) = 0;

	// Creates a navigation mesh and loads the one of pszMapName ("maps/" and the extension are stripped) from the
	// map's VPK
	virtual CNavMesh *CreateAndLoadNavMeshFromVPK( const IVPhysics2World *pPhysicsWorld, const char *pszMapName ) = 0;
};

#endif // ICUSTOMNAVGEN_H
