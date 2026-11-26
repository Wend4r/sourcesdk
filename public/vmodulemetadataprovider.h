#ifndef VMODULEMETADATAPROVIDER_H
#define VMODULEMETADATAPROVIDER_H

#pragma once

#include "tier0/platform.h"

class KeyValues3;

//-----------------------------------------------------------------------------
// A reader of one member of the metadata of a module. The providers of the game
// (CEntityAPIModuleMetadataProvider, CEntityClassInfoModuleMetadataProvider,
// CModuleMetadataProvider_ResourceManifests) are static objects that override its one method; the
// constructor links each one into the list of the module. One slot on both
// platforms, no virtual destructor.
//-----------------------------------------------------------------------------
abstract_class CBaseVModuleMetadataProvider
{
public:
	// Named by what it does: every provider reads its own member of pMetadata ("entity_api",
	// "resource_manifests", the entity class info) and returns true
	virtual bool ProcessModuleMetadata( const KeyValues3 *pMetadata ) = 0;

public:
	// Named by what it holds: the provider constructed before this one (the head of the list
	// is a static of the module).
	CBaseVModuleMetadataProvider *m_pNext;
};

#endif // VMODULEMETADATAPROVIDER_H
