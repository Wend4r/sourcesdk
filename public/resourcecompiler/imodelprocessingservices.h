//===== Copyright (c) 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: Model processing services of the resource compiler for the model editor
//
//===========================================================================//

#ifndef IMODELPROCESSINGSERVICES_H
#define IMODELPROCESSINGSERVICES_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "appframework/iappsystem.h"

class CUtlBuffer;

abstract_class IModelProcessingServices : public IAppSystem
{
public:
	// Forwards to IModelDocUtils::LoadModelFileAsDmx
	virtual bool LoadModelFileAsDmx( const char *pszSourceFilename, int nFlags, const char *pszFilter, float flScale, CUtlBuffer *pOutBuffer ) = 0;

	// Named by what it does: builds a cloth proxy mesh ( CVClothProxyMeshOptions, root bone "$cloth_root" ) from
	// the source mesh into pOutMesh
	virtual bool BuildClothProxyMesh( const void *pSourceMesh, const void *pOptions, void *pOutMesh, void *pContext ) = 0;

	// Named by what it does: forwards its four arguments to an internal model processing step
	virtual bool ProcessModelMeshes( const void *pSource, const void *pOptions, void *pOutput, void *pContext ) = 0;

	// Named by what it does: loads the model source file named by pSource ( its name at +8, "" when unset ) as
	// binary DMX and hands the result to the resource system
	virtual bool LoadModelSource( void *pOutput, const void *pSource ) = 0;
};

#endif // IMODELPROCESSINGSERVICES_H
