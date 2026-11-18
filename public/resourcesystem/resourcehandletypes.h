#ifndef RESOURCESYSTEM_RESOURCEHANDLETYPES_H
#define RESOURCESYSTEM_RESOURCEHANDLETYPES_H

#ifdef _WIN32
#pragma once
#endif

template < class T > class CWeakHandle;

class CModel;
class CTextureBase;
class CRenderMesh;

class InfoForResourceTypeCModel
{
public:
	using RuntimeClass_t = CModel;
};

class InfoForResourceTypeCTextureBase
{
public:
	using RuntimeClass_t = CTextureBase;
};

class InfoForResourceTypeCRenderMesh
{
public:
	using RuntimeClass_t = CRenderMesh;
};

using HRenderTexture = CWeakHandle< InfoForResourceTypeCTextureBase >;

#endif // RESOURCESYSTEM_RESOURCEHANDLETYPES_H
