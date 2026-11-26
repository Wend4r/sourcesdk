//===== Copyright (c) 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: Model document ( .vmdl ) helpers shared by the model editor, the asset tools and the
//          resource compiler
//
//===========================================================================//

#ifndef IMODELDOCUTILS_H
#define IMODELDOCUTILS_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "tier0/utlstring.h"
#include "tier1/utlvector.h"
#include "appframework/iappsystem.h"

class CUtlBuffer;

abstract_class IModelDocUtils : public IAppSystem
{
public:
	// The CModelDocLoaderUtils object created in Init
	virtual void *GetModelDocLoaderUtils() = 0;

	// Named by what they do: the animation nodes ( AnimationList / AnimBase classes ) of a model document of
	// kind 1 or 2 below pRoot, and of a whole document
	virtual void FindAnimationNodes( const void *pDocument, const void *pRoot, CUtlVector< void * > *pNodes ) = 0;
	virtual void FindAllAnimationNodes( const void *pDocument, CUtlVector< void * > *pNodes ) = 0;
	// Named by what it does: the names of the animations of the documents ( "unnamed_%d" for unnamed ones )
	virtual void GetAnimationNames( int nDocumentKind, const void *pDocument, CUtlVector< CUtlString > *pNames ) = 0;

	// Named by what they do: convert a compiled model ( pszSourceFilename ) or an MDL ( m_sMDLFilename ) into
	// a model document written to pszDestFilename; the errors and warnings go to pErrors
	// ( "==Errors==" / "==Warnings==" )
	virtual bool ConvertVModelToModelDoc( const void *pSource, const void *pOptions, const char *pszSourceFilename, const char *pszDestFilename, CUtlString *pErrors ) = 0;
	// Named by what they do: run a model document through its node checks and report "==Errors==" and
	// "==Warnings==" into pErrors
	virtual bool ValidateModelDoc( const void *pDocument, CUtlString *pErrors, void *pContext ) = 0;
	virtual bool ValidateModelDocAnimations( const void *pDocument, CUtlString *pErrors, void *pContext ) = 0;
	virtual bool ConvertMDLToModelDoc( const void *pSource, const void *pOptions, const char *pszSourceFilename, const char *pszDestFilename, CUtlString *pErrors ) = 0;

	// "Cannot bake simulation from %s: %s"
	virtual bool CanBakeSimulation( const void *pSource ) = 0;
	// "tools/model_archetypes.txt"
	virtual const char *GetModelArchetypesFilename() = 0;
	// Whether the archetype suppresses the warning ( names may carry the "suppress_warning_" prefix )
	virtual bool IsWarningSuppressed( const char *pszArchetype, const char *pszWarning ) = 0;

	// Named by what it does: loads a model source file ( pszFilter selects part of it, "" for all; flScale
	// scales it ) and serialises it as binary DMX into pOutBuffer
	virtual bool LoadModelFileAsDmx( const char *pszSourceFilename, int nFlags, const char *pszFilter, float flScale, CUtlBuffer *pOutBuffer ) = 0;
	// Named by what it does: the duration of the animation of a source file ( single take only )
	virtual float GetSourceAnimationDuration( const char *pszSourceFilename ) = 0;
	// "WriteFbxMesh": writes the model, skeleton and hull meshes of a document to an FBX file
	virtual bool WriteFbxMesh( const void *pDocument, const char *pszDestFilename ) = 0;
};

#endif // IMODELDOCUTILS_H
