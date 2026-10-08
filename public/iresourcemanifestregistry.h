#ifndef IRESOURCEMANIFESTREGISTRY_H
#define IRESOURCEMANIFESTREGISTRY_H

#pragma once

#include "interfaces/interfaces.h"
#include "resourcefile/resourcetype.h"

class IResourceManifestRegistry
{
public:
	virtual void RegisterFrameNumberTracker( int *pnFrameNumber ) = 0;
	virtual void UnregisterFrameNumberTracker( int *pnFrameNumber ) = 0;
	virtual void RegisterNamedManifest( ResourceManifestDesc_t *pDesc ) = 0;
	virtual void UnregisterNamedManifest( ResourceManifestDesc_t *pDesc ) = 0;
};

DECLARE_TIER2_INTERFACE( IResourceManifestRegistry, g_pResourceManifestRegistry );

#endif // IRESOURCEMANIFESTREGISTRY_H
