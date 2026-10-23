#ifndef INTROSPECTEDTOKV3TYPEMANAGER_H
#define INTROSPECTEDTOKV3TYPEMANAGER_H

#ifdef _WIN32
	#pragma once
#endif

#include "resourcetype.h"

#include <tier0/platform.h>
#include "resourcesystem/iresourcesystem.h"

class IResourceSystemUtils;
class IRD_RegisterResourceDataUtils;
class IResourceDeallocatorUtils;
class IAsyncProcessedDataResult;
class CKV3TransferLoadContext;
struct ResourceFileHeader_t;

class CBaseIntrospectedToKV3TypeManager : public IResourceTypeManager
{
public:
	ResourceType_t GetResourceType() const { return m_nResourceType; }

	// Implemented by the typed managers: create the resource data from the KV3 transfer, release it
	virtual void *AllocateAndLoad( CKV3TransferLoadContext &context, ResourceHandle_t hResource, IRD_RegisterResourceDataUtils *pUtils ) = 0;
	virtual void DeallocateDestroyCleanup( void *pData, IResourceDeallocatorUtils *pUtils ) = 0;

public:
	ResourceType_t m_nResourceType;
	uint16 m_nIntrospectedResourceVersion;
	uint16 m_nKV3ResourceVersion;
	IResourceSystemUtils *m_pResourceSystemUtils;
};

template < ResourceType_t RESOURCE_TYPE, typename T >
class CDefaultIntrospectedToKV3TypeManager : public CBaseIntrospectedToKV3TypeManager
{
public:
	void *AllocateAndLoad( CKV3TransferLoadContext &context, ResourceHandle_t hResource, IRD_RegisterResourceDataUtils *pUtils ) override { return nullptr; }
	void DeallocateDestroyCleanup( void *pData, IResourceDeallocatorUtils *pUtils ) override {}
};

class IKV3ResourceAllocatorUtils;
class IKV3ResourceDeallocatorUtils;

template < ResourceType_t RESOURCE_TYPE, typename T >
class CTypesafeIntrospectedToKV3TypeManager : public CBaseIntrospectedToKV3TypeManager
{
public:
	void *AllocateAndLoad( CKV3TransferLoadContext &context, ResourceHandle_t hResource, IRD_RegisterResourceDataUtils *pUtils ) override { return nullptr; }
	void DeallocateDestroyCleanup( void *pData, IResourceDeallocatorUtils *pUtils ) override {}

	virtual void *AllocateTypesafe( T *pData, IKV3ResourceAllocatorUtils *pUtils ) { return nullptr; }
	virtual void DeallocateTypesafe( void *pObject, IKV3ResourceDeallocatorUtils *pUtils ) {}
};

COMPILE_TIME_ASSERT( sizeof( CBaseIntrospectedToKV3TypeManager ) == 0x20 );

#endif // INTROSPECTEDTOKV3TYPEMANAGER_H
