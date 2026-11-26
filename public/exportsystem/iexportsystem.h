//===== Copyright (c) 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: Export of models, animations and textures to interchange formats ( FBX, DMX, PNG / JPG )
//
//===========================================================================//

#ifndef IEXPORTSYSTEM_H
#define IEXPORTSYSTEM_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "tier0/utlstring.h"
#include "appframework/iappsystem.h"

class IExportModel;
class IExportAnim;

abstract_class IExportSystem : public IAppSystem
{
public:
	// Converts a model / animation source file ( resolved through pszSourcePathID ) into the interchange file
	// pszDestFilename ( the writer is picked by its extension ); pErrorString gets the reason on failure
	virtual bool ConvertFile( const char *pszSourceFilename, const char *pszSourcePathID, const char *pszDestFilename, const char *pszDestPathID, CUtlString *pErrorString ) = 0;

	// Named by what it does: writes the meshes and the skeleton of a loaded model ( HModel ) to an FBX file;
	// pOptions goes to the mesh converter
	virtual bool ExportModel( const void *pModel, const char *pszDestFilename, const char *pszDestPathID, const void *pOptions, CUtlString *pErrorString ) = 0;

	// Named by what they do: an empty export model, and an export animation whose scene is FBX ( nSceneType 0 )
	// or DMX ( nSceneType 1 )
	virtual IExportModel *CreateExportModel( const char *pszName ) = 0;
	virtual IExportAnim *CreateExportAnim( const char *pszName, int nSceneType ) = 0;

	// Named by what they do: write a texture ( a loaded texture resource, or a texture by name ) to an image
	// file; bDecodeNormalMap decodes hemi-octahedron normal encodings first. The result is the written file name
	virtual CUtlString ExportTexture( const char *pszDestFilename, const char *pszDestPathID, const void *pTexture, bool bDecodeNormalMap ) = 0;
	virtual CUtlString ExportTextureByName( const char *pszDestFilename, const char *pszDestPathID, const char *pszTextureName, bool bDecodeNormalMap ) = 0;
};

#endif // IEXPORTSYSTEM_H
