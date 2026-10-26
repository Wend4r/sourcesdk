#ifndef RESOURCESYSTEM_RESOURCEHANDLETYPES_H
#define RESOURCESYSTEM_RESOURCEHANDLETYPES_H

#ifdef _WIN32
#pragma once
#endif

class CModel;
class CTextureBase;

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

#endif // RESOURCESYSTEM_RESOURCEHANDLETYPES_H
