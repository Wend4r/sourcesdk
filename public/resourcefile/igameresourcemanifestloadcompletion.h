#ifndef IGAMERESOURCEMANIFESTLOADCOMPLETION_H
#define IGAMERESOURCEMANIFESTLOADCOMPLETION_H

#ifdef COMPILER_MSVC
#pragma once
#endif

#include "resourcetype.h"

class IGameResourceManifestLoadCompletionCallback
{
public:
	// The game resource service calls it instead of OnGameResourceManifestLoaded when the callback is replaced
	// or the manifest is empty; standalone callbacks delete themselves here.
	virtual void Release() = 0;
	virtual void OnGameResourceManifestLoaded(HGameResourceManifest hManifest, int nResourceCount, ResourceHandle_t *pResourceHandles) = 0;
};

#endif // IGAMERESOURCEMANIFESTLOADCOMPLETION_H
