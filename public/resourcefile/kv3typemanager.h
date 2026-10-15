#ifndef KV3TYPEMANAGER_H
#define KV3TYPEMANAGER_H

#ifdef _WIN32
	#pragma once
#endif

#include "resourcetype.h"

#include <tier0/platform.h>
#include "kv3lib/kv3formats.h"
#include "resourcesystem/iresourcesystem.h"

class KeyValues3;
class CKV3ResourceBlockHelper;
class IRD_RegisterResourceDataUtils;
class IResourceDeallocatorUtils;

// Resource type managers of KV3 resources
template < ResourceType_t RESOURCE_TYPE >
class CKV3ResourceTypeManager : public IResourceTypeManager
{
public:
	bool Init( IResourceSystemUtils *pUtils, int nUnknown ) override;

	// Checks the resource version against m_nExpectedResourceVersion ("Unsupported version %d for resource '%s' (Expected %d)"),
	// then creates the resource data with AllocateEx
	void AllocateResource( ResourceHandle_t hResource, ResourceId_t nId, const ResourceFileHeader_t *pHeader, IRD_RegisterResourceDataUtils *pUtils ) override;

	// Releases the resource data with Deallocate
	void DeallocateResource( void *pData, IResourceDeallocatorUtils *pUtils ) override;

	bool WantsAsyncProcessData() const override;
	std::unique_ptr< IAsyncProcessedDataResult > AsyncProcessData( ResourceHandle_t hResource, const ResourceFileHeader_t *pHeader ) override;

	// Implemented by the typed managers: create the resource data from its KV3, release it
	virtual void *Allocate( ResourceHandle_t hResource, ResourceId_t nId, const KeyValues3 *pKV, IRD_RegisterResourceDataUtils *pUtils );
	virtual void Deallocate( ResourceHandle_t hResource, void *pData );

	// Calls Allocate with the KV3 of the block
	virtual void *AllocateEx( ResourceHandle_t hResource, ResourceId_t nId, CKV3ResourceBlockHelper *pBlock, IRD_RegisterResourceDataUtils *pUtils );

	// Returns false in every manager
	virtual bool unk027();

	void SetExpectedResourceVersion( int nVersion ) { m_nExpectedResourceVersion = nVersion; }

public:
	// Stored by Init
	IResourceSystemUtils *m_pResourceSystemUtils;

	int m_nExpectedResourceVersion;

	// g_KV3Format_Generic unless the manager expects its own format
	KV3ID_t m_ExpectedKV3Format;
};

template < ResourceType_t RESOURCE_TYPE >
class CDefaultKV3TransferResourceTypeManager : public CKV3ResourceTypeManager< RESOURCE_TYPE >
{
public:
	// Loads the resource data from the KV3 through the KV3 transfer
	void *Allocate( ResourceHandle_t hResource, ResourceId_t nId, const KeyValues3 *pKV, IRD_RegisterResourceDataUtils *pUtils ) override;
};

#endif // KV3TYPEMANAGER_H
