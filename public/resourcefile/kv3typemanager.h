#ifndef KV3TYPEMANAGER_H
#define KV3TYPEMANAGER_H

#ifdef _WIN32
	#pragma once
#endif

#include "resourcetype.h"

#include <tier0/platform.h>
#include "resourcesystem/iresourcesystem.h"

// Resource type managers of KV3 resources. Which of the two declares each member and virtual is not known,
// all of them are declared in the first one
template < ResourceType_t RESOURCE_TYPE >
class CKV3ResourceTypeManager : public IResourceTypeManager
{
public:
	virtual void unk024( void *p );

	// CVPhysics2SurfacePropertyManager frees the CUtlVector< CPhysSurfaceProperties * > of a loaded resource and its elements here
	virtual void unk025( void *p );

	// Calls unk024
	virtual void unk026( void *p );

	virtual bool unk027();

public:
	// Stored by IResourceTypeManager::Init
	IResourceSystemUtils *m_pResourceSystemUtils;

	// The version the loaded resources must have ("Unsupported version %d for resource '%s' (Expected %d)")
	int m_nResourceVersion;

	// "generic" in CVPhysics2SurfacePropertyManager
	const char *m_unk018;

	uint64 m_unk020[ 2 ];
};

template < ResourceType_t RESOURCE_TYPE >
class CDefaultKV3TransferResourceTypeManager : public CKV3ResourceTypeManager< RESOURCE_TYPE >
{
};

#endif // KV3TYPEMANAGER_H
