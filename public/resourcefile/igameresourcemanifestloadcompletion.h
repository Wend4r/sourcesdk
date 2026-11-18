#ifndef IGAMERESOURCEMANIFESTLOADCOMPLETION_H
#define IGAMERESOURCEMANIFESTLOADCOMPLETION_H

#ifdef COMPILER_MSVC
#pragma once
#endif

#include "resourcetype.h"

class IGameResourceManifestLoadCompletionCallback
{
public:
	// Named by what it does: empty in CBaseSpawnGroup on both ABIs (first slot of its table for this
	// base; Linux has no second destructor slot here, so it is not a destructor). No other
	// implementation or caller was found.
	virtual void EmptyVirtual() = 0;

	virtual void OnGameResourceManifestLoaded(HGameResourceManifest hManifest, int nResourceCount, ResourceHandle_t *pResourceHandles) = 0;
};

#endif // IGAMERESOURCEMANIFESTLOADCOMPLETION_H
